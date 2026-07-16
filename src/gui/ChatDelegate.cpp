// SPDX-License-Identifier: AGPL-3.0-or-later

#include "ChatDelegate.h"

#include <QAbstractItemView>
#include <QAbstractTextDocumentLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QTextCursor>
#include <QTextDocument>
#include <QUrl>

#include <cmath>

static constexpr double kPi = 3.14159265358979323846;

ChatDelegate::ChatDelegate(QObject *parent)
  : QStyledItemDelegate(parent) {
  BubbleColors c;
  c.sentBg = "#075e54";
  c.sentFg = "#ffffff";
  c.receivedBg = "#e5e5ea";
  c.receivedFg = "#1c1c1c";
  c.systemColor = "#888888";
  c.radius = 8;
  c.padV = 6;
  c.padH = 12;
  c.pendingBg = "#fff3cd";
  c.pendingFg = "#856404";
  mColors = c;
}

static void stripBlockMargins(QTextDocument *doc) {
  doc->setDocumentMargin(0);
  QTextCursor c(doc);
  c.movePosition(QTextCursor::Start);
  c.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
  QTextBlockFormat bf;
  bf.setTopMargin(0);
  bf.setBottomMargin(0);
  c.setBlockFormat(bf);
}

void ChatDelegate::setBubbleColors(const BubbleColors &c) {
  mColors = c;
}

static void drawShadow(QPainter *p, const QPainterPath &path, const BubbleShadow &shadow) {
  p->save();
  p->translate(shadow.offsetX, shadow.offsetY);

  if (shadow.blurRadius <= 0) {
    p->fillPath(path, shadow.color);
    p->restore();
    return;
  }

  QRectF bounds = path.boundingRect();
  int bx = (int)std::floor(bounds.x()) - 1;
  int by = (int)std::floor(bounds.y()) - 1;
  int bw = (int)std::ceil(bounds.width()) + 4;
  int bh = (int)std::ceil(bounds.height()) + 4;
  if (bw <= 0 || bh <= 0) {
    p->restore();
    return;
  }

  // Render at target res
  QImage render(bw, bh, QImage::Format_ARGB32_Premultiplied);
  render.fill(Qt::transparent);
  QPainter rp(&render);
  rp.setRenderHint(QPainter::Antialiasing);
  rp.translate(-bx, -by);
  rp.setPen(Qt::NoPen);
  rp.setBrush(shadow.color);
  rp.drawPath(path);
  rp.end();

  // Downsample → box blur, then upsample → smooth interpolation
  int factor = qMax(shadow.blurRadius, 2);
  int sw = qMax(bw / factor, 1), sh = qMax(bh / factor, 1);
  QImage blurred;
  if (sw < bw || sh < bh) {
    QImage small = render.scaled(sw, sh, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    blurred = small.scaled(bw, bh, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  } else {
    blurred = render;
  }

  p->drawImage(bx, by, blurred);
  p->restore();
}

void ChatDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  if (!index.isValid())
    return;

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);

  QString text = index.data(Qt::DisplayRole).toString();
  int type = index.data(MsgTypeRole).toInt();

  QRect r = option.rect;
  int margin = qMax(r.width() / 5, 80);

  QRect bubbleRect;
  QColor bg, fg;
  bool drawBubble;
  const BubbleGradient *grad = nullptr;

  if (type == MsgSystem) {
    drawBubble = (mColors.radius > 0 || mColors.systemGradient.isValid());
    bg = QColor("#e8e8e8");
    fg = QColor(mColors.systemColor);
    if (!fg.isValid())
      fg = QColor("#888888");
    int bw = qMin(r.width() * 3 / 4, 320);
    int bx = r.x() + (r.width() - bw) / 2;
    bubbleRect = QRect(bx, r.y() + 4, bw, r.height() - 8);
    if (mColors.systemGradient.isValid())
      grad = &mColors.systemGradient;
  } else if (type == MsgSent || type == MsgPending) {
    drawBubble = true;
    if (type == MsgPending) {
      bg = QColor(mColors.pendingBg);
      fg = QColor(mColors.pendingFg);
    } else {
      bg = QColor(mColors.sentBg);
      fg = QColor(mColors.sentFg);
      if (mColors.sentGradient.isValid())
        grad = &mColors.sentGradient;
    }
    bubbleRect = QRect(r.x() + margin, r.y() + mColors.padV, r.width() - margin - 4, r.height() - mColors.padV * 2);
  } else {
    drawBubble = true;
    bg = QColor(mColors.receivedBg);
    fg = QColor(mColors.receivedFg);
    bubbleRect = QRect(r.x() + 4, r.y() + mColors.padV, r.width() - margin - 4, r.height() - mColors.padV * 2);
    if (mColors.receivedGradient.isValid())
      grad = &mColors.receivedGradient;
  }

  // Selection highlight
  if (option.state & QStyle::State_Selected)
    painter->fillRect(r, option.palette.highlight());

  // Bubble path (shared by shadows and background)
  QPainterPath bubblePath;
  bubblePath.addRoundedRect(QRectF(bubbleRect).adjusted(0.5, 0.5, -0.5, -0.5), mColors.radius, mColors.radius);

  // Draw shadows (behind bubble)
  for (const auto &shadow : mColors.shadows)
    drawShadow(painter, bubblePath, shadow);

  // Bubble background with rounded rect
  if (drawBubble && (grad || bg.alpha() > 0)) {
    if (grad) {
      double rad = grad->angle * kPi / 180.0;
      double cx = bubbleRect.center().x(), cy = bubbleRect.center().y();
      double dx = std::sin(rad) * (bubbleRect.width() / 2.0);
      double dy = -std::cos(rad) * (bubbleRect.height() / 2.0);
      QLinearGradient lg(QPointF(cx - dx, cy - dy), QPointF(cx + dx, cy + dy));
      for (const auto &stop : grad->stops)
        lg.setColorAt(stop.pos, stop.color);
      painter->fillPath(bubblePath, lg);
    } else {
      painter->fillPath(bubblePath, bg);
    }
  }

  // Strip HTML color attributes so our foreground color takes effect
  QString renderText = text;
  renderText.replace(
    QRegularExpression("<font\\s+color\\s*=\\s*\"[^\"]*\"\\s*>", QRegularExpression::CaseInsensitiveOption), "<font>");
  renderText.replace(
    QRegularExpression("\\bcolor\\s*:\\s*#[0-9a-fA-F]+;?\\s*", QRegularExpression::CaseInsensitiveOption), "");
  renderText = QStringLiteral("<div style=\"color:%1;\">%2</div>").arg(fg.name(), renderText);

  auto *doc = new QTextDocument;
  doc->setDefaultFont(option.font);
  {
    QString ss = "a { color: #0000ff; }";
    if (!mColors.extraStylesheet.isEmpty())
      ss += "\n" + mColors.extraStylesheet;
    doc->setDefaultStyleSheet(ss);
  }
  doc->setHtml(renderText);
  stripBlockMargins(doc);

  if (drawBubble) {
    QRect textRect = bubbleRect.adjusted(mColors.padH, 1, -mColors.padH, -1);
    doc->setTextWidth(textRect.width());
    painter->translate(textRect.topLeft());
    doc->drawContents(painter, QRectF(0, 0, textRect.width(), textRect.height()));
  } else {
    int availW = r.width() - 8;
    doc->setTextWidth(availW);
    QSizeF ds = doc->documentLayout()->documentSize();
    int xOff = qMax(0, (r.width() - (int)ds.width()) / 2);
    painter->translate(r.x() + xOff, r.y() + 2);
    doc->drawContents(painter);
  }

  delete doc;
  painter->restore();
}

QSize ChatDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
  if (!index.isValid())
    return QSize();

  QString text = index.data(Qt::DisplayRole).toString();
  int type = index.data(MsgTypeRole).toInt();

  int w = option.rect.width();
  if (option.widget) {
    int vw = option.widget->width();
    if (vw > 50)
      w = vw;
  }
  if (w < 50)
    w = 400;

  int margin = qMax(w / 5, 80);
  int tw;
  if (type == MsgSystem) {
    if (mColors.radius > 0)
      tw = qMin(w * 3 / 4, 320) - 8;
    else
      tw = w - 8;
  } else {
    tw = w - margin - 4 - mColors.padH * 2;
  }
  if (tw < 50)
    tw = 50;

  // Strip HTML tags and measure plain text with font metrics
  QString plain = text;
  plain.replace(QRegularExpression("<br\\s*/?>", QRegularExpression::CaseInsensitiveOption), "\n");
  plain.remove(QRegularExpression("<[^>]*>"));
  plain = plain.trimmed();
  if (plain.isEmpty())
    plain = " ";

  QFontMetrics fm(option.font);
  QRect br = fm.boundingRect(QRect(0, 0, tw, 0), Qt::AlignLeft | Qt::TextWordWrap, plain);
  int textH = br.height() + 2; // small allowance for descenders

  int h = textH + mColors.padV * 2 + 2;
  if (type == MsgSystem && mColors.radius > 0)
    h = qMin(h, textH + 12);
  return QSize(w, h);
}

bool ChatDelegate::editorEvent(QEvent *event,
                               QAbstractItemModel *model,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) {
  if (event->type() != QEvent::MouseButtonRelease)
    return QStyledItemDelegate::editorEvent(event, model, option, index);

  auto *me = static_cast<QMouseEvent *>(event);
  QString text = index.data(Qt::DisplayRole).toString();
  int type = index.data(MsgTypeRole).toInt();

  auto *doc = new QTextDocument;
  doc->setDefaultFont(option.font);
  doc->setHtml(text);

  int margin = qMax(option.rect.width() / 5, 80);
  QPointF docOrigin;

  if (type == MsgSystem) {
    if (mColors.radius > 0) {
      int bw = qMin(option.rect.width() * 3 / 4, 320);
      int bx = option.rect.x() + (option.rect.width() - bw) / 2;
      QRect bubbleRect(bx, option.rect.y() + 4, bw, option.rect.height() - 8);
      docOrigin = bubbleRect.topLeft() + QPoint(4, 2);
      doc->setTextWidth(bubbleRect.width() - 8);
    } else {
      int availW = option.rect.width() - 8;
      doc->setTextWidth(availW);
      QSizeF ds = doc->documentLayout()->documentSize();
      int xOff = qMax(0, (option.rect.width() - (int)ds.width()) / 2);
      docOrigin = QPointF(option.rect.x() + xOff, option.rect.y() + 2);
    }
  } else if (type == MsgSent || type == MsgPending) {
    QRect bubbleRect(option.rect.x() + margin,
                     option.rect.y() + mColors.padV,
                     option.rect.width() - margin - 4,
                     option.rect.height() - mColors.padV * 2);
    docOrigin = bubbleRect.topLeft() + QPoint(mColors.padH, 2);
    doc->setTextWidth(bubbleRect.width() - mColors.padH * 2);
  } else {
    QRect bubbleRect(option.rect.x() + 4,
                     option.rect.y() + mColors.padV,
                     option.rect.width() - margin - 4,
                     option.rect.height() - mColors.padV * 2);
    docOrigin = bubbleRect.topLeft() + QPoint(mColors.padH, 2);
    doc->setTextWidth(bubbleRect.width() - mColors.padH * 2);
  }

  QPointF clickInDoc = QPointF(me->pos()) - docOrigin;
  QString anchor = doc->documentLayout()->anchorAt(clickInDoc);
  delete doc;

  if (!anchor.isEmpty()) {
    emit linkClicked(QUrl(anchor));
    return true;
  }

  return QStyledItemDelegate::editorEvent(event, model, option, index);
}
