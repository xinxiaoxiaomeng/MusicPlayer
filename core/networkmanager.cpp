#include "networkmanager.h"
#include <QVariant>
#include <QDir>
#include "stringconstants.h"

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
    , networkRequest(new QNetworkRequest())
    , networkAccessManager(new QNetworkAccessManager())
    , resultList(new QList<MusicTrack>())
    , jsonManager(new JsonManager())
    , md5Helper(new MD5Helper())
{
    setupConnections();
}

NetworkManager::~NetworkManager()
{

}

//void NetworkManager::downloadMusic(MusicTrack *track)
//{
//    networkCommand = NetworkCommand::Download;

//    saveFile.setFileName(QString("D:/MusicDownload/%1-%2.mp3").arg(track->artist).arg(track->title));

//    qDebug() << saveFile.fileName();

//    if (!saveFile.open(QIODevice::WriteOnly)) {
//        return;
//    }

//    networkAccess(track->downloadPath);
//}

void NetworkManager::downloadMusic(const QUrl &url)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");
    request.setRawHeader("Accept", "*/*");

    QNetworkReply *reply = networkAccessManager->get(request);

    connect(reply, &QNetworkReply::readyRead, this, [this, reply]() {
        saveFile.write(reply->readAll());
    });

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {

        // 处理重定向（正确时机）
        QVariant redirect =
            reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

        if (redirect.isValid()) {
            QUrl newUrl = reply->url().resolved(redirect.toUrl());
            reply->deleteLater();
            downloadMusic(newUrl);
            return;
        }

        saveFile.flush();
        saveFile.close();

        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << reply->errorString();
        }

        reply->deleteLater();
    });
}




void NetworkManager::downloadMusicInfo(MusicTrack *track)
{
    QString key = track->hash + "kgcloud";
    key = md5Helper->encryptString(key);
    QString url = kugouDownloadApi + QString("&hash=%1&key=%2&pid=1&forceDown=0&vip=1").arg(track->hash).arg(key);

    networkAccess(url);
}

void NetworkManager::playMusicInfo(MusicTrack *track)
{
    networkCommand = NetworkCommand::Play;
    // 返回的url
    QString url = kugouPlayInfoApi + QString("&hash=%1&album=%2").arg(track->hash).arg(track->album_id);
    networkAccess(url);

}

void NetworkManager::networkAccess(QString url)
{
    networkRequest->setUrl(url);
    networkRequest->setHeader(QNetworkRequest::UserAgentHeader,
                      "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");

    networkRequest->setRawHeader("Accept", "*/*");
    networkRequest->setRawHeader("Connection", "keep-alive");


    networkAccessManager->get(*networkRequest);
}

void NetworkManager::networkReply(QNetworkReply *reply)
{
    Q_UNUSED(reply);
}

void NetworkManager::setSavePath(QString path)
{
    QDir dir(StringConstants::Settings::musicDownloadPath);
    if (!dir.exists())
    {
        dir.mkdir(StringConstants::Settings::musicDownloadPath);
    }

    saveFile.setFileName(path);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qDebug() << "文件打开失败";
        return;
    }
}

void NetworkManager::searchLyric(MusicTrack *track)
{
    QString url = kugouLyricApi + QString("version=9108&highlight=1&keyword=%1&plat=0&pagesize=20&area_code=1&page=1&with_res_tag=1").arg(track->title+track->artist);
}

void NetworkManager::searchMusic(QString searchInfo)
{
    networkCommand = NetworkCommand::Search;
    QString url = kugouSearchApi + QString("format=json&keyword=%1&page=1&pagesize=20&showtype=1").arg(searchInfo);
    networkAccess(url);

//    networkRequest->setUrl(url);

//    networkAccessManager->get(*networkRequest);
}

void NetworkManager::searchReplyFinished(QNetworkReply *reply)
{


    // 错误处理
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Search request error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray jsonData = reply->readAll();

    resultList = jsonManager->hashJsonAnalysis(&jsonData);

    emit networkReturned(resultList);
//    networkCommand = Empty;
    reply->deleteLater();

}

void NetworkManager::setupConnections()
{
    connect(networkAccessManager, &QNetworkAccessManager::finished, this, &NetworkManager::searchReplyFinished);
}
