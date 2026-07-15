// SPDX-License-Identifier: AGPL-3.0-or-later

#include "I2PSamMessageAnalyser.h"

CI2PSamMessageAnalyser::CI2PSamMessageAnalyser(const QString &debugFrom)
  : mDebugFrom(debugFrom) {
  // Constructor
}

const SAM_MESSAGE CI2PSamMessageAnalyser::Analyse(const QString &Message) const {
  using namespace SAM_Message_Types;

  SAM_MESSAGE t;
  QStringList list = Message.split(" ", Qt::SkipEmptyParts);

  if ((list[0] == "HELLO") && (list[1] == "REPLY")) {
    t.type = HELLO_REPLAY;

    if (list[2].contains("RESULT=OK") == true)
      t.result = OK;
    else if (list[2].contains("RESULT=NOVERSION") == true)
      t.result = NOVERSION;
    else if (list[2].contains("RESULT=I2P_ERROR") == true)
      t.result = I2P_ERROR;

    // Get Message
    t.Message = getValue(Message, "Message");
  } else if ((list[0].contains("SESSION") == true) && (list[1].contains("STATUS") == true)) {
    t.type = SESSION_STATUS;

    // Get Result
    if (list[2].contains("RESULT=OK", Qt::CaseInsensitive))
      t.result = OK;
    else if (list[2].contains("RESULT=DUPLICATED_ID", Qt::CaseInsensitive))
      t.result = DUPLICATED_ID;
    else if (list[2].contains("RESULT=DUPLICATED_DEST", Qt::CaseInsensitive))
      t.result = DUPLICATED_DEST;
    else if (list[2].contains("RESULT=I2P_ERROR", Qt::CaseInsensitive))
      t.result = I2P_ERROR;
    else if (list[2].contains("RESULT=INVALID_KEY", Qt::CaseInsensitive))
      t.result = INVALID_KEY;
    else {
      t.type = ERROR_IN_ANALYSE;
    }
    // Get Message
    t.Message = getValue(Message, "Message");

  } else if ((list[0].contains("STREAM") == true) && (list[1].contains("STATUS") == true)) {
    t.type = STREAM_STATUS;

    // Get Result
    if (list[2].contains("RESULT=OK", Qt::CaseInsensitive))
      t.result = OK;
    else if (list[2].contains("RESULT=CANT_REACH_PEER", Qt::CaseInsensitive))
      t.result = CANT_REACH_PEER;
    else if (list[2].contains("RESULT=I2P_ERROR", Qt::CaseInsensitive))
      t.result = I2P_ERROR;
    else if (list[2].contains("RESULT=INVALID_KEY", Qt::CaseInsensitive))
      t.result = INVALID_KEY;
    else if (list[2].contains("RESULT=INVALID_ID", Qt::CaseInsensitive))
      t.result = INVALID_ID;
    else if (list[2].contains("RESULT=TIMEOUT", Qt::CaseInsensitive))
      t.result = TIMEOUT;
    else if (list[2].contains("RESULT=ALREADY_ACCEPTING", Qt::CaseInsensitive))
      t.result = ALREADY_ACCEPTING;
    else {
      t.type = ERROR_IN_ANALYSE;
    }

    // Get Message
    t.Message = getValue(Message, "Message");
  } else if ((list[0].contains("NAMING") == true) && (list[1].contains("REPLY") == true)) {
    t.type = NAMING_REPLY;

    // get Result
    if (list[2].contains("RESULT=OK", Qt::CaseInsensitive))
      t.result = OK;
    else if (list[2].contains("RESULT=INVALID_KEY", Qt::CaseInsensitive))
      t.result = INVALID_KEY;
    else if (list[2].contains("RESULT=KEY_NOT_FOUND", Qt::CaseInsensitive))
      t.result = KEY_NOT_FOUND;
    else {
      t.type = ERROR_IN_ANALYSE;
      return t;
    }
    t.Name = getValue(Message, "NAME");
    t.Value = getValue(Message, "VALUE");
    t.Message = getValue(Message, "MESSAGE");
  } else if ((list[0].contains("DEST") == true) && (list[1].contains("REPLY") == true)) {
    t.type = DEST_REPLY;

    // get the Keys
    t.PUB = getValue(Message, "PUB");
    t.PRIV = getValue(Message, "PRIV");

    // Get Message
    t.Message = getValue(Message, "Message");
  } else {
    t.type = ERROR_IN_ANALYSE;
  }

  return t;
}

CI2PSamMessageAnalyser::~CI2PSamMessageAnalyser() {}

QString CI2PSamMessageAnalyser::getValue(const QString &answer, const QString &key) const {
  if (key.isEmpty()) {
    return "";
  }

  const QString keyPattern = key + "=";
  size_t valueStart = answer.indexOf(keyPattern);
  if (valueStart == (size_t)-1) {
    return "";
  }

  valueStart += keyPattern.length();
  size_t valueEnd = answer.indexOf(' ', valueStart);

  if (valueEnd == (size_t)-1) {
    valueEnd = answer.indexOf('\n', valueStart);
  }

  return answer.mid(valueStart, valueEnd - valueStart);
}