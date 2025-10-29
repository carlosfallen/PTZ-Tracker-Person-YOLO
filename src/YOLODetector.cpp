#include "YOLODetector.h"
#include <algorithm>

YOLODetector::YOLODetector(const std::string& modelPath, float confThreshold)
    : confidenceThreshold(confThreshold), nmsThreshold(0.45f)
{
    // Carrega modelo YOLO ONNX
    net = cv::dnn::readNetFromONNX(modelPath);
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    
    // Classes COCO (apenas pessoa = 0)
    classNames = {"person"};
}

void YOLODetector::setConfidenceThreshold(float threshold) {
    confidenceThreshold = threshold;
}

std::vector<Detection> YOLODetector::detect(const cv::Mat& frame) {
    // Preprocessamento
    cv::Mat blob;
    cv::dnn::blobFromImage(frame, blob, 1.0/255.0, 
        cv::Size(416, 416), cv::Scalar(), true, false);
    
    net.setInput(blob);
    
    // Forward pass
    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());
    
    // Parse detecções
    return parseDetections(outputs, frame.size());
}

std::vector<Detection> YOLODetector::parseDetections(
    const std::vector<cv::Mat>& outputs,
    const cv::Size& frameSize)
{
    std::vector<Detection> detections;
    std::vector<cv::Rect> boxes;
    std::vector<float> confidences;
    std::vector<int> classIds;
    
    float x_factor = frameSize.width / 416.0f;
    float y_factor = frameSize.height / 416.0f;
    
    for (const auto& output : outputs) {
        float* data = (float*)output.data;
        
        int dimensions = output.size[1];
        int rows = output.size[2];
        
        for (int i = 0; i < rows; i++) {
            int index = i;
            float x = data[index];
            float y = data[index + rows];
            float w = data[index + 2 * rows];
            float h = data[index + 3 * rows];
            float confidence = data[index + 4 * rows];
            
            if (confidence >= confidenceThreshold) {
                int left = (int)((x - w/2) * x_factor);
                int top = (int)((y - h/2) * y_factor);
                int width = (int)(w * x_factor);
                int height = (int)(h * y_factor);
                
                boxes.push_back(cv::Rect(left, top, width, height));
                confidences.push_back(confidence);
                classIds.push_back(0);
            }
        }
    }
    
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confidenceThreshold, nmsThreshold, indices);
    
    for (int idx : indices) {
        Detection det;
        det.bbox = boxes[idx];
        det.confidence = confidences[idx];
        det.classId = classIds[idx];
        det.label = "Person";
        detections.push_back(det);
    }
    
    return detections;
}