#include "musiclistmodel.h"
#include <QModelIndex>

MusicListModel::MusicListModel(QObject *parent) : QAbstractTableModel(parent)
{
    headers << "序号" << "歌曲名" << "歌手" << "专辑" << "喜欢" << "专辑ID" << "播放路径" << "hash";
}

MusicListModel::~MusicListModel()
{

}

// 插入一条曲目
void MusicListModel::addTrack(const MusicTrack &track)
{
    beginInsertRows(QModelIndex(), musicTracks.size(), musicTracks.size());
    musicTracks.append(track);
    endInsertRows();
}

// 插入多条曲目
void MusicListModel::addTracks(const QList<MusicTrack> &tracks)
{
    if (tracks.size() == 0)
        return;
    beginInsertRows(QModelIndex(), musicTracks.size(), musicTracks.size()+tracks.size()-1);
    musicTracks.append(tracks);
    endInsertRows();
}

// 删除所有曲目
void MusicListModel::clear()
{
    if (musicTracks.isEmpty())
        return;

    beginRemoveRows(QModelIndex(), 0, musicTracks.size()-1);
    musicTracks.clear();
    endRemoveRows();
}

int MusicListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return columnCounts;
}

QModelIndex	MusicListModel::parent(const QModelIndex & index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

// 获取指定 index 和 role 的数据
QVariant MusicListModel::data(const QModelIndex &index , int role) const
{
    if (!index.isValid() || index.row() >= musicTracks.size())
        return QVariant();

    const MusicTrack track = musicTracks.at(index.row());
    int column = index.column();

//    static int id = 0;

    if (role == Qt::DisplayRole)
    {
        switch(column)
        {
            case indexColumn:
                return track.index;
            case titleColumn:
                return track.title.isEmpty() ? "未知标题" : track.title;
            case artistColumn:
                return track.artist.isEmpty() ? "未知歌手" : track.artist;
            case albumColumn:
                return track.album.isEmpty() ? "未知专辑" : track.album;
            case albumIdColumn:
                return track.album_id.isEmpty() ? "无专辑ID数据" : track.album_id;
            case likeColumn:
                return track.like;
            case filePathColumn:
                return track.filePath.isEmpty() ? "无播放源" : track.filePath;
            case hashColumn:
                return track.hash.isEmpty() ? "无hash数据" : track.hash;
            default:
                return QVariant();
        }

    }
//    else if (role == Qt::DecorationRole && column == titleColumn)
//    {
//        // 如果需要，可以为标题列提供图标
//        return QIcon(":/image/resources/icon/logo.png");
//    }

    return QVariant();
}

QVariant MusicListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        // 返回水平表头
        if (section >= 0 && section < headers.size())
                return headers.at(section);
    } else {
        // 返回垂直表头（行号）
        return section + 1;
    }

    return QVariant();
}

// 删除一条曲目
void MusicListModel::removeTrack(int row)
{
    if (row < 0 || row > musicTracks.size())
        return;

    beginRemoveColumns(QModelIndex(), row, row);
    musicTracks.removeAt(row);
    endRemoveColumns();
}

// 返回行数
int MusicListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return musicTracks.size();
}

// 返回row行数据
MusicTrack MusicListModel::trackAt(int row) const
{
    if (row < 0 || row >= musicTracks.size()) {
        // 返回一个默认构造的MusicTrack，或抛出异常
        return MusicTrack();
    }
    return musicTracks.at(row);
}





