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
#include "ChatmessageChanger.h"
#include "Core.h"

CChatMessageChanger* CChatMessageChanger::instanz=NULL;

CChatMessageChanger::CChatMessageChanger(QString& ConfigPath)
{
	mControllForChange=NULL;

	QSettings settings(ConfigPath,QSettings::IniFormat);
	settings.beginGroup("Chat");
		mDoChange=settings.value("Override",false).toBool();
		if(mDoChange==true){
			mControllForChange= new QTextEdit();
			mFont.fromString(settings.value("FontForOverwrite","SansSerif,10").toString());
			mColor.setNamedColor(settings.value("ColorForOverwrite","#000000").toString());

		}
	settings.endGroup();
}


CChatMessageChanger::~CChatMessageChanger()
{
	if(mControllForChange!=NULL){
		delete mControllForChange;
	}
}

QString CChatMessageChanger::changeChatMessage(QString Chatmessage)
{
	if(mDoChange==true){
		mControllForChange->clear();
		mControllForChange->insertHtml(Chatmessage);
		mControllForChange->selectAll();
		mControllForChange->setCurrentFont(mFont);
		mControllForChange->setTextColor(mColor);
		Chatmessage=mControllForChange->toHtml();
	}

	return Chatmessage;

}

CChatMessageChanger* CChatMessageChanger::exemplar(CCore& Core)
{
	if(instanz==NULL){
	    QString FileNameWithPath=Core.getConfigPath();
	    FileNameWithPath.append("/application.ini");
	    instanz= new CChatMessageChanger(FileNameWithPath);
	}
	return instanz;
}

