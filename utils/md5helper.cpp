#include "md5helper.h"



// 加密文件
QString MD5Helper::encryptFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件：" << filePath;
        return QString();
    }

    QCryptographicHash hash(QCryptographicHash::Md5);

    // 分块读取大文件，避免内存占用过高
    const qint64 bufferSize = 1024 * 1024; // 1MB
    QByteArray buffer(bufferSize, 0);

    qint64 bytesRead;
    while ((bytesRead = file.read(buffer.data(), bufferSize)) > 0) {
        hash.addData(buffer.constData(), bytesRead);
    }

    file.close();
    return hash.result().toHex();
}

// 加密字符串
QString MD5Helper::encryptString(const QString &input)
{
    if (input.isEmpty()) {
        return QString();
    }

    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(input.toUtf8());
    return hash.result().toHex();
}

// 生成加盐的MD5（更安全）
QString MD5Helper::encryptWithSalt(const QString &input, const QString &salt)
{
    QString combined = input + salt;
    return encryptString(combined);
}

// 验证MD5值
bool MD5Helper::verify(const QString &input, const QString &md5Hash)
{
    QString calculatedHash = encryptString(input);
    return (calculatedHash.toUpper() == md5Hash.toUpper());
}
