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

#include "PacketManager.h"
#include "ConnectionManager.h"
#include "I2PStream.h"
#include <iostream>

using namespace std;

CPacketManager::CPacketManager(CConnectionManager &ConnectionManager, qint32 ID)
    : mConnectionManager(ConnectionManager), mID(ID) {
  mData = new QByteArray();
}

CPacketManager::~CPacketManager() {
  delete mData;
  mData = NULL;
}

void CPacketManager::operator<<(const QByteArray t) {
  if (mData != NULL) {
    mData->append(t);
    checkifOnePacketIsCompleate();
  }
}

void CPacketManager::checkifOnePacketIsCompleate() {
  if (mData == NULL) {
    return;
  }

  if (mData != NULL && mData->length() >= 8) {
    QString sPacketLength = mData->mid(0, 4);

    bool OK = false;
    int iPacketLength = sPacketLength.toInt(&OK, 16);
    if (OK == false) {
      qCritical() << "File\t" << __FILE__ << Qt::endl
                  << "Line:\t" << __LINE__ << Qt::endl
                  << "Function:\t"
                  << "CPacketManager::checkifOnePacketIsCompleate" << Qt::endl
                  << "Message:\t"
                  << "cant parse PacketLength" << Qt::endl
                  << "StreamID: " << mID << Qt::endl
                  << "sPacketLength: " << sPacketLength << Qt::endl;

      CI2PStream *stream = mConnectionManager.getStreamObjectByID(mID);
      if (stream != NULL) {
        stream->doDisconnect();
      }
    }

    if (mData != NULL && mData->length() >= iPacketLength + 4) {
      QByteArray CurrentPacket(mData->mid(4), iPacketLength);
      mData->remove(0, iPacketLength + 4);

      emit signAPacketIsCompleate(mID, CurrentPacket);
      checkifOnePacketIsCompleate();
    }
  }
  return;
}

void CPacketManager::slotDataInput(qint32 ID, QByteArray t) {
  if (ID == this->mID) {
    *(this) << t;
  }
}
