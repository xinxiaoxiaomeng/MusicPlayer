#ifndef CUSTOMSLIDER_H
#define CUSTOMSLIDER_H

#include <QSlider>

class CustomSlider: public QSlider
{
    Q_OBJECT

public:
    CustomSlider();
    ~CustomSlider();
    explicit CustomSlider(Qt::Orientation orientation, QWidget *parent = nullptr) : QSlider(orientation, parent)
    {}

protected:
    void mousePressEvent(QMouseEvent *event) override;
    QString msToTimeString(qint64 ms, bool withHours = false);
    qint64 timeStringToMs(const QString &timeStr);

public slots:
    void updateDuration(qint64 duration);
    void updatePlaybackProgess(qint64 position);
};

#endif // CUSTOMSLIDER_H
