// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef FILETRANSFERSEND_H
#define FILETRANSFERSEND_H
#include "ConnectionManager.h"
#include "I2PStream.h"

#include <QElapsedTimer>
#include <QTimer>
#include <QtGlobal>
#include <QtGui>

/*
      Filetransferprotocol 0.3: (sender Receive)

      tags:
      0:\t{StartPos}\n  	accpted start from StartPos // at the moment
   only from 0 1:\t\n			not accepted 2:\t{remotePos}\n for
   progress
*/

namespace FileTransferProtocol {
const QString MINPROTOCOLVERSION = "0.3";
const double MINPROTOCOLVERSION_D = 0.3;
const QString MAXPROTOCOLVERSION = "0.3";
const double MAXPROTOCOLVERSION_D = 0.3;
// const QString FIRSTPACKET ="CHATSYSTEMFILETRANSFER\t"+PROTOCOLVERSION+"\n";
//+sizeinbit\nFileName
}; // namespace FileTransferProtocol

#define NORMPACKETSIZE 32768           // Increased from 4096 to 32KB
#define MAXPACKETSIZE 262144           // Increased from 65536 to 256KB
#define SEND_WINDOW 4                  // Chunks to send before yielding to event loop
#define AVERAGETRANSFERSPEEDPERIOD 500 // 0.5 sec for faster updates

class CCore;
class CFileTransferSend : public QObject {
  Q_OBJECT

public:
  CFileTransferSend(CCore &Core,
                    CConnectionManager &ConnectionManager,
                    const QString &FilePath,
                    const QString &Destination,
                    QString Protocolversion,
                    double ProtocolversionD);
  ~CFileTransferSend();

  // forbid some operators
  CFileTransferSend(const CFileTransferSend &) = delete;
  CFileTransferSend &operator=(const CFileTransferSend &) = delete;

  quint64 getFileSize() const { return mFileSize; };
  qint32 getStreamID() const { return mStreamID; };
  QString getDestination() const { return mDestination; };
  QString getFileName() const { return mFileName; };
  QString getUsingProtocolVersion() const { return mUsingProtocolVersion; };
  quint64 getAlreadySentSize() const { return mAlreadySentSize; };
  bool getAlreadyTransferAccepted() const { return mFileTransferAccepted; };
  bool getIsTransferring() const;
  bool getIsTransferComplete() const { return mAlreadyFinished; };
  void doConvertNumberToTransferSize(quint64 inNumber, QString &outNumber, QString &outType, bool addStoOutType = true);
public slots:
  void slotAbbortFileSend();

private slots:
  void slotStreamStatus(const SAM_Message_Types::RESULT result, const qint32 ID, const QString &Message);
  void slotDataReceived(const qint32 ID, const QByteArray &t);
  void slotCalcAverageTransferSpeed();

signals:
  void signAlreadySentSizeChanged(quint64 Size);
  void signFileTransferAccepted(bool t);
  void signFileTransferFinishedOK();
  void signFileTransferError();
  void signFileTransferAborted(); // recipient cancelled
  void signAverageTransferSpeed(QString SNumber, QString Type);
  void signETA(QString Value);

private:
  void StartFileTransfer(qint64 mFromPos = 0);
  void SendFile_v0dot1();
  void SendFile_v0dot2();
  void SendFile_v0dot3();
  void CalcETA(qint64 speed);

  CCore &mCore;
  CConnectionManager &mConnectionManager;
  const QString mFilePath;
  const QString mDestination;
  const QString mUsingProtocolVersion;
  const double mUsingProtocolVersionD;
  CI2PStream *mStream;
  qint64 mFileSize;
  qint64 mAlreadySentSize;
  qint64 mRemoteReceivedSize;
  qint32 mStreamID;
  QFile mFileForSend;
  bool mSendFirstPacket;
  bool mFileTransferAccepted;
  bool mAlreadyFinished;
  QString mFileName;

  QTimer mTimerForActAverageTransferSpeed;
  QElapsedTimer mTimer;
  int mCurrentPacketSize;
  QByteArray mRemoteDataReceiveBuffer;
};
#endif
