/***************************************************************************
 *   Copyright (C) 2008 by I2P-Messenger                                   *
 *   Messenger-Dev@I2P-Messenger                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "I2PStream.h"
#include "SessionController.h"
#include <QMap>
#include <QMapIterator>
#include <QTime>

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

  bool doCreateSession(SESSION_ENUMS::SESSION_STYLEV3 SessionStyle,
                       QString SamPrivKey, QString SessionOptions);

  void doStopp();
  void doReStart();
  void doNamingLookUP(QString Name);
  bool doDestroyStreamObjectByID(qint32 ID);
  CI2PStream *doCreateNewStreamObject(StreamMode Mode, bool Silence = false,
                                      bool dontConnectSendStreamStatus = false);

  bool isComponentStopped() const { return mComponentStateStopped; }

  QString getStreamControllerBridgeName() const;
  QString getSamPrivKey() const;
  CI2PStream *getStreamObjectByID(qint32 ID) const;
  CI2PStream *getStreamObjectByDestination(QString Destination) const;

  const QMap<qint32, CI2PStream *> *getAllStreamObjects() const {
    return &allStreams;
  }

  const QMap<qint32, CI2PStream *> *
  getAllStreamIncomingListenerObjects() const {
    return &StreamIncomingListener;
  }

public slots:
  void slotSessionStreamStatusOK(bool Status);
  void slotModeAcceptIncomingStream(qint32 ID);

signals:
  void signDebugMessages(const QString Message);
  void signStreamControllerStatusOK(bool Status);
  void signStreamStatusReceived(const SAM_Message_Types::RESULT result,
                                const qint32 ID, const QString Message);
  void signNamingReplyReceived(const SAM_Message_Types::RESULT result,
                               QString Name, QString Value = "",
                               QString Message = "");
  void signIncomingStream(CI2PStream *stream);
  void signNewSamPrivKeyGenerated(const QString SamPrivKey);

private:
  qint32 nextFreePosID() const;
  qint32 nextFreeNegID() const;
  QString generateBridgeName() const;
  void stopp();

  QMap<qint32, CI2PStream *> StreamIncomingListener;
  CSessionController *StreamController;

  bool mSessionStreamStatusOK;
  bool mComponentStateStopped;

  const QString mSamHost;
  const QString mSamPort;
  const QString mConfigPath;

  inline void SessionStreamStatusOKCheck() {
    if (mSessionStreamStatusOK == false)
      return;
  }
  QMap<qint32, CI2PStream *> allStreams;
};
#endif
