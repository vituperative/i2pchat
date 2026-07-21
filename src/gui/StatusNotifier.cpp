// SPDX-License-Identifier: AGPL-3.0-or-later

#include "StatusNotifier.h"

#include <QDBusAbstractAdaptor>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDebug>
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QSvgRenderer>

QDBusArgument &operator<<(QDBusArgument &argument, const KDbusImageStruct &image) {
  argument.beginStructure();
  argument << image.width << image.height << image.data;
  argument.endStructure();
  return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, KDbusImageStruct &image) {
  argument.beginStructure();
  argument >> image.width >> image.height >> image.data;
  argument.endStructure();
  return argument;
}

static KDbusImageVector pixmapToImageVector(const QPixmap &pm) {
  KDbusImageVector vec;
  if (pm.isNull())
    return vec;
  QImage img = pm.toImage().convertToFormat(QImage::Format_ARGB32);
  KDbusImageStruct s;
  s.width = img.width();
  s.height = img.height();
  s.data = QByteArray(reinterpret_cast<const char *>(img.constBits()), static_cast<int>(img.sizeInBytes()));
  vec.append(s);
  return vec;
}

// Adaptor that actually publishes the org.kde.StatusNotifierItem D-Bus
// interface. Plain QObject properties registered with ExportAllProperties are
// not served over D-Bus by Qt in this build, so we use QDBusAbstractAdaptor
// (the supported, reliable path) and forward to the owning CStatusNotifier.
class CStatusNotifierAdaptor : public QDBusAbstractAdaptor {
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.kde.StatusNotifierItem")

  Q_PROPERTY(QString Category READ category CONSTANT)
  Q_PROPERTY(QString Id READ id CONSTANT)
  Q_PROPERTY(QString Title READ title CONSTANT)
  Q_PROPERTY(QString Status READ status WRITE setStatus)
  Q_PROPERTY(QString IconName READ iconName WRITE setIconName)
  Q_PROPERTY(KDbusImageVector IconPixmap READ iconPixmap WRITE setIconPixmap)
  Q_PROPERTY(QString ToolTipTitle READ toolTipTitle WRITE setToolTipTitle)
  Q_PROPERTY(QString ToolTipText READ toolTipText WRITE setToolTipText)
  Q_PROPERTY(KDbusImageVector ToolTipIcon READ toolTipIcon WRITE setToolTipIcon)
  Q_PROPERTY(QString Menu READ menu WRITE setMenu)

public:
  explicit CStatusNotifierAdaptor(CStatusNotifier *parent)
    : QDBusAbstractAdaptor(parent)
    , mOwner(parent) {
    connect(parent, SIGNAL(propertyChanged()), this, SIGNAL(NewIcon()));
    connect(parent, SIGNAL(propertyChanged()), this, SIGNAL(NewToolTip()));
  }

  Q_INVOKABLE QString category() const { return QStringLiteral("Communications"); }
  Q_INVOKABLE QString id() const { return mOwner->id(); }
  Q_INVOKABLE QString title() const { return mOwner->title(); }
  Q_INVOKABLE QString status() const { return mOwner->status(); }
  Q_INVOKABLE void setStatus(const QString &s) { mOwner->setStatus(s); }
  Q_INVOKABLE QString iconName() const { return mOwner->iconName(); }
  Q_INVOKABLE void setIconName(const QString &n) { mOwner->setIconName(n); }
  Q_INVOKABLE KDbusImageVector iconPixmap() const { return mOwner->iconPixmap(); }
  Q_INVOKABLE void setIconPixmap(const KDbusImageVector &p) { mOwner->setIconPixmap(p); }
  Q_INVOKABLE QString toolTipTitle() const { return mOwner->toolTipTitle(); }
  Q_INVOKABLE void setToolTipTitle(const QString &t) { mOwner->setToolTipTitle(t); }
  Q_INVOKABLE QString toolTipText() const { return mOwner->toolTipText(); }
  Q_INVOKABLE void setToolTipText(const QString &t) { mOwner->setToolTipText(t); }
  Q_INVOKABLE KDbusImageVector toolTipIcon() const { return mOwner->toolTipIcon(); }
  Q_INVOKABLE void setToolTipIcon(const KDbusImageVector &p) { mOwner->setToolTipIcon(p); }
  Q_INVOKABLE QString menu() const { return mOwner->menu(); }
  Q_INVOKABLE void setMenu(const QString &m) { mOwner->setMenu(m); }

public slots:
  void Activate(int x, int y) { emit mOwner->activate(x, y); }
  void SecondaryActivate(int x, int y) { emit mOwner->secondaryActivate(x, y); }
  void ContextMenu(int x, int y) { emit mOwner->contextMenu(x, y); }
  void Scroll(int delta, const QString &orientation) { emit mOwner->scroll(delta, orientation); }

signals:
  void NewTitle();
  void NewIcon();
  void NewAttentionIcon();
  void NewToolTip();
  void NewStatus();

private:
  CStatusNotifier *mOwner;
};

CStatusNotifier::CStatusNotifier(QObject *parent)
  : QObject(parent) {
  qDBusRegisterMetaType<KDbusImageStruct>();
  qDBusRegisterMetaType<KDbusImageVector>();
  mId = QStringLiteral("i2pchat");
  mTitle = QStringLiteral("I2PChat");
  mStatus = QStringLiteral("Active");
  // Non-empty IconName is required: sntray ignores IconPixmap unless a name
  // is set. "i2pchat" resolves to the avatar installed in the local icon
  // themes, so the host renders the avatar consistently.
  mIconName = QStringLiteral("i2pchat");
  new CStatusNotifierAdaptor(this);
}

CStatusNotifier::~CStatusNotifier() {
  QDBusConnection bus = QDBusConnection::sessionBus();
  if (bus.isConnected()) {
    QDBusInterface watcher(QStringLiteral("org.kde.StatusNotifierWatcher"),
                           QStringLiteral("/StatusNotifierWatcher"),
                           QStringLiteral("org.kde.StatusNotifierWatcher"),
                           bus);
    if (watcher.isValid())
      watcher.call(QDBus::NoBlock, QStringLiteral("UnregisterStatusNotifierItem"), bus.baseService());
  }
}

bool CStatusNotifier::registerNotifier() {
  QDBusConnection bus = QDBusConnection::sessionBus();
  if (bus.isConnected() == false)
    return false;

  QString path = QStringLiteral("/StatusNotifierItem");
  if (bus.registerObject(path, this, QDBusConnection::ExportAdaptors) == false)
    return false;

  QDBusInterface watcher(QStringLiteral("org.kde.StatusNotifierWatcher"),
                         QStringLiteral("/StatusNotifierWatcher"),
                         QStringLiteral("org.kde.StatusNotifierWatcher"),
                         bus);
  if (watcher.isValid() == false)
    return false;

  QDBusMessage reg = watcher.call(QDBus::BlockWithGui, QStringLiteral("RegisterStatusNotifierItem"), bus.baseService());
  return true;
}

void CStatusNotifier::setIcon(const QPixmap &pixmap) {
  mIconPixmap = pixmap;
  mToolTipIcon = pixmap;
  // Keep mIconName as "i2pchat" (a resolvable theme icon installed locally);
  // sntray ignores IconPixmap when IconName is empty, so we must not clear it.
  emit propertyChanged();
}

void CStatusNotifier::setIconFromSvg(const QString &svgPath, int size) {
  // QSvgRenderer's QString ctor treats the argument as a FILE path, not a Qt
  // resource URL, so a ":/..." resource would silently fail to load. Read the
  // resource into a buffer and render from that instead.
  QByteArray svgData;
  {
    QFile f(svgPath);
    if (f.open(QIODevice::ReadOnly))
      svgData = f.readAll();
  }
  QPixmap big(48, 48);
  big.fill(Qt::transparent);
  {
    QPainter p(&big);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    QSvgRenderer(svgData).render(&p, QRectF(0, 0, 48, 48));
  }
  QImage img = big.toImage().convertToFormat(QImage::Format_ARGB32);
  if (img.isNull() == false)
    img = img.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  mIconPixmap = QPixmap::fromImage(img);
  mToolTipIcon = mIconPixmap;
  // sntray refuses to render IconPixmap unless IconName is non-empty, so set a
  // fixed dummy name. It points at no theme icon (no installs), so the host
  // falls back to our internal resource pixmap above — fully self-contained.
  mIconName = QStringLiteral("i2pchat");
  emit propertyChanged();
}

void CStatusNotifier::setToolTip(const QString &title, const QString &text) {
  mToolTipTitle = title;
  mToolTipText = text;
  emit propertyChanged();
}

KDbusImageVector CStatusNotifier::iconPixmap() const {
  return pixmapToImageVector(mIconPixmap);
}

KDbusImageVector CStatusNotifier::toolTipIcon() const {
  return pixmapToImageVector(mToolTipIcon);
}

#include "StatusNotifier.moc"
