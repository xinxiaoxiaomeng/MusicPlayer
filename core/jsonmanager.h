#ifndef JSONMANAGER_H
#define JSONMANAGER_H

#include <QObject>
#include <QJsonArray>               // 用于封装JSON数组
#include <QJsonObject>              // 用于封装JSON对象
#include <QJsonDocument>            // 管理JSON文档
#include "types.h"

class JsonManager: public QObject
{
    Q_OBJECT
public:
    explicit JsonManager(QObject *parent =nullptr);
    ~JsonManager();

    void handleJsonInfo(QByteArray *jsonData);
    QList<MusicTrack>* hashJsonAnalysis(QByteArray* jsonData);      // 音乐的hash和album_id的解析
    MusicTrack* musicJsonAnalysis(QByteArray *jsonData);     // 搜索音乐数据信息Json解析，解析音乐文件和歌词


};

#endif // JSONMANAGER_H
