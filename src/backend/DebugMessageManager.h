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
#ifndef DEBUGMESSAGEMANAGER_H
#define DEBUGMESSAGEMANAGER_H

#include <QSettings>
#include <QtGui>

#include "ConnectionManager.h"

class CDebugMessageManager : public QObject {
  Q_OBJECT

public:
  CDebugMessageManager(QString Group, QString configPath);
  ~CDebugMessageManager();

  // forbid some operators
  CDebugMessageManager(const CDebugMessageManager &) = delete;
  CDebugMessageManager &operator=(const CDebugMessageManager &) = delete;

public slots:
  void doClearAllMessages();
  const QStringList getAllMessages() const { return mMessages; }

private slots:
  void slotNewIncomingDebugMessage(const QString Message);

signals:
  void signNewDebugMessage(QString Message);

private:
  quint32 mMaxMessageCount;
  QStringList mMessages;
};
#endif
