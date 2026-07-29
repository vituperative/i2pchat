// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef I2PSAMMESSAGEANALYSER_H
#define I2PSAMMESSAGEANALYSER_H

#include <QObject>

namespace SAM_Message_Types {

enum TYPE { HELLO_REPLAY, SESSION_STATUS, STREAM_STATUS, NAMING_REPLY, DEST_REPLY, ERROR_IN_ANALYSE };

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
  CI2PSamMessageAnalyser(const QString &debugFrom = "not set");
  ~CI2PSamMessageAnalyser();

  CI2PSamMessageAnalyser(const CI2PSamMessageAnalyser &) = delete;
  CI2PSamMessageAnalyser &operator=(const CI2PSamMessageAnalyser &) = delete;

  const SAM_MESSAGE Analyse(const QString &Message) const;

private:
  const QString mDebugFrom;

  QString getValue(const QString &answer, const QString &key) const;
};
#endif
