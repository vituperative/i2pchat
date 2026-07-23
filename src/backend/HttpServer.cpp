// SPDX-License-Identifier: AGPL-3.0-or-later

#include "HttpServer.h"

#include "Protocol.h"

#include <QDir>
#include <QFile>
#include <QMimeDatabase>
#include <QMimeType>
#include <QUrl>

static bool hasNullBytes(const QString &s) {
  for (int i = 0; i < s.size(); i++) {
    if (s.at(i).unicode() == 0)
      return true;
  }
  return false;
}

HttpRequest CSimpleHttpServer::parseRequest(const QByteArray &raw) {
  HttpRequest req;
  req.method = QStringLiteral("GET");
  req.path = QStringLiteral("/");

  // Minimum valid request: "GET /x HTTP/1.0" = 15 bytes
  if (raw.size() < 15)
    return req;

  int firstSpace = raw.indexOf(' ');
  if (firstSpace <= 0)
    return req;
  int secondSpace = raw.indexOf(' ', firstSpace + 1);
  if (secondSpace <= firstSpace + 1)
    return req;

  QByteArray methodBytes = raw.left(firstSpace);
  QByteArray pathBytes = raw.mid(firstSpace + 1, secondSpace - firstSpace - 1);

  // Only accept GET and HEAD
  if (methodBytes != "GET" && methodBytes != "HEAD") {
    req.method = QStringLiteral("UNSUPPORTED");
    return req;
  }

  req.method = QString::fromUtf8(methodBytes);
  req.path = QString::fromUtf8(pathBytes);

  if (hasNullBytes(req.path))
    req.path = QStringLiteral("/");

  // Parse Cookie header for session token
  QByteArray lower = raw.toLower();
  int cookieIdx = lower.indexOf("cookie: ");
  if (cookieIdx >= 0) {
    int lineEnd_ = raw.indexOf('\n', cookieIdx);
    if (lineEnd_ < 0)
      lineEnd_ = raw.size();
    QByteArray cookieLine = raw.mid(cookieIdx + 8, lineEnd_ - cookieIdx - 8).trimmed();
    // Look for I2PChatSession=<token>
    int sessIdx = cookieLine.indexOf("i2pchatsession=");
    if (sessIdx >= 0) {
      sessIdx += 15; // len of "i2pchatsession="
      int semi = cookieLine.indexOf(';', sessIdx);
      int end = (semi >= 0) ? semi : cookieLine.size();
      req.sessionToken = QString::fromUtf8(cookieLine.mid(sessIdx, end - sessIdx).trimmed());
    }
  }

  // Parse Authorization: Basic header
  int headerStart = raw.indexOf('\n', secondSpace + 1);
  if (headerStart > 0) {
    QByteArray lower = raw.toLower();
    int authIdx = lower.indexOf("authorization: basic ", headerStart);
    if (authIdx >= 0) {
      int valueStart = authIdx + 21; // len of "Authorization: Basic "
      int lineEnd = raw.indexOf('\n', valueStart);
      if (lineEnd < 0)
        lineEnd = raw.size();
      QByteArray b64 = raw.mid(valueStart, lineEnd - valueStart).trimmed();
      QByteArray decoded = QByteArray::fromBase64(b64);
      int colon = decoded.indexOf(':');
      if (colon > 0) {
        req.authUser = QString::fromUtf8(decoded.left(colon));
        req.authPassword = QString::fromUtf8(decoded.mid(colon + 1));
        if (hasNullBytes(req.authUser) || hasNullBytes(req.authPassword)) {
          req.authUser.clear();
          req.authPassword.clear();
        }
      }
    }
  }

  return req;
}

QFileInfo CSimpleHttpServer::resolvePath(const QString &wwwRoot, const QString &requestPath) {
  QDir wwwDir(wwwRoot);
  QString rootCanonical = wwwDir.canonicalPath();
  if (rootCanonical.isEmpty())
    return QFileInfo();

  if (hasNullBytes(requestPath))
    return QFileInfo();

  QString decoded = QUrl::fromPercentEncoding(requestPath.toUtf8());

  // Reject null bytes after decoding
  if (hasNullBytes(decoded))
    return QFileInfo();

  int qmark = decoded.indexOf('?');
  if (qmark >= 0)
    decoded = decoded.left(qmark);

  // Reject fragment identifiers
  int hashPos = decoded.indexOf('#');
  if (hashPos >= 0)
    decoded = decoded.left(hashPos);

  if (decoded == QLatin1String("/") || decoded.isEmpty())
    decoded = QStringLiteral("/index.html");

  QString cleaned = QDir::cleanPath(decoded);
  if (cleaned.startsWith(QLatin1Char('/')))
    cleaned = cleaned.mid(1);

  QString absPath = wwwRoot + QLatin1String("/") + cleaned;
  QFileInfo fi(absPath);

  // Reject symlinks
  if (fi.isSymLink())
    return QFileInfo();

  QString fiCanonical = fi.canonicalFilePath();
  if (fiCanonical.isEmpty())
    return QFileInfo();

  // Enforce file is inside wwwRoot
  if (!fiCanonical.startsWith(rootCanonical + QLatin1String("/")))
    return QFileInfo();

  return fi;
}

QByteArray CSimpleHttpServer::buildResponse(const HttpRequest &req,
                                            const QFileInfo &file,
                                            const QString &wwwRoot,
                                            const QString &nickname,
                                            const QByteArray &avatarBase64,
                                            const QString &myDest) {
  Q_UNUSED(wwwRoot);

  if (!file.exists() || !file.isReadable())
    return buildErrorResponse(404, QStringLiteral("Not Found"));

  if (!file.isFile())
    return buildErrorResponse(404, QStringLiteral("Not Found"));

  // Only serve regular files
  if (!file.isFile())
    return buildErrorResponse(404, QStringLiteral("Not Found"));

  QFile f(file.absoluteFilePath());
  if (!f.open(QFile::ReadOnly))
    return buildErrorResponse(404, QStringLiteral("Not Found"));

  QByteArray content = f.readAll();
  f.close();

  QString contentType = mimeTypeForName(file.fileName());

  // Token injection only for HTML
  if (contentType.startsWith(QLatin1String("text/html"))) {
    QString html = QString::fromUtf8(content);
    QString safeNick = nickname;
    safeNick.replace(QLatin1String("<"), QString()).replace(QLatin1String(">"), QString());
    html.replace(QStringLiteral("[USERNAME]"), safeNick);
    html.replace(QStringLiteral("[AVATARIMAGE]"), QString::fromUtf8(avatarBase64));
    html.replace(QStringLiteral("[MYDEST]"), myDest);
    content = html.toUtf8();
  }

  QByteArray header = buildHeader(200, QStringLiteral("OK"), content.size(), contentType, generateCSP());

  // HEAD request: return headers only
  if (req.method == QStringLiteral("HEAD"))
    return header;

  return header + content;
}

static QString formatFileSize(qint64 bytes) {
  if (bytes < 1024)
    return QString::number(bytes) + QStringLiteral(" B");
  if (bytes < 1024 * 1024)
    return QString::number(bytes / 1024.0, 'f', 1) + QStringLiteral(" KB");
  return QString::number(bytes / (1024.0 * 1024.0), 'f', 1) + QStringLiteral(" MB");
}

QByteArray CSimpleHttpServer::buildDirectoryListing(const QFileInfo &dir, const QString &requestPath) {
  QDir d(dir.absoluteFilePath());
  QString displayPath = requestPath.isEmpty() ? QStringLiteral("/") : requestPath;
  if (!displayPath.startsWith(QLatin1Char('/')))
    displayPath.prepend(QLatin1Char('/'));
  if (!displayPath.endsWith(QLatin1Char('/')))
    displayPath.append(QLatin1Char('/'));

  QStringList parts = displayPath.split(QLatin1Char('/'), Qt::SkipEmptyParts);
  QString parentPath;
  if (parts.size() > 1) {
    parts.removeLast();
    parentPath = QStringLiteral("/") + parts.join(QLatin1String("/")) + QStringLiteral("/");
  } else {
    parentPath = QStringLiteral("/");
  }

  QString title = QStringLiteral("Index of ") + displayPath;
  QString body;
  body += QStringLiteral("<!DOCTYPE html>\n<html><head><meta charset=\"utf-8\">\n<title>");
  body += title;
  body += QStringLiteral("</title>\n");
  body += QStringLiteral("<style>body{font-family:sans-serif;margin:20px}"
                         "th{text-align:left;padding-right:2em}"
                         "td{padding-right:2em}</style>\n");
  body += QStringLiteral("</head><body>\n<h1>");
  body += title;
  body += QStringLiteral("</h1>\n<hr>\n<table>\n");
  body += QStringLiteral("<tr><th>Name</th><th>Size</th><th>Last Modified</th></tr>\n");

  // Parent directory link
  body += QStringLiteral("<tr><td><a href=\"") + parentPath.toHtmlEscaped() +
          QStringLiteral("\">..</a></td>"
                         "<td></td><td></td></tr>\n");

  QFileInfoList entries =
    d.entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs | QDir::Readable, QDir::Name | QDir::DirsFirst);
  for (const QFileInfo &entry : entries) {
    if (entry.fileName().startsWith(QLatin1Char('.')))
      continue;

    QString name = entry.fileName();
    QString href = displayPath + name.toHtmlEscaped();
    if (entry.isDir())
      href += QStringLiteral("/");

    QString sizeStr;
    if (entry.isFile())
      sizeStr = formatFileSize(entry.size());

    QString modStr = entry.lastModified().toString(QStringLiteral("yyyy-MM-dd HH:mm"));

    body += QStringLiteral("<tr><td><a href=\"") + href + QStringLiteral("\">") + name.toHtmlEscaped() +
            QStringLiteral("</a></td><td>") + sizeStr + QStringLiteral("</td><td>") + modStr +
            QStringLiteral("</td></tr>\n");
  }

  body += QStringLiteral("</table>\n<hr>\n</body></html>\n");

  QByteArray content = body.toUtf8();
  QString csp = generateCSP();
  QByteArray header =
    buildHeader(200, QStringLiteral("OK"), content.size(), QStringLiteral("text/html; charset=utf-8"), csp);
  return header + content;
}

QByteArray CSimpleHttpServer::buildAuthRequiredResponse(const QString &realm) {
  QString body = QStringLiteral("<html><head><title>401 Unauthorized</title></head>"
                                "<body><h1>401 Unauthorized</h1></body></html>");
  QByteArray content = body.toUtf8();
  QByteArray header;
  header += "HTTP/1.0 401 Unauthorized\r\n";
  header += "Content-Length: " + QByteArray::number(content.size()) + "\r\n";
  header += "Content-Type: text/html; charset=utf-8\r\n";
  header += "WWW-Authenticate: Basic realm=\"";
  header += realm.toUtf8();
  header += "\"\r\n";
  header += "\r\n";
  return header + content;
}

QByteArray CSimpleHttpServer::buildErrorResponse(int status, const QString &statusText) {
  QString body = QStringLiteral("<html><head><title>%1 %2</title></head><body><h1>%1 %2</h1></body></html>")
                   .arg(status)
                   .arg(statusText);
  QByteArray content = body.toUtf8();
  return buildHeader(status, statusText, content.size(), QStringLiteral("text/html; charset=utf-8"), QString()) +
         content;
}

QByteArray CSimpleHttpServer::buildHeader(int statusCode,
                                          const QString &statusText,
                                          qint64 contentLength,
                                          const QString &contentType,
                                          const QString &csp) {
  QByteArray header;
  header += "HTTP/1.0 " + QByteArray::number(statusCode) + " " + statusText.toUtf8() + "\r\n";
  header += "Content-Length: " + QByteArray::number(contentLength) + "\r\n";
  header += "Content-Type: " + contentType.toUtf8() + "\r\n";
  if (!csp.isEmpty())
    header += "Content-Security-Policy: " + csp.toUtf8() + "\r\n";
  header += "X-XSS-Protection: 1; mode=block\r\n";
  header += "X-Content-Type-Options: nosniff\r\n";
  header += "\r\n";
  return header;
}

QString CSimpleHttpServer::mimeTypeForName(const QString &filename) {
  QMimeDatabase db;
  QMimeType mime = db.mimeTypeForFile(filename, QMimeDatabase::MatchExtension);
  if (mime.isValid() && !mime.isDefault())
    return mime.name() + QStringLiteral("; charset=utf-8");
  return QStringLiteral("application/octet-stream");
}

QString CSimpleHttpServer::generateCSP() {
  return QStringLiteral("default-src 'self'; "
                        "script-src 'self'; "
                        "style-src 'self' 'unsafe-inline'; "
                        "img-src 'self' data:; "
                        "font-src 'self'; "
                        "form-action 'none'; "
                        "base-uri 'none'; "
                        "frame-ancestors 'none'");
}
