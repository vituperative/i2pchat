/***************************************************************************
 *   Copyright (C) 2008 by I2P-Messenger   				   *
 *   Messenger-Dev@I2P-Messenger   					   *
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
#include "DebugMessageManager.h"

//cDebugMessageManager::cDebugMessageManager(CConnectionManager* ConnectionManager)
CDebugMessageManager::CDebugMessageManager(QString Group, QString configPath)
{

	QSettings settings(configPath+"/application.ini",QSettings::IniFormat);
	settings.beginGroup(Group);
		this->mMaxMessageCount=settings.value("Debug_Max_Message_count","20").toInt();
	settings.endGroup();
	settings.sync();

	slotNewIncomingDebugMessage("• I2PChat Debug Message Manager started");
}

CDebugMessageManager::~CDebugMessageManager()
{

}

void CDebugMessageManager::doClearAllMessages()
{
	mMessages.clear();
}


void CDebugMessageManager::slotNewIncomingDebugMessage(const QString Message){
	while(mMessages.count()>= (signed int)mMaxMessageCount){
		mMessages.removeLast();
	}

	mMessages.prepend(Message);
	emit signNewDebugMessage(Message);
}

