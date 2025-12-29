#ifndef MUSICLISTCONTROLLER_H
#define MUSICLISTCONTROLLER_H

#include <QObject>
#include "musiclistview.h"
#include "musiclistmodel.h"
#include "sqlmanager.h"

class MusicListController: public QObject
{
    Q_OBJECT

public:
    MusicListController(QObject *parent);
    ~MusicListController();



    MusicListModel *model() const {return musicListModel;}
    MusicListView *view() const {return musicListView;}


    void addTrack(MusicTrack &track);
    void addTracks(QList<MusicTrack> &tracks);
    void deleteTrack(MusicTrack *track);
    void removeTrack(int row);

    QModelIndex getCurrentIndex() const;
    MusicTrack& getCurrentTrack();
    TypeList getCurrentType() const;
    bool hasSelected = false;

private:
    void setupConnections();


    MusicListModel *musicListModel = nullptr;
    MusicListView *musicListView = nullptr;
    SqlManager *sqlManager = nullptr;
    TypeList currentType = PlayHistoryListType;
    MusicTrack selectedTrack;
    QModelIndex selectedIndex;


    QList<MusicTrack> *playHistoryList = nullptr;
    QList<MusicTrack> *searchList = nullptr;
    QList<MusicTrack> *localList = nullptr;
    QList<MusicTrack> *likeList = nullptr;



signals:
    void musicListUpdated();
    void musicTrackSelected(MusicTrack track);

public slots:
    void addTrackToList(TypeList type, MusicTrack *track);

    void loadPlayHistoryList();
    void loadSearchList();
    void loadLocalList();
    void loadLikeList();

    void clearPlayHistoryList();
    void clearSearchList();
    void clearLocalList();

    void saveUrl(MusicTrack *track);

    bool trackExists(TypeList type, MusicTrack *track);

    void updateLocalList(MusicTrack *track);
    void updateSearchList(QList<MusicTrack> *resultList);
    void updateList(TypeList);
    void updateRows(QList<MusicTrack> *musicList);

};

#endif // MUSICLISTCONTROLLER_H
