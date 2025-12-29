#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkRequest>          // HTTP的URL请求管理
#include <QNetworkAccessManager>    // URL的上传管理
#include <QNetworkReply>            // 网页回复数据触发信号的类
#include <QEventLoop>               // 提供进入和离开事件循环的方法
#include "jsonmanager.h"
#include "types.h"
#include "md5helper.h"
#include <QNetworkReply>

class NetworkManager: public QObject
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    void setupConnections();
    void setSavePath(QString path);

private:

    QString kugouSearchApi = "http://mobilecdn.kugou.com/api/v3/search/song?";
//    QString kugouDownloadApi = "https://wwwapi.kugou.com/yy/index.php?";
//    QString kugouDownloadApi = "https://wwwapi.kugou.com/play/songinfo";
    QString kugouDownloadApi = "http://trackercdn.kugou.com/i/?cmd=4";
    QString kugouPlayInfoApi = "http://m.kugou.com/app/i/getSongInfo.php?cmd=playInfo";
    QString kugouLyricApi = "https://mobileservice.kugou.com/api/v3/lyric/search?";

    QNetworkRequest *networkRequest = nullptr;
    QNetworkAccessManager *networkAccessManager = nullptr;
//    QNetworkReply *reply = nullptr;

    QList<MusicTrack> *resultList = nullptr;

    JsonManager *jsonManager = nullptr;

    MD5Helper *md5Helper = nullptr;


    QFile saveFile;

    NetworkCommand networkCommand = Empty;

    void networkReply(QNetworkReply *reply);        // 读取网络数据
    void networkAccess(QString url);                       // 访问HTTP网页


public slots:
    void searchReplyFinished(QNetworkReply *reply);
    void searchMusic(QString searchInfo);
    void searchLyric(MusicTrack *track);
    void playMusicInfo(MusicTrack *track);
    void downloadMusicInfo(MusicTrack *track);
    void downloadMusic(const QUrl &url);

signals:
    void networkReturned(QList<MusicTrack>*);

};

#endif // NETWORKMANAGER_H
