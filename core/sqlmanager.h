#ifndef SQLMANAGER_H
#define SQLMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include "types.h"

class SqlManager: public QObject
{
    Q_OBJECT
public:
    explicit SqlManager(QObject *parent=nullptr);
    ~SqlManager();

    QHash<QString, QString> sqls;

    QSqlQuery execuateSql(QString sql);
    QList<MusicTrack>* getMusicListBySql(QString sql);


private:

    QSqlDatabase database;

    void connectDatabase();   // 判断连接状态
    bool hasField(const QSqlQuery &query, const QString &fieldName);
    QHash<QString, QString> loadSqlMap(const QString& sqlFile);

};

#endif // SQLMANAGER_H
