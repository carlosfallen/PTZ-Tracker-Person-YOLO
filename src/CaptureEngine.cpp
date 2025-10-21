#include "CaptureEngine.h"
#include <chrono>
#include <thread>

CaptureEngine::CaptureEngine(const std::string& source, int fps, float threshold)
    : videoSource(source), targetFPS(fps), confThreshold(threshold), 
      running(false), captureThread(nullptr)
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
    
    // Tenta abrir como device ID ou URL
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
    
    while (running) {
        auto startTime = std::chrono::steady_clock::now();
        
        cv::Mat frame;
        if (!cap.read(frame)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        // Detecção YOLO
        auto detections = detector->detect(frame);
        
        // Desenha detecções
        drawDetections(frame, detections);
        
        // Emite frame
        emit frameReady(matToQImage(frame));
        emit detectionCount(detections.size());
        
        // Calcula FPS
        frameCounter++;
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(now - lastFpsTime).count();
        
        if (elapsed >= 1.0) {
            double fps = frameCounter / elapsed;
            emit fpsUpdated(fps);
            frameCounter = 0;
            lastFpsTime = now;
        }
        
        // Controle de FPS
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
        // Bbox verde
        cv::rectangle(frame, det.bbox, cv::Scalar(0, 255, 0), 2);
        
        // Label com confiança
        std::string label = det.label + " " + 
                           std::to_string((int)(det.confidence * 100)) + "%";
        
        int baseline;
        cv::Size textSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 
                                            0.6, 2, &baseline);
        
        // Background do texto
        cv::rectangle(frame, 
            cv::Point(det.bbox.x, det.bbox.y - textSize.height - 5),
            cv::Point(det.bbox.x + textSize.width, det.bbox.y),
            cv::Scalar(0, 255, 0), -1);
        
        // Texto
        cv::putText(frame, label, 
            cv::Point(det.bbox.x, det.bbox.y - 5),
            cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 0), 2);
        
        // Centro
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
