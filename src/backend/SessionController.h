// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef SESSIONCONTROLLER_H
#define SESSIONCONTROLLER_H

#include "I2PSamMessageAnalyser.h"

#include <QTcpSocket>
#include <QTimer>

class CSessionController : public QObject {
  Q_OBJECT
public:
  CSessionController(QString SamHost,
                     QString SamPort,
                     QString BridgeName,
                     QString SamPrivKey,
                     QString ConfigPath,
                     QString SessionOptions = "");

  ~CSessionController();

  QString getBridgeName() const { return mBridgeName; }
  QString getSamPrivKey() const { return mSamPrivKey; }

  void doConnect();
  void doDisconnect();
  void doNamingLookUP(const QString &Name);
  void doDestGenerate(const QString &Options = "");

signals:
  void signDebugMessages(const QString Message);
  void signSessionStreamStatusOK(bool Status);
  void signReconnectAttempt();
  void signNamingReplyReceived(const SAM_Message_Types::RESULT result,
                               QString Name,
                               QString Value = "",
                               QString Message = "");
  void signNewSamPrivKeyGenerated(const QString SamPrivKey);

private slots:
  void slotConnected();
  void slotDisconnected();
  void slotReadFromSocket();
  void slotReconnectTimeout();

private:
  void doSessionCreate();

  const QString mSamHost;
  const QString mSamPort;
  const QString mBridgeName;
  QString mSamPrivKey;
  const QString mConfigPath;
  const QString mSessionOptions;

  QTcpSocket mTcpSocket;
  CI2PSamMessageAnalyser *mAnalyser;
  QByteArray *mIncomingPackets;
  QTimer *mReconnectTimer;

  bool mHandshakeSuccessful;
  bool mSessionWasSuccesfullCreated;
  bool mDoneDisconnect;

  inline void ConnectionReadyCheck() {
    if (mHandshakeSuccessful == false || mSessionWasSuccesfullCreated == false ||
        mTcpSocket.state() != QAbstractSocket::ConnectedState)
      return;
  }
};
#endif
