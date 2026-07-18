#include "form_TopicSubscribe.h"

#include "Core.h"

#include <QMenu>

form_topicSubscribe::form_topicSubscribe(CCore &Core)
  : mCore(Core) {
  setupUi(this);

  treeWidget_SubscribeResults->setColumnCount(2);
  treeWidget_SubscribeResults->setHeaderLabels(QStringList() << tr("Topic Id") << tr("values"));
  treeWidget_SubscribeResults->setContextMenuPolicy(Qt::CustomContextMenu);
  treeWidget_SubscribeResults->setColumnWidth(0, 250);
  treeWidget_SubscribeResults->setColumnWidth(1, 250);

  slot_onlineStateChanged();

  connect(cmd_subscribe, SIGNAL(pressed()), this, SLOT(slot_cmdSubscribe()));

  connect(cmd_close, SIGNAL(clicked()), this, SLOT(close()));

  connect(
    treeWidget_SubscribeResults, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slot_showContextMenu(QPoint)));

  connect(&mCore, SIGNAL(signOnlineStatusChanged()), this, SLOT(slot_onlineStateChanged()));
}

form_topicSubscribe::~form_topicSubscribe() {}

void form_topicSubscribe::slot_cmdSubscribe() {
  treeWidget_SubscribeResults->clear();
  label_ResultCount->setText("0");
  label_SubscribeState->setText(tr("Starting"));

  cmd_subscribe->setEnabled(false);
  inputField_topicId->setEnabled(false);
}

void form_topicSubscribe::closeEvent(QCloseEvent *e) {
  e->ignore();
  emit signClosingTopicSubscribeWindow();
}
void form_topicSubscribe::slot_showContextMenu(const QPoint &pos) {
  QTreeWidgetItem *item = treeWidget_SubscribeResults->itemAt(pos);
  if (!item)
    return;

  QAction *openTopicAction = new QAction(QIcon(ICON_NEWUSER), tr("Open Topic"), this);
  connect(openTopicAction, SIGNAL(triggered()), this, SLOT(slot_openTopic()));

  QMenu menu(tr("Context menu"), this);
  menu.addAction(openTopicAction);
  menu.exec(treeWidget_SubscribeResults->viewport()->mapToGlobal(pos));
}

void form_topicSubscribe::slot_openTopic() {
  QTreeWidgetItem *item = treeWidget_SubscribeResults->currentItem();
  Q_UNUSED(item);
}
void form_topicSubscribe::slot_onlineStateChanged() {
  ONLINESTATE status = mCore.getOnlineStatus();

  if (status != USEROFFLINE && status != USERTRYTOCONNECT) {
    label_ConnectednessStatus->setText(tr("I am online"));

    // FIXME this request should be persistently queued, and not be depending on
    // connectedness state
    cmd_subscribe->setEnabled(true);
    inputField_topicId->setEnabled(true);
  } else {
    label_ConnectednessStatus->setText(tr("I am offline or connecting"));

    // FIXME this request should be persistently queued, and not be depending on
    // connectedness state
    cmd_subscribe->setEnabled(false);
    inputField_topicId->setEnabled(false);
  }
}

void form_topicSubscribe::requestFocus() {
  this->activateWindow();
  this->setWindowState((windowState() & (~Qt::WindowMinimized)) | Qt::WindowActive);
  this->raise();
}

void form_topicSubscribe::keyPressEvent(QKeyEvent *event) {
  if (event->key() != Qt::Key_Escape) {
    QDialog::keyPressEvent(event);
  } else {
    event->accept();
    close();
  }
}
