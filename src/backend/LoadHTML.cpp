// SPDX-License-Identifier: AGPL-3.0-or-later

#include "Core.h"
#include "FileTransferSend.h"
#include "I2PStream.h"
#include "Protocol.h"
#include "User.h"
#include "UserManager.h"

#include <QErrorMessage>
#include <QSettings>

QString gethttpheader(QString pagedata) {
  QString header;
  header = "HTTP/1.0 200\r\nContent-Length: " + QString::number(pagedata.size()) + "\r\n" +
           "Content-Security-Policy: default-src 'none'; style-src 'unsafe-inline'; "
           "img-src data:\r\n" +
           "X-XSS-Protection: 1; mode=block\r\n" + "X-Content-Type-Options: nosniff\r\n" +
           "Content-Type: text/html; charset=utf-8\r\n\r\n";
  return header;
}

QString pngtobase64(QByteArray pngdata) {
  return pngdata.toBase64();
}

// QString processtags(QString htmldata) {
//	htmldata.
//}

QString loadfile(QString filename) {

  QString filecontents;
  QFile f(filename);
  if (!f.open(QFile::ReadOnly | QFile::Text)) {
    return "";
  }
  QTextStream in(&f);
  filecontents = in.readAll();
  f.close();
  return filecontents;
}
