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
#ifndef I2PSAMMESSAGEANALYSER_H
#define I2PSAMMESSAGEANALYSER_H

#include <QMessageBox>
#include <QtGui>

namespace SAM_Message_Types {

enum TYPE {
  HELLO_REPLAY,
  SESSION_STATUS,
  STREAM_STATUS,
  NAMING_REPLY,
  DEST_REPLY,
  ERROR_IN_ANALYSE
};

enum RESULT {
  OK,
  DUPLICATED_DEST,
  DUPLICATED_ID,
  I2P_ERROR,
  INVALID_ID,
  INVALID_KEY,
  CANT_REACH_PEER,
  TIMEOUT,
  FAILED,
  NOVERSION,
  KEY_NOT_FOUND,
  PEER_NOT_FOUND,
  ALREADY_ACCEPTING,
  CLOSED
};

} // namespace SAM_Message_Types

struct SAM_MESSAGE {
public:
  QString Message;

  QString Name;
  QString Value;
  QString PUB;  //	PublicKey from Dest Replay
  QString PRIV; //	PrivKey from Dest Replay
  SAM_Message_Types::TYPE type;
  SAM_Message_Types::RESULT result;
};

class CI2PSamMessageAnalyser : public QObject {
  Q_OBJECT
public:
  CI2PSamMessageAnalyser(const QString debugFrom = "not set");
  ~CI2PSamMessageAnalyser();

  // forbid some operators
  CI2PSamMessageAnalyser(const CI2PSamMessageAnalyser &) = delete;
  CI2PSamMessageAnalyser &operator=(const CI2PSamMessageAnalyser &) = delete;

  const SAM_MESSAGE Analyse(QString Message) const;

private:
  const QString mDebugFrom;

  QString getValue(const QString &answer, const QString &key) const;
};
#endif
