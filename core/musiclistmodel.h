#ifndef MUSICLISTMODEL_H
#define MUSICLISTMODEL_H

#include <QString>
#include <QAbstractTableModel>
#include <QList>
#include "types.h"


class MusicListModel: public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit MusicListModel(QObject *parent=nullptr);
    ~MusicListModel();



    // 必须实现的纯虚函数(函数后面的const表示该函数不会修改任何类的成员变量)
    int	rowCount (const QModelIndex & parent = QModelIndex()) const override;
    int	columnCount (const QModelIndex & parent = QModelIndex()) const override;
    QModelIndex	parent(const QModelIndex & index) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // 虚函数
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // 数据操作接口
    void addTrack(const MusicTrack &track);
    void addTracks(const QList<MusicTrack> &tracks);
    void removeTrack(int row);
    void clear();
    MusicTrack trackAt(int row) const;

private:
     QList<MusicTrack> musicTracks;
    int currentPlayingRow = -1;
    QStringList headers;

signals:
    void trackAdded();
    void trackRemoved();
    void trackUpdated();
};

#endif // MUSICLISTMODEL_H
