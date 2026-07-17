// SPDX-License-Identifier: AGPL-3.0-or-later

#include "ChatDelegate.h"

#include <QAbstractItemView>
#include <QAbstractTextDocumentLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QSvgRenderer>
#include <QTextCursor>
#include <QTextDocument>
#include <QUrl>

#include <cmath>

static constexpr double kPi = 3.14159265358979323846;
static constexpr int kCancelIconSize = 10;
static constexpr int kCancelIconMargin = 4;

static QPixmap cancelIconPixmap() {
  static QPixmap pix;
  if (pix.isNull()) {
    QSvgRenderer r(QString(":/icons/cancel.svg"));
    pix = QPixmap(kCancelIconSize, kCancelIconSize);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    r.render(&p);
    p.end();
  }
  return pix;
}

static QRect cancelIconRect(const QRect &bubbleRect) {
  return QRect(bubbleRect.right() - kCancelIconSize - kCancelIconMargin,
               bubbleRect.top() + kCancelIconMargin,
               kCancelIconSize,
               kCancelIconSize);
}

ChatDelegate::ChatDelegate(QObject *parent)
  : QStyledItemDelegate(parent) {
  BubbleColors c;
  c.sentBg = "#075e54";
  c.sentFg = "#ffffff";
  c.receivedBg = "#e5e5ea";
  c.receivedFg = "#1c1c1c";
  c.systemColor = "#888888";
  c.radius = 3;
  c.padV = 4;
  c.padH = 8;
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

// Sliding-window box blur preserving shape (image dimensions unchanged)
static void boxBlurImage(QImage &img, int radius) {
  if (radius <= 0 || img.isNull())
    return;
  int w = img.width(), h = img.height();
  if (w < 1 || h < 1)
    return;

  QImage tmp(w, h, QImage::Format_ARGB32_Premultiplied);
  tmp.fill(Qt::transparent);

  // Horizontal pass
  for (int y = 0; y < h; y++) {
    const QRgb *src = (const QRgb *)img.constScanLine(y);
    unsigned int *dst = (unsigned int *)tmp.scanLine(y);
    int sumR = 0, sumG = 0, sumB = 0, sumA = 0;
    // Initial window
    for (int x = -radius; x <= radius; x++) {
      int xi = qBound(0, x, w - 1);
      QRgb px = src[xi];
      sumR += qRed(px);
      sumG += qGreen(px);
      sumB += qBlue(px);
      sumA += qAlpha(px);
    }
    for (int x = 0; x < w; x++) {
      int outX = x - radius - 1;
      int inX = x + radius;
      if (outX >= 0) {
        QRgb px = src[outX];
        sumR -= qRed(px);
        sumG -= qGreen(px);
        sumB -= qBlue(px);
        sumA -= qAlpha(px);
      }
      if (inX < w) {
        QRgb px = src[inX];
        sumR += qRed(px);
        sumG += qGreen(px);
        sumB += qBlue(px);
        sumA += qAlpha(px);
      }
      int count = qMin(x + radius, w - 1) - qMax(x - radius, 0) + 1;
      dst[x] = qRgba(sumR / count, sumG / count, sumB / count, sumA / count);
    }
  }

  // Vertical pass (read from tmp, write back to img)
  for (int x = 0; x < w; x++) {
    int sumR = 0, sumG = 0, sumB = 0, sumA = 0;
    for (int y = -radius; y <= radius; y++) {
      int yi = qBound(0, y, h - 1);
      QRgb px = ((const unsigned int *)tmp.constScanLine(yi))[x];
      sumR += qRed(px);
      sumG += qGreen(px);
      sumB += qBlue(px);
      sumA += qAlpha(px);
    }
    for (int y = 0; y < h; y++) {
      int outY = y - radius - 1;
      int inY = y + radius;
      if (outY >= 0) {
        QRgb px = ((const unsigned int *)tmp.constScanLine(outY))[x];
        sumR -= qRed(px);
        sumG -= qGreen(px);
        sumB -= qBlue(px);
        sumA -= qAlpha(px);
      }
      if (inY < h) {
        QRgb px = ((const unsigned int *)tmp.constScanLine(inY))[x];
        sumR += qRed(px);
        sumG += qGreen(px);
        sumB += qBlue(px);
        sumA += qAlpha(px);
      }
      int count = qMin(y + radius, h - 1) - qMax(y - radius, 0) + 1;
      ((unsigned int *)img.scanLine(y))[x] = qRgba(sumR / count, sumG / count, sumB / count, sumA / count);
    }
  }
}

static void drawShadow(QPainter *p, const QPainterPath &path, const BubbleShadow &shadow) {
  p->save();

  QRectF bounds = path.boundingRect();
  int bx = (int)std::floor(bounds.x()) - 1;
  int by = (int)std::floor(bounds.y()) - 1;
  int bw = (int)std::ceil(bounds.width()) + 4;
  int bh = (int)std::ceil(bounds.height()) + 4;
  if (bw <= 0 || bh <= 0) {
    p->restore();
    return;
  }

  if (shadow.inset) {
    // Inset shadow: clip to the bubble path, draw shadow inside
    p->setClipPath(path);
    p->translate(shadow.offsetX, shadow.offsetY);

    if (shadow.blurRadius <= 0) {
      // Unblurred inset: just fill the whole area with shadow color
      // so it only shows inside the clip (at the offset position)
      QRectF r = bounds.translated(-shadow.offsetX, -shadow.offsetY);
      p->fillRect(r, shadow.color);
    } else {
      // Blurred inset: render at target res, blur, draw
      QImage render(bw, bh, QImage::Format_ARGB32);
      render.fill(Qt::transparent);
      {
        QPainter rp(&render);
        rp.setRenderHint(QPainter::Antialiasing);
        rp.translate(-bx, -by);
        rp.setPen(Qt::NoPen);
        rp.setBrush(shadow.color);
        rp.drawRect(bounds);
        rp.end();
      }
      boxBlurImage(render, shadow.blurRadius);
      p->drawImage(bx, by, render);
    }
    p->restore();
    return;
  }

  // Outer shadow
  p->translate(shadow.offsetX, shadow.offsetY);

  if (shadow.blurRadius <= 0) {
    p->fillPath(path, shadow.color);
    p->restore();
    return;
  }

  QImage render(bw, bh, QImage::Format_ARGB32);
  render.fill(Qt::transparent);
  {
    QPainter rp(&render);
    rp.setRenderHint(QPainter::Antialiasing);
    rp.translate(-bx, -by);
    rp.setPen(Qt::NoPen);
    rp.setBrush(shadow.color);
    rp.drawPath(path);
    rp.end();
  }

  boxBlurImage(render, shadow.blurRadius);
  p->drawImage(bx, by, render);
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

  // Strip HTML color attributes; convert <p> to <div> so the
  // div { margin:0; padding:0; } CSS guarantees zero block spacing
  QString renderText = text;
  renderText.replace(
    QRegularExpression("<font\\s+color\\s*=\\s*\"[^\"]*\"\\s*>", QRegularExpression::CaseInsensitiveOption), "<font>");
  renderText.replace(
    QRegularExpression("\\bcolor\\s*:\\s*#[0-9a-fA-F]+;?\\s*", QRegularExpression::CaseInsensitiveOption), "");
  renderText.replace(QRegularExpression("<p[^>]*>", QRegularExpression::CaseInsensitiveOption), "<div>");
  renderText.replace("</p>", "</div>");
  renderText = QStringLiteral("<div style=\"color:%1;\">%2</div>").arg(fg.name(), renderText);

  auto *doc = new QTextDocument;
  doc->setDefaultFont(option.font);
  doc->setDocumentMargin(0);
  {
    QString ss = "a { color: #0000ff; }"
                 "p, div, h1, h2, h3, h4, h5, h6, blockquote, pre, ul, ol, li, dl, dd {"
                 " margin: 0; padding: 0;"
                 "}";
    if (!mColors.extraStylesheet.isEmpty())
      ss += "\n" + mColors.extraStylesheet;
    doc->setDefaultStyleSheet(ss);
  }
  doc->setHtml(renderText);
  stripBlockMargins(doc);

  QString cancelUrl;
  bool hasCancel = (type == MsgPending && !index.data(CancelUrlRole).toString().isEmpty());
  if (hasCancel)
    cancelUrl = index.data(CancelUrlRole).toString();

  QRectF iconRect;
  if (hasCancel)
    iconRect = cancelIconRect(bubbleRect);

  if (drawBubble) {
    int rightPad = mColors.padH;
    if (hasCancel)
      rightPad += kCancelIconSize + kCancelIconMargin;
    QRect textRect = bubbleRect.adjusted(mColors.padH, 0, -rightPad, 0);
    doc->setTextWidth(textRect.width());
    painter->translate(textRect.topLeft());
    doc->drawContents(painter);
    painter->translate(-textRect.topLeft());
  } else {
    int availW = r.width() - 8;
    doc->setTextWidth(availW);
    QSizeF ds = doc->documentLayout()->documentSize();
    int xOff = qMax(0, (r.width() - (int)ds.width()) / 2);
    painter->translate(r.x() + xOff, r.y() + 2);
    doc->drawContents(painter);
    painter->translate(-(r.x() + xOff), -(r.y() + 2));
  }

  if (hasCancel) {
    QPixmap pix = cancelIconPixmap();
    if (!pix.isNull())
      painter->drawPixmap(iconRect.topLeft(), pix);
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
  if (type == MsgPending && !index.data(CancelUrlRole).toString().isEmpty())
    tw -= kCancelIconSize + kCancelIconMargin;
  if (tw < 50)
    tw = 50;

  // Measure height with QTextDocument as paint() does so heights match exactly
  QString renderText = text;
  renderText.replace(
    QRegularExpression("<font\\s+color\\s*=\\s*\"[^\"]*\"\\s*>", QRegularExpression::CaseInsensitiveOption), "<font>");
  renderText.replace(
    QRegularExpression("\\bcolor\\s*:\\s*#[0-9a-fA-F]+;?\\s*", QRegularExpression::CaseInsensitiveOption), "");
  renderText.replace(QRegularExpression("<p[^>]*>", QRegularExpression::CaseInsensitiveOption), "<div>");
  renderText.replace("</p>", "</div>");
  renderText = QStringLiteral("<div style=\"color:#000;\">%1</div>").arg(renderText);

  auto *doc = new QTextDocument;
  doc->setDefaultFont(option.font);
  doc->setDocumentMargin(0);
  {
    QString ss = "a { color: #0000ff; }"
                 "p, div, h1, h2, h3, h4, h5, h6, blockquote, pre, ul, ol, li, dl, dd {"
                 " margin: 0; padding: 0;"
                 "}";
    if (!mColors.extraStylesheet.isEmpty())
      ss += "\n" + mColors.extraStylesheet;
    doc->setDefaultStyleSheet(ss);
  }
  doc->setHtml(renderText);
  stripBlockMargins(doc);
  doc->setTextWidth(tw);
  QSizeF ds = doc->documentLayout()->documentSize();
  int textH = qMax((int)(ds.height() + 0.5), 1);
  delete doc;

  int h = textH + mColors.padV * 2;
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
  {
    QString ss = "a { color: #0000ff; }"
                 "p, div, h1, h2, h3, h4, h5, h6, blockquote, pre, ul, ol, li, dl, dd {"
                 " margin: 0; padding: 0;"
                 "}";
    if (!mColors.extraStylesheet.isEmpty())
      ss += "\n" + mColors.extraStylesheet;
    doc->setDefaultStyleSheet(ss);
  }
  doc->setHtml(text);
  stripBlockMargins(doc);

  int margin = qMax(option.rect.width() / 5, 80);
  QRect bubbleRect;
  QPointF docOrigin;

  if (type == MsgSystem) {
    if (mColors.radius > 0) {
      int bw = qMin(option.rect.width() * 3 / 4, 320);
      int bx = option.rect.x() + (option.rect.width() - bw) / 2;
      bubbleRect = QRect(bx, option.rect.y() + 4, bw, option.rect.height() - 8);
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
    bubbleRect = QRect(option.rect.x() + margin,
                       option.rect.y() + mColors.padV,
                       option.rect.width() - margin - 4,
                       option.rect.height() - mColors.padV * 2);
    docOrigin = bubbleRect.topLeft() + QPoint(mColors.padH, 2);
    doc->setTextWidth(bubbleRect.width() - mColors.padH * 2);
  } else {
    bubbleRect = QRect(option.rect.x() + 4,
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

  // Check cancel icon click for pending messages
  if (type == MsgPending) {
    QString cancelUrl = index.data(CancelUrlRole).toString();
    if (!cancelUrl.isEmpty() && cancelIconRect(bubbleRect).contains(me->pos())) {
      emit linkClicked(QUrl(cancelUrl));
      return true;
    }
  }

  return QStyledItemDelegate::editorEvent(event, model, option, index);
}
