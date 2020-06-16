#include "form_TopicSubscribe.h"
#include "Core.h"
#include <QMenu>

form_topicSubscribe::form_topicSubscribe(CCore &Core) : mCore(Core) {
  setupUi(this);

  treeWidget_SubscribeResults->setColumnCount(2);
  treeWidget_SubscribeResults->setHeaderLabels(QStringList() << tr("Topic Id")
                                                             << tr("values"));
  treeWidget_SubscribeResults->setContextMenuPolicy(Qt::CustomContextMenu);
  treeWidget_SubscribeResults->setColumnWidth(0, 250);
  treeWidget_SubscribeResults->setColumnWidth(1, 250);

  slot_onlineStateChanged();

  connect(cmd_subscribe, SIGNAL(pressed()), this, SLOT(slot_cmdSubscribe()));

  connect(cmd_close, SIGNAL(clicked()), this, SLOT(close()));

  connect(treeWidget_SubscribeResults,
          SIGNAL(customContextMenuRequested(QPoint)), this,
          SLOT(slot_showContextMenu(QPoint)));

  connect(&mCore, SIGNAL(signOnlineStatusChanged()), this,
          SLOT(slot_onlineStateChanged()));
}

form_topicSubscribe::~form_topicSubscribe() {}

void form_topicSubscribe::slot_cmdSubscribe() {
  treeWidget_SubscribeResults->clear();
  label_ResultCount->setText("0");
  label_SubscribeState->setText(tr("Starting"));

  // subscribeStruct.topicId=mCore.canonicalizeTopicId(inputField_topicId->text());

  cmd_subscribe->setEnabled(false);
  inputField_topicId->setEnabled(false); // FIXME queue subscription requests
  // mSeedlessManager.doSeedlessSubscribeToTopic(subscribeStruct);
}

void form_topicSubscribe::init() {}
void form_topicSubscribe::closeEvent(QCloseEvent *e) {
  e->ignore();
  emit signClosingTopicSubscribeWindow();
}
/*
void form_topicSubscribe::slot_SeedlessTopicSubscribeFinished(
        CSeedlessManager::SeedlessTopicSubscribeStruct SubscribeResult)
{
    QString TTLS;
    CSeedlessManager::SeedlessTopicSubscribeStruct &
currentResult(SubscribeResult); mSubscribeResult=SubscribeResult;
    treeWidget_SubscribeResults->clear();

    label_ResultCount->setText("1");

    QTreeWidgetItem* itemTopLevel= 	new QTreeWidgetItem;
    QTreeWidgetItem* itemTopicId= 	new QTreeWidgetItem;
    QTreeWidgetItem* itemTTL=       new QTreeWidgetItem;
    //QTreeWidgetItem* itemB32Dest = 	new QTreeWidgetItem;

    itemTopLevel->setText(0,currentResult.topicId);

    itemTopicId->setText(0,tr("Topic Id"));
    itemTopicId->setText(1,currentResult.topicId);

    TTLS.setNum(currentResult.ttl);
    itemTTL->setText(0,tr("TTL"));
    itemTTL->setText(1,TTLS);

    //itemB32Dest->setText(0,tr("b32"));
    //itemB32Dest->setText(1,currentResult.b32Dest);
    ////itemB32Dest->setHidden(true);

    itemTopLevel->addChild(itemTopicId);
    itemTopLevel->addChild(itemTTL);
    //itemTopLevel->addChild(itemB32Dest);

    treeWidget_SubscribeResults->addTopLevelItem(itemTopLevel);

    if(!cmd_subscribe->isEnabled()){
        cmd_subscribe->setEnabled(true);
        inputField_topicId->setEnabled(true);
    }
    treeWidget_SubscribeResults->sortByColumn(0,Qt::AscendingOrder);
}
*/
void form_topicSubscribe::slot_showContextMenu(const QPoint &pos) {
  QTreeWidgetItem *item = treeWidget_SubscribeResults->itemAt(pos);
  if (!item)
    return;

  QAction *openTopicAction =
      new QAction(QIcon(ICON_NEWUSER), tr("Open Topic"), this);
  connect(openTopicAction, SIGNAL(triggered()), this, SLOT(slot_openTopic()));

  QMenu menu(tr("Context menu"), this);
  menu.addAction(openTopicAction);
  menu.exec(treeWidget_SubscribeResults->viewport()->mapToGlobal(pos));
}

void form_topicSubscribe::slot_openTopic() {
  QTreeWidgetItem *item;
  QTreeWidgetItem *parent;
  QString topicId;
  // QString Destination;

  item = treeWidget_SubscribeResults->currentItem();
  if (item != NULL) {
    parent = item->parent();
    if (parent == NULL) {
      parent = item;
    }
  }
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
  this->setWindowState((windowState() & (~Qt::WindowMinimized)) |
                       Qt::WindowActive);
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
