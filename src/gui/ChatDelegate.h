// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef CHATDELEGATE_H
#define CHATDELEGATE_H

#include <QColor>
#include <QStyledItemDelegate>
#include <QTextDocument>
#include <QVector>

struct BubbleGradient {
  double angle; // CSS degrees (0 = to top)
  struct Stop {
    double pos; // 0.0-1.0, -1 = auto-distribute
    QColor color;
  };
  QVector<Stop> stops;
  bool isValid() const { return stops.size() >= 2; }
};

struct BubbleShadow {
  int offsetX, offsetY;
  int blurRadius;
  QColor color;
  bool inset = false;
};

class ChatDelegate : public QStyledItemDelegate {
  Q_OBJECT
public:
  struct BubbleColors {
    QString sentBg, sentFg;
    QString receivedBg, receivedFg;
    QString systemColor;
    QString pendingBg, pendingFg;
    QString extraStylesheet;
    int radius;
    int padV, padH;
    int padVInner;
    BubbleGradient sentGradient;
    BubbleGradient receivedGradient;
    BubbleGradient systemGradient;
    QVector<BubbleShadow> shadows;
  };

  explicit ChatDelegate(QObject *parent = nullptr);

  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

  void setBubbleColors(const BubbleColors &c);
  const BubbleColors &bubbleColors() const { return mColors; }

signals:
  void linkClicked(const QUrl &href);

protected:
  bool editorEvent(QEvent *event,
                   QAbstractItemModel *model,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) override;

private:
  BubbleColors mColors;
  mutable QTextDocument mDoc;
};

enum MsgType {
  MsgSent = 0,
  MsgReceived,
  MsgSystem,
  MsgPending,
  MsgFileOffer,
  MsgFileTransfer,
  MsgSentFileOffer,
};

static const int MsgTypeRole = Qt::UserRole + 1;
static const int CancelUrlRole = Qt::UserRole + 5;

#endif
