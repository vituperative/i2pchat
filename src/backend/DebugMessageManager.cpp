// SPDX-License-Identifier: AGPL-3.0-or-later

#include "DebugMessageManager.h"

CDebugMessageManager::CDebugMessageManager(const QString &Group, const QString &configPath) {

  QSettings settings(configPath + "/application.ini", QSettings::IniFormat);
  settings.beginGroup(Group);
  this->mMaxMessageCount = settings.value("Debug_Max_Message_count", "20").toInt();
  settings.endGroup();
  settings.sync();

  slotNewIncomingDebugMessage("• I2PChat Debug Message Manager started");
}

CDebugMessageManager::~CDebugMessageManager() {}

void CDebugMessageManager::doClearAllMessages() {
  mMessages.clear();
}

void CDebugMessageManager::slotNewIncomingDebugMessage(const QString &Message) {
  while (mMessages.count() >= (signed int)mMaxMessageCount) {
    mMessages.removeLast();
  }

  mMessages.prepend(Message);
  emit signNewDebugMessage(Message);
}
