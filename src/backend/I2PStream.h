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
#ifndef I2PSTREAM_H
#define I2PSTREAM_H

#include <QSocketNotifier>
#include <QTcpSocket>
#include <QTimer>

#include "I2PSamMessageAnalyser.h"

namespace STREAMS {
enum StreamMode { ACCEPT, CONNECT };
}

namespace CONNECTIONTYPE {
enum Type { UNKNOWN, KNOWN };
}

using namespace STREAMS;
using namespace CONNECTIONTYPE;
class CI2PStream : public QObject {
  Q_OBJECT

public:
  CI2PStream(QString SamHost, QString SamPort, qint32 ID,
             QString StreamBridgeName, StreamMode Mode, bool Silence = false,
             QString UsedFor = "");
  ~CI2PStream();

  // forbid some operators
  CI2PStream(const CI2PStream &) = delete;
  CI2PStream &operator=(const CI2PStream &) = delete;

  bool doConnect(QString Destination);
  bool doAccept();
  void doDisconnect();

  QString getDestination() const { return mDestination; }
  qint32 getID() const { return mID; }
  StreamMode getStreamMode() const { return mMode; }
  Type getConnectionType() const { return mConnectionType; }
  bool getFIRSTPAKETCHAT_allreadySended() const {
    return mFIRSTPAKETCHAT_allreadySended;
  }
  QString getUsedFor() { return mUsedFor; };
  void setConnectionType(const Type newTyp);
  void setFIRSTPAKETCHAT_allreadySended(bool theValue);
  void setUsedFor(QString value) { mUsedFor = value; };

  void operator<<(const QByteArray Data);
  void operator<<(const QString Data);

  void startUnlimintedReconnect(qint32 msec);
  void stopUnlimintedReconnect();

private slots:
  void slotConnected();
  void slotDisconnected();
  void slotReadFromSocket();
  void slotCheckForReconnect();
  void slotInitConnectionTimeout();

signals:
  void signDebugMessages(const QString Message);
  void signStreamStatusReceived(const SAM_Message_Types::RESULT result,
                               const qint32 ID, const QString Message);
  void signDataReceived(const qint32 ID, const QByteArray Data);
  void signModeAcceptIncomingStream(
      qint32 ID); // emit if Destination received (SILENCE=false)

private:
  const QString mSamHost;
  const QString mSamPort;
  const qint32 mID;
  const QString mStreamBridgeName;
  const StreamMode mMode;
  Type mConnectionType;
  const bool mSilence;
  QString mUsedFor; // like Filetransfer....
  QString mDestination;
  bool mDoneDisconnect;
  bool mModeStreamConnect;
  bool mModeStreamAccept;
  bool mStatusReceived;
  bool mHandShakeWasSuccesfullDone;
  bool mDestinationReceived;
  bool mFIRSTPAKETCHAT_allreadySended;
  QByteArray *mIncomingPackets;
  QTcpSocket mTcpSocket;
  QTimer *mTimer;
  QTimer mUnKnownConnectionTimeout;
  CI2PSamMessageAnalyser *mAnalyser;
};
#endif
