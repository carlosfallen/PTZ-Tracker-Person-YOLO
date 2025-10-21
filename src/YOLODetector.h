#ifndef YOLODETECTOR_H
#define YOLODETECTOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <vector>
#include <string>

struct Detection {
    cv::Rect bbox;
    float confidence;
    int classId;
    std::string label;
};

class YOLODetector {
public:
    YOLODetector(const std::string& modelPath, float confThreshold = 0.5f);
    std::vector<Detection> detect(const cv::Mat& frame);
    void setConfidenceThreshold(float threshold);
    
private:
    cv::dnn::Net net;
    float confidenceThreshold;
    float nmsThreshold;
    std::vector<std::string> classNames;
    
    std::vector<Detection> parseDetections(
        const std::vector<cv::Mat>& outputs,
        const cv::Size& frameSize
    );
};

#endif
