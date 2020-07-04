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
#include "SoundManager.h"

CSoundManager::CSoundManager(QString ConfigPath) : mConfigPath(ConfigPath) {
  mIsMute = false;
  reInit();
}

void CSoundManager::doMute(bool t) { mIsMute = t; }

void CSoundManager::slotUserGoOnline() {
  if (mIsMute == true)
    return;

  if (mEnable_eventUser_go_Online)
    QSound::play(mSoundFileUser_go_Online);
}
void CSoundManager::slotUserGoOffline() {
  if (mIsMute == true)
    return;
  if (mEnable_eventUser_go_Offline)
    QSound::play(mSoundFileUser_go_Offline);
}
void CSoundManager::slotFileSendFinished() {
  if (mIsMute == true)
    return;
  if (mEnable_eventFileSend_Finished)
    QSound::play(mSoundFileFileSend_Finished);
}
void CSoundManager::slotFileReceiveIncoming() {
  if (mIsMute == true)
    return;
  if (mEnable_eventFileReceive_Incoming)
    QSound::play(mSoundFileFileReceive_Incoming);
}

void CSoundManager::slotFileReceiveFinished() {
  if (mIsMute == true)
    return;
  if (mEnable_eventFileReceive_Finished)
    QSound::play(mSoundFileFileReceive_Finished);
}

void CSoundManager::slotNewChatMessage() {
  if (mIsMute == true)
    return;
  if (mEnable_eventNewChatMessage) {
    QSound::play(mSoundFileNewChatMessage);
  }
}

void CSoundManager::reInit() {
  QSettings settings(mConfigPath + "/application.ini", QSettings::IniFormat);
  settings.beginGroup("Sound");
  settings.beginGroup("Enable");
  mEnable_eventUser_go_Online = settings.value("User_go_Online", true).toBool();
  mEnable_eventUser_go_Offline =
      settings.value("User_go_Offline", true).toBool();
  mEnable_eventFileSend_Finished =
      settings.value("FileSend_Finished", true).toBool();
  mEnable_eventFileReceive_Incoming =
      settings.value("FileReceive_Incoming", true).toBool();
  mEnable_eventFileReceive_Finished =
      settings.value("FileReceive_Finished", true).toBool();
  mEnable_eventNewChatMessage = settings.value("NewChatMessage", true).toBool();
  settings.endGroup();

  settings.beginGroup("SoundFilePath");
  mSoundFileUser_go_Online =
      settings.value("User_go_Online", "./sounds/online.wav").toString();
  mSoundFileUser_go_Offline = settings.value("User_go_Offline", "./sounds/offline.wav").toString();
  mSoundFileFileSend_Finished =
      settings.value("FileSend_Finished", "./sounds/complete.wav").toString();
  mSoundFileFileReceive_Incoming =
      settings.value("FileReceive_Incoming", "./sounds/fileincoming.wav").toString();
  mSoundFileFileReceive_Finished =
      settings.value("FileReceive_Finished", "./sounds/complete.wav").toString();
  mSoundFileNewChatMessage =
      settings.value("NewChatMessage", "./sounds/newmessage.wav").toString();
  settings.endGroup();
  settings.endGroup();
  settings.sync();
}

CSoundManager::~CSoundManager() {}
