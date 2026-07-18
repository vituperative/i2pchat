// SPDX-License-Identifier: AGPL-3.0-or-later

#include "PacketManager.h"

#include "ConnectionManager.h"
#include "I2PStream.h"

#include <utility>

CPacketManager::CPacketManager(CConnectionManager &ConnectionManager, qint32 ID)
  : mConnectionManager(ConnectionManager)
  , mID(ID) {
  mData = new QByteArray();
}

CPacketManager::~CPacketManager() {
  delete mData;
  mData = NULL;
}

void CPacketManager::operator<<(const QByteArray &t) {
  if (mData != NULL) {
    mData->append(t);
    checkifOnePacketIsCompleate();
  }
}

void CPacketManager::checkifOnePacketIsCompleate() {
  /* Wire format: first 4 bytes = hex-encoded payload length, followed by
     the payload itself. Recursively extracts all complete packets from
     the accumulation buffer mData. */
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
      return;
    }

    if (mData != NULL && mData->length() >= iPacketLength + 4) {
      QByteArray CurrentPacket(mData->mid(4), iPacketLength);
      mData->remove(0, iPacketLength + 4);

      emit signAPacketIsCompleate(mID, CurrentPacket);
      checkifOnePacketIsCompleate();
    }
  }
}

void CPacketManager::slotDataInput(qint32 ID, const QByteArray &t) {
  if (ID == this->mID) {
    *(this) << t;
  }
}
