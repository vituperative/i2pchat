#include "form_UserSearch.h"
#include "Core.h"

form_userSearch::form_userSearch(CCore &Core) : mCore(Core) {
  setupUi(this);

  treeWidget_SearchResults->setColumnCount(2);
  treeWidget_SearchResults->setHeaderLabels(QStringList()
                                            << tr("Nicknames") << tr("values"));
  treeWidget_SearchResults->setContextMenuPolicy(Qt::CustomContextMenu);
  treeWidget_SearchResults->setColumnWidth(0, 250);
  treeWidget_SearchResults->setColumnWidth(1, 250);

  slot_onlineStateChanged();

  connect(cmd_search, SIGNAL(pressed()), this, SLOT(slot_cmdSearch()));

  connect(cmd_close, SIGNAL(clicked()), this, SLOT(close()));

  connect(checkGender_Female, SIGNAL(clicked()), this,
          SLOT(slot_genderFemale()));

  connect(checkGender_Male, SIGNAL(clicked()), this, SLOT(slot_genderMale()));

  connect(treeWidget_SearchResults, SIGNAL(customContextMenuRequested(QPoint)),
          this, SLOT(slot_showContextMenu(QPoint)));

  connect(&mCore, SIGNAL(signOnlineStatusChanged()), this,
          SLOT(slot_onlineStateChanged()));
}

form_userSearch::~form_userSearch() {}

void form_userSearch::slot_cmdSearch() {
  treeWidget_SearchResults->clear();
  label_ResultCount->setText("0");
  label_SearchState->setText(tr("Starting"));

  cmd_search->setEnabled(false);
}

void form_userSearch::init() {
  spinBox_Age->setMinimum(0);
  spinBox_Age->setMaximum(100);
}
void form_userSearch::closeEvent(QCloseEvent *e) {
  e->ignore();
  emit signClosingUserSearchWindow();
}

/*void form_userSearch::slot_SeedlessSearchFinished(QMap< QString,
CSeedlessManager::SeedlessSearchStruct > SearchResults)
{
    QString TTLS;
    QList<CSeedlessManager::SeedlessSearchStruct>
ListResults=SearchResults.values(); CSeedlessManager::SeedlessSearchStruct
currentResult; mSearchResults=SearchResults; treeWidget_SearchResults->clear();

    label_ResultCount->setText(QString::number(SearchResults.count()));

    for(int i=0;i<ListResults.count();i++){
        currentResult=ListResults.at(i);
        QTreeWidgetItem* itemTopLevel= 	new QTreeWidgetItem;
        QTreeWidgetItem* itemNickname= 	new QTreeWidgetItem;
        QTreeWidgetItem* itemAge= 	new QTreeWidgetItem;
        QTreeWidgetItem* itemGender= 	new QTreeWidgetItem;
        QTreeWidgetItem* itemInterests=	new QTreeWidgetItem;
        QTreeWidgetItem* itemTTL= 	new QTreeWidgetItem;
        QTreeWidgetItem* itemB32Dest = 	new QTreeWidgetItem;

        itemTopLevel->setText(0,currentResult.NickName);

        itemNickname->setText(0,tr("Nickname"));
        itemNickname->setText(1,currentResult.NickName);

        itemAge->setText(0,tr("Age"));
        itemAge->setText(1,QString::number(currentResult.Age));

        itemGender->setText(0,tr("Gender"));
        itemGender->setText(1,currentResult.Gender);

        itemInterests->setText(0,tr("Interests"));
        itemInterests->setText(1,currentResult.Interests);

        TTLS.setNum(currentResult.TTL);
        itemTTL->setText(0,tr("TTL"));
        itemTTL->setText(1,TTLS);

        itemB32Dest->setText(0,tr("b32"));
        itemB32Dest->setText(1,currentResult.b32Dest);
        //itemB32Dest->setHidden(true);

        itemTopLevel->addChild(itemNickname);
        itemTopLevel->addChild(itemAge);
        itemTopLevel->addChild(itemGender);
        itemTopLevel->addChild(itemInterests);
        itemTopLevel->addChild(itemTTL);
        itemTopLevel->addChild(itemB32Dest);

        treeWidget_SearchResults->addTopLevelItem(itemTopLevel);
    }
    if(cmd_search->isEnabled()==false){
        cmd_search->setEnabled(true);
    }
    treeWidget_SearchResults->sortByColumn(0,Qt::AscendingOrder);
}
*/
void form_userSearch::slot_showContextMenu(const QPoint &pos) {
  QTreeWidgetItem *item = treeWidget_SearchResults->itemAt(pos);
  if (!item)
    return;

  QAction *UserAdd = new QAction(QIcon(ICON_NEWUSER), tr("Add User"), this);
  connect(UserAdd, SIGNAL(triggered()), this, SLOT(slot_addUser()));

  QMenu menu(tr("Context menu"), this);
  menu.addAction(UserAdd);
  menu.exec(treeWidget_SearchResults->viewport()->mapToGlobal(pos));
}

void form_userSearch::slot_addUser() {
  QTreeWidgetItem *item;
  QTreeWidgetItem *parent;
  QString Nickname;
  QString Destination;

  item = treeWidget_SearchResults->currentItem();
  if (item != NULL) {
    parent = item->parent();
    if (parent == NULL) {
      parent = item;
    }
    //     qDebug()<<"0\t"<<parent->child(0)->text(1)<<endl;
    //     qDebug()<<"1\t"<<parent->child(1)->text(1)<<endl;
    //     qDebug()<<"2\t"<<parent->child(2)->text(1)<<endl;
    //     qDebug()<<"3\t"<<parent->child(3)->text(1)<<endl;
    //     qDebug()<<"4\t"<<parent->child(4)->text(1)<<endl;
    //     qDebug()<<"5\t"<<parent->child(5)->text(1)<<endl;

    Destination = parent->child(1)->text(1);
    /*
    CSeedlessManager::SeedlessSearchStruct
    tmp=mSearchResults.find(Destination).value();

    if(mSeedlessManager.addNewUser(tmp)==true){
        mSearchResults.remove(Destination);
        slot_SeedlessSearchFinished(mSearchResults);
    }*/
  }
}
void form_userSearch::slot_onlineStateChanged() {
  ONLINESTATE status = mCore.getOnlineStatus();

  if (status != USEROFFLINE && status != USERTRYTOCONNECT) {
    label_SearchState->setText(tr("Ready"));
    cmd_search->setEnabled(true);
  } else {
    label_SearchState->setText(tr("Offline"));
    cmd_search->setEnabled(false);
  }
}
void form_userSearch::getFocus() {
  this->activateWindow();
  this->setWindowState((windowState() & (~Qt::WindowMinimized)) |
                       Qt::WindowActive);
  this->raise();
}
void form_userSearch::slot_genderMale() {
  if (checkGender_Male->isChecked() && checkGender_Female->isChecked()) {
    checkGender_Male->setChecked(false);
  }
}
void form_userSearch::slot_genderFemale() {
  if (checkGender_Male->isChecked() && checkGender_Female->isChecked()) {
    checkGender_Female->setChecked(false);
  }
}

void form_userSearch::keyPressEvent(QKeyEvent *event) {
  if (event->key() != Qt::Key_Escape) {
    QDialog::keyPressEvent(event);
  } else {
    event->accept();
    close();
  }
}
