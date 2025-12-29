#ifndef MUSICLISTVIEW_H
#define MUSICLISTVIEW_H

#include <QTableView>
#include <QVBoxLayout>
#include "musiclistmodel.h"

class MusicListView : public QTableView
{
    Q_OBJECT

public:
    MusicListView(QWidget *parent = nullptr);
    ~MusicListView();

    void setMusicListModel(MusicListModel *model);

private:
    void setupUI();
    void setupConnections();

//signals:
//    void MusicTrackSelected(int index);

};

#endif // MUSICLISTVIEW_H
