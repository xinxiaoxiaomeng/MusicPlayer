#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QDebug>
#include <QVBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QSlider>
#include "customslider.h"
#include <QMediaPlayer>
#include "musiclistcontroller.h"
#include "types.h"
#include "playbackcontroller.h"
#include "lyricparser.h"
#include <QModelIndex>
#include <QMediaPlayer>
#include "sqlmanager.h"
#include "networkmanager.h"
#include <QMenu>
#include <QThread>
#include "volumepopup.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private:
    QWidget *centralWidget = nullptr;
    QWidget *mainWidget = nullptr;
    QWidget *titleWidget = nullptr;
    QWidget *leftWidget = nullptr;
    QWidget *rightWidget = nullptr;
    QWidget *bottomWidget = nullptr;
    QWidget *playWidget = nullptr;
    QWidget *skinPage = nullptr;
    QWidget *bottomLeftWidget = nullptr;
    QWidget *bottomCenterWidget = nullptr;
    QWidget *bottomRightWidget = nullptr;
    QWidget *bodyWidget = nullptr;

    QVBoxLayout *centralLayout = nullptr;
    QHBoxLayout *mainLayout = nullptr;
    QHBoxLayout *titleLayout = nullptr;
    QVBoxLayout *leftLayout = nullptr;
    QVBoxLayout *rightLayout = nullptr;
    QVBoxLayout *bottomLayout = nullptr;
    QHBoxLayout *playLayout = nullptr;
    QGridLayout *skinLayout = nullptr;
    QHBoxLayout *bottomLeftLayout = nullptr;
    QHBoxLayout *bottomCenterLayout = nullptr;
    QHBoxLayout *bottomRightLayout = nullptr;

    QLineEdit *inputSearch = nullptr;
    QPushButton *iconSearch = nullptr;
    QPushButton *iconSetting = nullptr;
    QPushButton *iconSkin = nullptr;
    QPushButton *iconMin = nullptr;
    QPushButton *iconScale = nullptr;
    QPushButton *iconClose = nullptr;

    QPushButton *iconRecommend = nullptr;
    QPushButton *iconMusicHouse = nullptr;
    QPushButton *iconFavouriteMusic = nullptr;
    QPushButton *iconPlayHistory = nullptr;
    QPushButton *iconLocal = nullptr;
    QPushButton *iconSearchResult = nullptr;
    QButtonGroup *buttonGroup = nullptr;

    QPushButton *iconPlaying = nullptr;
    QPushButton *iconStartPause = nullptr;
    QPushButton *iconNext = nullptr;
    QPushButton *iconPrevious = nullptr;
    QPushButton *iconPlayMode = nullptr;
    QPushButton *iconVolume = nullptr;
    QPushButton *iconLike = nullptr;

    QLabel *musicLogo = nullptr;
    QLabel *playingName = nullptr;
    QLabel *lyricText = nullptr;
    QLabel *infoText = nullptr;

    QMenu *rightClickedMenu = nullptr;

    CustomSlider *progressSlider = nullptr;
    VolumePopup *volumeSlider = nullptr;

    PlayMode playMode = PlayMode::Sequential;
    MusicListController *musicListController = nullptr;
    PlaybackController *playbackController = nullptr;
    LyricParser *lyricParser = nullptr;
    SqlManager *sqlManager = nullptr;
    NetworkManager *networkManager = nullptr;

    bool isDragging = false;
    bool isPlaying = false;
    bool isLike = false;
    bool isVolume = true;
    QPointF dragStartPosition;

    NetworkCommand networkCommand = Empty;

//    MusicTrack playingTrack;
//    int playingIndex = -1;

    PlayingInfo *playingInfo;

    QMediaPlayer::PlaybackState currentPlaybackState = QMediaPlayer::StoppedState;

private:
    QPixmap scalePixmap(const QString& path, int width, int height);

    void setupUI();
    void setupLeftWidget();
    void setupRightWidget();
    void setupTitleWidget();
    void setupBottomWidget();
    void setupBodyWidget();
    void setupRightClickedMenu();


    bool eventFilter(QObject *obj, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *event) override;

    bool isPointInWidget(QWidget *widget, const QPoint &pos);
    void startDragging(const QPointF &globalPos);
    void updateDragging(const QPointF &globalPos);
    void stopDragging();



    void centerWindow();
    void setupConnections();
    void playNewTrack(MusicTrack *track);
    void playRandomMusic();
    void infoShow(QString text);




public slots:
    void handlePlaybackFinished();
    void handleReturnedList(QList<MusicTrack>* returnList);
    void iconButtonGroupClicked(int id);
    void iconCloseClicked();
    void iconDeleteClicked();
    void iconDownloadClicked();
    void iconFavouriteMusicClicked();
    void iconLocalClicked();
    void iconLikeClicked();
    void iconMinClicked();
    void iconNextClicked();
    void iconPlayHistoryClicked();
    void iconPlayModeClicked();
    void iconPreviousClicked();   
    void playSelectedTrack(const QModelIndex &index);
    void iconSearchClicked();
    void iconSearchResultClicked();
    void iconSettingClicked();
    void iconScaleClicked();
    void iconSkinClicked();
    void iconStartPauseClicked();
    void iconVolumeClicked();
    void updatePlaybackState(QMediaPlayer::PlaybackState state);
    void updatePlayingTrackInfo();
    void updateLyric(QString);
    void updateMediaStatus(QMediaPlayer::MediaStatus status);
    void showContextMenu(const QPoint &pos);



signals:
    void playModeChanged(PlayMode mode);
    void likeStatusChanges(MusicTrack &track);
    void lyricShow(QString);
    void playingTrackChanged(MusicTrack *track);
    void requestPlayTrack(MusicTrack track);
    void requestPause();
    void requestPlay();
    void reqestSetVolume(float value);
    void requestDownloadInfo(MusicTrack track);
    void requestDownloadMusic(QString savePath, QString downloadPath);
    void requestSearchMusic(QString keyWord);
    void requestPlayMusicInfo(MusicTrack track);
//    void likeChanged(TypeList type, MusicTrack *track);
//    void listTypeChanged(MusicListController::typeList);

};
#endif // MAINWINDOW_H
