#ifndef STRINGCONSTANTS_H
#define STRINGCONSTANTS_H

#include <QObject>
#include <QString>


class StringConstants: public QObject
{
    Q_OBJECT;

public:

    StringConstants();
    ~StringConstants() = default;

    class UI
    {
    public:
        static const QString logoPath;
        static const QString searchPath;
        static const QString settingPath;
        static const QString skinPath;
        static const QString minPath;
        static const QString maxPath;
        static const QString normalPath;
        static const QString closePath;
        static const QString backgroundPath;
        static const QString musicHousePath;
        static const QString favouriteMusicPath;
        static const QString playHistoryPath;
        static const QString localPath;
        static const QString previousPath;
        static const QString nextPath;
        static const QString startPath;
        static const QString pausePath;
        static const QString recommendPath;
        static const QString songPath;
        static const QString silencePath;
        static const QString volumePath;
        static const QString likeBlackPath;
        static const QString likeRedPath;
        static const QString modeSequencePath;
        static const QString modeLoopPath;
        static const QString modeShufflePath;
        static const QString modeSignleLoopPath;

       enum LeftBar{
            FavouriteMusic = 1,
            PlayHistory,
            LoaclMusic,
            SearchResult
        };
    };

    class Settings
    {
    public:
        static const QString musicDownloadPath;
        static const QString databasePath;
    };
};

#endif // STRINGCONSTANTS_H
