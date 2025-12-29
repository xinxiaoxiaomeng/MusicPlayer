#include "playbackcontroller.h"
#include <QDebug>

PlaybackController::PlaybackController(QObject *parent):
    QObject(parent),
    audioEngine(new AudioEngine(this)),
    playMode(Sequential),
    playbackState(Stopped),
    currentIndex(-1)
{
    setupConnections();
}

PlaybackController::~PlaybackController()
{

}

MusicTrack PlaybackController::currentTrack() const
{
    return MusicTrack();
}

PlayMode PlaybackController::currentMode() const
{
    return playMode;
}

void PlaybackController::next()
{

}

void PlaybackController::pause()
{
    audioEngine->pause();
}

void PlaybackController::play()
{
    audioEngine->play();
}

//void PlaybackController::playbackStateChanged(PlaybackState playbackState)
//{
//    Q_UNUSED(playbackState)
//}

void PlaybackController::updatePlayMode(PlayMode playMode)
{
    Q_UNUSED(playMode)
}

void PlaybackController::playTrack(MusicTrack *track)
{
    qDebug() << track->filePath;
    audioEngine->stop();
    playingTrack = track;
    audioEngine->setTrack(playingTrack);
    audioEngine->play();
}

void PlaybackController::previous()
{

}

void PlaybackController::setupConnections()
{
    connect(audioEngine, &AudioEngine::positionChanged, this, [=](qint64 position){
        emit sliderPositionChanged(position);
    });

    connect(audioEngine->mediaPlayer, &QMediaPlayer::playbackStateChanged, this, [=](QMediaPlayer::PlaybackState newState){
        emit musicPlaybackStateChanged(newState);
    });

    connect(audioEngine->mediaPlayer, &QMediaPlayer::durationChanged, this, [=](qint64 duration){
        emit musicPlayDurationChanged(duration);
    });

    connect(audioEngine->mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status){
        emit statusChanged(status);
     });
}

void PlaybackController::setPlayMode(PlayMode mode)
{
    playMode = mode;
}

void PlaybackController::setVolume(float value)
{
    audioEngine->setVolume(value);
}

void PlaybackController::stop()
{
    audioEngine->stop();
}

void PlaybackController::updateProgress(qint64 position)
{
    audioEngine->setPosition(position);
}
