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
#include <QErrorMessage>
#include <QSettings>

#include "Core.h"
#include "FileTransferSend.h"
#include "I2PStream.h"
#include "Protocol.h"
#include "User.h"
#include "UserManager.h"

#include <iostream>
using namespace std;

QString gethttpheader(QString pagedata) {
	QString header;
	header  = "HTTP/1.0 200\r\nContent-Length: " + QString::number(pagedata.size()) + "\r\n\r\n";
	return header;
}

QString pngtobase64(QByteArray pngdata) {
	return pngdata.toBase64();
}

//QString processtags(QString htmldata) {
//	htmldata.
//}

QString loadfile(QString filename) {

	QString filecontents;
	QFile f(filename);
	if (!f.open(QFile::ReadOnly | QFile::Text)){
		return "";
	}
	QTextStream in(&f);
	filecontents = in.readAll();
	f.close();
	return filecontents;
}

