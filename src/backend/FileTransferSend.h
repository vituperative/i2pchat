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
#ifndef FILETRANSFERSEND_H
#define FILETRANSFERSEND_H
#include "ConnectionManager.h"
#include "I2PStream.h"
#include <QTime>
#include <QTimer>
#include <QtGlobal>
#include <QtGui>

/*
      Filetransferprotocol 0.3: (sender Recive)

      tags:
      0:\t{StartPos}\n  	accpted start from StartPos // at the moment only from
   0 1:\t\n			not accepted 2:\t{remotePos}\n		for
   progress
*/

namespace FileTransferProtocol {
const QString MINPROTOCOLVERSION = "0.1";
const double MINPROTOCOLVERSION_D = 0.1;
const QString MAXPROTOCOLVERSION = "0.3";
const double MAXPROTOCOLVERSION_D = 0.3;
// const QString FIRSTPAKET ="CHATSYSTEMFILETRANSFER\t"+PROTOCOLVERSION+"\n";
//+sizeinbit\nFileName
}; // namespace FileTransferProtocol

#define NORMPAKETSIZE 1024
#define MAXPACKETSIZE 30720
#define TIMERCOUNTFORAVERAGETRANSFERSPEED_WRITE 1000 // 1 sec

class CCore;
class CFileTransferSend : public QObject {
  Q_OBJECT

public:
  CFileTransferSend(CCore &Core, CConnectionManager &ConnectionManager,
                    QString FilePath, QString Destination,
                    QString Protocolversion, double ProtocolversionD);
  ~CFileTransferSend();

  // forbid some operators
  CFileTransferSend(const CFileTransferSend &) = delete;
  CFileTransferSend &operator=(const CFileTransferSend &) = delete;

  quint64 getFileSize() { return mFileSize; };
  qint32 getStreamID() { return mStreamID; };
  QString getDestination() { return mDestination; };
  QString getFileName() { return mFileName; };
  QString getUsingProtocolVersion() { return mUsingProtocolVersion; };
  quint64 getAllreadySendedSize() { return mAllreadySendedSize; };
  bool getAllreadyTransferAccepted() { return mFileTransferAccepted; };
  bool getIsTransfering();
  bool getIsAllreadyFinished() { return mAllreadyFinished; };
  void doConvertNumberToTransferSize(quint64 inNumber, QString &outNumber,
                                     QString &outType,
                                     bool addStoOutType = true);
public slots:
  void slotAbbortFileSend();

private slots:
  void slotStreamStatus(const SAM_Message_Types::RESULT result, const qint32 ID,
                        QString Message);
  void slotDataReceived(const qint32 ID, QByteArray t);
  void slotCalcAverageTransferSpeed();

signals:
  void signAllreadySendedSizeChanged(quint64 Size);
  void signFileTransferAccepted(bool t);
  void signFileTransferFinishedOK();
  void signFileTransferError();
  void signFileTransferAborted(); // the otherSide abort it
  void signAverageTransferSpeed(QString SNumber, QString Type);
  void signETA(QString Value);

private:
  void StartFileTransfer(qint64 mFromPos = 0);
  void SendFile_v0dot1();
  void SendFile_v0dot2();
  void SendFile_v0dot3();
  void CalcETA(int speed);

  CCore &mCore;
  CConnectionManager &mConnectionManager;
  const QString mFilePath;
  const QString mDestination;
  const QString mUsingProtocolVersion;
  const double mUsingProtocolVersionD;
  CI2PStream *mStream;
  qint64 mFileSize;
  qint64 mAllreadySendedSize;
  qint64 mRemoteReceivedSize;
  qint32 mStreamID;
  QFile mFileForSend;
  bool mSendFirstPaket;
  bool mFileTransferAccepted;
  bool mAllreadyFinished;
  QString mFileName;

  QTimer mTimerForActAverageTransferSpeed;
  QTime mTimer;
  int mCurrentPacketSize;
  QByteArray mRemoteDataReciveBuffer;
};
#endif
