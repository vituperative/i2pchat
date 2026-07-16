// SPDX-License-Identifier: AGPL-3.0-or-later

#include "SoundManager.h"

#include <utility>

CSoundManager::CSoundManager(QString ConfigPath)
  : mConfigPath(std::move(ConfigPath)) {
  mIsMute = false;
  mSoundEffect = new QSoundEffect(this);
  mSoundEffect->setLoopCount(1);
  reInit();
}

void CSoundManager::doMute(bool t) {
  mIsMute = t;
}

void CSoundManager::slotUserGoOnline() {
  if (mIsMute == true)
    return;

  if (mEnable_eventUser_go_Online) {
    mSoundEffect->setSource(QUrl::fromLocalFile(mSoundFileUser_go_Online));
    mSoundEffect->play();
  }
}
void CSoundManager::slotUserGoOffline() {
  if (mIsMute == true)
    return;
  if (mEnable_eventUser_go_Offline) {
    mSoundEffect->setSource(QUrl::fromLocalFile(mSoundFileUser_go_Offline));
    mSoundEffect->play();
  }
}
void CSoundManager::slotFileSendFinished() {
  if (mIsMute == true)
    return;
  if (mEnable_eventFileSend_Finished) {
    mSoundEffect->setSource(QUrl::fromLocalFile(mSoundFileFileSend_Finished));
    mSoundEffect->play();
  }
}
void CSoundManager::slotFileReceiveIncoming() {
  if (mIsMute == true)
    return;
  if (mEnable_eventFileReceive_Incoming) {
    mSoundEffect->setSource(QUrl::fromLocalFile(mSoundFileFileReceive_Incoming));
    mSoundEffect->play();
  }
}

void CSoundManager::slotFileReceiveFinished() {
  if (mIsMute == true)
    return;
  if (mEnable_eventFileReceive_Finished) {
    mSoundEffect->setSource(QUrl::fromLocalFile(mSoundFileFileReceive_Finished));
    mSoundEffect->play();
  }
}

void CSoundManager::slotNewChatMessage() {
  if (mIsMute == true)
    return;
  if (mEnable_eventNewChatMessage) {
    mSoundEffect->setSource(QUrl::fromLocalFile(mSoundFileNewChatMessage));
    mSoundEffect->play();
  }
}

void CSoundManager::reInit() {
  QSettings settings(mConfigPath + "/application.ini", QSettings::IniFormat);
  settings.beginGroup("Sound");
  settings.beginGroup("Enable");
  mEnable_eventUser_go_Online = settings.value("User_go_Online", true).toBool();
  mEnable_eventUser_go_Offline = settings.value("User_go_Offline", true).toBool();
  mEnable_eventFileSend_Finished = settings.value("FileSend_Finished", true).toBool();
  mEnable_eventFileReceive_Incoming = settings.value("FileReceive_Incoming", true).toBool();
  mEnable_eventFileReceive_Finished = settings.value("FileReceive_Finished", true).toBool();
  mEnable_eventNewChatMessage = settings.value("NewChatMessage", true).toBool();
  settings.endGroup();

  settings.beginGroup("SoundFilePath");
  mSoundFileUser_go_Online = settings.value("User_go_Online", "./sounds/online.wav").toString();
  mSoundFileUser_go_Offline = settings.value("User_go_Offline", "./sounds/offline.wav").toString();
  mSoundFileFileSend_Finished = settings.value("FileSend_Finished", "./sounds/complete.wav").toString();
  mSoundFileFileReceive_Incoming = settings.value("FileReceive_Incoming", "./sounds/fileincoming.wav").toString();
  mSoundFileFileReceive_Finished = settings.value("FileReceive_Finished", "./sounds/complete.wav").toString();
  mSoundFileNewChatMessage = settings.value("NewChatMessage", "./sounds/newmessage.wav").toString();
  settings.endGroup();
  settings.endGroup();
  settings.sync();
}

CSoundManager::~CSoundManager() {}
