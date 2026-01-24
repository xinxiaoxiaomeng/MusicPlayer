#ifndef PLAYBACKCONTROLLER_H
#define PLAYBACKCONTROLLER_H

#include <QObject>
#include "audioengine.h"
#include "types.h"

class PlaybackController: public QObject
{
    Q_OBJECT

public:
    explicit PlaybackController(QObject *parent = nullptr);
    ~PlaybackController();

    void setupConnections();


    int currentSelectedIndex() const;
    void updateProgress(qint64 position);

    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void playTrack(MusicTrack track);
    void setVolume(float value);

    MusicTrack currentTrack() const;

    PlayMode currentMode() const;
    void setPlayMode(PlayMode mode);

private:
    //MusicListController *musicListController = nullptr;
    AudioEngine *audioEngine = nullptr;

    PlayMode playMode;
    PlaybackState playbackState;
    int currentIndex;
    MusicTrack *playingTrack = nullptr;
    QList<MusicTrack> playList;



public slots:
    void updatePlayMode(PlayMode playMode);
    void initialized();

signals:
    void sliderPositionChanged(qint64 position);
    void musicPlaybackStateChanged(QMediaPlayer::PlaybackState newState);
    void musicPlayDurationChanged(qint64 duration);
    void statusChanged(QMediaPlayer::MediaStatus status);
    void requestWarningMessage(QString title, QString text);
};

#endif // PLAYBACKCONTROLLER_H
