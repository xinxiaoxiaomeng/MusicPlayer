#include "audioengine.h"
#include <QDebug>
#include <QMessageBox>

AudioEngine::AudioEngine(QObject *parent):
    QObject(parent),
    mediaPlayer(new QMediaPlayer(parent)),
    audioOutput(new QAudioOutput(parent))
{
    mediaPlayer->setAudioOutput(audioOutput);
    setupConnections();
}

AudioEngine::~AudioEngine()
{

}

// 返回当前播放曲目的时长
qint64 AudioEngine::duration() const
{
    if (mediaPlayer)
        return mediaPlayer->duration();
    return 0;
}

// 暂停播放
void AudioEngine::pause()
{
    if(mediaPlayer->playbackState() == QMediaPlayer::PlayingState)
    {
        mediaPlayer->pause();
        emit playbackStateChanged(QMediaPlayer::PausedState);
    }
}

// 播放曲目
void AudioEngine::play()
{
    if (mediaPlayer->playbackState() == QMediaPlayer::PausedState)
    {
       mediaPlayer->setPosition(playPosition);
    }
    else
    {
       if (!currentTrack || currentTrack->filePath.isEmpty())
       {
           qDebug() << "2025/12/29 test";
           QMessageBox::warning(0, "警告", "未找到播放文件!");
           mediaPlayer->setPosition(0);
           emit positionChanged(0);
           //emit errorOccurred("曲目文件路径为空");
           return;
       }

       mediaPlayer->setSource(QUrl(currentTrack->filePath));
    }
    mediaPlayer->play();
    emit playbackStateChanged(QMediaPlayer::PlayingState);
    emit currentTrackChanged(currentTrack);
}

//void AudioEngine::playAfterPause()
//{
//    if(mediaPlayer->playbackState() == QMediaPlayer::PausedState)
//    {
//        mediaPlayer->play();
//    }
//    //mediaPlayer->setPosition(playPosition);
//}

//void AudioEngine::playRestart()
//{
//    if (!currentTrack || currentTrack->filePath.isEmpty())
//    {
//        emit errorOccurred("曲目文件路径为空");
//        return;
//    }

//    qDebug() << currentTrack->filePath;
//    mediaPlayer->setSource(QUrl(currentTrack->filePath));
//    mediaPlayer->play();
//}

// 播放位置
qint64 AudioEngine::position() const
{
    if (mediaPlayer)
    {
        return mediaPlayer->position();
    }

    return 0;
}

// 设置播放位置
void AudioEngine::setPosition(qint64 position)
{
    if (mediaPlayer)
    {
        playPosition = position;
        mediaPlayer->setPosition(position);
    }
    else
    {
        emit errorOccurred("更改播放位置出错");
    }
}

// 设置进行播放的曲目
void AudioEngine::setTrack(MusicTrack *track)
{
    currentTrack = track;
    emit currentTrackChanged(currentTrack);
}

void AudioEngine::setupConnections()
{
    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, [=](qint64 position){
        playPosition = position;
        count++;
        if (count >= 10)
        {
            count = 0;
            emit positionChanged(position);
        }
    });

//    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onMediaStatusChanged);

//    connect(mediaPlayer, &QMediaPlayer::playbackStateChanged, this, [=](QMediaPlayer::PlaybackState newState){
//        emit playbackStateChanged(newState);
//    });

//    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, [=](qint64 duration){
//        emit durationChanged(duration);
//    });
}

// 设置播放音量
void AudioEngine::setVolume(float volume)
{
    if (audioOutput)
        audioOutput->setVolume(volume);
}

// 停止播放
void AudioEngine::stop()
{
    if (mediaPlayer)
    {
        mediaPlayer->stop();
        emit playbackStateChanged(QMediaPlayer::StoppedState);
    }
}

// 返回播放音量
qint64 AudioEngine::volume() const
{
    if (audioOutput)
        return audioOutput->volume();
    return 0;
}
