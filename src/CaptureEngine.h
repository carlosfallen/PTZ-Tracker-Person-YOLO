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
    void setManualTarget(float x, float y);

signals:
    void frameReady(const QImage& frame);
    void fpsUpdated(double fps);
    void detectionCount(int count);
    void ptzAdjustmentNeeded(int pan, int tilt);

private:
    void captureLoop();
    void processPTZControl(const cv::Mat& frame, const std::vector<Detection>& detections, float dt);
    Detection selectBestTarget(const cv::Mat& frame, const std::vector<Detection>& detections);
    float applyDeadband(float err);
    float applyNonLinearity(float raw_speed);
    void resetPIDState();
    QImage matToQImage(const cv::Mat& mat);
    void drawDetections(cv::Mat& frame, const std::vector<Detection>& dets);
    
    std::string videoSource;
    int targetFPS;
    float confThreshold;
    std::atomic<bool> running;
    std::atomic<bool> autoTracking;
    QThread* captureThread;
    std::unique_ptr<YOLODetector> detector;
    
    // PID control state
    float integral_x, integral_y;
    float prev_err_x, prev_err_y;
    float filtered_derivative_x, filtered_derivative_y;
    float prev_ptz_speed_x, prev_ptz_speed_y;
    float last_nx, last_ny, last_nz;
    int lost_frames;
    
    // Manual mode
    bool manual_mode;
    float manual_target_x, manual_target_y;
    
    // Control parameters
    float conf_min, nz_min, deadband;
    float Kp_x, Ki_x, Kd_x;
    float Kp_y, Ki_y, Kd_y;
    float I_max, gamma;
    float near_edge_threshold, approach_limit, v_thresh;
    float slew_rate, lpf_tau, stop_threshold;
    int lost_max_frames;
};

#endif