#pragma once

#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QWidget>

// VideoPlayer 类：用于循环播放本地视频文件的控件
class VideoPlayer : public QWidget {
    Q_OBJECT
public:
    // 构造函数，path为视频文件路径，parent为父窗口
    explicit VideoPlayer(const QString& path, QWidget* parent = nullptr);

    // 循环播放视频
    void playLoop();
    // 暂停播放
    void pause();
    // 停止播放
    void stop();

private:
    QMediaPlayer* player_; // 媒体播放器对象
    QMediaPlaylist* playlist_; // 播放列表（用于循环播放）
    QVideoWidget* videoWidget_; // 视频显示控件
    int frameCount_; // 统计帧数（用于简单帧计数）
};
