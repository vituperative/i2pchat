// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef DEBUGMESSAGEMANAGER_H
#define DEBUGMESSAGEMANAGER_H

#include "ConnectionManager.h"

#include <QSettings>
#include <QtGui>

class CDebugMessageManager : public QObject {
  Q_OBJECT

public:
  CDebugMessageManager(const QString &Group, const QString &configPath);
  ~CDebugMessageManager();

  // forbid some operators
  CDebugMessageManager(const CDebugMessageManager &) = delete;
  CDebugMessageManager &operator=(const CDebugMessageManager &) = delete;

public slots:
  void doClearAllMessages();
  const QStringList getAllMessages() const { return mMessages; }

private slots:
  void slotNewIncomingDebugMessage(const QString &Message);

signals:
  void signNewDebugMessage(QString Message);

private:
  quint32 mMaxMessageCount;
  QStringList mMessages;
};
#endif
