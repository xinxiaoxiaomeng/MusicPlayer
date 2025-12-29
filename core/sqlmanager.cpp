#include "sqlmanager.h"
#include <QMessageBox>
#include <QFile>
#include <QRegularExpression>
#include <QCoreApplication>
#include <QDebug>
#include "stringconstants.h"


SqlManager::SqlManager(QObject *parent):
    QObject(parent)
{

    //qDebug() << QCoreApplication::applicationDirPath();
    sqls = loadSqlMap(":/resources/commandSql.sql");
    connectDatabase();
}

SqlManager::~SqlManager()
{

}

void SqlManager::connectDatabase()
{
    // 1. 判断数据库连接是否存在
    if(QSqlDatabase::contains("sql_default_connection"))
    {
        database = QSqlDatabase::database("sql_default_connection");
    }
    else
    {
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName(StringConstants::Settings::databasePath);
    }

    // 2. 打开数据库
    if(!database.open())
    {
        QMessageBox::critical(0, QObject::tr("打开数据库失败!"), database.lastError().text());
    }
    else
    {
        // 定义query对象，得到打开的数据库表示
        QSqlQuery query;

        // 搜索记录
        if(!query.exec(sqls["create_searchList"]))
        {
            QMessageBox::critical(0, QObject::tr("create searchList table error!"), database.lastError().text());
        }

        // 播放记录
        if(!query.exec(sqls["create_playHistoryList"]))
        {
            QMessageBox::critical(0, QObject::tr("create playHistoryList table error!"), database.lastError().text());
        }

        // 本地音乐
        if(!query.exec(sqls["create_localList"]))
        {
            QMessageBox::critical(0, QObject::tr("create localList table error!"), database.lastError().text());
        }

        // 收藏列表
        if(!query.exec(sqls["create_likeList"]))
        {
            QMessageBox::critical(0, QObject::tr("create likeList table error!"), database.lastError().text());
        }
    }
}

QSqlQuery SqlManager::execuateSql(QString sql)
{
    QSqlQuery query;
    if(!query.exec(sql))
    {
        QMessageBox::warning(0, "警告", "数据库操作失败！");
    }
    return query;
}

QList<MusicTrack>* SqlManager::getMusicListBySql(QString sql)
{
    QSqlQuery query;

//    if(!query.exec(sql))
//    {
//        QMessageBox::critical(0, QObject::tr("search information error!"), database.lastError().text());
//    }
    query = execuateSql(sql);

    QList<MusicTrack> *musicList = new QList<MusicTrack>();
    while(query.next())
    {
        //qDebug() << "处理搜索结果";
        MusicTrack track;
        track.index = query.value("id").toInt();
        track.title = query.value("title").toString();
        track.artist = query.value("artist").toString();
        track.album = query.value("album").toString();
//        track.duration = query.value("duration").toString();
//        track.like = query.value("like").toBool();
        track.filePath = query.value("filePath").toString();
        track.album_id = query.value("album_id").toString();
        track.hash = query.value("hash").toString();

        musicList->append(track);
    }

    return musicList;
}

bool SqlManager::hasField(const QSqlQuery &query, const QString &fieldName)
{
    // 获取查询结果的记录结构（即使没有调用next也可以获取）
    QSqlRecord record = query.record();

    // 检查字段是否存在
    int fieldIndex = record.indexOf(fieldName);

    // 如果返回-1表示字段不存在，否则存在
    return fieldIndex != -1;
}

QHash<QString, QString> SqlManager::loadSqlMap(const QString& sqlFile)
{
    QHash<QString, QString> sqlMap;                             // 用于存储解析后的sql

    QFile file(sqlFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open SQL file:" << sqlFile;
        return sqlMap;
    }

    QString content = QString::fromUtf8(file.readAll());

    static QRegularExpression re(
        R"(--\s*name:\s*(\w+)\s*\n(.*?)(?=\n--\s*name:|$))",
        QRegularExpression::DotMatchesEverythingOption
        );

    auto matches = re.globalMatch(content);
    while (matches.hasNext()) {
        auto m = matches.next();
        QString name = m.captured(1);
        QString sql  = m.captured(2).trimmed();
        sqlMap.insert(name, sql);
    }

    return sqlMap;
}

