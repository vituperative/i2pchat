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
#ifndef PROTOCOL
#define PROTOCOL

#include <QThread>
#include <QtGui>

/*
        First packet on connection must be:
                CHATSYSTEM\tProtocolVersion\n
   -> for normal Connection
                CHATSYSTEMFILETRANSFER\tProtocolVersion\nSizeinBit\nFileName
   -> for FileTransfer else send <the html info-page > 	//maybe with information
   about the user ???
                                                //maybe good for usersearch ?

        Every packet must be >= 8 Byte
        1-4 Byte = Packetlength in Byte (HEX) without the 4 Byte Packetlength
        5-8 Byte = PacketInfo
         >8 Byte = PacketData
*/

namespace Protocol_Info {
const QString PROTOCOLVERSION = "0.6";
const double PROTOCOLVERSION_D = 0.6;
const QString FIRSTPACKETCHAT = "CHATSYSTEM\t" + PROTOCOLVERSION + "\n";
const QString HTTPPAGE =
    "<!DOCTYPE HTML><html><head><title>[USERNAME] | I2PChat "
    "Destination</title><meta http-equiv=\"Content-Type\" content=\"text/html; "
    "charset=UTF-8\"><meta http-equiv=\"Content-Security-Policy\" "
    "content=\"default-src \'none\'; img-src data:; style-src "
    "\'unsafe-inline\';\" /><style type=\"text/css\">body, "
    "html{margin: 0; "
    "background: #000} body{text-align: center; font-family: Open Sans, Segoe "
    "UI, sans-serif; font-size: 13pt; color: #ddd} a, a:visited{color: #900; "
    "text-decoration: none; outline: none} a:hover{color: #b00} "
    "#container{position: absolute; top: 0; height: 100%; width: 100%; "
    "display: table} #box, #moreinfo{box-shadow: inset 0 0 0 1px #300} "
    "#main{display: table-cell; position: relative; vertical-align: middle; "
    "text-align: center} #box{margin: 2% auto 5%; padding: 30px; width: 50%; "
    "min-width: 400px; max-width: 600px; border: 1px solid #333; background: "
    "#111} #avatar{padding: 3px; width: 96px; height: 96px; border: 2px solid "
    "#322; border-radius: 50%; background: #000} #dest{margin: 3px auto "
    "-25px; padding: 1px 8px; max-width: 400px; white-space: nowrap; overflow: "
    "hidden; "
    "text-overflow: ellipsis; font-size: 80%; color: #090; border: 1px solid "
    "#090; "
    "border-radius: 15px; user-select: all} #moreinfo{margin: "
    "50px -30px -30px; padding: 5px 0; font-size: 75%; font-weight: 600; "
    "border-top: 1px "
    "solid #333} </style></head><body><div id=\"container\"><div "
    "id=\"main\"><div id=\"box\"><img id=\"avatar\" "
    "src=\"data:image/"
    "svg+xml;base64,"
    "PHN2ZyBlbmFibGUtYmFja2dyb3VuZD0ibmV3IDAgMCA1MDguNjA5IDUwOC42MDkiIHZpZXdCb3"
    "g9IjAgMCA1MDguNjA5IDUwOC42MDkiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2"
    "ZyI+"
    "PGNpcmNsZSBjeD0iMTUwLjg4NyIgY3k9IjIwMC4wNTMiIGZpbGw9IiNlZWI0OTAiIHI9IjM5Lj"
    "Y3MiIvPjxjaXJjbGUgY3g9IjM1Ny43MjIiIGN5PSIyMDAuMDUzIiBmaWxsPSIjZWViNDkwIiBy"
    "PSIzOS42NzIiLz48cGF0aCBkPSJtNDYzLjUxMyA1MDguNjA5aC00MTguNDE2di03NC45MzVjMC"
    "0zNC45MjUgMjYuMTA5LTY0LjA4NSA2MC4zNTUtNjguNDkzbDkyLjIyOC0zNi42MmgxMTMuMjVs"
    "OTIuMjI4IDM2LjYyYzM0LjU4NSA0LjA2OSA2MC4zNTUgMzMuNTY4IDYwLjM1NSA2OC40OTN6Ii"
    "BmaWxsPSIjOWQ5MGM1Ii8+"
    "PHBhdGggZD0ibTE3OC4wMTMgMzQ4LjU2N3YzMy41NjhjMCA0Mi43MjMgMzQuOTI0IDc4LjY2NS"
    "A3Ny42NDggNzcuOTg3IDQxLjM2Ny0uNjc4IDc0LjU5Ni0zNC41ODUgNzQuNTk2LTc2LjI5MXYt"
    "MzUuMjY0YzAtMTIuMjA3LTkuODMzLTIyLjA0LTIyLjA0LTIyLjA0aC0xMDguNTAzYy0xMS44Nj"
    "guMzM5LTIxLjcwMSAxMC4xNzItMjEuNzAxIDIyLjA0eiIgZmlsbD0iIzgwNzVhZiIvPjxwYXRo"
    "IGQ9Im0yMDIuMDg3IDMwMC40MTl2NTUuNjA4YzAgMjguODIxIDIzLjM5NiA1Mi4yMTcgNTIuMj"
    "E3IDUyLjIxN3M1Mi4yMTctMjMuMzk2IDUyLjIxNy01Mi4yMTd2LTU1LjYwOHoiIGZpbGw9IiNl"
    "ZWI0OTAiLz48cGF0aCBkPSJtMzA2Ljg2MSA5NC42MDFoLTEwNS4xMTNjLTI0Ljc1MiAwLTQ0Lj"
    "c1OCAyMC4wMDUtNDQuNzU4IDQ0Ljc1OHYxMDAuNzA1YzAgNTMuOTEzIDQzLjc0IDk3LjMxNCA5"
    "Ny4zMTQgOTcuMzE0IDUzLjkxMyAwIDk3LjMxNC00My43NCA5Ny4zMTQtOTcuMzE0di0xMDAuNz"
    "A1Yy4wMDEtMjQuNzUyLTIwLjAwNS00NC43NTgtNDQuNzU3LTQ0Ljc1OHoiIGZpbGw9IiNmYWNj"
    "YjQiLz48ZyBmaWxsPSIjNTY1NDVmIj48cGF0aCBkPSJtMzUxLjYxOSA1Mi44OTVzNzQuOTM1ID"
    "kuODMzIDAgMTUxLjIyNnYtNTcuOTgxcy01LjQyNS00MS43MDYtMzAuODU2LTQ0LjA3OXoiLz48"
    "cGF0aCBkPSJtMzAwLjc1OCAwaC05Mi45MDZjLTMuNzMgMC03LjQ2LjMzOS0xMC44NSAxLjAxNy"
    "0xNDAuMzc2IDAtNDAuMDExIDIxMC4yMjUtNDAuMDExIDIxMC4yMjUgMC04MS4wMzggNTAuODYx"
    "LTgxLjM3NyA1MC44NjEtODEuMzc3aDkzLjI0NWMzNS45NDIgMCA2NC43NjMtMjkuMTYgNjQuNz"
    "YzLTY0Ljc2MyAwLTM1Ljk0Mi0yOS4xNjEtNjUuMTAyLTY1LjEwMi02NS4xMDJ6Ii8+"
    "PC9nPjwvc3ZnPg==\">"
    "<h3>You have reached the I2PChat Destination for [USERNAME]</h3><div "
    "id=\"dest\">[MYDEST]</div><div "
    "id=\"moreinfo\">More information: <a "
    "href=\"https://vituperative.github.io/i2pchat/\">https://"
    "vituperative.github.io/i2pchat/</a></div></div></div></div></body></html>";
}; // namespace Protocol_Info

namespace PROTOCOL_TAGS {
enum COMMANDS_TAGS {
  PING,
  GET_PROTOCOLVERSION,
  GET_MAX_PROTOCOLVERSION_FILETRANSFER,
  GET_MIN_PROTOCOLVERSION_FILETRANSFER,
  GET_CLIENTVERSION,
  GET_CLIENTNAME,
  GET_USER_ONLINESTATUS,
  GET_USER_INFOS,
  // Protocolversion >= 0.5
  GET_AVATARIMAGE
  //-----------------------
  // Protocolversion >= 0.6
  //-----------------------

};
enum MESSAGES_TAGS {
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

  // Protocolversion >= 0.5
  ANSWER_OF_GET_AVATARIMAGE_IMAGE,
  //-----------------------
  // Protocolversion >= 0.6
  AVATARIMAGE_CHANGED,
  //-----------------------

};

}; // namespace PROTOCOL_TAGS
using namespace Protocol_Info;
using namespace PROTOCOL_TAGS;
class CCore;
class CUser;
class CProtocol : public QObject {
  Q_OBJECT
public:
  CProtocol(CCore &Core);
  ~CProtocol();

  // forbid some operators
  CProtocol(const CProtocol &) = delete;
  CProtocol &operator=(const CProtocol &) = delete;

  QString getProtocolVersion() const { return PROTOCOLVERSION; };
  void newConnectionChat(const qint32 ID);

  void send(const MESSAGES_TAGS TAG, const qint32 ID, QByteArray Data) const;
  void send(const MESSAGES_TAGS TAG, const qint32 ID, QString Data) const;
  void send(const COMMANDS_TAGS TAG, const qint32 ID) const;

public slots:
  void slotInputUnknown(const qint32 ID, const QByteArray Data);
  void slotInputKnown(const qint32 ID, const QByteArray Data);

private:
  CCore &mCore;
};
#endif
