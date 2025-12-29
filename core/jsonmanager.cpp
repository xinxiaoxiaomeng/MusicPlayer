#include "jsonmanager.h"

JsonManager::JsonManager(QObject *parent):
    QObject(parent)
{

}


JsonManager::~JsonManager()
{

}

//void JsonManager::handleJsonInfo(QByteArray *jsonData)
//{
//    QJsonDocument jsonDocument = QJsonDocument::fromJson(*jsonData);
//    if (jsonDocument.isObject())
//    {
//        QJsonObject data = jsonDocument.object();
//        if (data.contains("data"))
//        {
//            hashJsonAnalysis(QJsonObject *jsonData)
//        }
//    }
//}



//QList<MusicTrack>* JsonManager::hashJsonAnalysis(QJsonObject *data)
//{
////    QJsonDocument jsonDocument = QJsonDocument::fromJson(*jsonData);
//    QList<MusicTrack> *returnList = new QList<MusicTrack>();
////    if (jsonDocument.isObject())
////    {
////        QJsonObject data = jsonDocument.object();

//    if(data->contains("data"))
//    {
//        QJsonObject objectInfo = data->value("data").toObject();
//        if (objectInfo.contains("info"))
//        {
//            QJsonArray objectHash = objectInfo.value("info").toArray();

//            for (int i = 0; i < objectHash.count(); i++)
//            {
//                //                    QString title, artist, albumId, hash;
//                MusicTrack track;
//                QJsonObject albumObject = objectHash.at(i).toObject();

//                if (albumObject.contains("album_name"))
//                {
//                    track.album = albumObject.value("album_name").toString();
//                }

//                if (albumObject.contains("album_id"))
//                {
//                    track.album_id = albumObject.value("album_id").toString();
//                }

//                if (albumObject.contains("singername"))
//                {
//                    track.artist = albumObject.value("singername").toString();
//                }

//                if (albumObject.contains("songname"))
//                {
//                    track.title = albumObject.value("songname").toString();
//                }

//                if (albumObject.contains("hash"))
//                {
//                    track.hash = albumObject.value("hash").toString();
//                }

//                returnList->append(track);
//            }

//        }
//    }
////    }


//    if (jsonDocument.isArray())
//    {
//        qDebug() << "Array";
//    }
//    return returnList;
//}

QList<MusicTrack>* JsonManager::hashJsonAnalysis(QByteArray *jsonData)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(*jsonData);
    QList<MusicTrack> *returnList = new QList<MusicTrack>();
    if (jsonDocument.isObject())
    {
        QJsonObject data = jsonDocument.object();

        if(data.contains("data"))
        {
            QJsonObject objectInfo = data.value("data").toObject();
            if (objectInfo.contains("info"))
            {
                QJsonArray objectHash = objectInfo.value("info").toArray();

                for (int i = 0; i < objectHash.count(); i++)
                {
//                    QString title, artist, albumId, hash;
                    MusicTrack track;
                    QJsonObject albumObject = objectHash.at(i).toObject();

                    if (albumObject.contains("album_name"))
                    {
                        track.album = albumObject.value("album_name").toString();
                    }

                    if (albumObject.contains("album_id"))
                    {
                        track.album_id = albumObject.value("album_id").toString();
                    }

                    if (albumObject.contains("singername"))
                    {
                        track.artist = albumObject.value("singername").toString();
                    }

                    if (albumObject.contains("songname"))
                    {
                        track.title = albumObject.value("songname").toString();
                    }

                    if (albumObject.contains("filename"))
                    {
                        track.title = albumObject.value("filename").toString();
                    }

                    if (albumObject.contains("hash"))
                    {
                        track.hash = albumObject.value("hash").toString();
                    }

                    returnList->append(track);
                }

            }
        }
        else if (data.contains("authors"))
        {
            QJsonObject authorInfo = data.value("authors").toObject();
            MusicTrack track;

            if(authorInfo.contains("author_name"))
            {
                track.artist = authorInfo.value("author_name").toString();
            }

            if(data.contains("url"))
            {
                track.filePath = data.value("url").toString();
                qDebug() << track.filePath;
            }

            if (data.contains("songName"))
            {
                track.title = data.value("songName").toString();
            }

            returnList->append(track);
//            returnList->append(track);
        }
        else if (data.contains("trans_param"))
        {
            qDebug() << "download info";
            MusicTrack track;
//            QJsonObject downladInfo = data.value("trans_param").toObject();

            if (data.contains("url"))
            {

                track.downloadPath = data.value("url").toString();
                qDebug() << track.downloadPath;
            }

            if (data.contains("fileName"))
            {
                track.title = data.value("fileName").toString();
            }

            returnList->append(track);
        }
    }


    if (jsonDocument.isArray())
    {
        qDebug() << "Array";
    }
    return returnList;
}


MusicTrack* JsonManager::musicJsonAnalysis(QByteArray *jsonData)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(*jsonData);

    MusicTrack *track = new MusicTrack();
    if(jsonDocument.isObject())
    {
         QJsonObject data = jsonDocument.object();
        if (data.contains("authors"))
        {
            QJsonObject authorInfo = data.value("authors").toObject();

            if(authorInfo.contains("author_name"))
            {
                track->artist = authorInfo.value("author_name").toString();
            }
        }

        if(data.contains("url"))
        {
            track->filePath = data.value("url").toString();
        }
    }
    return track;
}
