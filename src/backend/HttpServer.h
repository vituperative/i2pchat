// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef HTTPSERVER_H__
#define HTTPSERVER_H__

#include <QByteArray>
#include <QFileInfo>
#include <QString>

struct HttpRequest {
  QString method;
  QString path;
  QString authUser;
  QString authPassword;
  QString sessionToken;
};

class CSimpleHttpServer {
public:
  static HttpRequest parseRequest(const QByteArray &raw);
  static QFileInfo resolvePath(const QString &wwwRoot, const QString &requestPath);
  static QByteArray buildResponse(const HttpRequest &req,
                                  const QFileInfo &file,
                                  const QString &wwwRoot,
                                  const QString &nickname,
                                  const QByteArray &avatarBase64,
                                  const QString &myDest);
  static QByteArray buildDirectoryListing(const QFileInfo &dir, const QString &requestPath);
  static QByteArray buildAuthRequiredResponse(const QString &realm);
  static QByteArray buildErrorResponse(int status, const QString &statusText);
  static QByteArray tryCustomErrorPage(const QString &docroot, int statusCode, const QString &statusText);
  static QByteArray minifyBody(const QString &filename, const QByteArray &body);

  static QByteArray buildHeader(int statusCode,
                                const QString &statusText,
                                qint64 contentLength,
                                const QString &contentType,
                                const QString &csp);

private:
  static QString mimeTypeForName(const QString &filename);
  static QString generateCSP();
};

#endif
