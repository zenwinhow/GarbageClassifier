#include "VideoPlayer.h"
#include <QUrl>
#include <QVBoxLayout>

// 构造函数：初始化播放器、播放列表和视频控件
VideoPlayer::VideoPlayer(const QString& path, QWidget* parent)
    : QWidget(parent)
    , frameCount_(0)
{
    player_ = new QMediaPlayer(this); // 创建媒体播放器
    playlist_ = new QMediaPlaylist(this); // 创建播放列表
    videoWidget_ = new QVideoWidget(this); // 创建视频显示控件

    // 添加本地视频到播放列表
    playlist_->addMedia(QUrl::fromLocalFile(path));
    playlist_->setCurrentIndex(0);
    playlist_->setPlaybackMode(QMediaPlaylist::Loop); // 设置循环播放

    player_->setPlaylist(playlist_); // 设置播放器使用播放列表
    player_->setVideoOutput(videoWidget_); // 设置视频输出控件

    // 设置布局，将视频控件填充整个窗口
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(videoWidget_);
    setLayout(layout);

    // 每次 position 变化都认为是新帧（近似帧计数）
    connect(player_, &QMediaPlayer::positionChanged, [this](qint64) {
        ++frameCount_;
    });
}

// 循环播放视频
void VideoPlayer::playLoop()
{
    player_->play();
}

// 暂停播放
void VideoPlayer::pause()
{
    player_->pause();
}

// 停止播放
void VideoPlayer::stop()
{
    player_->stop();
}
