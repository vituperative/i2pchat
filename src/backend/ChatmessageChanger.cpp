// SPDX-License-Identifier: AGPL-3.0-or-later

#include "ChatmessageChanger.h"

#include "Core.h"

CChatMessageChanger *CChatMessageChanger::instanz = NULL;

CChatMessageChanger::CChatMessageChanger(QString &ConfigPath) {
  mControllForChange = NULL;

  QSettings settings(ConfigPath, QSettings::IniFormat);
  settings.beginGroup("Chat");
  mDoChange = settings.value("Override", false).toBool();
  if (mDoChange == true) {
    mControllForChange = new QTextEdit();
    mFont.fromString(settings.value("FontForOverwrite", "SansSerif,10").toString());
    mColor.setNamedColor(settings.value("ColorForOverwrite", "#000000").toString());
  }
  settings.endGroup();
}

CChatMessageChanger::~CChatMessageChanger() {
  if (mControllForChange != NULL) {
    delete mControllForChange;
  }
}

QString CChatMessageChanger::changeChatMessage(QString Chatmessage) {
  if (mDoChange == true) {
    mControllForChange->clear();
    mControllForChange->insertHtml(Chatmessage);
    mControllForChange->selectAll();
    mControllForChange->setCurrentFont(mFont);
    mControllForChange->setTextColor(mColor);
    Chatmessage = mControllForChange->toHtml();
  }

  return Chatmessage;
}

CChatMessageChanger *CChatMessageChanger::exemplar(CCore &Core) {
  if (instanz == NULL) {
    QString FileNameWithPath = Core.getConfigPath();
    FileNameWithPath.append("/application.ini");
    instanz = new CChatMessageChanger(FileNameWithPath);
  }
  return instanz;
}
