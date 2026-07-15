// SPDX-License-Identifier: AGPL-3.0-or-later

#include "form_HelpDialog.h"

#include <QContextMenuEvent>
#include <QCursor>
#include <QMenu>
#include <QMouseEvent>
#include <QPixmap>
#include <QPoint>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <iostream>
#include <sstream>

form_HelpDialog::form_HelpDialog(QString ProgrammVersion, QString ProtocolVersion, QString ConfigPath, QWidget *parent)
  : QDialog(parent)
  , mConfigPath(ConfigPath) {

  ui.setupUi(this);
  this->setAttribute(Qt::WA_DeleteOnClose, true);

  QString tmp;

  QFile aboutFile(QString(ConfigPath + "/about.html"));
  if (aboutFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&aboutFile);
    tmp = in.readAll();
    tmp.replace("[APPVERSIONSTRING]", ProgrammVersion);
    tmp.replace("[PROTOCOLVERSIONSTRING]", ProtocolVersion);
    ui.about->setText(tmp);
  }

  /*
    QFile authorsFile(QLatin1String(":/authors.html"));
     if (authorsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
          QTextStream in(&authorsFile);
          ui.authors->setText(in.readAll());
     }
    QFile thanksFile(QLatin1String(":/thanks.html"));
     if (thanksFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
          QTextStream in(&thanksFile);
          ui.thanks->setText(in.readAll());
     }
    */
  // ui.authors->setText

  ui.label_2->setMinimumWidth(20);
}
