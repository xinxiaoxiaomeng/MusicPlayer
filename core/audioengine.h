#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "types.h"

class AudioEngine: public QObject
{
    Q_OBJECT

public:
    AudioEngine(QObject *parent);
    ~AudioEngine();

    void setupConnections();

    void play();
    void pause();
    void stop();
//    void playAfterPause();
//    void playRestart();
    //void playbackState();

    void setTrack(MusicTrack *track);

    qint64 volume() const;
    void setVolume(float volume);

    void setPosition(qint64 position);
    qint64 position() const;
    qint64 duration() const;

     QMediaPlayer *mediaPlayer = nullptr;
private:

    QAudioOutput *audioOutput = nullptr;
    MusicTrack *currentTrack = nullptr;
    qint64 playPosition = 0;
    int count = 0;

signals:
    void playbackStateChanged(QMediaPlayer::PlaybackState state);
    void positionChanged(qint64 position);
    void volumeChanged(qint64 volume);
    void playbackFinished();
    void currentTrackChanged(MusicTrack *track);
    void errorOccurred(const QString &error);
    void durationChanged(qint64 duration);
    void showWarningMessage(QString title, QString test);

};

#endif // AUDIOENGINE_H
