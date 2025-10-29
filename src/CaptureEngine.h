#ifndef CAPTUREENGINE_H
#define CAPTUREENGINE_H

#include <QObject>
#include <QThread>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <atomic>
#include <memory>
#include "YOLODetector.h"

class CaptureEngine : public QObject {
    Q_OBJECT

public:
    CaptureEngine(const std::string& source, int fps, float threshold);
    ~CaptureEngine();
    
    void start();
    void stop();
    void setConfidenceThreshold(float threshold);
    void setAutoTracking(bool enabled);

signals:
    void frameReady(const QImage& frame);
    void fpsUpdated(double fps);
    void detectionCount(int count);
    void ptzAdjustmentNeeded(int pan, int tilt);

private:
    void captureLoop();
    QImage matToQImage(const cv::Mat& mat);
    void drawDetections(cv::Mat& frame, const std::vector<Detection>& dets);
    
    std::string videoSource;
    int targetFPS;
    float confThreshold;
    std::atomic<bool> running;
    std::atomic<bool> autoTracking;
    QThread* captureThread;
    std::unique_ptr<YOLODetector> detector;
};

#endif