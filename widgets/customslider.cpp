#include "customslider.h"
#include <QMouseEvent>
#include <QDebug>

CustomSlider::CustomSlider()
{
    // 设置窗口属性：无边框、透明背景、始终置顶
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
}

CustomSlider::~CustomSlider()
{

}

//void CustomSlider::mousePressEvent(QMouseEvent *event)
//{
//    QSlider::mousePressEvent(event);

//    if (event->button() == Qt::LeftButton) {
//        // 获取鼠标点击位置相对于Slider的坐标
//        int currentPos = (orientation() == Qt::Horizontal) ? event->pos().x() : event->pos().y();

//        // 计算点击位置对应的值
//        double percentage = static_cast<double>(currentPos) /
//                            (orientation() == Qt::Horizontal ? width() : height());

//        int range = maximum() - minimum();
//        int value = minimum() + percentage * range;

//        // 设置新值
//        setValue(value);

//        // 发送滑块移动信号
//        emit sliderMoved(value);
//    }
//}

void CustomSlider::mousePressEvent(QMouseEvent *event)
{
    // 先调用基类处理，确保基础功能正常
    QSlider::mousePressEvent(event);

    if (event->button() == Qt::LeftButton) {
        // 获取鼠标点击位置
        QPoint clickPos = event->pos();
        double percentage = 0.0;

        if (orientation() == Qt::Horizontal) {
            // 水平滑块：从左到右，0%到100%
            percentage = static_cast<double>(clickPos.x()) / width();
        } else {
            // 垂直滑块：从下到上，0%到100%（需要反转y坐标）
            percentage = 1.0 - (static_cast<double>(clickPos.y()) / height());
        }

        // 确保百分比在[0,1]范围内
        percentage = qBound(0.0, percentage, 1.0);

        // 计算对应的滑块值
        int range = maximum() - minimum();
        int value = minimum() + qRound(percentage * range);

        // 设置新值
        setValue(value);

        // 发送滑块移动信号
        emit sliderMoved(value);

        // 也可以发送valueChanged信号，如果需要
        emit valueChanged(value);
    }
}

// 毫秒数 → 格式化的时间字符串（用于显示）
QString CustomSlider::msToTimeString(qint64 ms, bool withHours) {
    int totalSeconds = ms / 1000;
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;

    if (withHours || hours > 0) {
        // 显示时:分:秒，如 "1:05:30"
        return QString("%1:%2:%3")
            .arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    } else {
        // 显示分:秒，如 "65:30"
        return QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    }
}

// 自动检测数据库格式并转换（推荐）
//QString autoFormatTime(qint64 ms, const QString &originalFormat) {
//    // 根据原格式决定是否显示小时
//    bool showHours = originalFormat.contains(':') &&
//                     originalFormat.split(':').size() == 3;
//    return msToTimeString(ms, showHours);
//}

// 字符串时间 —> ms
qint64 CustomSlider::timeStringToMs(const QString &timeStr) {
    // 支持格式: "3:10" (分:秒), "1:23:45" (时:分:秒), "45" (秒)
    QStringList parts = timeStr.split(':');
    qint64 totalMs = 0;

    if (parts.size() == 1) {                    // 只有秒，如 "45"
        totalMs = parts[0].toInt() * 1000;
    }
    else if (parts.size() == 2) {               // 分:秒，如 "3:10"
        totalMs = (parts[0].toInt() * 60 + parts[1].toInt()) * 1000;
    }
    else if (parts.size() == 3) {               // 时:分:秒，如 "1:23:45"
        totalMs = (parts[0].toInt() * 3600 + parts[1].toInt() * 60 + parts[2].toInt()) * 1000;
    }
    else {
        qWarning() << "无法解析的时间格式:" << timeStr;
        return 0;
    }

    //qDebug() << totalMs;
    return totalMs;
}


void CustomSlider::updateDuration(qint64 duration)
{
    //setMaximum(timeStringToMs(duration));
//    qDebug() << duration;
    setMaximum(duration);
}

void CustomSlider::updatePlaybackProgess(qint64 position)
{
//    qDebug() << position;
    setSliderPosition(position);
}
