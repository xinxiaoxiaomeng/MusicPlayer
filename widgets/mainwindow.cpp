#include "mainwindow.h"
#include "stringconstants.h"
#include<QGuiApplication>
#include <QHeaderView>
#include <QButtonGroup>
#include <QTimer>
#include <QAction>
#include <QFileInfo>
#include <QRandomGenerator64>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , musicListController(new MusicListController(this))
    , playbackController(new PlaybackController())  // 不能有父对象
    , lyricParser(new LyricParser(this))
    , networkManager(new NetworkManager())
    , playingInfo(new PlayingInfo())   
{
    setupUI();
    setMinimumSize(1100, 750);
    setupConnections();
    emit iconFavouriteMusic->clicked();
    emit buttonGroup->idClicked(1);

    // 播放线程
    QThread *playThread = new QThread();
    playbackController->moveToThread(playThread);
    playThread->start();

    // 初始化
    connect(playThread, &QThread::started, playbackController, &PlaybackController::initialized, Qt::QueuedConnection);

    // 添加线程清理
    connect(this, &MainWindow::destroyed, this, [playThread]() {
        playThread->quit();
        playThread->wait();
        playThread->deleteLater();
    });

    // 网络线程
    QThread *networkThread = new QThread();
    networkManager->moveToThread(networkThread);
    networkThread->start();

    connect(networkThread, &QThread::started, networkManager, &NetworkManager::initialized, Qt::QueuedConnection);

    connect(this, &MainWindow::destroyed, this, [networkThread](){
        networkThread->quit();
        networkThread->wait();
        networkThread->deleteLater();
    });
}

void MainWindow::centerWindow()
{
    // 获取屏幕的可用几何区域（排除任务栏等）
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();

    // 计算居中的位置
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;

    // 移动窗口到中心位置
    move(x, y);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == iconVolume)
    {
        if (event->type() == QEvent::Enter)
        {
            QPoint pos = iconVolume->mapToGlobal(
                QPoint(iconVolume->width() / 2, 0));

            volumeSlider->move(pos.x() - volumeSlider->width() / 2,
                               pos.y() - volumeSlider->height());

            volumeSlider->show();
            return true;
        }
    }


    if (obj == volumeSlider && event->type() == QEvent::Leave)
    {
        volumeSlider->hide();
        return true;
    }

    return QWidget::eventFilter(obj, event);
}

void MainWindow::handlePlaybackFinished()
{
    switch (playMode)
    {
        case PlayMode::Sequential:
            if (playingInfo->currentIndex <= musicListController->model()->rowCount())
                emit iconNext->clicked();
            break;
        case PlayMode::Loop:
            emit iconNext->clicked();
            break;
        case PlayMode::SignleLoop:
            emit playingTrackChanged(&playingInfo->playingTrack);
            break;
        case PlayMode::Random:
            playRandomMusic();
            break;
        default: break;
    }
}

void MainWindow::handleReturnedList(QList<MusicTrack>* returnList)
{
    if (returnList && returnList->size()>0)
    {
        if (networkCommand == Search)
        {
            musicListController->updateSearchList(returnList);
            emit iconSearchResult->clicked(true);
            emit buttonGroup->idClicked(StringConstants::UI::LeftBar::SearchResult);
        }
        else if (networkCommand == Play)
        {
            playingInfo->playingTrack.filePath = returnList->at(0).filePath;
            musicListController->saveUrl(&playingInfo->playingTrack);
            emit playingTrackChanged(&playingInfo->playingTrack);
        }
        else if (networkCommand == Download)
        {
            MusicTrack track = returnList->at(0);
            MusicTrack selectedTrack = musicListController->getCurrentTrack();
            if (track.downloadPath.isEmpty())
            {
                QMessageBox::critical(0, QObject::tr("警告"), "版权限制，无该歌曲下载权限!");
                return;
            }

            selectedTrack.downloadPath = track.downloadPath;
            QString savePath = StringConstants::Settings::musicDownloadPath+QString("%1 %2.mp3").arg(selectedTrack.artist).arg(selectedTrack.title);
            qDebug() << savePath;
            selectedTrack.filePath = savePath;
            emit requestDownloadMusic(savePath, selectedTrack.downloadPath);
//            networkManager->setSavePath(savePath);
//            networkManager->downloadMusic(selectedTrack.downloadPath);
//            if (QFileInfo(savePath).size() > 0)
//            {
//                infoShow("下载成功");
//            }
            musicListController->updateLocalList(&selectedTrack);

        }
        networkCommand = Empty;
    }
}

void MainWindow::iconCloseClicked()
{
    close();
}

void MainWindow::iconDeleteClicked()
{
    musicListController->deleteTrack(&musicListController->getCurrentTrack());
    musicListController->updateList(musicListController->getCurrentType());
}

void MainWindow::iconDownloadClicked()
{
    networkCommand = Download;
    emit requestDownloadInfo(musicListController->getCurrentTrack());
    //networkManager->downloadMusicInfo(&(musicListController->getCurrentTrack()));
}

void MainWindow::iconFavouriteMusicClicked()
{
    musicListController->updateList(TypeList::LikeListType);
}

void MainWindow::iconMinClicked()
{
    showMinimized();
}

void MainWindow::iconNextClicked()
{
    int totalRows = musicListController->model()->rowCount();
    if (totalRows < 1)
        return;
    playingInfo->currentIndex = (playingInfo->currentIndex + 1 < totalRows) ?  playingInfo->currentIndex + 1 : 0;
    playingInfo->playingTrack = musicListController->model()->trackAt(playingInfo->currentIndex);
    emit playingTrackChanged(&playingInfo->playingTrack);
}

void MainWindow::iconScaleClicked()
{
    if (isFullScreen())
    {
        showNormal();
        iconScale->setIcon(QIcon(StringConstants::UI::maxPath));
        iconScale->setToolTip("最大化");
    }
    else
    {
        showFullScreen();
        iconScale->setIcon(QIcon(StringConstants::UI::normalPath));
        iconScale->setToolTip("向下还原");
    }

}

void MainWindow::iconLikeClicked()
{
    if (playingInfo->playingTrack.title.isEmpty())
        return;

    if(playingInfo->playingTrack.like == true)
    {
        isLike = false;
        playingInfo->playingTrack.like = false;
        iconLike->setIcon(QIcon(StringConstants::UI::likeBlackPath));
        infoShow("取消喜欢");
    }
    else
    {
        isLike = true;
        playingInfo->playingTrack.like = true;
        iconLike->setIcon(QIcon(StringConstants::UI::likeRedPath));
        infoShow("已添加到我喜欢");
    }

    musicListController->addTrackToList(LikeListType, &playingInfo->playingTrack);
}

void MainWindow::iconLocalClicked()
{
    musicListController->updateList(TypeList::LocalListType);
}

void MainWindow::iconPlayHistoryClicked()
{
    musicListController->updateList(TypeList::PlayHistoryListType);
}

void MainWindow::iconPlayModeClicked()
{
    if (playMode == PlayMode::Sequential)
    {
        playMode = PlayMode::Loop;
        iconPlayMode->setIcon(QIcon(StringConstants::UI::modeLoopPath));
    }
    else if(playMode == PlayMode::Loop)
    {
        playMode = PlayMode::Random;
        iconPlayMode->setIcon(QIcon(StringConstants::UI::modeShufflePath));
    }
    else if (playMode == PlayMode::Random)
    {
        playMode = PlayMode::SignleLoop;
        iconPlayMode->setIcon(QIcon(StringConstants::UI::modeSignleLoopPath));
    }
    else if (playMode == PlayMode::SignleLoop)
    {
        playMode = PlayMode::Sequential;
        iconPlayMode->setIcon(QIcon(StringConstants::UI::modeSequencePath));
    }
}

void MainWindow::iconPreviousClicked()
{
    int totalRows = musicListController->model()->rowCount();
    if (totalRows < 1)
        return;
    playingInfo->currentIndex = ( playingInfo->currentIndex - 1 >= 0) ?   playingInfo->currentIndex - 1 : totalRows - 1;
    playingInfo->playingTrack = musicListController->model()->trackAt( playingInfo->currentIndex);
    emit playingTrackChanged(&playingInfo->playingTrack);
}

void MainWindow::iconSearchClicked()
{
    networkCommand = Search;
    musicListController->clearSearchList();
    QString keyWord = inputSearch->text();
    emit requestSearchMusic(keyWord);
    //networkManager->searchMusic(text);
}

void MainWindow::iconSearchResultClicked()
{
    musicListController->updateList(SearchListType);
}

void MainWindow::iconSettingClicked()
{

}

void MainWindow::iconSkinClicked()
{

}

void MainWindow::iconStartPauseClicked()
{
    if (playingInfo->playingTrack.filePath.isEmpty())
        return;
    if (currentPlaybackState==QMediaPlayer::PlayingState)
    {
        // 暂停
        //playbackController->pause();
        emit requestPause();
    }
    else
    {
        // 播放
        //playbackController->play();
        emit requestPlay();
    }

}

void MainWindow::iconVolumeClicked()
{
    if(isVolume)
    {
        isVolume = false;
        playingInfo->volume = 0;
        iconVolume->setIcon(QIcon(StringConstants::UI::silencePath));
        //playbackController->setVolume(0);
        emit reqestSetVolume(0);
    }
    else
    {
        isVolume = true;
        playingInfo->volume = (float)0.2;
        //playbackController->setVolume(playingInfo->volume);

        iconVolume->setIcon(QIcon(StringConstants::UI::volumePath));
        emit reqestSetVolume(playingInfo->volume);
    }
}

// pos 是否在 widget 范围内
bool MainWindow::isPointInWidget(QWidget *widget, const QPoint &pos)
{
    if (!widget)
        return false;

    return widget->geometry().contains(pos);
}

void MainWindow::iconButtonGroupClicked(int id)
{
    for(int i = 1; i <= buttonGroup->buttons().count(); i++)
    {
        QAbstractButton *btn = buttonGroup->button(i);
        btn->setProperty("highlighted", false);
        if (i == id)
        {
            btn->setProperty("highlighted", true);
        }

        btn->style()->unpolish(btn);    // 移除按钮上当前的样式信息，将其恢复到默认状态，但不会改变已经设置的样式表。
        btn->style()->polish(btn);      // 重新应用样式表到按钮上，根据按钮当前的属性来计算新的样式
        btn->update();                  // 会触发按钮的重绘事件，确保按钮的外观被更新以反映新的样式。
    }
}

void MainWindow::infoShow(QString text)
{
    infoText->setText(text);
    infoText->raise();
    // 使用单次定时器在窗口显示后定位
    QPoint center = bodyWidget->rect().center();
    infoText->setVisible(true);
    infoText->move(center - QPoint(infoText->width()/2, infoText->height()/2));
    QTimer::singleShot(2000, this, [this]() {
        if (infoText) infoText->setVisible(false);
    });
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() ==  Qt::Key_Return)
    {
        if (inputSearch->hasFocus())
        {
            iconSearchClicked();
        }
        event->accept();
    }
    else
    {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (isPointInWidget(titleWidget, event->pos()) && event->button() == Qt::LeftButton)
    {
        iconScaleClicked();
    }

    QMainWindow::mouseDoubleClickEvent(event);
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if (isPointInWidget(titleWidget, event->pos()))
        {
            startDragging(event->globalPosition());
            event->accept();
        }
    }

    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isDragging)
    {
        event->accept();
        stopDragging();
    }
    QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging && (event->buttons() & Qt::LeftButton))
    {
        updateDragging(event->globalPosition());
        event->accept();
    }
    QMainWindow::mouseMoveEvent(event);
}


void MainWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);
}

void MainWindow::playNewTrack(MusicTrack *selectedTrack)
{
    qDebug() << "播放新音乐";

    playingInfo->playingTrack = *selectedTrack;
    musicListController->addTrackToList(PlayHistoryListType, &playingInfo->playingTrack);
    emit requestPlayTrack(playingInfo->playingTrack);
    //playingTrack = *selectedTrack;
    //playbackController->playTrack(&playingInfo->playingTrack);
    //progressSlider->updateDuration(playingInfo->playingTrack.duration);
    updatePlayingTrackInfo();
}

void MainWindow::playRandomMusic()
{
    int size = musicListController->model()->rowCount();

    int randomValue = QRandomGenerator::global()->bounded(size);

    QModelIndex index = musicListController->model()->index(randomValue, 2, QModelIndex());

    playSelectedTrack(index);

//    MusicTrack track = musicListController->model()->trackAt(randomValue);

//    playNewTrack(&track);
}

void MainWindow::playSelectedTrack(const QModelIndex &index)
{
    if(index.isValid())
    {
        playingInfo->currentIndex = index.row();
        MusicTrack selectedTrack = musicListController->model()->trackAt(playingInfo->currentIndex);
//        qDebug() << "当前选中音乐 selectedTrack :" << selectedTrack.title << selectedTrack.filePath;

        if (!playingInfo->playingTrack.filePath.isEmpty() && playingInfo->playingTrack.filePath == selectedTrack.filePath)
        {
            iconStartPauseClicked();
            return;
        }

        if (musicListController->getCurrentType() != LocalListType)
        {
            networkCommand = Play;
            playingInfo->playingTrack = selectedTrack;
            emit requestPlayMusicInfo(selectedTrack);
            //networkManager->playMusicInfo(selectedTrack);
        }
    }
}


QPixmap MainWindow::scalePixmap(const QString& path, int width, int height)
{
    QPixmap pixmap(path);
    if (pixmap.isNull()) {
        qWarning() << "无法加载图片:" << path;
        return QPixmap();
    }
    return pixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void MainWindow::setupBodyWidget()
{
    bodyWidget = new QWidget(this);
    lyricText = new QLabel(bodyWidget);
    lyricText->setText("歌词");
    lyricText->setVisible(false);
    QSpacerItem *spacer = new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Fixed);
    QVBoxLayout *bodyLayout = new QVBoxLayout(bodyWidget);

//    bodyLayout->setContentsMargins(0, 0, 20, 0);  // lrtb
//    bodyLayout->setSpacing(0);  -- 用法需要整理笔记

    infoText = new QLabel(bodyWidget);
//    likeInfo->move(bodyWidget->geometry().center() - QPoint(likeInfo->width()/2, likeInfo->height()/2));
    infoText->setText("已添加到我喜欢");
    infoText->setAlignment(Qt::AlignCenter);
    // 设置 likeInfo 为浮动窗口样式
    infoText->setAttribute(Qt::WA_TransparentForMouseEvents);  // 鼠标穿透
//    likeInfo->raise();  // 置顶显示
    // 设置 likeInfo 样式
    infoText->setStyleSheet(R"(
        QLabel {
            background-color: rgba(45, 45, 45, 100);
            color: white;
            border-radius: 8px;
            padding: 10px 16px;
            font-size: 14px;
        }
    )");
    infoText->adjustSize();
    infoText->setVisible(false);

    bodyLayout->addWidget(musicListController->view(), 1);
    bodyLayout->addItem(spacer);
    bodyLayout->addWidget(lyricText);
    lyricText->setAlignment(Qt::AlignCenter);

    bodyWidget->setLayout(bodyLayout);
}


void MainWindow::setupBottomWidget()
{
    bottomWidget = new QWidget(this);
    bottomLayout = new QVBoxLayout(bottomWidget);

    playWidget = new QWidget(bottomWidget);
    playLayout = new QHBoxLayout(playWidget);

    bottomLeftWidget = new QWidget(playWidget);
    bottomLeftLayout = new QHBoxLayout(bottomLeftWidget);

    bottomCenterWidget = new QWidget(playWidget);
    bottomCenterLayout = new QHBoxLayout(bottomCenterWidget);

    bottomRightWidget = new QWidget(playWidget);
    bottomRightLayout = new QHBoxLayout(bottomRightWidget);

    progressSlider = new CustomSlider( Qt::Horizontal, bottomWidget);
    progressSlider->setRange(0, 100);   // 待完善

    iconPlaying = new QPushButton(bottomLeftWidget);
    iconPlaying->setIcon(QIcon(StringConstants::UI::songPath));

    playingName = new QLabel(bottomLeftWidget);
    playingName->setText("歌曲名");
    playingName->setMaximumWidth(200);

    iconLike = new QPushButton(bottomLeftWidget);
    iconLike->setIcon(QIcon(StringConstants::UI::likeBlackPath));

    iconPrevious = new QPushButton(bottomCenterWidget);
    iconPrevious->setIcon(QIcon(StringConstants::UI::previousPath));
    iconPrevious->setToolTip("上一首");

    iconStartPause = new QPushButton(bottomCenterWidget);
    iconStartPause->setIcon(QIcon(StringConstants::UI::pausePath));
    iconStartPause->setToolTip("播放/暂停");

    iconNext =  new QPushButton(bottomCenterWidget);
    iconNext->setIcon(QIcon(StringConstants::UI::nextPath));
    iconNext->setToolTip("下一首");

    iconPlayMode = new QPushButton(playWidget);
    iconPlayMode->setIcon(QIcon(StringConstants::UI::modeSequencePath));
    iconPlayMode->setToolTip("播放模式");

    iconVolume = new QPushButton(bottomRightWidget);
    iconVolume->setIcon(QIcon(StringConstants::UI::volumePath));
    iconVolume->setToolTip("音量");

    volumeSlider = new VolumePopup(this);


    volumeSlider->hide();
    volumeSlider->setStyleSheet(R"(
    /* 垂直滑块的轨道 */
    QWidget{
        background-color: #FFFFFF;
        }
    QSlider::groove:vertical {
        border: 0px;
        width: 3px;           /* 垂直滑块的宽度对应水平滑块的高度 */
        background: #90A4AE;  /* 轨道背景色 */
        margin: 0px;
        border-radius: 4px;
    }

    /* 垂直滑块的手柄 */
    QSlider::handle:vertical {
        background: #2196F3;  /* 手柄颜色 */
        border: 0px;
        width: 8px;           /* 手柄宽度（水平方向）*/
        height: 8px;          /* 手柄高度（垂直方向）*/
        margin: 0 -4px;       /* 垂直滑块：第一个值控制垂直边距，第二个控制水平边距 */
        border-radius: 4px;   /* 圆角半径 */
    }

    /* 垂直滑块已填充部分（从最小值到当前位置）*/
    QSlider::add-page:vertical {
        background: #87CEEB;  /* 填充颜色 */
        border-radius: 1px;   /* 圆角 */
    }
)"
                                  );
//    volumeSlider->setMaximum(100);
//    volumeSlider->setVisible(false);

    bottomLeftLayout->addWidget(iconPlaying);
    bottomLeftLayout->addWidget(playingName);
    playingName->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    bottomLeftLayout->addWidget(iconLike);
    bottomLeftLayout->setAlignment(Qt::AlignLeft);

    //    bottomLayout->addWidget(iconPlayMode);
    bottomCenterLayout->addWidget(iconPrevious);
    bottomCenterLayout->addWidget(iconStartPause);
    bottomCenterLayout->addWidget(iconNext);

    bottomRightLayout->addWidget(iconPlayMode);
    bottomRightLayout->addWidget(iconVolume);
    bottomRightLayout->setAlignment(Qt::AlignRight);

    playLayout->addWidget(bottomLeftWidget);
    playLayout->addWidget(bottomCenterWidget);
    playLayout->addWidget(bottomRightWidget);

    bottomLayout->addWidget(progressSlider);
    bottomLayout->addWidget(playWidget);
    bottomLayout->setSpacing(0);



    progressSlider->setStyleSheet(R"(
    QSlider::groove:horizontal {
        border: 0px;
        height: 3px;
        background: #90A4AE;
        margin: 0px;
        border-radius: 4px;
    }

    QSlider::handle:horizontal {
        background: #2196F3;
        border: 0px;
        width: 8px;
        height: 8px;
        margin: -4px 0;
        border-radius: 4px;
    }

    QSlider::sub-page:horizontal {
        background: #F0F8FF;
        border-radius: 1px;
    }
    )"
    );

    iconVolume->installEventFilter(this);
    volumeSlider->installEventFilter(this);

}


void MainWindow::setupConnections()
{
    // 信号发射时携带的参数会自动传递给所有连接的槽函数。无论是传统的槽函数还是lambda表达式，只要它们的参数列表与信号匹配，就会接收到这些参数。
    connect(buttonGroup, &QButtonGroup::idClicked, this, &MainWindow::iconButtonGroupClicked);
    connect(iconClose, &QPushButton::clicked, this, &MainWindow::iconCloseClicked);
    connect(iconFavouriteMusic, &QPushButton::clicked, this, &MainWindow::iconFavouriteMusicClicked);
    connect(iconLike, &QPushButton::clicked, this, &MainWindow::iconLikeClicked);
    connect(iconLocal, &QPushButton::clicked, this, &MainWindow::iconLocalClicked);
    connect(iconMin, &QPushButton::clicked, this, &MainWindow::iconMinClicked);
    connect(iconNext, &QPushButton::clicked, this, &MainWindow::iconNextClicked);
    connect(iconPrevious, &QPushButton::clicked, this, &MainWindow::iconPreviousClicked);
    connect(iconPlayMode, &QPushButton::clicked, this, &MainWindow::iconPlayModeClicked);
    connect(iconPlayHistory, &QPushButton::clicked, this, &MainWindow::iconPlayHistoryClicked);
    connect(iconScale, &QPushButton::clicked, this, &MainWindow::iconScaleClicked);
    connect(iconSearch, &QPushButton::clicked, this, &MainWindow::iconSearchClicked);
    connect(iconSearchResult, &QPushButton::clicked, this, &MainWindow::iconSearchResultClicked);
    connect(iconSetting, &QPushButton::clicked, this, &MainWindow::iconSettingClicked);
    connect(iconSkin, &QPushButton::clicked, this, &MainWindow::iconSkinClicked);
    connect(iconStartPause, &QPushButton::clicked, this, &MainWindow::iconStartPauseClicked);
//    connect(iconVolume, &QPushButton::clicked, this, &MainWindow::iconVolumeClicked);
//    connect(iconVolume, &QPushButton, this, [=](){
//        volumeSlider
//        volumeSlider->show();
//    });

    connect(progressSlider, &QSlider::sliderMoved, playbackController, &PlaybackController::updateProgress, Qt::QueuedConnection);

    connect(this, &MainWindow::lyricShow, lyricParser, &LyricParser::lyricTextShow);
    connect(this, &MainWindow::playingTrackChanged, this, &MainWindow::playNewTrack);
    connect(this, &MainWindow::customContextMenuRequested, this, &MainWindow::showContextMenu);
    connect(this, &MainWindow::requestPlayTrack, playbackController, &PlaybackController::playTrack, Qt::QueuedConnection);
    connect(this, &MainWindow::requestPlay, playbackController, &PlaybackController::play, Qt::QueuedConnection);
    connect(this, &MainWindow::requestPause, playbackController, &PlaybackController::pause, Qt::QueuedConnection);
    connect(this, &MainWindow::requestDownloadMusic, networkManager, [=](QString savePath, QString downloadPath){
        networkManager->setSavePath(savePath);
        networkManager->downloadMusic(downloadPath);
        if (QFileInfo(savePath).size() > 0)
        {
            infoShow("下载成功");
        }
        }, Qt::QueuedConnection);

    connect(this, &MainWindow::requestDownloadInfo, networkManager, &NetworkManager::downloadMusicInfo, Qt::QueuedConnection);
    connect(this, &MainWindow::requestSearchMusic, networkManager, &NetworkManager::searchMusic, Qt::QueuedConnection);
    connect(this, &MainWindow::requestPlayMusicInfo, networkManager, &NetworkManager::playMusicInfo, Qt::QueuedConnection);

    connect(musicListController->view(), &QTableView::doubleClicked, this, &MainWindow::playSelectedTrack);

    connect(playbackController, &PlaybackController::sliderPositionChanged, progressSlider, &CustomSlider::updatePlaybackProgess, Qt::QueuedConnection);
    connect(playbackController, &PlaybackController::musicPlaybackStateChanged, this, &MainWindow::updatePlaybackState, Qt::QueuedConnection);
    connect(playbackController, &PlaybackController::musicPlayDurationChanged, progressSlider, &CustomSlider::updateDuration, Qt::QueuedConnection);
    connect(playbackController, &PlaybackController::statusChanged, this, &MainWindow::updateMediaStatus, Qt::QueuedConnection);
    connect(playbackController, &PlaybackController::requestWarningMessage, this, [=](QString title, QString text){QMessageBox::warning(0, title, text);}, Qt::QueuedConnection);

    connect(networkManager, &NetworkManager::networkReturned, this, &MainWindow::handleReturnedList, Qt::QueuedConnection);


//    connect(networkManager, &NetworkManager::networkReturned, musicListController, [&](QList<MusicTrack> * returnList){
//        if (returnList && returnList->size()>0)
//        {
//            if (networkCommand == Search)
//                musicListController->updateSearchList(returnList);
//            else if (networkCommand == Play)
//            {
//                playingInfo->playingTrack.filePath = returnList->at(0).filePath;
//                musicListController->saveUrl(&playingInfo->playingTrack);
//                emit playingTrackChanged(&playingInfo->playingTrack);
//                //musicListController->addTrackToList(PlayHistoryListType, &playingInfo->playingTrack);
//            }
//            else if (networkCommand == Download)
//            {
//                MusicTrack track = returnList->at(0);

//                track.artist = musicListController->getCurrentTrack().artist;
//                networkManager->setSavePath(QString("D:/MusicDownload/%1-%2.mp3").arg(track.artist).arg(track.title));
//                networkManager->downloadMusic(track.downloadPath);
//            }
//            networkCommand = Empty;
//        }
//    });

    connect(volumeSlider, &VolumePopup::volumeChanged, this, [=](int value){
        playingInfo->volume = value;
        playbackController->setVolume(playingInfo->volume*0.01);
        }, Qt::QueuedConnection);

}


void MainWindow::setupLeftWidget()
{
    leftWidget = new QWidget(this);
    leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setAlignment(Qt::AlignTop);

    buttonGroup = new QButtonGroup(leftWidget);

    musicLogo = new QLabel(leftWidget);
    musicLogo->setPixmap(QIcon(StringConstants::UI::logoPath).pixmap(64, 64));
    musicLogo->setAlignment(Qt::AlignCenter);

//    iconRecommend = new QPushButton(leftWidget);
//    iconRecommend->setText("  推荐");
//    iconRecommend->setIcon(QIcon(StringConstants::UI::recommendPath));

//    iconMusicHouse = new QPushButton(leftWidget);
//    iconMusicHouse->setText("  乐库");
//    iconMusicHouse->setIcon(QIcon(StringConstants::UI::musicHousePath));

    iconFavouriteMusic = new QPushButton(leftWidget);
    iconFavouriteMusic->setText("  我喜欢的音乐");
    iconFavouriteMusic->setIcon(QIcon(StringConstants::UI::favouriteMusicPath));

    iconPlayHistory = new QPushButton(leftWidget);
    iconPlayHistory->setText("  播放历史");
    iconPlayHistory->setIcon(QIcon(StringConstants::UI::playHistoryPath));

    iconLocal = new QPushButton(leftWidget);
    iconLocal->setText("  本地音乐");
    iconLocal->setIcon(QIcon(StringConstants::UI::localPath));

    iconSearchResult = new QPushButton(leftWidget);
    iconSearchResult->setText("  搜索结果");
    iconSearchResult->setIcon(QIcon(StringConstants::UI::searchPath));

//    buttonGroup->addButton(iconRecommend, 1);
//    buttonGroup->addButton(iconMusicHouse,)

    buttonGroup->addButton(iconFavouriteMusic, 1);
    buttonGroup->addButton(iconPlayHistory, 2);
    buttonGroup->addButton(iconLocal, 3);
    buttonGroup->addButton(iconSearchResult, 4);
    buttonGroup->setExclusive(true); // 互斥

    leftLayout->addWidget(musicLogo);
//    leftLayout->addWidget(iconRecommend);
//    leftLayout->addWidget(iconMusicHouse);
    leftLayout->addWidget(iconFavouriteMusic);
    leftLayout->addWidget(iconPlayHistory);
    leftLayout->addWidget(iconLocal);
    leftLayout->addWidget(iconSearchResult);
}

void MainWindow::setupRightClickedMenu()
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    rightClickedMenu = new QMenu(this);
    QAction *playAction = rightClickedMenu->addAction("播放");
    QAction *likeAction = rightClickedMenu->addAction("添加/取消喜欢");
    QAction *downloadAction = rightClickedMenu->addAction("下载");
    QAction *deleteAction = rightClickedMenu->addAction("删除");

    connect(playAction, &QAction::triggered, this, [=]()
    {
        playSelectedTrack(musicListController->getCurrentIndex());
    });

    connect(likeAction, &QAction::triggered, this, &MainWindow::iconLikeClicked);

    connect(downloadAction, &QAction::triggered, this, &MainWindow::iconDownloadClicked);

    connect(deleteAction, &QAction::triggered, this, &MainWindow::iconDeleteClicked);

//    rightClickedMenu->setStyleSheet("QWidget{background-color: ;}"
//                                    "QMenu::item::selected{background-color: #F8F8FF;}"
//                                   );

    // 设置菜单样式表
    rightClickedMenu->setStyleSheet(
        "QMenu {"
        "   background-color: #FFFFFF;"           // 菜单背景色
        "   color: #696969;"                      // 默认文字颜色
        "   border: 1px solid #FFFFFF;"
        "   padding: 5px;"
        "}"
        "QMenu::item {"
        "   background-color: transparent;"       // 默认透明背景
        "   padding: 6px 20px 6px 10px;"         // 上下左右内边距
        "   border: 1px solid transparent;"      // 透明边框
        "}"
        "QMenu::item:selected {"
        "   background-color: #F0F8FF;"          // 选中时的背景色
        "   color: #696969;"                     // 选中时的文字颜色（白色）
        "   border: 1px solid #555555;"
        "}"
        "QMenu::item:disabled {"
        "   color: #666666;"                     // 禁用状态的颜色
        "}"
        "QMenu::separator {"
        "   height: 1px;"
        "   background-color: #444444;"
        "   margin: 5px 0px;"
        "}"
        );
}

void MainWindow::setupRightWidget()
{
    rightWidget = new QWidget(this);
    rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setAlignment(Qt::AlignTop);

    // 设置布局属性
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(20);
}

void MainWindow::setupTitleWidget()
{
    titleWidget = new QWidget(this);
    titleLayout = new QHBoxLayout(titleWidget);

    inputSearch = new QLineEdit(titleWidget);
    inputSearch->setMaximumWidth(250);
    inputSearch->setPlaceholderText("搜索歌曲");

    iconSearch = new QPushButton(titleWidget);
    iconSearch->setIcon(QIcon(StringConstants::UI::searchPath));
    iconSearch->setToolTip("搜索");

    iconSetting = new QPushButton(titleWidget);
    iconSetting->setIcon(QIcon(StringConstants::UI::settingPath));
    iconSetting->setToolTip("设置");

    iconSkin = new QPushButton(titleWidget);
    iconSkin->setIcon(QIcon(StringConstants::UI::skinPath));
    iconSkin->setToolTip("换肤");

    iconMin = new QPushButton(titleWidget);
    iconMin->setIcon(QIcon(StringConstants::UI::minPath));
    iconMin->setToolTip("最小化");

    iconScale = new QPushButton(titleWidget);
    iconScale->setIcon(QIcon(StringConstants::UI::maxPath));
    iconScale->setToolTip("最大化");

    iconClose = new QPushButton(titleWidget);
    iconClose->setIcon(QIcon(StringConstants::UI::closePath));
    iconClose->setToolTip("关闭");

    titleLayout->addWidget(inputSearch);
    titleLayout->addWidget(iconSearch);
    titleLayout->addStretch();
    titleLayout->addWidget(iconSetting);
    titleLayout->addWidget(iconSkin);
    titleLayout->addWidget(iconMin);
    titleLayout->addWidget(iconScale);
    titleLayout->addWidget(iconClose);

}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    mainWidget = new QWidget(centralWidget);

    centralLayout = new QVBoxLayout(centralWidget);
    mainLayout = new QHBoxLayout(mainWidget);

    // 完全移除标题栏（包括最小化、最大化、关闭按钮）
    setWindowFlags(Qt::FramelessWindowHint);
    setMenuBar(nullptr);

    setupLeftWidget();
    setupRightWidget();
    setupBottomWidget();
    setupTitleWidget();
    setupBodyWidget();
    setupRightClickedMenu();


    if(titleWidget)
    {
        // 设置按钮透明
        titleWidget->setStyleSheet(
            "QPushButton {"
            "    background: transparent;"
            "    border: none;"
            "    border-radius: 3px;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(255, 255, 255, 0.8);"
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(255, 255, 255, 1);"
            "}"
            "QPushButton:disabled {"
            "    background-color: rgba(255, 255, 255, 0.05);"
            "}"
            );

        rightLayout->addWidget(titleWidget);
    }

    if(bodyWidget)
    {
//        qDebug() << bodyWidget->size();
        bodyWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        bodyWidget->setStyleSheet(
            "QWidget{"
            "   font-size: 8pt;"
            "   color: 	#696969;"
            "}"
            "QTableView {"
            "   background-color: transparent;"
            "   border: none;"
            "   outline: none;"
            "   gridline-color: transparent;"
            "}"
            "QTableView::item {"
            "   background-color: transparent;"
            "   border: none;"
            "}"
            "QTableView::item:selected {"
            "   background-color: rgba(100, 100, 100, 50);" // 半透明的深灰色背景
            "   color: #ffffff;" // 选中时文字变为白色
            "   border: none;"
            "}"
            "QLabel{"
            "   qproperty-alignment: AlignCenter;"
            "}"
            );

        rightLayout->addWidget(bodyWidget, 1);
    }

    if(mainWidget)
        centralLayout->addWidget(mainWidget);
    centralLayout->addStretch();
    if(bottomWidget)
    {
        bottomWidget->setStyleSheet(
            "QWidget{"
            " background-color:#FFFFFF;"
            "}"
            "QPushButton {"
            "    background: transparent;"
            "    border: none;"
            "    border-radius: 3px;"
            "    width: 60px;"
            "    height: 40px;"
            "    color: #708090;"
            "    text-align: center;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(230, 230, 250, 0.5);"
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(230, 230, 250, 0.8);"
            "}"
            "QPushButton:disabled {"
            "    background-color: rgba(255, 255, 255, 0.05);"
            "}"
            );

        centralLayout->addWidget(bottomWidget);
    }

    centralLayout->setContentsMargins(0, 0, 0, 0);    //ltrb

    if (leftWidget)
    {
        leftWidget->setStyleSheet(
            "QWidget{"
            " background-color:#F0F8FF;"
            "}"
            "QPushButton {"
            "    background: transparent;"
            "    border: none;"
            "    border-radius: 3px;"
            "    width: 130px;"
            "    height: 50px;"
            "    color: #708090;"
            "    text-align: left;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(255, 255, 255, 1);"
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(255, 255, 255, 1);"
            "}"
            "QPushButton:disabled {"
            "    background-color: rgba(255, 255, 255, 0.05);"
            "}"
            "QPushButton[highlighted=\"true\"] "
                "{ background-color: rgba(255, 255, 255, 0.8); }"
            );
        mainLayout->addWidget(leftWidget);
    }

    if (rightWidget)
        mainLayout->addWidget(rightWidget);

    setCentralWidget(centralWidget);

    centralWidget->setStyleSheet("QWidget { background-color:#F8F8FF; margin: 0px; padding: 0px; border: none; }");
    centralLayout->setSpacing(0);
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    if (musicListController->hasSelected == false)
        return;

    QPoint globalPos = mapToGlobal(pos);
    rightClickedMenu->exec(globalPos);
}

void MainWindow::showEvent(QShowEvent *event) {
    centerWindow();
    QMainWindow::showEvent(event);
    //    qDebug() << "MainWindow size:" << size();
    //    qDebug() << "centralWidget size:" << centralWidget->size();
    //    qDebug() << "mainWidget size:" << mainWidget->size();
    //    qDebug() << "leftWidget size:" << leftWidget->size();
    //    qDebug() << "rightWidget size:" << rightWidget->size();
    //    qDebug() << "titleWidget size:" << titleWidget->size();
    //    qDebug() << "bodyWidget size:" << bodyWidget->size();
    //    qDebug() << "musicListWidget size:" << musicListWidget->size();
    //    qDebug() << "musicListWidget viewport size:" << musicListWidget->viewport()->size();
}

void MainWindow::startDragging(const QPointF &globalPos)
{
    if(isMaximized() || isFullScreen())
        return;

    isDragging = true;
    dragStartPosition = globalPos - frameGeometry().topLeft();      // 鼠标相对窗口左上角的偏移量，在拖动过程中保持这个偏移不变，让窗口跟随鼠标平滑移动而不跳动。
}


void MainWindow::stopDragging()
{
    isDragging = false;
}

void MainWindow::updateDragging(const QPointF &globalPos)
{
    QPointF newPosition = globalPos - dragStartPosition;
    move(newPosition.toPoint());
}

void MainWindow::updatePlaybackState(QMediaPlayer::PlaybackState newState)
{
    currentPlaybackState = newState;
    if (currentPlaybackState == QMediaPlayer::PlayingState)
    {
        iconStartPause->setIcon(QIcon(StringConstants::UI::startPath));
    }
    else
    {
        iconStartPause->setIcon(QIcon(StringConstants::UI::pausePath));
    }
}

void MainWindow::updateLyric(QString text)
{
    lyricText->setText(text);
}

void MainWindow::updateMediaStatus(QMediaPlayer::MediaStatus status)
{
    switch(status)
    {
    case QMediaPlayer::EndOfMedia:
        qDebug() << playingInfo->playingTrack.title << " 播放结束";
        handlePlaybackFinished();
        break;
    case QMediaPlayer::InvalidMedia:
        qWarning() << "无效媒体文件：" << playingInfo->playingTrack.filePath;
        emit iconNext->clicked();  // 跳过无效文件
        break;
    default: break;
    }
}

void MainWindow::updatePlayingTrackInfo()
{
    playingInfo->playingTrack.like = musicListController->trackExists(LikeListType, &playingInfo->playingTrack);

    if (playingInfo->playingTrack.like == true)
    {
        iconLike->setIcon(QIcon(StringConstants::UI::likeRedPath));
    }
    else
    {
        iconLike->setIcon(QIcon(StringConstants::UI::likeBlackPath));
    }

    playingName->setText(playingInfo->playingTrack.title);
}


