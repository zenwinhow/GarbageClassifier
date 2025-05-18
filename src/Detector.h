#ifndef DETECTOR_H
#define DETECTOR_H

#include <QImage>
#include <QThread>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

// Detector 类：基于OpenCV DNN和QThread实现的目标检测器
class Detector : public QThread {
    Q_OBJECT
public:
    // 构造函数，加载模型和类别名，设置置信度阈值
    Detector(const std::string& modelPath, float thresh = 0.5f);
    // 析构函数，安全停止线程
    ~Detector();

    // 设置检测置信度阈值
    void setThreshold(float t);
    // 停止检测线程
    void stop();

signals:
    // 检测信号：每帧检测完成后发射，包含检测到的图片、框、置信度和类别标签
    void detection(const QImage& frame,
        const std::vector<cv::Rect>& boxes,
        const std::vector<float>& confs,
        const std::vector<std::string>& labels);

protected:
    // QThread的主循环，执行摄像头采集和目标检测
    void run() override;

private:
    cv::dnn::Net net_;                   // OpenCV DNN网络对象
    float threshold_;                    // 检测置信度阈值
    bool running_;                       // 线程运行标志
    std::vector<std::string> classNames_;// coco.names类别名列表
};

#endif // DETECTOR_H
