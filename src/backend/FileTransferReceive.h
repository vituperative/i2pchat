// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include "I2PStream.h"

#include <QElapsedTimer>
#include <QTimer>
#include <QtGui>

#define TIMERCOUNTFORAVERAGETRANSFERSPEED_READ 1000 // 1 sec

class CConnectionManager;
class CCore;
class CFileTransferReceive : public QObject {

  Q_OBJECT
public:
  CFileTransferReceive(CCore &Core,
                       CI2PStream &Stream,
                       qint32 StreamID,
                       QString FileName,
                       quint64 FileSize,
                       const QString &Destination,
                       QString Protocolversion,
                       double ProtocolversionD);
  ~CFileTransferReceive();

  // forbid some operators
  CFileTransferReceive(const CFileTransferReceive &) = delete;
  CFileTransferReceive &operator=(const CFileTransferReceive &) = delete;

  void start(const QString &FilePath, bool Accepted);

  quint64 getFileSize() { return mFileSize; };
  QString getFileName() { return mFileName; };
  QString getDestination() { return mDestination; };
  qint32 getStreamID() { return mStreamID; };
  quint64 getTransferredSize() { return mAlreadyReceivedSize; };
  QString getUsingProtocolVersion() { return mUsingProtocolVersion; };

  bool checkIfRequestAccepted() { return mRequestAccepted; };

  void doConvertNumberToTransferSize(quint64 inNumber, QString &outNumber, QString &outType, bool addStoOutType = true);

public slots:
  void slotAbbortFileReceive();

private slots:
  void slotStreamStatusReceived(const SAM_Message_Types::RESULT result, const qint32 ID, const QString &Message);
  void slotDataReceived(const qint32 ID, const QByteArray &t);
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
  QElapsedTimer mTimer;
  CConnectionManager *mConnectionManager;

  // Performance improvements
  QByteArray mWriteBuffer;
  static const qint64 BUFFER_SIZE = 1024 * 1024; // 1MB buffer

  void CalcETA(quint64 speed);
  void flushBuffer();
};
#endif
