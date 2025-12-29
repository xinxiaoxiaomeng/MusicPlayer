#ifndef MD5HELPER_H
#define MD5HELPER_H

#include <QObject>
#include <QCryptographicHash>
#include <QFile>
#include <QMessageBox>
#include <QDebug>

class MD5Helper : public QObject
{
    Q_OBJECT

public:
    explicit MD5Helper(QObject *parent = nullptr) : QObject(parent) {}

    QString encryptString(const QString &input);

    QString encryptFile(const QString &filePath);

    bool verify(const QString &input, const QString &md5Hash);

    QString encryptWithSalt(const QString &input, const QString &salt);
};

#endif // MD5HELPER_H
