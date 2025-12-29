#ifndef VOLUMEPOPUP_H
#define VOLUMEPOPUP_H

#include <QObject>
#include <QSlider>
#include <QWidget>

class VolumePopup : public QWidget
{
    Q_OBJECT
public:
    explicit VolumePopup(QWidget *parent = nullptr);

protected:
    void leaveEvent(QEvent *event) override;

signals:
    void volumeChanged(int value);

private:
    QSlider *slider;
};


#endif // VOLUMEPOPUP_H
