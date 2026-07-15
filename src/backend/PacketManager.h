// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef PACKETMANAGER_H
#define PACKETMANAGER_H

#include <Qt>
#include <QtGui>

class CConnectionManager;
class CPacketManager : public QObject {
  Q_OBJECT

public:
  CPacketManager(CConnectionManager &ConnectionManager, qint32 ID);
  ~CPacketManager();

  // forbid some operators
  CPacketManager(const CPacketManager &) = delete;
  CPacketManager &operator=(const CPacketManager &) = delete;

  void operator<<(const QByteArray &t);
  qint32 getID() const { return mID; }

public slots:
  void slotDataInput(qint32 ID, const QByteArray &t);

signals:
  void signAPacketIsCompleate(const qint32 ID, const QByteArray CurrentPacket);

private:
  CConnectionManager &mConnectionManager;
  const qint32 mID;
  QByteArray *mData;

  void checkifOnePacketIsCompleate();
};
#endif
