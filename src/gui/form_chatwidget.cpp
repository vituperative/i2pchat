#include "form_chatwidget.h"

#include "ChatDelegate.h"
#include "User.h"

#include <QDir>
#include <QErrorMessage>
#include <QFile>
#include <QMap>
#include <QMenu>
#include <QTextStream>
#include <QVBoxLayout>
#include <QXmlStreamReader>

bool ChatEventEater::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if (obj->objectName() == "message") {
      if (keyEvent->key() == Qt::Key_Return && keyEvent->modifiers() == Qt::NoModifier) {
        emit sendMessage();
        return true;
      }
      if (keyEvent->key() == Qt::Key_Return && keyEvent->modifiers() == Qt::ControlModifier) {
        emit sendMessage();
        return true;
      }
    }
    return QObject::eventFilter(obj, event);
  }

  if (event->type() == QEvent::FocusIn) {
    emit haveFocus(true);
    return true;
  }
  if (event->type() == QEvent::FocusOut) {
    emit haveFocus(false);
    return true;
  }

  return false;
}

form_ChatWidget::form_ChatWidget(CUser &user, CCore &Core, QDialog *parent /* = 0 */)
  : QMainWindow(parent, Qt::WindowFlags())
  , user(user)
  , Core(Core) {
  setupUi(this);

  QTextEdit *message = this->message;
  QToolButton *txtBold = this->txtBold;
  QToolButton *txtItalic = this->txtItalic;
  QToolButton *txtUnder = this->txtUnder;

  // Both widgets share the same chatPanel layout via a stacked widget
  mChatModel = new QStandardItemModel(this);
  mChatDelegate = new ChatDelegate(this);

  mChatListView = new QListView;
  mChatListView->setModel(mChatModel);
  mChatListView->setItemDelegate(mChatDelegate);
  mChatListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  mChatListView->setSelectionMode(QAbstractItemView::ContiguousSelection);
  mChatListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  mChatListView->setResizeMode(QListView::Adjust);
  mChatListView->setUniformItemSizes(false);
  mChatListView->setSpacing(0);
  mChatListView->setFont(chat->font());
  mChatListView->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(
    mChatListView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));
  connect(mChatDelegate, SIGNAL(linkClicked(const QUrl &)), this, SLOT(anchorClicked(const QUrl &)));

  chat->setOpenLinks(false);
  connect(chat, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(anchorClicked(const QUrl &)));

  mChatStack = new QStackedWidget;
  auto *vlayout = qobject_cast<QVBoxLayout *>(chatPanel->layout());
  for (int i = 0; i < vlayout->count(); i++) {
    if (vlayout->itemAt(i)->widget() == chat) {
      vlayout->removeWidget(chat);
      break;
    }
  }
  mChatStack->addWidget(chat);          // page 0 = classic
  mChatStack->addWidget(mChatListView); // page 1 = themed
  vlayout->insertWidget(0, mChatStack);
  mFileWatcher = new QFileSystemWatcher(this);
  {
    QString themeDir = Core.getConfigPath() + "/themes/chat";
    qDebug() << "theme dir:" << themeDir << "exists:" << QDir(themeDir).exists();
    bool ok = mFileWatcher->addPath(themeDir);
    qDebug() << "  addPath returned:" << ok;
  }
  mReloadTimer = new QTimer(this);
  mReloadTimer->setSingleShot(true);
  mReloadTimer->setInterval(200);
  connect(mReloadTimer, &QTimer::timeout, this, &form_ChatWidget::loadChatStyle);
  connect(mFileWatcher, &QFileSystemWatcher::fileChanged, this, [this](const QString &p) {
    qDebug() << "fileChanged:" << p;
    mReloadTimer->start();
  });
  connect(mFileWatcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString &p) {
    qDebug() << "directoryChanged:" << p;
    mReloadTimer->start();
  });
  m_event_eater = new ChatEventEater(this);

  connect(m_event_eater, SIGNAL(sendMessage()), send, SLOT(click()));

  connect(m_event_eater, SIGNAL(haveFocus(bool)), this, SLOT(focusEvent(bool)));

  message->installEventFilter(m_event_eater);

  connect(&user, SIGNAL(signNewMessageReceived()), this, SLOT(newMessageReceived()));

  connect(&user, SIGNAL(signOnlineStateChanged()), this, SLOT(changeWindowsTitle()));

  connect(&user, SIGNAL(signUserDeleted()), this, SLOT(close()));

  connect(&user, SIGNAL(signNewAvatarImage()), this, SLOT(remoteAvatarImageChanged()));

  connect(&user, SIGNAL(signPendingCanceled()), this, SLOT(slotPendingCanceled()));

  connect(this, SIGNAL(sendChatMessage(QString)), &user, SLOT(slotSendChatMessage(QString)));

  connect(cmd_SendFile, SIGNAL(clicked()), this, SLOT(newFileTransfer()));

  connect(&Core, SIGNAL(signChatStyleChanged()), this, SLOT(loadChatStyle()));

  connect(avatarFrameButton, SIGNAL(toggled(bool)), this, SLOT(showAvatarFrame(bool)));

  connect(message, SIGNAL(textChanged()), this, SLOT(messageTextChanged()));

  setAcceptDrops(true);

  mCurrentFont = user.getTextFont();
  textColor = user.getTextColor();
  mHaveFocus = false;

  QPixmap pxm(22, 22);
  pxm.fill(textColor);
  // txtColor->setIcon(pxm);

  connect(send, SIGNAL(clicked()), SLOT(sendMessageSignal()));
  connect(txtColor, SIGNAL(clicked()), SLOT(setTextColor()));
  connect(txtBold, SIGNAL(clicked(bool)), SLOT(setBold(bool)));
  connect(txtFont, SIGNAL(clicked()), SLOT(setFont()));
  connect(txtUnder, SIGNAL(clicked(bool)), SLOT(setUnderline(bool)));
  connect(txtItalic, SIGNAL(clicked(bool)), SLOT(setItalic(bool)));
  QPalette pal = message->palette();
  pal.setBrush(QPalette::Text, QBrush(textColor));
  message->setPalette(pal);

  message->setCurrentFont(mCurrentFont);
  message->setFont(mCurrentFont);

  txtBold->setChecked(mCurrentFont.bold());
  txtItalic->setChecked(mCurrentFont.italic());
  txtUnder->setChecked(mCurrentFont.underline());

  resize(480, 360); // maybe too from QSS?
  centerDialog();

  loadChatStyle();
  changeWindowsTitle();

  slotLoadOwnAvatarImage();

  useravatar_label->setAlignment(Qt::AlignCenter);
  remoteAvatarImageChanged();

  // QTimer *updater_chat = new QTimer(this);
  // connect(timer, &QTimer::timeout, this, &form_ChatWidget::raise);
  // timer->start();
}

void form_ChatWidget::loadChatStyle() {
  QSettings settings(Core.getConfigPath() + "/application.ini", QSettings::IniFormat);
  QString style = settings.value("Chat/ChatStyle", "classic").toString();
  mChatStyle = style;
  qDebug() << "loadChatStyle: style =" << style;
  QString styleLower = style.toLower();

  bool themed = (styleLower != "classic");
  if (styleLower == "bubbles") {
    mBubbleStyle = {"#075e54", "#ffffff", "#e5e5ea", "#1c1c1c", "#888888", 3, 2, 6, 0, true};
  } else if (styleLower == "modern") {
    mBubbleStyle = {"#8774e1", "#ffffff", "#e8f0fe", "#1c1c1c", "#888888", 3, 2, 6, 0, true};
  } else {
    mBubbleStyle = {"", "", "", "", "#888888", 0, 0, 0, 0, false};
  }

  ChatDelegate::BubbleColors dc;
  dc.sentBg = mBubbleStyle.sentBg;
  dc.sentFg = mBubbleStyle.sentColor;
  dc.receivedBg = mBubbleStyle.receivedBg;
  dc.receivedFg = mBubbleStyle.receivedColor;
  dc.systemColor = mBubbleStyle.systemColor;
  dc.radius = mBubbleStyle.radius;
  dc.padV = mBubbleStyle.padV;
  dc.padH = mBubbleStyle.padH;
  dc.padVInner = mBubbleStyle.padVInner;
  dc.pendingBg = "#fff3cd";
  dc.pendingFg = "#856404";

  if (themed)
    applyThemeCss(style, mBubbleStyle, dc);

  txtColor->setVisible(!themed);
  txtFont->setVisible(!themed);

  mChatStack->setCurrentIndex(themed ? 1 : 0);
  mChatDelegate->setBubbleColors(dc);
  if (themed)
    addAllMessages();
  else
    addAllMessagesClassic();

  // Watch the current theme CSS for live reload
  // (directory is watched continuously from constructor — catches atomic-save recreation)
  QStringList oldFiles = mFileWatcher->files();
  for (const auto &f : oldFiles)
    mFileWatcher->removePath(f);
  if (themed) {
    QString cap = mChatStyle;
    QString cssPath = Core.getConfigPath() + "/themes/chat/" + cap + ".css";
    if (!QFile::exists(cssPath)) {
      cap[0] = cap[0].toUpper();
      cssPath = Core.getConfigPath() + "/themes/chat/" + cap + ".css";
    }
    if (QFile::exists(cssPath))
      mFileWatcher->addPath(cssPath);
    // If file doesn't exist yet (atomic-save race), directoryChanged will retry
  }
}

static QStringList cssSplit(const QString &s) {
  QStringList parts;
  int depth = 0, start = 0;
  for (int i = 0; i < s.size(); i++) {
    if (s[i] == '(')
      depth++;
    else if (s[i] == ')')
      depth--;
    else if (s[i] == ',' && depth == 0) {
      parts << s.mid(start, i - start).trimmed();
      start = i + 1;
    }
  }
  parts << s.mid(start).trimmed();
  return parts;
}

static QColor parseCSSColor(const QString &raw) {
  QString s = raw.trimmed();
  if (s.isEmpty())
    return QColor();
  // rgba(r,g,b,a) or rgb(r,g,b)
  static QRegularExpression funRgb("rgba?\\((\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*(?:,\\s*([\\d.]+)\\s*)?\\)",
                                   QRegularExpression::CaseInsensitiveOption);
  auto m = funRgb.match(s);
  if (m.hasMatch()) {
    int r = m.captured(1).toInt(), g = m.captured(2).toInt(), b = m.captured(3).toInt();
    int a = 255;
    if (!m.captured(4).isEmpty())
      a = qBound(0, qRound(m.captured(4).toDouble() * 255.0), 255);
    return QColor(r, g, b, a);
  }
  // 8-digit hex: #RRGGBBAA
  if (s.startsWith('#') && s.length() == 9) {
    bool ok;
    QRgb rgba = s.mid(1).toUInt(&ok, 16);
    if (ok)
      return QColor(qRed(rgba), qGreen(rgba), qBlue(rgba), qAlpha(rgba));
  }
  // 4-digit hex shorthand: #RGBA → #RRGGBBAA
  if (s.startsWith('#') && s.length() == 5) {
    QString rr = QString(2, s[1]);
    QString gg = QString(2, s[2]);
    QString bb = QString(2, s[3]);
    QString aa = QString(2, s[4]);
    bool ok;
    QRgb rgba = (rr + gg + bb + aa).toUInt(&ok, 16);
    if (ok)
      return QColor(qRed(rgba), qGreen(rgba), qBlue(rgba), qAlpha(rgba));
  }
  return QColor(s); // hex/named (returns invalid if unparseable)
}

static BubbleGradient parseGradient(const QString &val) {
  BubbleGradient g;
  static QRegularExpression gradRx("linear-gradient\\s*\\(\\s*(.*)\\s*\\)",
                                   QRegularExpression::CaseInsensitiveOption |
                                     QRegularExpression::DotMatchesEverythingOption);
  auto m = gradRx.match(val);
  if (!m.hasMatch())
    return g;

  QString body = m.captured(1).trimmed();
  // Extract angle/direction from start
  double angle = -1;
  static QRegularExpression degRx("^(\\d+(?:\\.\\d+)?)\\s*deg\\s*,\\s*", QRegularExpression::CaseInsensitiveOption);
  auto dm = degRx.match(body);
  if (dm.hasMatch()) {
    angle = dm.captured(1).toDouble();
    body = body.mid(dm.capturedLength()).trimmed();
  } else {
    static QRegularExpression toRx("^to\\s+(\\S+)\\s*,\\s*", QRegularExpression::CaseInsensitiveOption);
    auto tm = toRx.match(body);
    if (tm.hasMatch()) {
      QString dir = tm.captured(1);
      if (dir == "top")
        angle = 0;
      else if (dir == "bottom")
        angle = 180;
      else if (dir == "left")
        angle = 270;
      else
        angle = 90; // right
      body = body.mid(tm.capturedLength()).trimmed();
    }
  }
  g.angle = (angle < 0) ? 180 : angle; // default bottom-to-top

  // Split remaining by top-level commas
  QStringList parts = cssSplit(body);
  // Map named positions
  static QRegularExpression pctRx("^(.+)\\s+(\\d+(?:\\.\\d+)?)%$");
  int autoCount = parts.size();
  for (const auto &p : parts) {
    auto pm = pctRx.match(p);
    if (pm.hasMatch()) {
      double pos = pm.captured(2).toDouble() / 100.0;
      QColor c = parseCSSColor(pm.captured(1));
      if (c.isValid())
        g.stops.append({pos, c});
    } else {
      QColor c = parseCSSColor(p);
      if (c.isValid())
        g.stops.append({-1, c}); // auto position
    }
  }
  if (autoCount == g.stops.size()) {
    // All auto: distribute evenly
    for (int i = 0; i < g.stops.size(); i++)
      g.stops[i].pos = (double)i / (g.stops.size() - 1);
  }
  return g;
}

static QVector<BubbleShadow> parseShadows(const QString &val) {
  QVector<BubbleShadow> shadows;
  QString v = val.trimmed().toLower();
  if (v == "none" || v.isEmpty())
    return shadows;
  QStringList parts = cssSplit(val);
  for (const auto &p : parts) {
    QStringList tokens = p.trimmed().split(QRegularExpression("\\s+"));
    if (tokens.isEmpty())
      continue;
    BubbleShadow sh = {};
    int idx = 0;
    if (!tokens.isEmpty() && tokens[0].toLower() == "inset") {
      sh.inset = true;
      idx = 1;
    }
    // Parse numeric px values in order (accept unitless 0)
    for (; idx < tokens.size(); idx++) {
      QString t = tokens[idx];
      bool ok = false;
      int n;
      if (t == "0") {
        n = 0;
        ok = true;
      } else {
        n = t.left(t.size() - 2).toInt(&ok);
        if (!ok || !t.endsWith("px", Qt::CaseInsensitive))
          break;
      }
      if (sh.offsetX == 0 && sh.offsetY == 0)
        sh.offsetX = n;
      else if (sh.offsetY == 0)
        sh.offsetY = n;
      else if (sh.blurRadius == 0)
        sh.blurRadius = n;
    }
    // Remaining tokens are color
    QString colorStr;
    int ci = 0;
    for (; idx < tokens.size(); idx++, ci++)
      colorStr += (ci > 0 ? " " : "") + tokens[idx];
    sh.color = parseCSSColor(colorStr);
    if (!sh.color.isValid())
      sh.color = QColor(0, 0, 0, 77); // default semi-transparent black
    shadows.append(sh);
  }
  return shadows;
}

void form_ChatWidget::applyThemeCss(const QString &style, ChatBubbleStyle &bs, ChatDelegate::BubbleColors &dc) {
  QString dir = Core.getConfigPath() + "/themes/chat";
  QString cap = style;
  QString path = dir + "/" + cap + ".css";
  if (!QFile::exists(path)) {
    if (!cap.isEmpty())
      cap[0] = cap[0].toUpper();
    path = dir + "/" + cap + ".css";
  }
  QDir().mkpath(dir);
  QFile f(path);
  if (!f.exists()) {
    qWarning() << "applyThemeCss:" << path << "not found, recreating with defaults";
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
      qWarning() << "applyThemeCss: cannot create" << path;
      return;
    }
    QTextStream out(&f);
    out << "/* I2PChat " << style << " */\n";
    out << ".bubble {\n";
    out << "  border-radius: " << bs.radius << "px;\n";
    out << "  padding: " << bs.padV << "px " << bs.padH << "px;\n";
    out << "  box-shadow: 0 1px 2px rgba(0,0,0,0.15);\n";
    out << "}\n";
    out << ".sent { background: " << bs.sentBg << "; color: " << bs.sentColor << "; }\n";
    out << ".received { background: " << bs.receivedBg << "; color: " << bs.receivedColor << "; }\n";
    out << ".system { background: #f5f5f5; color: " << bs.systemColor << "; }\n";
    out << ".pending { background: #fafafa; color: #999; }\n";
    out << ".msg-header { background-color: rgba(0,0,0,0.04); padding: 1px 6px; }\n";
    out << ".msg-time { font-size: smaller; color: #888; }\n";
    out << ".msg-sender { font-weight: bold; }\n";
    f.close();
    // Fall through to read the file we just wrote
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
      return;
  } else if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return;
  }
  QTextStream in(&f);

  auto applyProp = [&](const QString &className, const QString &prop, const QString &val) {
    auto setBg = [&](QString &bsBg, QString &dcBg, BubbleGradient &grad) {
      bsBg = dcBg = val;
      if (val.contains("linear-gradient", Qt::CaseInsensitive))
        grad = parseGradient(val);
      else
        grad = BubbleGradient();
    };
    if (className == "bubble") {
      if (prop == "border-radius" || prop == "radius") {
        QString v = val;
        dc.radius = bs.radius = v.remove("px", Qt::CaseInsensitive).trimmed().toInt();
      } else if (prop == "padding") {
        static QRegularExpression pRx("^(\\d+)\\s*px(?:\\s+(\\d+)\\s*px)?");
        auto pm = pRx.match(val);
        if (pm.hasMatch()) {
          dc.padV = bs.padV = pm.captured(1).toInt();
          dc.padH = bs.padH = pm.captured(2).isEmpty() ? bs.padV : pm.captured(2).toInt();
        }
      } else if (prop == "box-shadow")
        dc.shadows = parseShadows(val);
      else if (prop == "pad-v-inner" || prop == "padding-inner") {
        QString vv = val;
        dc.padVInner = bs.padVInner = vv.remove("px", Qt::CaseInsensitive).trimmed().toInt();
      }
      return;
    }
    if (className == "pending") {
      if (prop == "background" || prop == "background-color")
        dc.pendingBg = val;
      else if (prop == "color")
        dc.pendingFg = val;
      return;
    }
    // Per-type classes — update BOTH bs (mBubbleStyle) and dc (what paint() reads)
    QString *bsBg = nullptr, *bsFg = nullptr;
    QString *dcBg = nullptr, *dcFg = nullptr;
    BubbleGradient *grad = nullptr;
    if (className == "sent") {
      bsBg = &bs.sentBg;
      dcBg = &dc.sentBg;
      bsFg = &bs.sentColor;
      dcFg = &dc.sentFg;
      grad = &dc.sentGradient;
    } else if (className == "received" || className == "rcvd") {
      bsBg = &bs.receivedBg;
      dcBg = &dc.receivedBg;
      bsFg = &bs.receivedColor;
      dcFg = &dc.receivedFg;
      grad = &dc.receivedGradient;
    } else if (className == "system") {
      bsBg = &bs.systemColor;
      dcBg = &dc.systemColor;
      bsFg = &bs.systemColor;
      dcFg = &dc.systemColor;
      grad = &dc.systemGradient;
    }
    if (!bsBg)
      return;
    if (prop == "background" || prop == "background-color")
      setBg(*bsBg, *dcBg, *grad);
    else if (prop == "color")
      *bsFg = *dcFg = val;
  };

  static QRegularExpression classOpenRx("^\\s*\\.([\\w-]+)\\s*\\{");
  static QRegularExpression classCloseRx("^\\s*\\}\\s*$");
  static QRegularExpression classPropRx("^\\s*([\\w-]+)\\s*:\\s*(.+)\\s*;\\s*$");
  // Extract inline properties from remainder of a class-open line
  static QRegularExpression inlinePropRx("([\\w-]+)\\s*:\\s*([^;}]+)");

  enum { StTop, StClass } state = StTop;
  QString curClass;
  QMap<QString, QString> curProps;

  auto flushClass = [&] {
    static const QStringList bubbleClasses = {"bubble", "sent", "received", "rcvd", "system", "pending"};
    if (bubbleClasses.contains(curClass)) {
      for (auto it = curProps.begin(); it != curProps.end(); ++it)
        applyProp(curClass, it.key(), it.value());
    } else {
      QString rule = "." + curClass + " {";
      for (auto it = curProps.begin(); it != curProps.end(); ++it) {
        QString p = it.key();
        QString v = it.value().trimmed();
        // Convert #RGBA / #RRGGBBAA to rgba() — QTextDocument doesn't support alpha hex
        if (v.startsWith('#') && (v.length() == 5 || v.length() == 9)) {
          QColor c = parseCSSColor(v);
          if (c.isValid())
            v =
              QStringLiteral("rgba(%1,%2,%3,%4)").arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alphaF(), 0, 'f', 6);
        }
        // QTextDocument supports background-color but not the background shorthand
        if (p == "background")
          p = "background-color";
        rule += " " + p + ": " + v + ";";
      }
      rule += " }\n";
      dc.extraStylesheet += rule;
    }
    curProps.clear();
    curClass.clear();
    state = StTop;
  };

  while (!in.atEnd()) {
    QString line = in.readLine();

    if (state == StTop) {
      // Try standard variable syntax (backward compat)
      static QRegularExpression varRx("^\\s*(?:--)?([\\w-]+)\\s*:\\s*(.+)\\s*;\\s*$");
      auto m = varRx.match(line);
      if (m.hasMatch()) {
        QString name = m.captured(1);
        QString val = m.captured(2).trimmed();
        if (name == "sent-bg") {
          bs.sentBg = val;
          dc.sentGradient = parseGradient(val);
        } else if (name == "sent-fg")
          bs.sentColor = val;
        else if (name == "received-bg") {
          bs.receivedBg = val;
          dc.receivedGradient = parseGradient(val);
        } else if (name == "received-fg")
          bs.receivedColor = val;
        else if (name == "system-color") {
          bs.systemColor = val;
          dc.systemGradient = parseGradient(val);
        } else if (name == "radius" || name == "border-radius") {
          bs.radius = val.remove("px").trimmed().toInt();
        } else if (name == "pad-v" || name == "padding-top" || name == "padding-bottom")
          bs.padV = val.remove("px").trimmed().toInt();
        else if (name == "pad-h" || name == "padding-left" || name == "padding-right")
          bs.padH = val.remove("px").trimmed().toInt();
        else if (name == "pad-v-inner" || name == "padding-inner")
          bs.padVInner = val.remove("px").trimmed().toInt();
        else if (name == "padding") {
          static QRegularExpression pRx("^(\\d+)\\s*px(?:\\s+(\\d+)\\s*px)?");
          auto pm = pRx.match(val);
          if (pm.hasMatch()) {
            bs.padV = pm.captured(1).toInt();
            bs.padH = pm.captured(2).isEmpty() ? bs.padV : pm.captured(2).toInt();
          }
        } else if (name == "box-shadow")
          dc.shadows = parseShadows(val);
        continue;
      }

      auto cm = classOpenRx.match(line);
      if (cm.hasMatch()) {
        curClass = cm.captured(1);
        curProps.clear();
        // Extract any inline properties on the same line after the {
        QString rest = line.mid(cm.capturedEnd(0));
        auto ip = inlinePropRx.globalMatch(rest);
        while (ip.hasNext()) {
          auto im = ip.next();
          curProps[im.captured(1).trimmed()] = im.captured(2).trimmed();
        }
        bool hasClose = rest.contains('}');
        if (hasClose) {
          flushClass();
        } else {
          state = StClass;
        }
      }
    } else if (state == StClass) {
      auto pm = classPropRx.match(line);
      if (pm.hasMatch())
        curProps[pm.captured(1)] = pm.captured(2).trimmed();
      if (classCloseRx.match(line).hasMatch())
        flushClass();
    }
  }
  f.close();
}

void form_ChatWidget::newMessageReceived() {
  QStringList messages = user.getNewMessages(mHaveFocus);

  if (mChatStyle == "classic") {
    QScrollBar *sb = chat->verticalScrollBar();
    int oldVal = sb->value();
    bool restore = (sb->maximum() - oldVal > 2);
    for (const auto &tmp : messages)
      addMessage(tmp);
    if (restore)
      sb->setValue(oldVal);
    else
      sb->setValue(sb->maximum());
    return;
  }

  QScrollBar *sb = mChatListView->verticalScrollBar();
  int oldVal = sb->value();
  bool restore = (sb->maximum() - oldVal > 2);
  for (const auto &tmp : messages)
    addMessage(tmp);
  if (restore)
    sb->setValue(oldVal);
  else
    mChatListView->scrollToBottom();
}

static int detectMsgType(const QString &text, const QString &selfName) {
  if (text.contains("[System]"))
    return MsgSystem;
  if (text.contains("(pending)"))
    return MsgPending;
  if (text.contains("[Accept]") || text.contains("[Reject]"))
    return MsgFileOffer;
  int arrow = text.indexOf(" ‣ ");
  if (arrow != -1) {
    int after = arrow + 3;
    int colon = text.indexOf(':', after);
    if (colon != -1) {
      if (text.mid(after, colon - after).trimmed() == selfName)
        return MsgSent;
    }
  }
  return MsgReceived;
}

void form_ChatWidget::addAllMessagesClassic() {
  chat->clear();
  QTextCursor cursor(chat->document());
  cursor.movePosition(QTextCursor::End);

  QStringList messages = user.getAllChatMessages();
  if (messages.isEmpty())
    return;

  cursor.beginEditBlock();
  // First message just sets HTML; subsequent ones insert at end
  bool first = true;
  for (const auto &tmp : messages) {
    QString t = tmp;
    CTextEmotionChanger::exemplar()->checkMessageForEmoticons(t);
    if (first) {
      cursor.insertHtml(t);
      first = false;
    } else {
      cursor.insertHtml(t);
    }
  }
  cursor.endEditBlock();
  chat->verticalScrollBar()->setValue(chat->verticalScrollBar()->maximum());
}

void form_ChatWidget::addAllMessages() {
  if (mChatStyle == "classic") {
    addAllMessagesClassic();
    return;
  }
  mChatModel->clear();
  QStringList messages = user.getAllChatMessages();
  for (const auto &tmp : messages)
    addMessage(tmp);
  mChatListView->scrollToBottom();
}

void form_ChatWidget::addMessage(QString text) {
  CTextEmotionChanger::exemplar()->checkMessageForEmoticons(text);

  if (mChatStyle == "classic") {
    QTextCursor cursor(chat->document());
    cursor.movePosition(QTextCursor::End);
    cursor.beginEditBlock();
    cursor.insertHtml(text);
    cursor.endEditBlock();
    chat->verticalScrollBar()->setValue(chat->verticalScrollBar()->maximum());
    return;
  }

  int type = detectMsgType(text, Core.getUserInfos().Nickname);

  // Extract cancel URL from pending/file-offer messages and strip status badges
  QString cancelUrl;
  if (type == MsgPending) {
    QRegularExpression re("<a\\s+href=\"(cancel(?:msg|file):\\d+)\">[^<]*</a>");
    QRegularExpressionMatch m = re.match(text);
    if (m.hasMatch()) {
      cancelUrl = m.captured(1);
      text.remove(m.capturedStart(0), m.capturedLength(0));
    }
    // Strip ugly <i>(pending)</i> / <i>(sent)</i> badges
    text.remove(QRegularExpression("<i>[^<]*</i>"));
  }

  QString typeClass;
  switch (type) {
  case MsgSent:
    typeClass = "sent";
    break;
  case MsgReceived:
    typeClass = "received";
    break;
  case MsgSystem:
    typeClass = "system";
    break;
  case MsgPending:
    typeClass = "pending";
    break;
  case MsgFileOffer:
    typeClass = "fileoffer";
    break;
  default:
    typeClass = "received";
    break;
  }

  if (type == MsgSystem || type == MsgFileOffer) {
    text.remove(QRegularExpression("(?:<br\\s*/?>\\s*)+$", QRegularExpression::CaseInsensitiveOption));
    text = QStringLiteral("<div class=\"msg msg-%1\">%2</div>").arg(typeClass, text);
  } else {
    int arrow = text.indexOf(" ‣ ");
    if (arrow != -1) {
      QString timePart = text.left(arrow);
      QString rest = text.mid(arrow + 3);
      int colon = rest.indexOf(':');
      if (colon != -1) {
        QString sender = rest.left(colon);
        QString body = rest.mid(colon + 1);
        body.remove(QRegularExpression("(?:<br\\s*/?>\\s*)+$", QRegularExpression::CaseInsensitiveOption));
        QString sep = (type == MsgPending) ? QString::fromUtf8(" …") : ":";
        text = QStringLiteral("<div class=\"msg msg-%1\">"
                              "<div class=\"msg-header\">"
                              "<span class=\"msg-time\">%2</span> ‣ "
                              "<span class=\"msg-sender\">%3</span>%4"
                              "</div>"
                              "<div class=\"msg-body\">%5</div></div>")
                 .arg(typeClass, timePart.toHtmlEscaped(), sender.toHtmlEscaped(), sep, body);
      } else {
        text = QStringLiteral("<div class=\"msg msg-%1\">"
                              "<div class=\"msg-header\">"
                              "<span class=\"msg-time\">%2</span>%3"
                              "</div></div>")
                 .arg(typeClass, timePart.toHtmlEscaped(), rest);
      }
    } else {
      text = QStringLiteral("<div class=\"msg msg-%1\">%2</div>").arg(typeClass, text);
    }
  }

  auto *item = new QStandardItem(text);
  item->setEditable(false);
  item->setData(type, MsgTypeRole);
  if (!cancelUrl.isEmpty())
    item->setData(cancelUrl, CancelUrlRole);
  mChatModel->appendRow(item);
  mChatListView->scrollToBottom();
}

void form_ChatWidget::setTextColor() {
  QTextEdit *message = this->message;
  textColor = QColorDialog::getColor(user.getTextColor(), this);

  // textColor = QColorDialog::getColor(message->textColor(), this);
  QPixmap pxm(22, 22);
  pxm.fill(textColor);
  // txtColor->setIcon(pxm);
  user.setTextColor(textColor);

  QPalette pal = message->palette();
  pal.setBrush(QPalette::Text, QBrush(textColor));
  message->setPalette(pal);
}

void form_ChatWidget::setFont() {
  QTextEdit *message = this->message;
  bool ok;
  mCurrentFont = QFontDialog::getFont(&ok, mCurrentFont, this);

  user.setTextFont(mCurrentFont);
  message->setCurrentFont(mCurrentFont);
  message->setFont(mCurrentFont);
  message->setFocus();
}

void form_ChatWidget::setBold(bool t) {

  QTextEdit *message = this->message;

  mCurrentFont.setBold(t);
  user.setTextFont(mCurrentFont);

  message->setCurrentFont(mCurrentFont);
  message->setFont(mCurrentFont);
}

void form_ChatWidget::closeEvent(QCloseEvent *e) {
  disconnect(&user, SIGNAL(signNewMessageReceived()), this, SLOT(newMessageReceived()));

  emit closingChatWindow(user.getI2PDestination());
  e->ignore();
}

static QString minifyChatHtml(const QString &html) {
  QString out = html;
  // Strip DOCTYPE, <html>, <head>...</head>, <body...> and </body>
  out.remove(QRegularExpression("<!DOCTYPE[^>]*>", QRegularExpression::CaseInsensitiveOption));
  out.remove(QRegularExpression("<html[^>]*>", QRegularExpression::CaseInsensitiveOption));
  out.remove("</html>");
  out.remove(QRegularExpression("<head>[^<]*</head>", QRegularExpression::CaseInsensitiveOption));
  out.remove(QRegularExpression("<body[^>]*>", QRegularExpression::CaseInsensitiveOption));
  out.remove("</body>");
  // Strip Qt-internal margin/padding from <p> inline styles (keep font/size/color)
  out.replace(QRegularExpression("margin-[a-z]+:0px;?"), "");
  out.replace(QRegularExpression("-qt-block-indent:0;?"), "");
  out.replace(QRegularExpression("text-indent:0px;?"), "");
  return out.trimmed();
}

void form_ChatWidget::sendMessageSignal() {
  QTextEdit *message = this->message;
  if (message->toPlainText().length() == 0)
    return;

  mControllForChange.setHtml(message->toHtml());
  mControllForChange.selectAll();
  mControllForChange.setCurrentFont(mCurrentFont);
  mControllForChange.setFont(mCurrentFont);
  mControllForChange.setTextColor(user.getTextColor());

  mControllForChange.setFontUnderline(mCurrentFont.underline());
  mControllForChange.setFontItalic(mCurrentFont.italic());

  QString NewMessage = minifyChatHtml(mControllForChange.toHtml());

  if (NewMessage.length() < 65535) {
    user.slotSendChatMessage(NewMessage);
    message->clear();
    newMessageReceived();
  } else {
    QMessageBox *msgBox = new QMessageBox(NULL);
    msgBox->setIcon(QMessageBox::Critical);
    msgBox->setText("Cannot send, message is too long!");
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setDefaultButton(QMessageBox::Ok);
    msgBox->setWindowModality(Qt::NonModal);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->show();
  }
}

void form_ChatWidget::changeWindowsTitle() {
  QString OnlineStatus;
  switch (user.getOnlineState()) {

  case USERTRYTOCONNECT:
  case USERINVISIBLE:
  case USEROFFLINE: {
    OnlineStatus = tr("Offline");
    this->setWindowIcon(QIcon(ICON_USER_OFFLINE));
    break;
  }
  case USERONLINE: {
    OnlineStatus = tr("Online");
    this->setWindowIcon(QIcon(ICON_USER_ONLINE));
    break;
  }
  case USERWANTTOCHAT: {
    OnlineStatus = tr("Want to chat");
    this->setWindowIcon(QIcon(ICON_USER_WANTTOCHAT));
    break;
  }
  case USERAWAY: {
    OnlineStatus = tr("Away");
    this->setWindowIcon(QIcon(ICON_USER_AWAY));
    break;
  }
  case USERDONT_DISTURB: {
    OnlineStatus = tr("Do not disturb");
    this->setWindowIcon(QIcon(ICON_USER_DONT_DISTURB));
    break;
  }
  case USERBLOCKEDYOU: {
    OnlineStatus = tr("You have been blocked");
    this->setWindowIcon(QIcon(ICON_USER_BLOCKED_YOU));
    break;
  }
  default:
    break;
  }
  this->setWindowTitle(user.getName() + " [" + OnlineStatus + "]");
}

void form_ChatWidget::newFileTransfer() {
  if (user.getConnectionStatus() == ONLINE) {
    QString FilePath = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("all Files (*)"));

    if (FilePath.endsWith("/") == true) {
      return;
    }

    if (!FilePath.isEmpty())
      startFileTransfer(FilePath);

  } else {
    // Queue file offer for later delivery
    QString FilePath = QFileDialog::getOpenFileName(
      this, tr("Open File (will be sent when contact comes online)"), ".", tr("all Files (*)"));
    if (FilePath.isEmpty() || FilePath.endsWith("/"))
      return;

    QFileInfo fi(FilePath);
    if (!fi.exists() || !fi.isFile())
      return;

    user.slotSendFileOffer(fi.fileName(), fi.size(), FilePath);
  }
}

void form_ChatWidget::anchorClicked(const QUrl &link) {
  if (link.scheme() == "cancelmsg") {
    bool ok = false;
    qint32 id = link.toString().mid(link.scheme().length() + 1).toInt(&ok);
    if (ok) {
      user.cancelPendingMessage(id);
      // Remove the canceled row from the model directly (slotPendingCanceled
      // no longer fires for individual cancels — it only fires for batch send)
      if (mChatStyle == "classic")
        addAllMessagesClassic();
      else
        for (int i = 0; i < mChatModel->rowCount(); ++i)
          if (mChatModel->item(i)->data(CancelUrlRole).toString() == link.toString())
            mChatModel->removeRow(i);
    }
    return;
  }
  if (link.scheme() == "cancelfile") {
    bool ok = false;
    qint32 id = link.toString().mid(link.scheme().length() + 1).toInt(&ok);
    if (ok) {
      user.cancelPendingFileOffer(id);
      if (mChatStyle == "classic")
        addAllMessagesClassic();
      else
        for (int i = 0; i < mChatModel->rowCount(); ++i)
          if (mChatModel->item(i)->data(CancelUrlRole).toString() == link.toString())
            mChatModel->removeRow(i);
    }
    return;
  }

  if (link.scheme() == "fileoffer") {
    // Format: fileoffer:accept:filename or fileoffer:reject:filename
    QStringList parts = link.toString().split(":");
    if (parts.size() < 3)
      return;

    QString action = parts.at(1);
    QString fileName = parts.mid(2).join(":"); // Restore filename if it contained colons

    if (action == "accept") {
      // Send acceptance over chat protocol
      Core.getProtocol()->send(FILE_OFFER_ACCEPTED, user.getI2PStreamID(), fileName.toUtf8());
      // Show confirmation in chat
      user.slotIncomingMessageFromSystem(
        tr("You accepted the file \"%1\". Waiting for sender to start transfer...").arg(fileName), true);
    } else if (action == "reject") {
      Core.getProtocol()->send(FILE_OFFER_REJECTED, user.getI2PStreamID(), fileName.toUtf8());
      user.slotIncomingMessageFromSystem(tr("You rejected the file \"%1\".").arg(fileName), true);
    }
    return;
  }

  // Open browser, after clicking to link? TODO: add WARNING MESSAGE!!!
  if (link.scheme() == "http" || link.scheme() == "https")
    QDesktopServices::openUrl(link);
  else if (link.scheme() == "") {
    // it's probably a web adress, let's add http:// at the beginning of the
    // link
    QString newAddress = link.toString();
    newAddress.prepend("http://");
    QDesktopServices::openUrl(QUrl(newAddress));
  }
}

void form_ChatWidget::focusEvent(bool b) {
  mHaveFocus = b;

  if (user.getHaveNewUnreadMessages() == true) {
    newMessageReceived();
  }
}

void form_ChatWidget::slotPendingCanceled() {
  if (mChatStyle == "classic") {
    addAllMessages();
    return;
  }
  // Themed mode: update pending items to sent in-place instead of
  // rebuilding the entire model (which would also re-add the system
  // notification about messages being sent).
  for (int i = 0; i < mChatModel->rowCount(); ++i) {
    auto *item = mChatModel->item(i);
    if (item && item->data(MsgTypeRole).toInt() == MsgPending) {
      item->setData(static_cast<int>(MsgSent), MsgTypeRole);
      item->setData(QVariant(), CancelUrlRole);
    }
  }
  mChatListView->viewport()->update();
}

void form_ChatWidget::showContextMenu(const QPoint &pos) {
  QModelIndex idx = mChatListView->indexAt(pos);
  if (!idx.isValid())
    return;
  QString msg = idx.data(Qt::DisplayRole).toString();
  if (msg.isEmpty())
    return;
  QTextDocument doc;
  doc.setHtml(msg);
  QString plain = doc.toPlainText();
  // Strip technical markers: (pending), ✕
  plain.remove(QRegularExpression("\\(pending\\)"));
  plain.remove(QChar(0x2715));
  plain = plain.trimmed();
  QMenu menu;
  QAction *copyAct = menu.addAction(tr("Copy message"));
  if (menu.exec(mChatListView->mapToGlobal(pos)) == copyAct)
    QApplication::clipboard()->setText(plain);
}

void form_ChatWidget::getFocus() {
  this->activateWindow();
  this->setWindowState((windowState() & (~Qt::WindowMinimized)) | Qt::WindowActive);
  this->raise();
  //  this->setFocus(); // don't automatically focus chat window
}

void form_ChatWidget::setUnderline(bool t) {
  mCurrentFont.setUnderline(t);
  user.setTextFont(mCurrentFont);

  message->setCurrentFont(mCurrentFont);
  message->setFont(mCurrentFont);
  message->setFocus();
}

void form_ChatWidget::setItalic(bool t) {
  mCurrentFont.setItalic(t);
  user.setTextFont(mCurrentFont);

  message->setCurrentFont(mCurrentFont);
  message->setFont(mCurrentFont);
  message->setFocus();
}

void form_ChatWidget::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData() && event->mimeData()->hasUrls()) {
    event->acceptProposedAction();
  }
}

void form_ChatWidget::dropEvent(QDropEvent *event) {
  if (!event->mimeData() || !event->mimeData()->hasUrls())
    return;

  const auto urls = event->mimeData()->urls();
  if (urls.isEmpty())
    return;

  QString filePath = urls.first().toLocalFile();
  if (filePath.isEmpty())
    return;

  QFileInfo fi(filePath);
  if (!fi.exists() || !fi.isFile())
    return;

  startFileTransfer(filePath);
}

void form_ChatWidget::startFileTransfer(const QString &filePath) {
  if (user.getConnectionStatus() == ONLINE) {
    Core.getFileTransferManager()->addNewFileTransfer(filePath, user.getI2PDestination());
  } else {
    QFileInfo fi(filePath);
    if (fi.exists() && fi.isFile()) {
      user.slotSendFileOffer(fi.fileName(), fi.size(), filePath);
    }
  }
}

form_ChatWidget::~form_ChatWidget() {}

void form_ChatWidget::keyPressEvent(QKeyEvent *event) {
  if (event->key() != Qt::Key_Escape) {
    QMainWindow::keyPressEvent(event);
  } else {
    event->accept();
    close();
  }
}
void form_ChatWidget::showAvatarFrame(bool show) {
  if (show) {
    avatarframe->setVisible(false);
    avatarFrameButton->setChecked(true);
    avatarFrameButton->setToolTip(tr("Hide Avatar"));
    avatarFrameButton->setIcon(QIcon(tr(":icons/hide_frame.png")));
  } else {
    avatarframe->setVisible(true);
    avatarFrameButton->setChecked(false);
    avatarFrameButton->setToolTip(tr("Show Avatar"));
    avatarFrameButton->setIcon(QIcon(tr(":icons/show_frame.png")));
  }
}

void form_ChatWidget::remoteAvatarImageChanged() {
  if (user.getReceivedUserInfos().AvatarImage.size() > 0) {
    QPixmap pxm;
    pxm.loadFromData(user.getReceivedUserInfos().AvatarImage);
    int w = useravatar_label->width();
    int h = useravatar_label->height();
    if (pxm.width() != w || pxm.height() != h) {
      QImage img = pxm.toImage();
      img = CCore::scaleImageLanczos(img, w, h);
      mUserAvatar = QPixmap::fromImage(img);
    } else {
      mUserAvatar = pxm;
    }
    useravatar_label->setPixmap(mUserAvatar);
  }
}

void form_ChatWidget::messageTextChanged() {
  if (user.getProtocolVersion_D() < 0.5) {
    return;
  }

  QTextCursor tmpCursor = message->textCursor();
  int cursorPos = tmpCursor.position();

  QString messageString = message->toHtml();

  CTextEmotionChanger::exemplar()->checkMessageForEmoticons(messageString);

  disconnect(message, SIGNAL(textChanged()), this, SLOT(messageTextChanged()));
  message->setHtml(messageString);
  connect(message, SIGNAL(textChanged()), this, SLOT(messageTextChanged()));

  tmpCursor.movePosition(QTextCursor::End);
  int maxPos = tmpCursor.position();

  if (cursorPos <= maxPos) {
    tmpCursor.setPosition(cursorPos);
  } else {
    tmpCursor.setPosition(maxPos);
  }
  message->setTextCursor(tmpCursor);
}
void form_ChatWidget::centerDialog() {
  QRect scr = QGuiApplication::primaryScreen()->geometry();
  move(scr.center() - rect().center());
}

void form_ChatWidget::slotLoadOwnAvatarImage() {
  ownavatar_label->setAlignment(Qt::AlignCenter);
  QPixmap pxm;
  pxm.loadFromData(Core.getUserInfos().AvatarImage);
  int w = ownavatar_label->width();
  int h = ownavatar_label->height();
  if (pxm.width() != w || pxm.height() != h) {
    QImage img = pxm.toImage();
    img = CCore::scaleImageLanczos(img, w, h);
    mOwnAvatar = QPixmap::fromImage(img);
  } else {
    mOwnAvatar = pxm;
  }
  ownavatar_label->setPixmap(mOwnAvatar);
}
