#include "musiclistcontroller.h"

MusicListController::MusicListController(QObject *parent)
    : QObject(parent)
    , musicListModel(new MusicListModel())
    , musicListView(new MusicListView())
    , sqlManager(new SqlManager())
    , playHistoryList(new QList<MusicTrack>())
    , searchList(new QList<MusicTrack>())
    , localList(new QList<MusicTrack>())
    , likeList(new QList<MusicTrack>())
{
    musicListView->setMusicListModel(musicListModel);
//    selectedIndex = 0;

    setupConnections();
}

MusicListController::~MusicListController()
{

}

void MusicListController::addTrack(MusicTrack &track)
{
    musicListModel->addTrack(track);
    emit musicListUpdated();
}

void MusicListController::addTracks(QList<MusicTrack> &tracks)
{
    musicListModel->addTracks(tracks);
    emit musicListUpdated();
}

void MusicListController::addTrackToList(TypeList type, MusicTrack *track)
{
    if(!track)
        return;


    if (type == TypeList::PlayHistoryListType)
    {
        if (track->filePath.isEmpty())
            return;

        QString sql = QString(sqlManager->sqls["trackExists_playHistoryList"]).arg(track->title).arg(track->artist);

        QSqlQuery query = sqlManager->execuateSql(sql);

        if (query.next())       // 播放历史已经有该记录
        {
            sql = QString(sqlManager->sqls["deleteTrack_playHistoryList"]).arg(track->title).arg(track->artist);
            sqlManager->execuateSql(sql);
//            qDebug() << sql;
        }

        sql = QString(sqlManager->sqls["update_playHistoryList"]).arg(track->title).arg(track->artist).arg(track->album).arg(track->album_id).arg(track->hash).arg(track->filePath);
        sqlManager->execuateSql(sql);

        if (currentType == TypeList::PlayHistoryListType)
            updateList(TypeList::PlayHistoryListType);
    }
    else if (type == TypeList::LikeListType)
    {
        QString sql = QString(sqlManager->sqls["trackExists_likeList"]).arg(track->title).arg(track->artist);

        QSqlQuery query = sqlManager->execuateSql(sql);

        // 已经存在，删除
        if (query.next())
        {
            sql = QString(sqlManager->sqls["deleteTrack_likeList"]).arg(track->title).arg(track->artist);
            sqlManager->execuateSql(sql);
        }
        // 不存在，插入
        else
        {
            sql = QString(sqlManager->sqls["update_likeList"]).arg(track->title).arg(track->artist).arg(track->album).arg(track->album_id).arg(track->hash).arg(track->filePath);
            sqlManager->execuateSql(sql);
        }

        if (currentType == TypeList::LikeListType)
            updateList(TypeList::LikeListType);

    }
}

void MusicListController::clearPlayHistoryList()
{
    sqlManager->execuateSql(sqlManager->sqls["clear_playHistoryList"]);
}

void MusicListController::clearLocalList()
{
    sqlManager->execuateSql(sqlManager->sqls["clear_localList"]);
}

void MusicListController::clearSearchList()
{
    sqlManager->execuateSql(sqlManager->sqls["clear_searchList"]);
    sqlManager->execuateSql(sqlManager->sqls["reset_searchList"]);
}

void MusicListController::deleteTrack(MusicTrack *track)
{
    QString sql;
    if (currentType == LocalListType)
    {
        sql = QString(sqlManager->sqls["deleteTrack_localList"]).arg(track->title).arg(track->artist);
    }
    else if (currentType == LikeListType)
    {
        sql = QString(sqlManager->sqls["deleteTrack_likeList"]).arg(track->title).arg(track->artist);
    }
    else if (currentType == PlayHistoryListType)
    {
        sql = QString(sqlManager->sqls["deleteTrack_playHistoryList"]).arg(track->title).arg(track->artist);
    }
    else if (currentType == SearchListType)
    {
        sql = QString(sqlManager->sqls["deleteTrack_searchList"]).arg(track->title).arg(track->artist);
    }

    sqlManager->execuateSql(sql);
}

QModelIndex MusicListController::getCurrentIndex() const
{
    return selectedIndex;
}

MusicTrack& MusicListController::getCurrentTrack()
{
    return selectedTrack;
}

TypeList MusicListController::getCurrentType() const
{
    return currentType;
}

void MusicListController::loadPlayHistoryList()
{
    playHistoryList = sqlManager->getMusicListBySql(sqlManager->sqls["get_playHistoryList"]);
    updateRows(playHistoryList);
//    qDebug() << playHistoryList->size();
//    musicListModel->clear();
//    musicListModel->addTracks(*playHistoryList);
}

void MusicListController::loadLikeList()
{
    likeList = sqlManager->getMusicListBySql(sqlManager->sqls["get_likeList"]);
    updateRows(likeList);
}

void MusicListController::loadLocalList()
{
    localList = sqlManager->getMusicListBySql(sqlManager->sqls["get_localList"]);
    updateRows(localList);
}

void MusicListController::loadSearchList()
{
    searchList = sqlManager->getMusicListBySql(sqlManager->sqls["get_searchList"]);
    updateRows(searchList);
}

void MusicListController::removeTrack(int row)
{
    musicListModel->removeTrack(row);
    emit musicListUpdated();
}

void MusicListController::saveUrl(MusicTrack *track)
{
    if(!track->hash.isEmpty())
    {
        QString sql = QString(sqlManager->sqls["save_url"]).arg(track->filePath).arg(track->hash);
//        qDebug() << sql;
        sqlManager->execuateSql(sql);
    }
}

bool MusicListController::trackExists(TypeList type, MusicTrack *track)
{
    if (!track)
        return false;

    if (type == LikeListType)
    {
        QString sql = QString(sqlManager->sqls["trackExists_likeList"]).arg(track->title).arg(track->artist);

        QSqlQuery query = sqlManager->execuateSql(sql);

        if(query.next())
        {
            return true;
        }

    }

    return false;
}

void MusicListController::setupConnections()
{
    // 更新选中行的信息
    connect(musicListView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this](){
        QModelIndexList modelIndexList = musicListView->selectionModel()->selectedRows();
        if (modelIndexList.isEmpty())
        {
//            selectedIndex.row() = -1;

            hasSelected = false;
            return;
        }
        hasSelected = true;
        selectedIndex = modelIndexList.at(0);
//        selectedIndex = selectedIndex.row();

        selectedTrack = musicListModel->trackAt(selectedIndex.row());
//        qDebug() << "selectedIndex changed:" << selectedIndex.row();
    });

}

void MusicListController::updateList(TypeList type)
{
    currentType = type;
//    qDebug() << type << " 更新";
    musicListModel->clear();
    switch (type)
    {
        case SearchListType:
            loadSearchList();
            musicListModel->addTracks(*searchList);
            break;
        case PlayHistoryListType:
            loadPlayHistoryList();
            musicListModel->addTracks(*playHistoryList);
            break;
        case LocalListType:
            loadLocalList();
            musicListModel->addTracks(*localList);
            break;
        case LikeListType:
            loadLikeList();
            musicListModel->addTracks(*likeList);
            break;
        default:
            break;
    }
//        qDebug() << type << musicListView->model()->rowCount();
}

void MusicListController::updateLocalList(MusicTrack *track)
{
    QString sql = QString(sqlManager->sqls["update_localList"]).arg(track->title).arg(track->artist).arg(track->album).arg(track->album_id).arg(track->hash).arg(track->filePath).arg(track->downloadPath);
    sqlManager->execuateSql(sql);
}

void MusicListController::updateRows(QList<MusicTrack> *musicList)
{
        for (int i = 0; i < musicList->size(); i++)
        {
            (*musicList)[i].index = i+1;

        }
}

void MusicListController::updateSearchList(QList<MusicTrack> *resultList)
{
    for (int i = 0; i < resultList->size(); i++)
    {
        MusicTrack track = resultList->at(i);
        searchList->append(track);
        QString sql = QString(sqlManager->sqls["update_searchList"]).arg(track.title).arg(track.artist).arg(track.album).arg(track.album_id).arg(track.hash);
//        qDebug() << sql;
        sqlManager->execuateSql(sql);
    }

    updateList(SearchListType);
}








