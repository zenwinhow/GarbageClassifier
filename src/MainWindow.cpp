#include "CocoMap.h"
#include "MainWindow.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QHBoxLayout>
#include <QPainter>
#include <QVBoxLayout>

// MainWindow 构造函数，初始化主界面和各控件
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , threshold_(0.5f) // 初始置信度阈值为0.5
    , showCameraFps_(true) // 默认显示摄像头FPS
    , cameraFrameCount_(0) // FPS统计帧数
    , lastCameraFpsUpdateMs_(QDateTime::currentMSecsSinceEpoch()) // 上次FPS更新时间
    , currentCameraFps_(0.0f) // 当前FPS
{
    // --- 窗口初始大小 ---
    resize(1280, 960);

    // --- 置信度滑块 & 值显示 ---
    thresholdSlider_ = new QSlider(Qt::Horizontal); // 水平滑块
    thresholdSlider_->setRange(1, 100); // 范围1~100
    thresholdSlider_->setValue(int(threshold_ * 100)); // 初始值
    thresholdValueLabel_ = new QLabel(QString("%1%").arg(int(threshold_ * 100))); // 显示百分比

    // --- Start/Stop 按钮 ---
    startBtn_ = new QPushButton("Start Detect"); // 开始检测按钮
    stopBtn_ = new QPushButton("Stop Detect");   // 停止检测按钮

    // --- FPS控制按钮 & FPS标签 ---
    cameraFpsBtn_ = new QPushButton("Hide Camera FPS"); // 控制FPS显示的按钮

    // --- 视频播放页 ---
    QString videoPath = QDir::current().absoluteFilePath("../resources/loop.mp4"); // 视频路径
    videoPlayer_ = new VideoPlayer(videoPath); // 视频播放器控件
    videoPage_ = new QWidget; // 视频页面
    QVBoxLayout* v1 = new QVBoxLayout(videoPage_); // 垂直布局
    v1->setContentsMargins(0, 0, 0, 0);
    v1->addWidget(videoPlayer_); // 添加播放器到页面

    // --- 检测展示页 ---
    detPage_ = new QLabel; // 用于显示检测结果的标签
    detPage_->setAlignment(Qt::AlignCenter); // 居中
    detPage_->setStyleSheet("background-color: black;"); // 黑色背景

    // --- 堆栈布局 ---
    stacked_ = new QStackedWidget; // 堆叠窗口
    stacked_->addWidget(videoPage_); // idx=0，视频页
    stacked_->addWidget(detPage_);   // idx=1，检测页

    // --- 控件区 ---
    QHBoxLayout* ctrl = new QHBoxLayout; // 控件水平布局
    ctrl->addWidget(new QLabel("Confidence:")); // 置信度标签
    ctrl->addWidget(thresholdSlider_); // 置信度滑块
    ctrl->addWidget(thresholdValueLabel_); // 置信度值
    ctrl->addStretch(); // 拉伸填充
    ctrl->addWidget(cameraFpsBtn_); // FPS显示按钮
    ctrl->addWidget(startBtn_); // 开始按钮
    ctrl->addWidget(stopBtn_);  // 停止按钮

    // --- 主布局 ---
    QWidget* central = new QWidget; // 主窗口中心部件
    QVBoxLayout* mainL = new QVBoxLayout(central); // 主垂直布局
    mainL->setContentsMargins(0, 0, 0, 0);
    mainL->addWidget(stacked_); // 添加堆叠窗口
    mainL->addLayout(ctrl);     // 添加控制区
    setCentralWidget(central);  // 设置中心部件

    // --- Detector 设置 & 连接 ---
    detector_ = new Detector("../resources/yolov5s.onnx", threshold_); // 初始化检测器
    connect(detector_, &Detector::detection,
        this, &MainWindow::onDetection,
        Qt::QueuedConnection); // 检测结果信号连接到槽
    connect(thresholdSlider_, &QSlider::valueChanged,
        this, &MainWindow::onThresholdChanged); // 滑块变化信号连接

    // --- 定时器 ---
    noDetTimer_ = new QTimer(this); // 检测超时定时器
    noDetTimer_->setInterval(2000); // 2秒无检测回到视频页
    connect(noDetTimer_, &QTimer::timeout,
        this, &MainWindow::onNoDetectionTimeout); // 定时器超时信号连接

    // --- 按钮事件 ---
    connect(startBtn_, &QPushButton::clicked,
        this, &MainWindow::onStartClicked); // 开始按钮点击
    connect(stopBtn_, &QPushButton::clicked,
        this, &MainWindow::onStopClicked);  // 停止按钮点击

    connect(cameraFpsBtn_, &QPushButton::clicked, this, &MainWindow::toggleCameraFpsDisplay); // FPS显示按钮点击

    // 自动开始检测
    onStartClicked();
}

// 析构函数，安全停止检测线程
MainWindow::~MainWindow()
{
    detector_->stop();
    detector_->wait();
}

// 开始检测按钮槽函数
void MainWindow::onStartClicked()
{
    qDebug() << "[MainWindow] Start Detect clicked";
    videoPlayer_->playLoop(); // 播放循环视频
    detector_->start();       // 启动检测线程
}

// 停止检测按钮槽函数
void MainWindow::onStopClicked()
{
    qDebug() << "[MainWindow] Stop Detect clicked";
    detector_->stop(); // 停止检测
    stacked_->setCurrentIndex(0); // 切回视频页
    videoPlayer_->playLoop();     // 播放循环视频
}

// 置信度滑块变化槽函数
void MainWindow::onThresholdChanged(int value)
{
    threshold_ = value / 100.0f; // 转换为0~1
    qDebug() << "[MainWindow] Confidence threshold changed to:" << threshold_;
    thresholdValueLabel_->setText(QString("%1%").arg(value)); // 更新显示
    detector_->setThreshold(threshold_); // 设置检测器阈值
}

// 切换摄像头FPS显示
void MainWindow::toggleCameraFpsDisplay()
{
    showCameraFps_ = !showCameraFps_; // 取反
    cameraFpsBtn_->setText(showCameraFps_ ? "Hide Camera FPS" : "Show Camera FPS"); // 更新按钮文本
}

// 检测结果槽函数
void MainWindow::onDetection(const QImage& frame,
    const std::vector<cv::Rect>& boxes,
    const std::vector<float>& confs,
    const std::vector<std::string>& labels)
{
    // FPS统计
    cameraFrameCount_++;
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (now - lastCameraFpsUpdateMs_ >= 1000) {
        currentCameraFps_ = cameraFrameCount_ * 1000.0f / (now - lastCameraFpsUpdateMs_);
        lastCameraFpsUpdateMs_ = now;
        cameraFrameCount_ = 0;
    }

    // 找第一个非 continue 的垃圾分类
    int idx = -1;
    std::string garbageType;
    for (size_t i = 0; i < labels.size(); ++i) {
        garbageType = CocoMap::getGarbageType(labels[i]);
        if (garbageType != "continue") {
            idx = int(i);
            break;
        }
    }
    if (idx < 0) {
        // 没有检测到有效目标，重启定时器
        noDetTimer_->stop();
        noDetTimer_->start();
        return;
    }

    videoPlayer_->pause(); // 暂停视频
    noDetTimer_->stop();   // 停止定时器

    QImage img = frame.copy(); // 拷贝检测帧
    QPainter painter(&img);    // 创建画笔
    painter.setPen(QPen(Qt::red, 3)); // 红色画笔
    QFont font = painter.font();
    font.setPointSize(24); // 设置字体大小
    painter.setFont(font);

    // 绘制检测框
    QRect r(boxes[idx].x, boxes[idx].y,
        boxes[idx].width, boxes[idx].height);
    painter.drawRect(r);
    QString info = QString("this is %1").arg(QString::fromStdString(garbageType));
    painter.drawText(r.topLeft() + QPoint(0, 30), info); // 绘制类别文本

    // 摄像头FPS显示
    if (showCameraFps_) {
        painter.setPen(Qt::yellow);
        painter.setFont(QFont("Arial", 20, QFont::Bold));
        painter.drawText(10, 40, QString("Camera FPS: %1").arg(QString::number(currentCameraFps_, 'f', 1)));
        painter.setPen(QPen(Qt::red, 3)); // 恢复红色画笔
    }

    painter.end(); // 结束绘制
    detPage_->setPixmap(QPixmap::fromImage(img)); // 显示检测结果
    stacked_->setCurrentIndex(1); // 切换到检测页
    noDetTimer_->start(); // 启动定时器，超时后回到视频页
}

// 检测超时槽函数，回到视频页
void MainWindow::onNoDetectionTimeout()
{
    qDebug() << "[MainWindow] onNoDetectionTimeout: resume video page";
    stacked_->setCurrentIndex(0); // 切回视频页
    videoPlayer_->playLoop();     // 播放循环视频
}
