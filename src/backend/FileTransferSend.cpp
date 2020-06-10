/***************************************************************************
 *   Copyright (C) 2008 by I2P-Messenger   				   *
 *   Messenger-Dev@I2P-Messenger   					   *
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



#include "FileTransferSend.h"
#include "Core.h"
#include "UserManager.h"

CFileTransferSend::CFileTransferSend(CCore& Core,CConnectionManager& ConnectionManager,
		QString FilePath,QString Destination,QString Protocolversion,double ProtocolversionD)

	:mCore(Core),mConnectionManager(ConnectionManager),mFilePath(FilePath),mDestination(Destination),
	mUsingProtocolVersion(Protocolversion),mUsingProtocolVersionD(ProtocolversionD)
{


	mStream=ConnectionManager.doCreateNewStreamObject(CONNECT,false);
	mStream->setUsedFor("FileTransferSend");

	connect(mStream,SIGNAL(signStreamStatusRecived(const SAM_Message_Types::RESULT, const qint32, const QString)),
		this,SLOT(slotStreamStatus(const SAM_Message_Types::RESULT, const qint32, QString)));

	connect(mStream,SIGNAL(signDataRecived(const qint32, const QByteArray)),
		this,SLOT(slotDataRecived(const qint32, QByteArray)));

	connect(&mTimerForActAverageTransferSpeed,SIGNAL(timeout()),
		this,SLOT(slotCalcAverageTransferSpeed()));

	mStreamID=mStream->getID();
	mCore.setStreamTypeToKnown(mStreamID,NULL,true);
	mStream->doConnect(Destination);

	mAllreadyFinished=false;
	mSendFirstPaket=true;
	mFileName=FilePath.mid(FilePath.lastIndexOf("/")+1);
	mFileForSend.setFileName(mFilePath);
	mFileTransferAccepted=false;
	mFileSize=mFileForSend.size();
	mAllreadySendedSize=0;
	mCurrentPacketSize=NORMPAKETSIZE;
	mRemoteRecivedSize=0;
}

void CFileTransferSend::slotAbbortFileSend()
{
	mFileForSend.close();
	mTimerForActAverageTransferSpeed.stop();
	mConnectionManager.doDestroyStreamObjectByID(mStream->getID());
	mCore.getFileTransferManager()->removeFileTransfer(mStreamID);


}

void CFileTransferSend::slotStreamStatus(const SAM_Message_Types::RESULT result, const qint32 ID, QString Message)
{
	using namespace FileTransferProtocol;

	if(mStreamID!=ID){
	    qCritical()	<<"File\t"<<__FILE__<<endl
			<<"Line:\t"<<__LINE__<<endl
			<<"Function:\t"<<"CFileTransferSend::slotStreamStatus"<<endl
			<<"Message:\t"<<"mStreamID!=ID WTF"<<endl
			<<"mStreamID:\t"<<mStreamID<<endl
			<<"ID:\t"<<ID<<endl;
	}


	switch(result){
		case (SAM_Message_Types::OK):{
			if(mSendFirstPaket==true){
				QString StringFileSize;
				StringFileSize.setNum(mFileSize);

				mStream->operator<<(QString("CHATSYSTEMFILETRANSFER\t"+mUsingProtocolVersion+"\n"+StringFileSize+"\n"+mFileName));
				//mStream->operator <<(FIRSTPAKET+StringFileSize+"\n"+mFileName);
				mSendFirstPaket=false;
			}
			break;
		}

		case (SAM_Message_Types::CANT_REACH_PEER):
		case (SAM_Message_Types::TIMEOUT):
		case (SAM_Message_Types::CLOSED):{
			mTimerForActAverageTransferSpeed.stop();

			if(mAllreadySendedSize==mFileSize){
			emit signFileTransferFinishedOK();
				if(mAllreadyFinished==false){
					mCore.getUserManager()->getUserByI2P_Destination(mDestination)->
					slotIncomingMessageFromSystem(tr("Filetransfer finished (OK)<br>FileName: %1<br>").arg(mFileName));
					mAllreadyFinished=true;
				}
			}
			else{
				emit signFileTransferAborted();
				if(mAllreadySendedSize==0){
					mCore.getUserManager()->getUserByI2P_Destination(mDestination)->
					slotIncomingMessageFromSystem(tr("Can't connect to the user for file transfer<br>Filename: %1").arg(mFileName));
				}
				else
				{
					mCore.getUserManager()->getUserByI2P_Destination(mDestination)->
						slotIncomingMessageFromSystem(tr("The recipient cancelled the file transfer<br>Filename: %1").arg(mFileName));
				}
			}
			mFileForSend.close();
			mConnectionManager.doDestroyStreamObjectByID(mStreamID);
			mCore.getFileTransferManager()->removeFileTransfer(mStreamID);

			break;
		}
		case (SAM_Message_Types::I2P_ERROR):{
			emit signFileTransferAborted();
			mCore.getUserManager()->getUserByI2P_Destination(mDestination)->
				slotIncomingMessageFromSystem(tr("Filetransfer: %1 I2P stream error:<br>%2").arg(mFileName).arg(Message));

			mFileForSend.close();
			mConnectionManager.doDestroyStreamObjectByID(mStreamID);
			mCore.getFileTransferManager()->removeFileTransfer(mStreamID);
			break;
		}
		case (SAM_Message_Types::INVALID_KEY):{
			emit signFileTransferAborted();
			mCore.getUserManager()->getUserByI2P_Destination(mDestination)->
				slotIncomingMessageFromSystem(tr("Filetransfer: %1 Stream get a INVALID_KEY ERROR:<br>%2").arg(mFileName).arg(Message));

			mFileForSend.close();
			mConnectionManager.doDestroyStreamObjectByID(mStreamID);
			mCore.getFileTransferManager()->removeFileTransfer(mStreamID);
			break;
		}
		case (SAM_Message_Types::INVALID_ID):{
			emit signFileTransferAborted();
			mCore.getUserManager()->getUserByI2P_Destination(mDestination)->
				slotIncomingMessageFromSystem("Filetransfer: "+mFileName+"Stream get a INVALID_ID ERROR:<br>"+Message);

			mFileForSend.close();
			mConnectionManager.doDestroyStreamObjectByID(mStreamID);
			mCore.getFileTransferManager()->removeFileTransfer(mStreamID);
			break;
		}
		default:{
			break;
		}
	}
}


void CFileTransferSend::slotDataRecived(const qint32 ID,QByteArray t)
{
	if(mUsingProtocolVersionD<=0.2){
	      if(t.length()==1){

		      if(t.contains("0")){//true
			      emit signFileTransferAccepted(true);
			      mFileTransferAccepted=true;
			      StartFileTransfer();

		      }
		      else if(t.contains("1")){//false
			      emit signFileTransferAccepted(false);
			      mCore.getUserManager()->getUserByI2P_Destination(mDestination)->
				      slotIncomingMessageFromSystem(tr("Filetransfer don't accepted,filesending abborted<br>Filename: %1").arg(mFileName));
			      mConnectionManager.doDestroyStreamObjectByID(ID);
			     mCore.getFileTransferManager()->removeFileTransfer(mStreamID);

		      }
		      else if(t.contains("2")){//next block	(Proto 0.2)
			      SendFile_v0dot2();
		      }
	      }
	      else{
		      emit signFileTransferAccepted(false);
		      mCore.getUserManager()->getUserByI2P_Destination(mDestination)->
				      slotIncomingMessageFromSystem(tr("Filetransfer don't accepted,filesending abborted<br>Filename: %1").arg(mFileName));
		      mConnectionManager.doDestroyStreamObjectByID(ID);
		      mCore.getFileTransferManager()->removeFileTransfer(mStreamID);
	      }
	}
	else if(mUsingProtocolVersionD==0.3){
	      mRemoteDataReciveBuffer.append(t);
	      while(mRemoteDataReciveBuffer.contains('\n')==true){
		    QString CurrentPacket=mRemoteDataReciveBuffer.mid(0,mRemoteDataReciveBuffer.indexOf('\n',0)+1);
		    mRemoteDataReciveBuffer.remove(0,CurrentPacket.length());

		    QString CurrentAction=CurrentPacket.mid(0,1);
		    CurrentPacket.remove(0,3);	//remove {0,1,2}:\t

		    if(CurrentAction=="0"){
			  //Filetransfer Accepted
			  emit signFileTransferAccepted(true);
			  qint64 from=CurrentPacket.remove('\n').toInt();
			  mFileTransferAccepted=true;
			  StartFileTransfer(from);
		    }
		    else if(CurrentAction=="1"){
		        //Filetransfer don't Accepted
			     emit signFileTransferAccepted(false);
			     mCore.getUserManager()->getUserByI2P_Destination(mDestination)->
				   slotIncomingMessageFromSystem(tr("Filetransfer don't accepted,filesending abborted<br>Filename: %1").arg(mFileName));
			     mConnectionManager.doDestroyStreamObjectByID(ID);
			    mCore.getFileTransferManager()->removeFileTransfer(mStreamID);
		    }
		    else if(CurrentAction=="2"){
			//next block & current size recived remote
			CurrentPacket.remove('\n');
			mRemoteRecivedSize+=CurrentPacket.toInt();
			emit signAllreadySendedSizeChanged(mRemoteRecivedSize);
			if((mAllreadySendedSize-mRemoteRecivedSize)<=1024){
			    SendFile_v0dot3();
			}
		    }
	      }
	}
}

void CFileTransferSend::StartFileTransfer(qint64 mFromPos)
{
	mAllreadySendedSize=0;
	mFileForSend.open(QIODevice::ReadOnly);
	mFileForSend.seek(mFromPos);
	if(mFromPos!=0){
	    mAllreadySendedSize=mFromPos;
	}

	mTimer.start();
	mTimerForActAverageTransferSpeed.start(TIMERCOUNTFORAVERAGETRANSFERSPEED_WRITE);


	if(mUsingProtocolVersionD==0.1){
		SendFile_v0dot1();
	}
	else if(mUsingProtocolVersionD==0.2){
		SendFile_v0dot2();
	}
	else if(mUsingProtocolVersionD==0.3){
		mCurrentPacketSize=MAXPACKETSIZE;
		SendFile_v0dot3();
	}
	else{
		qWarning()<<"File\t"<<__FILE__<<endl
			  <<"Line:\t"<<__LINE__<<endl
			  <<"Function:\t"<<"CFileTransferSend::StartFileTransfer"<<endl
			  <<"Message:\t"<<"Unsupported Protocolversion:"<<mUsingProtocolVersion<<endl;

		mCore.getFileTransferManager()->removeFileTransfer(mStreamID);
	}
}

void CFileTransferSend::SendFile_v0dot3()
{
   QByteArray Buffer;

	Buffer=mFileForSend.read(mCurrentPacketSize);
	mAllreadySendedSize+=Buffer.length();

	mStream->operator <<(Buffer);

	if(mAllreadySendedSize==mFileSize&&mAllreadyFinished==false){
		emit signFileTransferFinishedOK();
		mCore.getUserManager()->getUserByI2P_Destination(mDestination)->
			slotIncomingMessageFromSystem(tr("Filetransfer finished (OK)<br>FileName: %1<br>").arg(mFileName));
			mAllreadyFinished=true;
	}
}


void CFileTransferSend::SendFile_v0dot2()
{

	QByteArray Buffer;

	Buffer=mFileForSend.read(mCurrentPacketSize);
	mAllreadySendedSize+=Buffer.length();

	mStream->operator <<(Buffer);
	emit signAllreadySendedSizeChanged(mAllreadySendedSize);

	if(mAllreadySendedSize==mFileSize&&mAllreadyFinished==false){
		emit signFileTransferFinishedOK();
		mCore.getUserManager()->getUserByI2P_Destination(mDestination)->
			slotIncomingMessageFromSystem(tr("Filetransfer finished (OK)<br>FileName: %1<br>").arg(mFileName));
			mAllreadyFinished=true;
	}
}


void CFileTransferSend::SendFile_v0dot1()
{
	while(mAllreadySendedSize<mFileSize){
		QByteArray Buffer;

		Buffer=mFileForSend.read(NORMPAKETSIZE);
		mAllreadySendedSize+=Buffer.length();

		mStream->operator <<(Buffer);
		emit signAllreadySendedSizeChanged(mAllreadySendedSize);
	}

	if(mAllreadySendedSize==mFileSize && mAllreadyFinished==false){
		emit signFileTransferFinishedOK();
		mCore.getUserManager()->getUserByI2P_Destination(mDestination)->
			slotIncomingMessageFromSystem(tr("Filetransfer finished (OK)<br>FileName: %1<br>").arg(mFileName));
			mAllreadyFinished=true;
	}
}

CFileTransferSend::~ CFileTransferSend()
{
	mTimerForActAverageTransferSpeed.stop();
}

bool CFileTransferSend::getIsTransfering()
{
	if(mFileTransferAccepted==true && mAllreadyFinished==false){
		return true;
	}
	else{
		return false;
	}
}

void CFileTransferSend::slotCalcAverageTransferSpeed()
{
	QString speedSize;
	QString speedType;
	int speed;
	int departedtime=(mTimer.elapsed()/1000);

	if(departedtime<=0)departedtime=1;

	if(mUsingProtocolVersionD<=0.2){
		speed=mAllreadySendedSize / departedtime;
	}else{
		speed=mRemoteRecivedSize / departedtime;
	}

	mCore.doConvertNumberToTransferSize(speed,speedSize,speedType);
	emit signAverageTransferSpeed(speedSize,speedType);
	CalcETA(speed);
}


void CFileTransferSend::doConvertNumberToTransferSize(quint64 inNumber, QString & outNumber, QString & outType,bool addStoOutType)
{
	return mCore.doConvertNumberToTransferSize(inNumber,outNumber,outType,addStoOutType);
}

void CFileTransferSend::CalcETA(int speed)
{
	QString EmitString;
	int secLeft;

	if(speed>0){
	    if(mUsingProtocolVersionD<=0.2){
		secLeft=(mFileSize-mAllreadySendedSize) / speed ;
	    }else{
	       secLeft=(mFileSize-mRemoteRecivedSize) / speed ;
	    }
	}else{
	    if(mUsingProtocolVersionD<=0.2){
	       secLeft =mFileSize-mAllreadySendedSize;
	    }
	    else{
	      secLeft =mFileSize-mRemoteRecivedSize;
	    }
	}

	if(secLeft> 86400){
	    //> 24h
	    emit signETA(tr("never or (>24 h)"));
	}else{
	    int hours=0;
	    int minutes=0;
	     int secs=0;

	    if(secLeft>=3600){
		    //hours
		    hours=secLeft/3600;
		    secLeft-=hours*3600;
	    }
	    if(secLeft>=60){
		  minutes=secLeft/60;
		  secLeft-=minutes*60;
	    }
	    secs=secLeft;

	    //hours
		  if(hours<=9){
		    EmitString.append("0");
		  }
		  EmitString.append(QString::number(hours,10)+":");
	    //---------------------------------------------------------
	    //minutes
	      if(minutes<=9){
		EmitString.append("0");
	      }
	      EmitString.append(QString::number(minutes,10)+":");
	    //---------------------------------------------------------
	    //secs
	    if(secs<=9){
		EmitString.append("0");
	    }
	    EmitString.append(QString::number(secs,10));

	    signETA(EmitString);
	}
}
