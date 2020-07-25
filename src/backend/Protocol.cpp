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
#include <QErrorMessage>
#include <QSettings>

#include "Core.h"
#include "FileTransferSend.h"
#include "I2PStream.h"
#include "LoadHTML.cpp"
#include "Protocol.h"
#include "User.h"
#include "UserManager.h"

#include <iostream>
using namespace std;

CProtocol::CProtocol(CCore &Core) : mCore(Core) {}

void CProtocol::newConnectionChat(const qint32 ID) {
  using namespace Protocol_Info;
  CI2PStream *stream = mCore.getI2PStreamObjectByID(ID);

  // send the ChatSystem\tProtocolVersion
  if (stream->getFIRSTPACKETCHAT_alreadySent() == false) {
    // sometime StreamStatusReceived is called again with streamstatus Ok
    stream->setFIRSTPACKETCHAT_alreadySent(true);
    *(stream) << (QString)FIRSTPACKETCHAT;
  }
}

void CProtocol::slotInputKnown(const qint32 ID, const QByteArray Data) {
  using namespace Protocol_Info;

  if (Data.length() < 4)
    return;

  QString ProtocolInfoTag(Data.left(4));

  // COMMANDS
  if (ProtocolInfoTag == "1000") { // PING:
    send(ECHO_OF_PING, ID, QString(""));
  } else if (ProtocolInfoTag == "1001") { // GET_CLIENTVERSION:
    send(ANSWER_OF_GET_CLIENTVERSION, ID, mCore.getClientVersion());
  } else if (ProtocolInfoTag == "1002") { // GET_CLIENTNAME:
    send(ANSWER_OF_GET_CLIENTNAME, ID, mCore.getClientName());
  } else if (ProtocolInfoTag == "1003") { // GET_USER_ONLINESTATUS:
    if (mCore.getUserManager()->getUserByI2P_ID(ID)->getIsInvisible() == true) {
      send(USER_ONLINESTATUS_OFFLINE, ID, QString(""));
    } else {
      switch (mCore.getOnlineStatus()) {
      case USERONLINE: {
        send(USER_ONLINESTATUS_ONLINE, ID, QString(""));
        break;
      }
      case USEROFFLINE:
      case USERINVISIBLE: {
        send(USER_ONLINESTATUS_OFFLINE, ID, QString(""));
        break;
      }
      case USERAWAY: {
        send(USER_ONLINESTATUS_AWAY, ID, QString(""));
        break;
      }
      case USERWANTTOCHAT: {
        send(USER_ONLINESTATUS_WANTTOCHAT, ID, QString(""));
        break;
      }
      case USERDONT_DISTURB: {
        send(USER_ONLINESTATUS_DONT_DISTURB, ID, QString(""));
        break;
      }
      default: {
        QMessageBox *msgBox = new QMessageBox(NULL);
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setText(tr("cProtocol(inputKnown)"));
        msgBox->setInformativeText(tr("Unknown USERSTATE"));
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->setDefaultButton(QMessageBox::Ok);
        msgBox->setWindowModality(Qt::NonModal);
        msgBox->show();
      }
      } // switch
    }   // else

  } else if (ProtocolInfoTag == "1004") { // GET_PROTOCOLVERSION,
    send(ANSWER_OF_GET_PROTOCOLVERSION, ID, mCore.getProtocolVersion());
  } else if (ProtocolInfoTag ==
             "1005") { // GET_MAX_PROTOCOLVERSION_FILETRANSFER
    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);

    if (thisUser != NULL) {
      if (thisUser->getProtocolVersion() == "0.3") {
        // BUG in Messenger_0.2.15 BETA :(
        send(ANSWER_OF_GET_MAX_PROTOCOLVERSION_FILETRANSFER, ID,
             QString("0.2"));
      } else {
        send(ANSWER_OF_GET_MAX_PROTOCOLVERSION_FILETRANSFER, ID,
             FileTransferProtocol::MAXPROTOCOLVERSION);
      }
    }
  } else if (ProtocolInfoTag == "1006") { // GET_USER_INFOS
    using namespace User;
    QString sAge;
    CReceivedInfos Infos = mCore.getUserInfos();
    send(USER_INFO_NICKNAME, ID, Infos.Nickname);
    send(USER_INFO_GENDER, ID, Infos.Gender);
    send(USER_INFO_INTERESTS, ID, Infos.Interests);

    sAge.setNum(Infos.Age, 10);
    send(USER_INFO_AGE, ID, sAge);
  } else if (ProtocolInfoTag ==
             "1007") { // GET_MIN_PROTOCOLVERSION_FILETRANSFER
    send(ANSWER_OF_GET_MIN_PROTOCOLVERSION_FILETRANSFER, ID,
         FileTransferProtocol::MINPROTOCOLVERSION);
  } else if (ProtocolInfoTag == "1008") { // GET_AVATARIMAGE
    send(ANSWER_OF_GET_AVATARIMAGE_IMAGE, ID, mCore.getUserInfos().AvatarImage);
  }

  // end of commands

  // Messages
  else if (ProtocolInfoTag == "0001") { // ANSWER_OF_GET_CLIENTVERSION
    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (thisUser != NULL) {
      QString ClientVersion = Data.mid(4);
      thisUser->setClientVersion(ClientVersion);
    }

  } else if (ProtocolInfoTag == "0002") { // ANSWER_OF_GET_CLIENTNAME
    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (thisUser != NULL) {
      QString Clientname = Data.mid(4);
      thisUser->setClientName(Clientname);
    }

  } else if (ProtocolInfoTag == "0003") { // chatmessage
    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (thisUser != NULL) {
      QByteArray temp = Data.mid(4);
      thisUser->slotIncomingNewChatMessage(temp);
    }
  } else if (ProtocolInfoTag == "0004") { // USER_ONLINESTATUS_ONLINE
    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (thisUser != NULL) {
      thisUser->setOnlineState(USERONLINE);
    }
  } else if (ProtocolInfoTag == "0005") { // USER_ONLINESTATUS_OFFLINE ||
                                          // USER_ONLINESTATUS_INVISIBLE
    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (thisUser != NULL) {
      thisUser->setOnlineState(USEROFFLINE);
    }
  } else if (ProtocolInfoTag == "0006") { // USER_ONLINESTATUS_WANTTOCHAT
    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (thisUser != NULL) {
      thisUser->setOnlineState(USERWANTTOCHAT);
    }
  } else if (ProtocolInfoTag == "0007") { // USER_ONLINESTATUS_AWAY
    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (thisUser != NULL) {
      thisUser->setOnlineState(USERAWAY);
    }
  } else if (ProtocolInfoTag == "0008") { // USER_ONLINESTATUS_DONT_DISTURB
    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (thisUser != NULL) {
      thisUser->setOnlineState(USERDONT_DISTURB);
    }
  } else if (ProtocolInfoTag == "0009") { // ANSWER_OF_GET_PROTOCOLVERSION,
    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (thisUser != NULL) {
      QString temp = Data.mid(4);
      thisUser->setProtocolVersion(temp);
    }
  } else if (ProtocolInfoTag ==
             "0010") { // ANSWER_OF_GET_MAX_PROTOCOLVERSION_FILETRANSFER,
    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (thisUser != NULL) {
      QString temp = Data.mid(4);
      thisUser->setMaxProtocolVersionFiletransfer(temp);
    }
  } else if (ProtocolInfoTag == "0011") { // USER_INFO_NICKNAME
    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (thisUser != NULL) {
      QString temp = Data.mid(4);
      thisUser->setReceivedUserInfos(User::NICKNAME, temp);
    }
  } else if (ProtocolInfoTag == "0012") { // USER_INFO_GENDER
    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (thisUser != NULL) {
      QString temp = Data.mid(4);
      thisUser->setReceivedUserInfos(User::GENDER, temp);
    }
  } else if (ProtocolInfoTag == "0013") { // USER_INFO_AGE
    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (thisUser != NULL) {
      QString temp = Data.mid(4);
      thisUser->setReceivedUserInfos(User::AGE, temp);
    }
  } else if (ProtocolInfoTag == "0014") { // USER_INFO_INTERESTS
    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (thisUser != NULL) {
      QString temp = Data.mid(4);
      thisUser->setReceivedUserInfos(User::INTERESTS, temp);
    }
  } else if (ProtocolInfoTag == "0015") { // USER_BLOCK_INVISIBLE
    // SHOW USER AS OFFLINE
    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (thisUser != NULL) {
      mCore.deletePacketManagerByID(ID);
      if (mCore.getConnectionManager()->isComponentStopped() == false) {
        mCore.getConnectionManager()->doDestroyStreamObjectByID(ID);
      }
      thisUser->setConnectionStatus(OFFLINE);
    }
  } else if (ProtocolInfoTag == "0016") { // USER_BLOCK_NORMAL
    // SHOW USER AS BLOCKED

    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (thisUser != NULL) {
      thisUser->setOnlineState(USERBLOCKEDYOU);
      thisUser->setConnectionStatus(OFFLINE);
      mCore.deletePacketManagerByID(ID);

      if (mCore.getConnectionManager()->isComponentStopped() == false) {
        mCore.getConnectionManager()->doDestroyStreamObjectByID(ID);
      }
    }
  } else if (ProtocolInfoTag ==
             "0017") { // ANSWER_OF_GET_MIN_PROTOCOLVERSION_FILETRANSFER:
    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (thisUser != NULL) {
      QString temp = Data.mid(4);
      thisUser->setMinProtocolVersionFiletransfer(temp);
    }
  } else if (ProtocolInfoTag == "0018") { // ANSWER_OF_GET_AVATARIMAGE_IMAGE
    CUser *thisUser = mCore.getUserManager()->getUserByI2P_ID(ID);
    if (thisUser != NULL) {
      QByteArray temp = Data.mid(4);
      thisUser->setAvatarImage(temp);
    }
  } else if (ProtocolInfoTag == "0019") { // AVATARIMAGE_CHANGED
    send(GET_AVATARIMAGE, ID);
  }
  /*else if(ProtocolInfoTag=="0020"){//VIDEOCALL
                  //
  }
  else if(ProtocolInfoTag=="0021"){//AUDIOCALL
                  //
  }*/
  // end Messages
  else {
    qWarning() << "File\t" << __FILE__ << endl
               << "Line:\t" << __LINE__ << endl
               << "Function:\t"
               << "CProtocol::slotInputKnown" << endl
               << "Message:\t"
               << "Unknown ProtocolInfoTag: " << ProtocolInfoTag << endl
               << "Packet ignored" << endl;
  }
}

void CProtocol::slotInputUnknown(const qint32 ID, const QByteArray Data) {
  using namespace Protocol_Info;

  CI2PStream *stream = mCore.getI2PStreamObjectByID(ID);

  if (stream == 0) {
    auto msg = "Protocol.cpp : stream object is null, ignoring incoming data\n";
    qDebug() << msg;
    //    QErrorMessage *box = new QErrorMessage();
    //    box->showMessage(msg);
    return;
  }

  if (stream->getConnectionType() == UNKNOWN) {
    // check if First Packet = from another CHATSYSTEM
    if (Data.contains("CHATSYSTEM\t") == true) {
      QByteArray temp = Data.mid(Data.indexOf("\t") + 1,
                                 Data.indexOf("\n") - Data.indexOf("\t") - 1);
      QString version(temp);
      bool OK = false;
      double versiond = version.toDouble(&OK);
      if (OK == false) {
        qCritical() << "File\t" << __FILE__ << endl
                    << "Line:\t" << __LINE__ << endl
                    << "Function:\t"
                    << "CProtocol::slotInputUnknown" << endl
                    << "Message:\t"
                    << "Can't convert QString to double" << endl
                    << "QString:\t" << version << endl;
      }

      // don't send the first packet if you have connected someone
      //(the firstpacket is sended from core::StreamStatusReceived)
      if (ID < 0) {
        newConnectionChat(ID); // someone connect you

        if (mCore.getUserBlockManager()->isDestinationInBlockList(
                stream->getDestination()) == true) {
          // is blocked

          if (versiond < 0.4) {
            send(
                CHATMESSAGE, ID,
                QString("You have been blocked, all packets will be ignored!"));
            mCore.getConnectionManager()->doDestroyStreamObjectByID(ID);
            return;
          } else {
            QSettings settings(mCore.getConfigPath() + "/application.ini",
                               QSettings::IniFormat);
            settings.beginGroup("Security");
            if (settings.value("BlockStyle", "Normal").toString() == "Normal") {
              send(CHATMESSAGE, ID,
                   QString(
                       "You have been blocked, all packets will be ignored!"));
              send(USER_BLOCK_NORMAL, ID, QString(""));
            } else {
              // Block-Style Invisible
              send(USER_BLOCK_INVISIBLE, ID, QString(""));
            }
            settings.endGroup();
            settings.sync();
            mCore.getConnectionManager()->doDestroyStreamObjectByID(ID);
            return;
          }
        }
      }

      // remove Firstpacket
      QByteArray Data2 = Data;
      Data2 = Data2.remove(0, Data.indexOf("\n") + 1);
      if (mCore.getUserManager()->checkIfUserExistsByI2PDestination(
              stream->getDestination()) == false) {

        if (versiond >= 0.3) {
          mCore.getUserManager()->addNewUser("...identifying...",
                                             stream->getDestination(), ID);
        } else {
          mCore.getUserManager()->addNewUser("Unknown",
                                             stream->getDestination(), ID);
        }

        CUser *User = mCore.getUserManager()->getUserByI2P_Destination(
            stream->getDestination());
        User->setI2PStreamID(ID);
        User->setProtocolVersion(version);
        User->setConnectionStatus(ONLINE);
        mCore.setStreamTypeToKnown(ID, Data2, false);
        if (versiond >= 0.3) {
          User->setReceivedNicknameToUserNickname();
        }
      } else {
        if (mCore.useThisChatConnection(stream->getDestination(), ID) == true) {
          CUser *User = mCore.getUserManager()->getUserByI2P_Destination(
              stream->getDestination());
          User->setI2PStreamID(ID);
          User->setProtocolVersion(version);
          User->setConnectionStatus(ONLINE);
          mCore.setStreamTypeToKnown(ID, Data2, false);
        }
      }
      // return Data2;
    } else if (Data.contains("CHATSYSTEMFILETRANSFER\t") == true) {
      // FIRSTPACKET
      // ="CHATSYSTEMFILETRANSFER\t"+PROTOCOLVERSION+"\nSizeinBit\nFileName";
      QByteArray Data2 = Data;

      QString ProtovolVersion =
          Data2.mid(Data.indexOf("\t") + 1,
                    Data2.indexOf("\n") - Data2.indexOf("\t") - 1);
      Data2.remove(0, Data2.indexOf("\n") +
                          1); // CHATSYSTEMFILETRANSFER\tPROTOCOLVERSION

      QString FileSize = Data2.mid(0, Data2.indexOf("\n"));
      Data2.remove(0, Data2.indexOf("\n") + 1);

      QString FileName = Data2;
      QString Destination;

      Destination = stream->getDestination();

      if (mCore.getUserBlockManager()->isDestinationInBlockList(
              stream->getDestination()) == true) {
        mCore.getConnectionManager()->doDestroyStreamObjectByID(ID);
        Data2.clear();
        return;
      }

      Data2.clear();
      mCore.setStreamTypeToKnown(ID, Data2, true);
      mCore.setUserProtocolVersionByStreamID(ID, ProtovolVersion);
      mCore.getFileTransferManager()->addNewFileReceive(
          ID, FileName, FileSize, Destination, ProtovolVersion);
    } else {
      // not from another CHATSYSTEM
      bool webprofileenabled = false;
      if (mCore.getUserBlockManager()->isDestinationInBlockList(
              stream->getDestination()) == true) {
        mCore.getConnectionManager()->doDestroyStreamObjectByID(ID);
      } else {
        QSettings settings(mCore.getConfigPath() + "/application.ini",
                           QSettings::IniFormat);
        settings.beginGroup("Security");
        if (settings.value("WebProfile", "Enabled").toString() == "Enabled") {
          webprofileenabled = true;
        } else {
          webprofileenabled = false;
        }
        if (settings.value("HideWebProfileWhenInvisible", "True").toString() ==
            "True") {
          QList<CUser *> users = mCore.getUserManager()->getUserList();
          for (int i = 0; i < users.size(); i++) {
            if (users.at(i)->getIsInvisible() == true) {
              webprofileenabled = false;
              break;
            }
          }
          if (mCore.getOnlineStatus() == USERINVISIBLE) {
            webprofileenabled = false;
          }
        }
        settings.endGroup();
        settings.sync();

        if (webprofileenabled == true) {
          //          printf("Web Profile enabled\n"); // Debug
          QString TEMPHTTPPAGE =
              loadfile(mCore.getConfigPath() + "/www/index.html");
          if (TEMPHTTPPAGE.isEmpty()) {
            TEMPHTTPPAGE = HTTPPAGE;
          }
          QString myNick = mCore.getUserInfos().Nickname;
          myNick = myNick.replace("<", "").replace(">", "");
          TEMPHTTPPAGE.replace("[USERNAME]", myNick);

          TEMPHTTPPAGE.replace("[AVATARIMAGE]",
                               mCore.getUserInfos().AvatarImage.toBase64());
          TEMPHTTPPAGE.replace("[MYDEST]", mCore.getMyDestination());

          *(stream) << (QString)(gethttpheader(TEMPHTTPPAGE) + TEMPHTTPPAGE);
          mCore.getConnectionManager()->doDestroyStreamObjectByID(ID);
        } else { // what to do if the webprofile is disabled?
        }
      }
    }
  }
}

void CProtocol::send(const COMMANDS_TAGS TAG, const qint32 ID) const {
  using namespace Protocol_Info;

  CI2PStream *stream = mCore.getI2PStreamObjectByID(ID);

  QString ProtocolInfoTag;
  QString Data = "";
  switch (TAG) {
  case PING: {
    ProtocolInfoTag = "1000";
    break;
  }
  case GET_CLIENTVERSION: {
    ProtocolInfoTag = "1001";
    break;
  }
  case GET_CLIENTNAME: {
    ProtocolInfoTag = "1002";
    break;
  }
  case GET_USER_ONLINESTATUS: {
    ProtocolInfoTag = "1003";
    break;
  }
  case GET_PROTOCOLVERSION: {
    ProtocolInfoTag = "1004";
    break;
  }
  case GET_MAX_PROTOCOLVERSION_FILETRANSFER: {
    ProtocolInfoTag = "1005";
    break;
  }
  case GET_USER_INFOS: {
    ProtocolInfoTag = "1006";
    break;
  }
  case GET_MIN_PROTOCOLVERSION_FILETRANSFER: {
    ProtocolInfoTag = "1007";
    break;
  }
  case GET_AVATARIMAGE: {
    ProtocolInfoTag = "1008";
    break;
  }
  default: {
    qCritical() << "File\t" << __FILE__ << endl
                << "Line:\t" << __LINE__ << endl
                << "Function:\t"
                << "CProtocol::send" << endl
                << "Message:\t"
                << "unhandeld Command-TAG"
                << "exit" << endl;
    exit(-1);
  }
  }
  Data.insert(0, ProtocolInfoTag);
  Data.insert(0, "0004"); // No packet data
  *(stream) << Data;
}

void CProtocol::send(const MESSAGES_TAGS TAG, const qint32 ID,
                     QString Data) const {
  QByteArray t = "";
  t.insert(0, Data);

  send(TAG, ID, t);
}
void CProtocol::send(const MESSAGES_TAGS TAG, const qint32 ID,
                     QByteArray Data) const {
  QString ProtocolInfoTag;

  CI2PStream *stream = mCore.getI2PStreamObjectByID(ID);

  switch (TAG) {
  case ECHO_OF_PING: {
    ProtocolInfoTag = "0000";
    break;
  }
  case ANSWER_OF_GET_CLIENTVERSION: {
    ProtocolInfoTag = "0001";
    break;
  }
  case ANSWER_OF_GET_CLIENTNAME: {
    ProtocolInfoTag = "0002";
    break;
  }
  case CHATMESSAGE: {
    ProtocolInfoTag = "0003";
    break;
  }
  case USER_ONLINESTATUS_ONLINE: {
    ProtocolInfoTag = "0004";
    break;
  }
  case USER_ONLINESTATUS_OFFLINE:
  case USER_ONLINESTATUS_INVISIBLE: {
    ProtocolInfoTag = "0005";
    break;
  }
  case USER_ONLINESTATUS_WANTTOCHAT: {
    ProtocolInfoTag = "0006";
    break;
  }
  case USER_ONLINESTATUS_AWAY: {
    ProtocolInfoTag = "0007";
    break;
  }
  case USER_ONLINESTATUS_DONT_DISTURB: {
    ProtocolInfoTag = "0008";
    break;
  }
  case ANSWER_OF_GET_PROTOCOLVERSION: {
    ProtocolInfoTag = "0009";
    break;
  }
  case ANSWER_OF_GET_MAX_PROTOCOLVERSION_FILETRANSFER: {
    ProtocolInfoTag = "0010";
    break;
  }
  case USER_INFO_NICKNAME: {
    ProtocolInfoTag = "0011";
    break;
  }
  case USER_INFO_GENDER: {
    ProtocolInfoTag = "0012";
    break;
  }
  case USER_INFO_AGE: {
    ProtocolInfoTag = "0013";
    break;
  }
  case USER_INFO_INTERESTS: {
    ProtocolInfoTag = "0014";
    break;
  }
  case USER_BLOCK_INVISIBLE: {
    ProtocolInfoTag = "0015";
    break;
  }
  case USER_BLOCK_NORMAL: {
    ProtocolInfoTag = "0016";
    break;
  }
  case ANSWER_OF_GET_MIN_PROTOCOLVERSION_FILETRANSFER: {
    ProtocolInfoTag = "0017";
    break;
  }
  case ANSWER_OF_GET_AVATARIMAGE_IMAGE: {
    ProtocolInfoTag = "0018";
    break;
  }
  case AVATARIMAGE_CHANGED: {
    ProtocolInfoTag = "0019";
    break;
  }
  default: {
    qCritical() << "File\t" << __FILE__ << endl
                << "Line:\t" << __LINE__ << endl
                << "Function:\t"
                << "CProtocol::send" << endl
                << "Message:\t"
                << "unhandeld Message-TAG"
                << "exit" << endl;

    exit(-1);
  }
  }
  QString temp;

  temp.setNum(Data.length() + 4, 16); // hex
  QString Packetlength = QString("%1").arg(temp, 4, '0');

  Data.insert(0, ProtocolInfoTag);
  Data.insert(0, Packetlength);
  *(stream) << Data;
}

CProtocol::~CProtocol() {}
