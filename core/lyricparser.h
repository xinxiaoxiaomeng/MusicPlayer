#ifndef LYRICPARSER_H
#define LYRICPARSER_H

#include <QObject>

class LyricParser: public QObject
{
    Q_OBJECT

public:
    explicit LyricParser(QObject *parent = nullptr);
    ~LyricParser();

public slots:
    void lyricTextShow(QString);
};

#endif // LYRICPARSER_H
