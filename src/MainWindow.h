#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Detector.h"
#include "VideoPlayer.h"
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QStackedWidget>
#include <QTimer>

// MainWindow类，主界面窗口，负责UI和检测逻辑
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    // 构造函数，parent为父窗口指针
    MainWindow(QWidget* parent = nullptr);
    // 析构函数
    ~MainWindow();

private slots:
    // 检测结果回调槽函数
    void onDetection(const QImage& frame,
        const std::vector<cv::Rect>& boxes,
        const std::vector<float>& confs,
        const std::vector<std::string>& labels);
    // 检测超时槽函数
    void onNoDetectionTimeout();
    // 置信度滑块变化槽函数
    void onThresholdChanged(int value);
    // 开始检测按钮槽函数
    void onStartClicked();
    // 停止检测按钮槽函数
    void onStopClicked();
    // 切换摄像头FPS显示槽函数
    void toggleCameraFpsDisplay();

private:
    VideoPlayer* videoPlayer_;      // 视频播放器控件
    Detector* detector_;            // 检测器对象
    QSlider* thresholdSlider_;      // 置信度阈值滑块
    QLabel* thresholdValueLabel_;   // 显示当前阈值的标签
    QPushButton* startBtn_;         // 开始检测按钮
    QPushButton* stopBtn_;          // 停止检测按钮
    QStackedWidget* stacked_;       // 堆叠窗口（视频页/检测页）
    QWidget* videoPage_;            // 视频播放页面
    QLabel* detPage_;               // 检测结果展示页面
    QTimer* noDetTimer_;            // 检测超时定时器
    float threshold_;               // 当前置信度阈值

    // FPS相关
    QPushButton* cameraFpsBtn_;     // 控制FPS显示的按钮
    bool showCameraFps_;            // 是否显示摄像头FPS
    // 摄像头FPS统计
    int cameraFrameCount_;          // 统计帧数
    qint64 lastCameraFpsUpdateMs_;  // 上次FPS更新时间
    float currentCameraFps_;        // 当前计算出的FPS
};

#endif // MAINWINDOW_H
