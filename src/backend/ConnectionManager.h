// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "I2PStream.h"
#include "SessionController.h"

#include <QDateTime>
#include <QElapsedTimer>
#include <QMap>
#include <QMapIterator>

namespace SESSION_ENUMS {
enum SESSION_STYLEV3 {
  STREAM,
  DATAGRAMM //,
            // RAW
};
};

class CConnectionManager : public QObject {
  Q_OBJECT
public:
  CConnectionManager(QString SamHost, QString SamPort, QString ConfigPath);
  ~CConnectionManager();

  // forbid some operators
  CConnectionManager(const CConnectionManager &) = delete;
  CConnectionManager &operator=(const CConnectionManager &) = delete;

  bool doCreateSession(SESSION_ENUMS::SESSION_STYLEV3 SessionStyle, QString SamPrivKey, QString SessionOptions);

  void doStopp();
  void doReStart();
  void doNamingLookUP(const QString &Name);
  bool doDestroyStreamObjectByID(qint32 ID);
  CI2PStream *doCreateNewStreamObject(StreamMode Mode, bool Silence = false, bool dontConnectSendStreamStatus = false);

  bool isComponentStopped() const { return mComponentStateStopped; }

  QString getStreamControllerBridgeName() const;
  QString getSamPrivKey() const;
  CI2PStream *getStreamObjectByID(qint32 ID) const;
  CI2PStream *getStreamObjectByDestination(const QString &Destination) const;

  const QMap<qint32, CI2PStream *> *getAllStreamObjects() const { return &allStreams; }

  const QMap<qint32, CI2PStream *> *getAllStreamIncomingListenerObjects() const { return &StreamIncomingListener; }

public slots:
  void slotSessionStreamStatusOK(bool Status);
  void slotModeAcceptIncomingStream(qint32 ID);

signals:
  void signDebugMessages(const QString Message);
  void signStreamControllerStatusOK(bool Status);
  void signReconnectAttempt();
  void signStreamStatusReceived(const SAM_Message_Types::RESULT result, const qint32 ID, const QString Message);
  void signNamingReplyReceived(const SAM_Message_Types::RESULT result,
                               QString Name,
                               QString Value = "",
                               QString Message = "");
  void signIncomingStream(CI2PStream *stream);
  void signNewSamPrivKeyGenerated(const QString SamPrivKey);

private:
  qint32 nextFreePosID() const;
  qint32 nextFreeNegID() const;
  QString generateBridgeName() const;
  void stopp();

  QMap<qint32, CI2PStream *> StreamIncomingListener; // listeners waiting for incoming streams (ACCEPT mode)
  CSessionController *StreamController;

  bool mSessionStreamStatusOK;
  bool mComponentStateStopped;

  const QString mSamHost;
  const QString mSamPort;
  const QString mConfigPath;

  inline bool SessionStreamStatusOKCheck() { return mSessionStreamStatusOK; }
  QMap<qint32, CI2PStream *> allStreams; // all active streams (both CONNECT and ACCEPT), keyed by stream ID
};
#endif
