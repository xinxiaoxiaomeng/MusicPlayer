#include "musiclistview.h"
#include <QListView>
#include <QHeaderView>
#include "musiclistmodel.h"
#include <QFontMetrics>
#include "customdelegate.h"

MusicListView::MusicListView(QWidget *parent):
    QTableView(parent)
{
    setupUI();
    setupConnections();
}

MusicListView::~MusicListView()
{

}

void MusicListView::setupUI()
{
    //this->setModel(musicListModel);

    // 设置列宽
//    this->setColumnWidth(Columns::indexColumn, 30);
//    this->setColumnWidth(Columns::titleColumn, 200);
//    this->setColumnWidth(Columns::artistColumn, 100);
//    this->setColumnWidth(Columns::albumColumn, 200);
//    this->setColumnWidth(Columns::likeColumn, 30);
//    this->setColumnWidth(Columns::durationColumn, 50);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 表头设置
    horizontalHeader()->setStretchLastSection(true);
    //horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    // 其他设置
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setAlternatingRowColors(true);
    verticalHeader()->setVisible(false);

    // 设置滚动条策略
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    this->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    this->setStyleSheet("font-size:8pt; color:#696969;");

    QFontMetrics fm(this->font());
    int rowHeight = fm.height() * 2.5; // 假设行高是字体高度的1.5倍
    this->verticalHeader()->setDefaultSectionSize(rowHeight);

    int m_visibleRows = 10;

    // 计算表头高度（如果表头可见）
    int headerHeight = this->horizontalHeader()->height();
    // 如果表头高度为0（可能还没显示）
    if (headerHeight == 0) {
        headerHeight = fm.height() * 1.5; // 表头高度比行高稍小
    }

    // 计算总高度
    int totalHeight = headerHeight + m_visibleRows * rowHeight;

    // 设置固定高度
    this->setFixedHeight(totalHeight);

    // 单独设置水平表头的样式表
    this->horizontalHeader()->setStyleSheet(
        "QTbaleView{"
        "   font: 15pt: \"宋体\";"
        "}"
        "QHeaderView::section {"
        "   background-color: transparent;"
        "   border: none;"
        "   text-align:left;"
        "}"
        );

    // 为标题列设置自定义委托
    CustomDelegate *delegate = new CustomDelegate(this);
    this->setItemDelegateForColumn(titleColumn, delegate);

    // 禁用水平滚动条
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void MusicListView::setupConnections()
{
}

void MusicListView::setMusicListModel(MusicListModel *model)
{
    if (model)
    {
        this->setModel(model);
        this->setColumnHidden(4, true);
        this->setColumnHidden(5, true);
        this->setColumnHidden(6, true);
        this->setColumnHidden(7, true);
    }

}
