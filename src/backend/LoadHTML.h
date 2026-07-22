#ifndef LOADHTML_H__
#define LOADHTML_H__

#include <QByteArray>
#include <QString>

QString gethttpheader(QString pagedata);
QString pngtobase64(QByteArray pngdata);
QString loadfile(QString filename);

#endif
