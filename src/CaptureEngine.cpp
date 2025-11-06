#include "CaptureEngine.h"
#include <chrono>
#include <thread>
#include <cmath>

CaptureEngine::CaptureEngine(const std::string& source, int fps, float threshold)
    : videoSource(source), targetFPS(fps), confThreshold(threshold), 
      running(false), captureThread(nullptr), autoTracking(false),
      // PID state
      integral_x(0), integral_y(0), prev_err_x(0), prev_err_y(0),
      filtered_derivative_x(0), filtered_derivative_y(0),
      prev_ptz_speed_x(0), prev_ptz_speed_y(0),
      last_nx(0.5), last_ny(0.5), last_nz(0),
      lost_frames(0), manual_mode(false),
      manual_target_x(0.5), manual_target_y(0.5)
{
    detector = std::make_unique<YOLODetector>("yolov8n.onnx", threshold);
    
    // Parâmetros de controle
    conf_min = 0.5f;
    nz_min = 0.02f;
    deadband = 0.03f;
    
    Kp_x = 1.2f; Ki_x = 0.05f; Kd_x = 0.06f;
    Kp_y = 1.0f; Ki_y = 0.05f; Kd_y = 0.05f;
    I_max = 0.5f;
    
    gamma = 0.8f;
    near_edge_threshold = 0.15f;
    approach_limit = 0.05f;
    v_thresh = 0.02f;
    slew_rate = 0.6f;
    lpf_tau = 0.08f;
    stop_threshold = 0.02f;
    lost_max_frames = 15;
}

CaptureEngine::~CaptureEngine() {
    stop();
}

void CaptureEngine::setConfidenceThreshold(float threshold) {
    confThreshold = threshold;
    if (detector) {
        detector->setConfidenceThreshold(threshold);
    }
}

void CaptureEngine::setAutoTracking(bool enabled) {
    autoTracking = enabled;
    if (enabled) {
        resetPIDState();
    }
}

void CaptureEngine::setManualTarget(float x, float y) {
    manual_mode = true;
    manual_target_x = x;
    manual_target_y = y;
    resetPIDState();
}

void CaptureEngine::resetPIDState() {
    integral_x = 0;
    integral_y = 0;
    prev_err_x = 0;
    prev_err_y = 0;
    filtered_derivative_x = 0;
    filtered_derivative_y = 0;
    prev_ptz_speed_x = 0;
    prev_ptz_speed_y = 0;
    lost_frames = 0;
}

void CaptureEngine::start() {
    if (running) return;
    running = true;
    
    captureThread = QThread::create([this]() { captureLoop(); });
    captureThread->start();
}

void CaptureEngine::stop() {
    running = false;
    if (captureThread) {
        captureThread->wait();
        delete captureThread;
        captureThread = nullptr;
    }
}

void CaptureEngine::captureLoop() {
    cv::VideoCapture cap;
    
    try {
        int deviceId = std::stoi(videoSource);
        cap.open(deviceId);
    } catch (...) {
        cap.open(videoSource);
    }
    
    if (!cap.isOpened()) {
        return;
    }
    
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    
    double frameTime = 1000.0 / targetFPS;
    auto lastFpsTime = std::chrono::steady_clock::now();
    auto lastFrameTime = std::chrono::steady_clock::now();
    int frameCounter = 0;
    
    while (running) {
        auto startTime = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - lastFrameTime).count();
        lastFrameTime = now;
        
        cv::Mat frame;
        if (!cap.read(frame)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        auto detections = detector->detect(frame);
        drawDetections(frame, detections);
        
        // Controle PTZ avançado
        if (autoTracking || manual_mode) {
            processPTZControl(frame, detections, dt);
        }
        
        emit frameReady(matToQImage(frame));
        emit detectionCount(detections.size());
        
        frameCounter++;
        auto elapsed = std::chrono::duration<double>(now - lastFpsTime).count();
        
        if (elapsed >= 1.0) {
            double fps = frameCounter / elapsed;
            emit fpsUpdated(fps);
            frameCounter = 0;
            lastFpsTime = now;
        }
        
        auto processingTime = std::chrono::steady_clock::now() - startTime;
        auto waitTime = std::chrono::milliseconds((int)frameTime) - 
                       std::chrono::duration_cast<std::chrono::milliseconds>(processingTime);
        
        if (waitTime.count() > 0) {
            std::this_thread::sleep_for(waitTime);
        }
    }
    
    cap.release();
}

void CaptureEngine::processPTZControl(const cv::Mat& frame, 
                                      const std::vector<Detection>& detections, 
                                      float dt) {
    float nx, ny, nz;
    bool target_found = false;
    
    if (manual_mode) {
        // Modo manual: usar coordenada do clique
        nx = manual_target_x;
        ny = manual_target_y;
        nz = 0.1f;
        target_found = true;
    } else if (!detections.empty()) {
        // Auto mode: selecionar melhor alvo
        Detection bestTarget = selectBestTarget(frame, detections);
        
        if (bestTarget.confidence >= conf_min) {
            // Normalizar bbox
            float cx = bestTarget.bbox.x + bestTarget.bbox.width / 2.0f;
            float cy = bestTarget.bbox.y + bestTarget.bbox.height / 2.0f;
            float diag = std::sqrt(frame.cols * frame.cols + frame.rows * frame.rows);
            
            nx = cx / frame.cols;
            ny = cy / frame.rows;
            nz = std::sqrt(bestTarget.bbox.width * bestTarget.bbox.height) / diag;
            
            if (nz >= nz_min) {
                target_found = true;
                last_nx = nx;
                last_ny = ny;
                last_nz = nz;
                lost_frames = 0;
            }
        }
    }
    
    if (!target_found) {
        lost_frames++;
        
        if (lost_frames < lost_max_frames) {
            // Manter último comando com decaimento
            integral_x *= 0.95f;
            integral_y *= 0.95f;
            nx = last_nx;
            ny = last_ny;
        } else {
            // Alvo perdido - parar
            if (autoTracking) {
                emit ptzAdjustmentNeeded(0, 0);
            }
            return;
        }
    }
    
    // Calcular erro normalizado
    float err_x = nx - 0.5f;
    float err_y = ny - 0.5f;
    float err_mag = std::sqrt(err_x * err_x + err_y * err_y);
    
    // Modo manual: parar quando próximo do alvo
    if (manual_mode && err_mag < stop_threshold) {
        manual_mode = false;
        emit ptzAdjustmentNeeded(0, 0);
        return;
    }
    
    // Aplicar deadband
    float err_x_eff = applyDeadband(err_x);
    float err_y_eff = applyDeadband(err_y);
    
    // Calcular distâncias às bordas
    float dist_x_edge = std::min(nx, 1.0f - nx);
    float dist_y_edge = std::min(ny, 1.0f - ny);
    
    // Estimar velocidade do alvo
    float v_target_x = (err_x - prev_err_x) / (dt + 1e-6f);
    float v_target_y = (err_y - prev_err_y) / (dt + 1e-6f);
    
    // Calcular PID para X
    float u_p_x = Kp_x * err_x_eff;
    integral_x += Ki_x * err_x_eff * dt;
    integral_x = std::clamp(integral_x, -I_max, I_max);
    
    float derivative_x = (err_x_eff - prev_err_x) / (dt + 1e-6f);
    float alpha_d = std::exp(-dt / lpf_tau);
    filtered_derivative_x = alpha_d * filtered_derivative_x + (1 - alpha_d) * derivative_x;
    float u_d_x = Kd_x * filtered_derivative_x;
    
    float raw_speed_x = u_p_x + integral_x + u_d_x;
    
    // Calcular PID para Y
    float u_p_y = Kp_y * err_y_eff;
    integral_y += Ki_y * err_y_eff * dt;
    integral_y = std::clamp(integral_y, -I_max, I_max);
    
    float derivative_y = (err_y_eff - prev_err_y) / (dt + 1e-6f);
    filtered_derivative_y = alpha_d * filtered_derivative_y + (1 - alpha_d) * derivative_y;
    float u_d_y = Kd_y * filtered_derivative_y;
    
    float raw_speed_y = u_p_y + integral_y + u_d_y;
    
    // Aplicar não-linearidade (suavização)
    float speed_factor_x = applyNonLinearity(raw_speed_x);
    float speed_factor_y = applyNonLinearity(raw_speed_y);
    
    // Mapear para range PTZ [1, 2]
    float ptz_speed_x_target = 1.0f + speed_factor_x * 1.0f;
    float ptz_speed_y_target = 1.0f + speed_factor_y * 1.0f;
    
    // Regras dinâmicas near-edge
    if (dist_x_edge <= near_edge_threshold) {
        if (dist_x_edge <= approach_limit && std::abs(v_target_x) > v_thresh) {
            ptz_speed_x_target = 2.0f; // Aceleração de recuperação
        } else {
            ptz_speed_x_target = std::max(1.0f, ptz_speed_x_target * 0.7f);
        }
    }
    
    if (dist_y_edge <= near_edge_threshold) {
        if (dist_y_edge <= approach_limit && std::abs(v_target_y) > v_thresh) {
            ptz_speed_y_target = 2.0f;
        } else {
            ptz_speed_y_target = std::max(1.0f, ptz_speed_y_target * 0.7f);
        }
    }
    
    // Limitar aceleração (slew rate)
    float max_delta = slew_rate * dt;
    ptz_speed_x_target = std::clamp(ptz_speed_x_target, 
                                     prev_ptz_speed_x - max_delta,
                                     prev_ptz_speed_x + max_delta);
    ptz_speed_y_target = std::clamp(ptz_speed_y_target,
                                     prev_ptz_speed_y - max_delta,
                                     prev_ptz_speed_y + max_delta);
    
    // LPF final
    float alpha_lpf = std::exp(-dt / lpf_tau);
    float ptz_speed_x = alpha_lpf * prev_ptz_speed_x + (1 - alpha_lpf) * ptz_speed_x_target;
    float ptz_speed_y = alpha_lpf * prev_ptz_speed_y + (1 - alpha_lpf) * ptz_speed_y_target;
    
    // Clamping final
    ptz_speed_x = std::clamp(ptz_speed_x, 1.0f, 2.0f);
    ptz_speed_y = std::clamp(ptz_speed_y, 1.0f, 2.0f);
    
    // Converter para comandos PTZ com direção
    int pan_cmd = static_cast<int>(ptz_speed_x * std::copysign(1.0f, err_x_eff) * 6.0f);
    int tilt_cmd = static_cast<int>(-ptz_speed_y * std::copysign(1.0f, err_y_eff) * 5.0f);
    
    // Enviar comando apenas se significativo
    if (std::abs(err_x_eff) > 0.01f || std::abs(err_y_eff) > 0.01f) {
        emit ptzAdjustmentNeeded(pan_cmd, tilt_cmd);
    } else if (!manual_mode) {
        emit ptzAdjustmentNeeded(0, 0);
    }
    
    // Atualizar estados
    prev_err_x = err_x_eff;
    prev_err_y = err_y_eff;
    prev_ptz_speed_x = ptz_speed_x;
    prev_ptz_speed_y = ptz_speed_y;
}

Detection CaptureEngine::selectBestTarget(const cv::Mat& frame, 
                                          const std::vector<Detection>& detections) {
    Detection best = detections[0];
    float bestScore = 0;
    
    cv::Point2f frameCenter(frame.cols / 2.0f, frame.rows / 2.0f);
    
    for (const auto& det : detections) {
        cv::Point2f center(det.bbox.x + det.bbox.width / 2.0f,
                          det.bbox.y + det.bbox.height / 2.0f);
        
        float area = det.bbox.width * det.bbox.height;
        float distToCenter = std::sqrt(std::pow(center.x - frameCenter.x, 2) + 
                                      std::pow(center.y - frameCenter.y, 2));
        
        float score = (area / (distToCenter + 100.0f)) * det.confidence;
        
        if (score > bestScore) {
            bestScore = score;
            best = det;
        }
    }
    
    return best;
}

float CaptureEngine::applyDeadband(float err) {
    if (std::abs(err) < deadband) {
        return 0.0f;
    }
    return std::copysign(std::abs(err) - deadband, err);
}

float CaptureEngine::applyNonLinearity(float raw_speed) {
    float abs_speed = std::abs(raw_speed);
    float a = 1.5f; // Fator de normalização
    float speed_factor = a * std::pow(abs_speed, gamma);
    return std::clamp(speed_factor, 0.0f, 1.0f);
}

void CaptureEngine::drawDetections(cv::Mat& frame, const std::vector<Detection>& dets) {
    for (const auto& det : dets) {
        cv::rectangle(frame, det.bbox, cv::Scalar(0, 255, 0), 2);
        
        std::string label = det.label + " " + 
                           std::to_string((int)(det.confidence * 100)) + "%";
        
        int baseline;
        cv::Size textSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 
                                            0.6, 2, &baseline);
        
        cv::rectangle(frame, 
            cv::Point(det.bbox.x, det.bbox.y - textSize.height - 5),
            cv::Point(det.bbox.x + textSize.width, det.bbox.y),
            cv::Scalar(0, 255, 0), -1);
        
        cv::putText(frame, label, 
            cv::Point(det.bbox.x, det.bbox.y - 5),
            cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 0), 2);
        
        int cx = det.bbox.x + det.bbox.width / 2;
        int cy = det.bbox.y + det.bbox.height / 2;
        cv::circle(frame, cv::Point(cx, cy), 5, cv::Scalar(0, 255, 255), -1);
    }
}

QImage CaptureEngine::matToQImage(const cv::Mat& mat) {
    cv::Mat rgb;
    cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
    return QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, 
                  QImage::Format_RGB888).copy();
}