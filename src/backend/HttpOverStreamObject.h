// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef HTTPOVERSTREAMOBJECT_H
#define HTTPOVERSTREAMOBJECT_H

#include "ConnectionManager.h"
#include "I2PStream.h"

#include <QByteArray>
#include <QStringList>

class CHttpOverStreamObject : public QObject {

  Q_OBJECT

public:
  enum HTTPMODE {
    GET,
    POST,
  };

  CHttpOverStreamObject(CConnectionManager &ConnectionManager);
  ~CHttpOverStreamObject();

  // forbid some operators
  CHttpOverStreamObject(const CHttpOverStreamObject &) = delete;
  CHttpOverStreamObject &operator=(const CHttpOverStreamObject &) = delete;

  void doHttpRequest(HTTPMODE mode, const QString &Destination, QStringList HttpHeader);

signals:
  void signDoneSuccessfully(bool, QByteArray);

private slots:
  void slotStreamStatus(const SAM_Message_Types::RESULT result, const qint32 ID, const QString &Message);
  void slotDataReceived(const qint32 ID, const QByteArray &t);

private:
  CConnectionManager &mConnectionManager;
  CI2PStream *mStream;
  bool mFirstStreamStatus;
  bool mIsTimeOutCantReachPeerCLosed;
  HTTPMODE mMode;
  QString mDestination;
  QStringList mHttpHeader;
  QByteArray mDataReceived;
  qint32 mStreamID;

  void sendRequest();
};
#endif
