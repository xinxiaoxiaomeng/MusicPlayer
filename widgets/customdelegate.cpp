#include "customdelegate.h"
#include <QApplication>
#include "stringconstants.h"


CustomDelegate::CustomDelegate(QObject *parent): QStyledItemDelegate(parent)
{
    defaultIcon = QPixmap(StringConstants::UI::logoPath);
}

CustomDelegate::~CustomDelegate()
{

}


void CustomDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    // 不要调用 QStyledItemDelegate::paint(painter, option, index);
    // 而是自己实现完整的绘制逻辑

    // 1. 保存 painter 状态
    painter->save();

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // 2. 清空文本和图标，因为我们自己绘制
    opt.text = "";           // 清空文本，避免默认绘制
    opt.icon = QIcon();      // 清空图标，避免默认绘制
    opt.features &= ~QStyleOptionViewItem::HasDisplay;  // 可选：移除显示特性

    // 3. 使用 QStyle 绘制背景和焦点框
    const QWidget *widget = option.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);


    // 4. 绘制图标（左侧）
    QRect iconRect = option.rect.adjusted(5, 5, -5, -5);
    iconRect.setWidth(50); // 固定图标宽度

    QPixmap icon(defaultIcon);
    if (!icon.isNull()) {
        // 缩放并居中显示图标
        icon = icon.scaled(iconRect.height(), iconRect.height(),
                           Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int x = iconRect.left() + (iconRect.width() - icon.width()) / 2;
        int y = iconRect.top() + (iconRect.height() - icon.height()) / 2;
        painter->drawPixmap(x, y, icon);
    }

    // 5. 绘制文本（右侧）
    QString text = index.data(Qt::DisplayRole).toString();  // 原来的文本数据
    if (!text.isEmpty()) {
        QRect textRect = option.rect;
        textRect.setLeft(iconRect.right() + 10); // 从图标右侧开始
        textRect.setRight(textRect.right() - 5); // 右边留出边距

        // 设置字体
        QFont font = painter->font();
        font.setPointSize(9);
        painter->setFont(font);

        // 绘制文本，支持省略号
        QString elidedText = painter->fontMetrics().elidedText(
            text, Qt::ElideRight, textRect.width());
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, elidedText);
    }

    // 6. 恢复 painter 状态
    painter->restore();
}

QSize CustomDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 返回一个合适的单元格大小
    QSize baseSize = QStyledItemDelegate::sizeHint(option, index);
    return QSize(baseSize.width(), 40); // 设置行高为40像素
}
