// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <QSettings>
#include <QSoundEffect>
#include <QtGui>

class CSoundManager : public QObject {
  Q_OBJECT
public:
  CSoundManager(QString ConfigPath);
  ~CSoundManager();

  // forbid some operators
  CSoundManager(const CSoundManager &) = delete;
  CSoundManager &operator=(const CSoundManager &) = delete;

  void reInit();
  void doMute(bool t);
public slots:

  void slotUserGoOnline();
  void slotUserGoOffline();
  void slotFileSendFinished();
  void slotFileReceiveIncoming();
  void slotFileReceiveFinished();
  void slotNewChatMessage();

private:
  bool mIsMute;
  QString mSoundFileUser_go_Online;
  QString mSoundFileUser_go_Offline;
  QString mSoundFileFileSend_Finished;
  QString mSoundFileFileReceive_Incoming;
  QString mSoundFileFileReceive_Finished;
  QString mSoundFileNewChatMessage;
  const QString mConfigPath;

  bool mEnable_eventUser_go_Online;
  bool mEnable_eventUser_go_Offline;
  bool mEnable_eventFileSend_Finished;
  bool mEnable_eventFileReceive_Incoming;
  bool mEnable_eventFileReceive_Finished;
  bool mEnable_eventNewChatMessage;
  QSoundEffect *mSoundEffect;
};
#endif
