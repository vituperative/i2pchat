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

#ifndef HTTPOVERSTREAMOBJECT_H
#define HTTPOVERSTREAMOBJECT_H

#include <QByteArray>
#include <QStringList>

#include "ConnectionManager.h"
#include "I2PStream.h"

class CHttpOverStreamObject : public QObject {

  Q_OBJECT

public:
  enum HTTPMODE {
    GET,
    POST,
  };

  CHttpOverStreamObject(CConnectionManager &ConnectionManager);
  ~CHttpOverStreamObject();

  // forbid some operators
  CHttpOverStreamObject(const CHttpOverStreamObject &) = delete;
  CHttpOverStreamObject &operator=(const CHttpOverStreamObject &) = delete;

  void doHttpRequest(HTTPMODE mode, QString Destination,
                     QStringList HttpHeader);

signals:
  void signDoneSuccessfully(bool, QByteArray);

private slots:
  void slotStreamStatus(const SAM_Message_Types::RESULT result, const qint32 ID,
                        QString Message);
  void slotDataReceived(const qint32 ID, QByteArray t);

private:
  CConnectionManager &mConnectionManager;
  CI2PStream *mStream;
  bool mFirstStreamStatus;
  bool mIsTimeOutCantReachPeerCLosed;
  HTTPMODE mMode;
  QString mDestination;
  QStringList mHttpHeader;
  QByteArray mDataReceived;
  qint32 mStreamID;

  void sendRequest();
};
#endif
