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
    
    // YOLOv8 output format: [1, 84, 8400]
    // 84 = 4 (bbox) + 80 (classes)
    for (const auto& output : outputs) {
        float* data = (float*)output.data;
        
        // Transpor: [1, 84, 8400] -> [8400, 84]
        int dimensions = output.size[2]; // 84
        int rows = output.size[1];       // 8400
        
        cv::Mat transposed(rows, dimensions, CV_32F);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < dimensions; j++) {
                transposed.at<float>(i, j) = data[j * rows + i];
            }
        }
        
        for (int i = 0; i < transposed.rows; i++) {
            float* row = transposed.ptr<float>(i);
            
            // Bbox: x, y, w, h
            float x = row[0];
            float y = row[1];
            float w = row[2];
            float h = row[3];
            
            // Classes: row[4] até row[83]
            // Para pessoa (class 0), pegamos row[4]
            float confidence = row[4];
            
            if (confidence >= confidenceThreshold) {
                // Converte coordenadas normalizadas
                int left = (int)((x - w/2) * frameSize.width);
                int top = (int)((y - h/2) * frameSize.height);
                int width = (int)(w * frameSize.width);
                int height = (int)(h * frameSize.height);
                
                boxes.push_back(cv::Rect(left, top, width, height));
                confidences.push_back(confidence);
                classIds.push_back(0); // pessoa
            }
        }
    }
    
    // Non-Maximum Suppression
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
