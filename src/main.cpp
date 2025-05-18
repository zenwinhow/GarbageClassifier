#include "MainWindow.h"
#include <QApplication>
#include <QMetaType>
#include <opencv2/core.hpp>
#include <vector>

// 声明 std::vector<cv::Rect> 为 Qt 的元类型，便于在信号槽中传递
Q_DECLARE_METATYPE(std::vector<cv::Rect>)
// 声明 std::vector<float> 为 Qt 的元类型
Q_DECLARE_METATYPE(std::vector<float>)
// 声明 std::vector<std::string> 为 Qt 的元类型
Q_DECLARE_METATYPE(std::vector<std::string>)

int main(int argc, char* argv[])
{
    // 注册 std::vector<cv::Rect> 类型到 Qt 元对象系统
    qRegisterMetaType<std::vector<cv::Rect>>("std::vector<cv::Rect>");
    // 注册 std::vector<float> 类型到 Qt 元对象系统
    qRegisterMetaType<std::vector<float>>("std::vector<float>");
    // 注册 std::vector<std::string> 类型到 Qt 元对象系统
    qRegisterMetaType<std::vector<std::string>>("std::vector<std::string>");

    // 创建 Qt 应用程序对象，管理应用程序的控制流和主要设置
    QApplication app(argc, argv);

    // 创建主窗口对象
    MainWindow w;
    // 显示主窗口
    w.show();

    // 进入 Qt 事件循环，等待用户操作
    return app.exec();
}
