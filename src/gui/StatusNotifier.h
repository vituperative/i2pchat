// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef STATUSNOTIFIER_H
#define STATUSNOTIFIER_H

#include <QDBusArgument>
#include <QObject>
#include <QPixmap>
#include <QVector>

// Icon data as exchanged over D-Bus (org.kde.StatusNotifierItem):
// a vector of (width, height, ARGB32 byte array) structs.
struct KDbusImageStruct {
  int width = 0;
  int height = 0;
  QByteArray data;
};
Q_DECLARE_METATYPE(KDbusImageStruct)

struct KDbusImageVector : public QVector<KDbusImageStruct> {};
Q_DECLARE_METATYPE(KDbusImageVector)

QDBusArgument &operator<<(QDBusArgument &argument, const KDbusImageStruct &image);
const QDBusArgument &operator>>(const QDBusArgument &argument, KDbusImageStruct &image);

// Self-contained StatusNotifierItem (KDE StatusNotifier protocol) so the
// tray icon shows on modern desktops (XFCE4 4.14+, KDE, etc.) whose panels
// only speak the StatusNotifier D-Bus protocol and no longer host the legacy
// XEmbed system tray that Qt5's QSystemTrayIcon relies on.
class CStatusNotifier : public QObject {
  Q_OBJECT

public:
  explicit CStatusNotifier(QObject *parent = nullptr);
  ~CStatusNotifier();

  // Register on the session bus and announce to the StatusNotifierWatcher.
  // Returns false if D-Bus registration failed (e.g. no session bus).
  bool registerNotifier();

  void setIcon(const QPixmap &pixmap);
  void setIconFromSvg(const QString &svgPath, int size = 22);
  void setToolTip(const QString &title, const QString &text);

  QString id() const { return mId; }
  QString title() const { return mTitle; }
  QString status() const { return mStatus; }
  QString iconName() const { return mIconName; }
  KDbusImageVector iconPixmap() const;
  QString toolTipTitle() const { return mToolTipTitle; }
  QString toolTipText() const { return mToolTipText; }
  KDbusImageVector toolTipIcon() const;
  QString menu() const { return mMenu; }

  void setStatus(const QString &s) {
    mStatus = s;
    emit propertyChanged();
  }
  void setIconName(const QString &n) {
    mIconName = n;
    emit propertyChanged();
  }
  void setIconPixmap(const KDbusImageVector &) { emit propertyChanged(); }
  void setToolTipTitle(const QString &t) {
    mToolTipTitle = t;
    emit propertyChanged();
  }
  void setToolTipText(const QString &t) {
    mToolTipText = t;
    emit propertyChanged();
  }
  void setToolTipIcon(const KDbusImageVector &) { emit propertyChanged(); }
  void setMenu(const QString &m) {
    mMenu = m;
    emit propertyChanged();
  }

signals:
  void activate(int x, int y);
  void secondaryActivate(int x, int y);
  void contextMenu(int x, int y);
  void scroll(int delta, const QString &orientation);
  void propertyChanged();

private:
  QString mId;
  QString mTitle;
  QString mStatus;
  QString mIconName;
  QPixmap mIconPixmap;
  QString mToolTipTitle;
  QString mToolTipText;
  QPixmap mToolTipIcon;
  QString mMenu;
};

#endif
