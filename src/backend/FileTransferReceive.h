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
#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include "I2PStream.h"
#include <QTime>
#include <QTimer>
#include <QtGui>

#define TIMERCOUNTFORAVERAGETRANSFERSPEED_READ 1000 // 1 sec

class CConnectionManager;
class CCore;
class CFileTransferReceive : public QObject {

  Q_OBJECT
public:
  CFileTransferReceive(CCore &Core, CI2PStream &Stream, qint32 StreamID,
                       QString FileName, quint64 FileSize, QString Destination,
                       QString Protocolversion, double ProtocolversionD);
  ~CFileTransferReceive();

  // forbid some operators
  CFileTransferReceive(const CFileTransferReceive &) = delete;
  CFileTransferReceive &operator=(const CFileTransferReceive &) = delete;

  void start(QString FilePath, bool Accepted);

  quint64 getFileSize() { return mFileSize; };
  QString getFileName() { return mFileName; };
  QString getDestination() { return mDestination; };
  qint32 getStreamID() { return mStreamID; };
  quint64 getTransferredSize() { return mAlreadyReceivedSize; };
  QString getUsingProtocolVersion() { return mUsingProtocolVersion; };

  bool checkIfRequestAccepted() { return mRequestAccepted; };

  void doConvertNumberToTransferSize(quint64 inNumber, QString &outNumber,
                                     QString &outType,
                                     bool addStoOutType = true);

public slots:
  void slotAbbortFileReceive();

private slots:
  void slotStreamStatusReceived(const SAM_Message_Types::RESULT result,
                                const qint32 ID, QString Message);
  void slotDataReceived(const qint32 ID, QByteArray t);
  void slotCalcAverageTransferSpeed();

signals:
  void signgetTransferredSizeChanged(quint64 Size);
  void signFileReceiveError();
  void signFileReceivedFinishedOK();
  void signFileReceiveAborted();
  void signFileNameChanged();
  void signAverageReceiveSpeed(QString SNumber, QString Type);
  void signETA(QString Value);

private:
  CCore &mCore;
  CI2PStream &mStream;
  const qint32 mStreamID;
  QString mFileName;
  const quint64 mFileSize;
  const QString mDestination;
  const QString mUsingProtocolVersion;
  const double mUsingProtocolVersionD;
  quint64 mAlreadyReceivedSize;
  QFile mFileForReceive;
  bool mRequestAccepted;
  QTimer mTimerForActAverageTransferSpeed;
  QTime mTimer;
  CConnectionManager *mConnectionManager;

  void CalcETA(int speed);
};
#endif
