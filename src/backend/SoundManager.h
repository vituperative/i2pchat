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
#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <QSettings>
#include <QSound>
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
  void slotFileReciveIncoming();
  void slotFileReciveFinished();
  void slotNewChatMessage();

private:
  bool mIsMute;
  QString mSoundFileUser_go_Online;
  QString mSoundFileUser_go_Offline;
  QString mSoundFileFileSend_Finished;
  QString mSoundFileFileRecive_Incoming;
  QString mSoundFileFileRecive_Finished;
  QString mSoundFileNewChatMessage;
  const QString mConfigPath;

  bool mEnable_eventUser_go_Online;
  bool mEnable_eventUser_go_Offline;
  bool mEnable_eventFileSend_Finished;
  bool mEnable_eventFileRecive_Incoming;
  bool mEnable_eventFileRecive_Finished;
  bool mEnable_eventNewChatMessage;
};
#endif
