#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QLabel>

struct MusicTrack
{
    int index;
    QString title;
    QString artist;
    QString album;
    QString album_id;
    bool like;
    QString duration;
    QString filePath;
    QString hash;
    QString downloadPath;
};

enum Columns
{
    indexColumn = 0,
    titleColumn,
    artistColumn,
    albumColumn,
    likeColumn,
    albumIdColumn,
    filePathColumn,
    hashColumn,
    columnCounts,
};

struct MusicItem
{
    QLabel img;
    QLabel title;
};

enum PlayMode{
    Sequential,
    Loop,
    Random,
    SignleLoop,
};

enum TypeList{
    EmptyType,
    SearchListType,
    PlayHistoryListType,
    LocalListType,
    LikeListType,
};

// 播放状态枚举
enum PlaybackState {
    Stopped,     // 停止
    Playing,     // 播放中
    Paused       // 暂停
};

struct PlayingInfo {
    MusicTrack playingTrack;
    float volume;
    int currentIndex;
    PlayMode mode;
};

enum NetworkCommand{
    Empty,
    Search,
    Play,
    Download
};

#endif // TYPES_H
