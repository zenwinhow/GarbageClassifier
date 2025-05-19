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
            /**
             * 参数解释：
             * frame: 原始图像，类型为 cv::Mat。
             * blob: 输出参数，生成的4维张量（NCHW：batch, channels, height, width）。
             * 1 / 255.0: 缩放因子，把像素值从 [0, 255] 缩放到 [0, 1]（神经网络更易处理）。
             * cv::Size(INPUT_SIZE, INPUT_SIZE): 目标尺寸，例如 YOLOv5 通常用 640x640，表示将图像缩放到指定大小。
             * cv::Scalar(): 均值减除值（均值归一化用的），为空则不做减均值操作。
             * true: swapRB，表示是否交换 R 和 B 通道。因为 OpenCV 默认是 BGR，很多模型需要 RGB，因此这里设为 true。
             * false: crop，是否在缩放图像时裁剪，设为 false 表示不裁剪，只缩放。
             * 
             */
            cv::dnn::blobFromImage(frame, blob, 1 / 255.0, cv::Size(INPUT_SIZE, INPUT_SIZE), cv::Scalar(), true, false);
            //将预处理后的图像张量 blob 作为输入喂给神经网络 net_。
            net_.setInput(blob);
        } catch (cv::Exception& e) {
            qDebug() << "[Detector] blobFromImage/setInput error:" << e.what();
            continue;
        }

        // 前向推理，获取模型输出
        /**
         * net_.forward(outputs, net_.getUnconnectedOutLayersNames());
         * 
         * 作用：
         *   - 执行神经网络的前向传播（推理），将输入数据传递给网络，得到输出结果。
         *   - outputs: 用于存放网络的输出张量。
         *   - net_.getUnconnectedOutLayersNames(): 获取所有未连接输出层的名称（即模型的最终输出层），
         *     这样可以确保输出的是模型的最终推理结果（如YOLO的检测结果）。
         * 相关函数：
         *   - cv::dnn::Net::forward(std::vector<cv::Mat>& outputBlobs, const std::vector<std::string>& outBlobNames):
         *       - outputBlobs: 用于接收输出层的结果（可以有多个输出）。
         *       - outBlobNames: 指定要获取的输出层名称列表。
         *   - cv::dnn::Net::getUnconnectedOutLayersNames():
         *       - 返回所有未连接输出层（即最终输出层）的名称列表，常用于检测模型（如YOLO、SSD等）。
         * 
         *   该行代码将模型的推理输出存储到outputs中，供后续解析检测结果使用。
         */
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
        int dims = out.size[2]; // 每个检测框的属性数
        float* data = (float*)out.data; // 指向输出数据

        std::vector<cv::Rect> boxes; // 检测框
        std::vector<float> confs; // 置信度
        std::vector<std::string> labels; // 类别标签
        int validCount = 0; // 有效检测数

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
