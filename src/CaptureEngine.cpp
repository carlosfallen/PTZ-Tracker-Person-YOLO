#include "CaptureEngine.h"
#include <chrono>
#include <thread>

CaptureEngine::CaptureEngine(const std::string& source, int fps, float threshold)
    : videoSource(source), targetFPS(fps), confThreshold(threshold), 
      running(false), captureThread(nullptr), autoTracking(false)
{
    detector = std::make_unique<YOLODetector>("yolov8n.onnx", threshold);
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
    int frameCounter = 0;
    
    // Variáveis de controle do auto-tracking
    cv::Point lastTargetCenter(-1, -1);
    int stableFrames = 0;
    const int STABILITY_THRESHOLD = 8;
    const int CENTER_DEADZONE = 60;
    auto lastPtzCommand = std::chrono::steady_clock::now();
    const int PTZ_COMMAND_INTERVAL_MS = 200;
    
    while (running) {
        auto startTime = std::chrono::steady_clock::now();
        
        cv::Mat frame;
        if (!cap.read(frame)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        auto detections = detector->detect(frame);
        drawDetections(frame, detections);
        
        // Auto tracking com estabilização avançada
        if (autoTracking && !detections.empty()) {
            Detection bestTarget = detections[0];
            float maxArea = 0;
            
            // Seleciona o alvo mais próximo do centro ou maior
            cv::Point frameCenter(frame.cols / 2, frame.rows / 2);
            float minDistanceToCenter = std::numeric_limits<float>::max();
            
            for (const auto& det : detections) {
                cv::Point center(det.bbox.x + det.bbox.width / 2,
                               det.bbox.y + det.bbox.height / 2);
                float distToCenter = std::sqrt(std::pow(center.x - frameCenter.x, 2) + 
                                              std::pow(center.y - frameCenter.y, 2));
                
                if (distToCenter < minDistanceToCenter) {
                    minDistanceToCenter = distToCenter;
                    bestTarget = det;
                }
            }
            
            cv::Point center(
                bestTarget.bbox.x + bestTarget.bbox.width / 2,
                bestTarget.bbox.y + bestTarget.bbox.height / 2
            );
            
            cv::Point offset = center - frameCenter;
            
            // Verifica se está fora da deadzone
            if (std::abs(offset.x) > CENTER_DEADZONE || std::abs(offset.y) > CENTER_DEADZONE) {
                
                // Verifica estabilidade do alvo
                if (lastTargetCenter.x != -1) {
                    cv::Point movement = center - lastTargetCenter;
                    float movementMagnitude = std::sqrt(movement.x * movement.x + movement.y * movement.y);
                    
                    if (movementMagnitude < 30) {
                        stableFrames++;
                    } else {
                        stableFrames = 0;
                    }
                } else {
                    stableFrames = 0;
                }
                
                // Envia comando PTZ apenas se alvo estável e respeitando intervalo
                auto now = std::chrono::steady_clock::now();
                auto timeSinceLastCommand = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - lastPtzCommand).count();
                
                if (stableFrames >= STABILITY_THRESHOLD && timeSinceLastCommand >= PTZ_COMMAND_INTERVAL_MS) {
                    // Movimento proporcional suavizado com limite
                    int panSpeed = std::clamp(offset.x / 40, -8, 8);
                    int tiltSpeed = std::clamp(-offset.y / 40, -8, 8);
                    
                    // Apenas envia se movimento significativo
                    if (std::abs(panSpeed) >= 2 || std::abs(tiltSpeed) >= 2) {
                        emit ptzAdjustmentNeeded(panSpeed, tiltSpeed);
                        lastPtzCommand = now;
                        stableFrames = 0;
                    }
                }
                
                lastTargetCenter = center;
            } else {
                // Dentro da deadzone - resetar tracking
                stableFrames = 0;
                lastTargetCenter = cv::Point(-1, -1);
            }
        } else {
            // Sem detecções - resetar estado
            stableFrames = 0;
            lastTargetCenter = cv::Point(-1, -1);
        }
        
        emit frameReady(matToQImage(frame));
        emit detectionCount(detections.size());
        
        frameCounter++;
        auto now = std::chrono::steady_clock::now();
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