#include "CocoMap.h"
#include "Detector.h"
#include <QDebug>
#include <fstream>
#include <iostream>

// 构造函数：加载模型和类别名文件
Detector::Detector(const std::string& modelPath, float thresh)
    : threshold_(thresh)
    , running_(false)
{
    // 输出尝试加载模型的信息
    qDebug() << "[Detector] Trying to load model from:" << QString::fromStdString(modelPath);
    try {
        // 加载ONNX模型
        net_ = cv::dnn::readNet(modelPath);
        // 设置推理后端为CUDA
        net_.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        // 设置推理目标为CUDA FP16
        net_.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA_FP16);
        qDebug() << "[Detector] Model loaded.";
    } catch (cv::Exception& e) {
        // 捕获加载模型异常
        qDebug() << "[Detector] Model load failed:" << e.what();
    }

    // 获取模型所在目录
    std::string baseDir = modelPath.substr(0, modelPath.find_last_of("/\\"));
    // 构造coco.names路径
    std::string namesFile = baseDir.empty() ? "coco.names" : (baseDir + "/coco.names");
    qDebug() << "[Detector] Loading coco.names from:" << QString::fromStdString(namesFile);
    std::ifstream ifs(namesFile);
    if (!ifs.is_open()) {
        // 打开失败
        qDebug() << "[Detector] Failed to open coco.names";
    } else {
        // 逐行读取类别名
        std::string line;
        while (std::getline(ifs, line)) {
            if (!line.empty())
                classNames_.push_back(line);
        }
        qDebug() << "[Detector] Loaded class count:" << classNames_.size();
    }
}

// 析构函数：停止线程并等待结束
Detector::~Detector()
{
    stop();
    wait();
}

// 设置置信度阈值
void Detector::setThreshold(float t)
{
    threshold_ = t;
}

// 停止检测线程
void Detector::stop()
{
    running_ = false;
}

// 检测主循环（线程函数）
void Detector::run()
{
    running_ = true;
    qDebug() << "[Detector] Thread started.";
    // 打开摄像头
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        qDebug() << "[Detector] Cannot open camera!";
        return;
    } else {
        qDebug() << "[Detector] Camera opened.";
    }

    cv::Mat frame;
    int frameId = 0;
    const float INPUT_SIZE = 640.0f; // yolov5s.onnx 输入尺寸

    // 主循环
    while (running_) {
        // 读取一帧
        if (!cap.read(frame) || frame.empty()) {
            qDebug() << "[Detector] Empty frame or read fail.";
            continue;
        }
        ++frameId;
        // 计算输入输出缩放比例
        float xScale = float(frame.cols) / INPUT_SIZE;
        float yScale = float(frame.rows) / INPUT_SIZE;
        qDebug() << "[Detector] Frame" << frameId << "captured:" << frame.cols << "x" << frame.rows << "scale:" << xScale << yScale;

        // 图像预处理：归一化、缩放、通道变换
        cv::Mat blob;
        try {
            cv::dnn::blobFromImage(frame, blob, 1 / 255.0, cv::Size(INPUT_SIZE, INPUT_SIZE), cv::Scalar(), true, false);
            net_.setInput(blob);
        } catch (cv::Exception& e) {
            qDebug() << "[Detector] blobFromImage/setInput error:" << e.what();
            continue;
        }

        // 前向推理
        std::vector<cv::Mat> outputs;
        try {
            net_.forward(outputs, net_.getUnconnectedOutLayersNames());
            qDebug() << "[Detector] Forward finished. Outputs size:" << outputs.size();
        } catch (cv::Exception& e) {
            qDebug() << "[Detector] forward() error:" << e.what();
            continue;
        }

        // 检查输出
        if (outputs.empty()) {
            qDebug() << "[Detector] No outputs.";
            continue;
        }

        // 解析输出张量
        cv::Mat& out = outputs[0];
        int numProposals = out.size[1]; // 检测框数量
        int dims = out.size[2];         // 每个检测框的属性数
        float* data = (float*)out.data; // 指向输出数据

        std::vector<cv::Rect> boxes;         // 检测框
        std::vector<float> confs;            // 置信度
        std::vector<std::string> labels;     // 类别标签
        int validCount = 0;                  // 有效检测数

        // 遍历所有检测框
        for (int i = 0; i < numProposals; ++i) {
            float conf = data[4]; // 置信度
            if (conf >= threshold_) {
                // 将检测框坐标从输入尺寸映射回原图尺寸
                float cx = data[0] * xScale;
                float cy = data[1] * yScale;
                float w = data[2] * xScale;
                float h = data[3] * yScale;
                int left = int(cx - w / 2);
                int top = int(cy - h / 2);
                int width = int(w);
                int height = int(h);

                // 边界修正，防止越界
                left = std::max(0, left);
                top = std::max(0, top);
                width = std::min(width, frame.cols - left);
                height = std::min(height, frame.rows - top);

                boxes.emplace_back(left, top, width, height);
                confs.push_back(conf);

                // 解析类别分数，找到最大类别
                float maxCls = 0;
                int clsId = -1;
                for (int c = 5; c < dims; ++c) {
                    if (data[c] > maxCls) {
                        maxCls = data[c];
                        clsId = c - 5;
                    }
                }
                // 获取类别名
                std::string cocoName = (clsId >= 0 && clsId < (int)classNames_.size())
                    ? classNames_[clsId]
                    : std::to_string(clsId);
                labels.push_back(cocoName);

                // 输出检测信息
                qDebug() << "[Detector] Detected:"
                         << QString::fromStdString(cocoName)
                         << "conf:" << conf
                         << "box:" << left << top << width << height;
                ++validCount;
            }
            // 指针移动到下一个检测框
            data += dims;
        }

        qDebug() << "[Detector] Detections this frame:" << validCount;

        // 若有检测结果，转换为QImage并发射信号
        if (!boxes.empty()) {
            cv::Mat rgb;
            cv::cvtColor(frame, rgb, cv::COLOR_BGR2RGB);
            QImage img((uchar*)rgb.data, rgb.cols, rgb.rows, int(rgb.step), QImage::Format_RGB888);

            emit detection(img, boxes, confs, labels);
        }

        // 休眠33ms，约30帧每秒
        QThread::msleep(33);
    }
}
