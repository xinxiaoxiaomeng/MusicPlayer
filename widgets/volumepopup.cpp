#include "volumepopup.h"
#include <QVBoxLayout>

VolumePopup::VolumePopup(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    slider = new QSlider(Qt::Vertical, this);
    slider->setRange(0, 100);
    slider->setValue(50);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 6, 6, 6);  // lrtp
    layout->addWidget(slider);

    connect(slider, &QSlider::valueChanged,
            this, &VolumePopup::volumeChanged);
}

void VolumePopup::leaveEvent(QEvent *)
{
    hide();
}
