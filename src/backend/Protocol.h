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
#ifndef PROTOCOL
#define PROTOCOL

#include <QtGui>
#include <QThread>


/*
	First packet on connection must be:
		CHATSYSTEM\tProtocolVersion\n					-> for normal Connection
		CHATSYSTEMFILETRANSFER\tProtocolVersion\nSizeinBit\nFileName	-> for FileTransfer
	else
		send <the html info-page > 	//maybe with information about the user ???
						//maybe good for usersearch ?

	Every packet must be >= 8 Byte
	1-4 Byte = Paketlength in Byte (HEX) without the 4 Byte Paketlength
	5-8 Byte = PaketInfo
	 >8 Byte = PaketData
*/



namespace Protocol_Info{
	const QString PROTOCOLVERSION  ="0.6";
	const double  PROTOCOLVERSION_D= 0.6;
	const QString FIRSTPAKETCHAT="CHATSYSTEM\t"+PROTOCOLVERSION+"\n";
	const QString HTTPPAGE="<html><header></header><body>This is not an eepsite,this is a I2PChat Destination<br><br></body></html>\n\n\n";
};

namespace PROTOCOL_TAGS{
	enum COMMANDS_TAGS{
		PING,
		GET_PROTOCOLVERSION,
		GET_MAX_PROTOCOLVERSION_FILETRANSFER,
		GET_MIN_PROTOCOLVERSION_FILETRANSFER,
		GET_CLIENTVERSION,
		GET_CLIENTNAME,
		GET_USER_ONLINESTATUS,
		GET_USER_INFOS,
		//Protocolversion >= 0.5
		  GET_AVATARIMAGE
		//-----------------------
		//Protocolversion >= 0.6
		//-----------------------

	};
	enum MESSAGES_TAGS{
		CHATMESSAGE,
		ECHO_OF_PING,

		ANSWER_OF_GET_PROTOCOLVERSION,
		ANSWER_OF_GET_MAX_PROTOCOLVERSION_FILETRANSFER,
		ANSWER_OF_GET_MIN_PROTOCOLVERSION_FILETRANSFER,
		ANSWER_OF_GET_CLIENTVERSION,
		ANSWER_OF_GET_CLIENTNAME,

		USER_ONLINESTATUS_ONLINE,
		USER_ONLINESTATUS_OFFLINE,
		USER_ONLINESTATUS_INVISIBLE,
		USER_ONLINESTATUS_WANTTOCHAT,
		USER_ONLINESTATUS_AWAY,
		USER_ONLINESTATUS_DONT_DISTURB,

		USER_INFO_NICKNAME,
		USER_INFO_GENDER,
		USER_INFO_AGE,
		USER_INFO_INTERESTS,

		USER_BLOCK_INVISIBLE,
		USER_BLOCK_NORMAL,

		//Protocolversion >= 0.5
		  ANSWER_OF_GET_AVATARIMAGE_IMAGE,
		//-----------------------
		//Protocolversion >= 0.6
		  AVATARIMAGE_CHANGED,
		//-----------------------

	};


};
using namespace Protocol_Info;
using namespace PROTOCOL_TAGS;
class CCore;
class CUser;
class CProtocol:public QObject{
Q_OBJECT
public:
	CProtocol(CCore& Core);
	~CProtocol();

	//forbid some operators
	CProtocol(const CProtocol&)=delete;
	CProtocol& operator=(const CProtocol&)=delete;

	QString getProtocolVersion()const {return PROTOCOLVERSION;};
	void newConnectionChat(const qint32 ID);

	void send(const MESSAGES_TAGS TAG,const qint32 ID,QByteArray Data) const;
	void send(const MESSAGES_TAGS TAG,const qint32 ID,QString Data) const;
	void send(const COMMANDS_TAGS TAG,const qint32 ID) const;

public slots:
	void slotInputUnknown(const qint32 ID,const QByteArray Data);
	void slotInputKnown(const qint32 ID, const QByteArray Data);

private:
	CCore& mCore;



};
#endif
