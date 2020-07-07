#include "TextEmotionChanger.h"

CTextEmotionChanger *CTextEmotionChanger::instanz = NULL;

CTextEmotionChanger *CTextEmotionChanger::exemplar() {
  if (instanz == NULL) {
    instanz = new CTextEmotionChanger();
  }
  return instanz;
}

CTextEmotionChanger::~CTextEmotionChanger() {}

CTextEmotionChanger::CTextEmotionChanger() {
  m_check_space = false;
  m_emoticons_path = ":/emoticons/emoticons.xml";
  setEmoticonPath(m_emoticons_path);
}
void CTextEmotionChanger::checkMessageForEmoticons(QString &message) {
  CTextEmotionChanger::HtmlState state = OutsideHtml;
  bool at_amp = false;
  const QChar *begin = message.constData();
  const QChar *chars = message.constData();
  QChar cur;
  QString result;
  QList<QPair<QString, QString>>::const_iterator it;
  while (!chars->isNull()) {
    cur = *chars;
    if (cur == '<') {
      if (state == OutsideHtml)
        state = FirstTag;
      else
        state = SecondTag;
    } else if (state == FirstTag || state == SecondTag) {
      switch (cur.unicode()) {
      case L'/':
        state = SecondTag;
        break;
      case L'"':
      case L'\'':
        do
          result += *(chars++);
        while (!chars->isNull() && *chars != cur);
        break;
      case L'>':
        state = static_cast<HtmlState>((state + 1) % 4);
        break;
      default:
        break;
      }
    } else if (state != TagText && at_amp) {
      do
        result += *(chars++);
      while (!chars->isNull() && *chars != ';');
      cur = *chars;
      at_amp = false;
    } else if (state != TagText) {
      at_amp = cur == '&';
      if (!m_check_space || chars == begin || (chars - 1)->isSpace()) {
        bool found = false;
        it = m_emoticons.constBegin();
        for (; it != m_emoticons.constEnd(); ++it) {
          int length = (*it).first.length();
          if (compareEmoticon(chars, (*it).first) &&
              (!m_check_space || (chars + length)->isNull() ||
               (chars + length)->isSpace())) {
            appendEmoticon(
                result, (*it).second,
                QStringRef(&message, chars - begin, (*it).first.length()));
            found = true;
            at_amp = false;
            chars += length;
            cur = *chars;
            break;
          }
        }
        if (found)
          continue;
      }
    }
    if (cur.isNull())
      break;
    result += cur;
    chars++;
  }

  message = result;
}

bool CTextEmotionChanger::compareEmoticon(const QChar *c,
                                          const QString &smile) const {
  const QChar *s = smile.constData();
  while (c->toLower() == *s) {
    if (s->isNull())
      return true;
    s++;
    c++;
  }
  return s->isNull();
}

void CTextEmotionChanger::appendEmoticon(QString &text, const QString &url,
                                         const QStringRef &emo) const {
  int i = 0, last = 0;
  while ((i = url.indexOf(QLatin1String("%4"), last)) != -1) {
    text += QStringRef(&url, last, i - last);
    text += emo;
    last = i + 2;
  }
  text += QStringRef(&url, last, url.length() - last);
}

bool CTextEmotionChanger::lengthLessThan(const QString &s1, const QString &s2) {
  return s1.size() > s2.size();
}
void CTextEmotionChanger::setEmoticonPath(const QString &path) {
  m_emoticon_list.clear();
  m_emoticons.clear();
  QFile file(path);
  QString dirPath = QFileInfo(path).absolutePath();
  m_dir_path = dirPath;
  QDir dir(dirPath);
  QStringList fileList = dir.entryList(QDir::Files);
  if (file.exists() && file.open(QIODevice::ReadOnly)) {
    QDomDocument doc;
    if (doc.setContent(&file)) {
      QDomElement rootElement = doc.documentElement();
      int emoticonCount = rootElement.childNodes().count();
      QDomElement emoticon = rootElement.firstChild().toElement();
      for (int i = 0; i < emoticonCount; i++) {
        if (emoticon.tagName() == "emoticon") {
          QString regexp = "(^";
          regexp += QRegExp::escape(emoticon.attribute("file"));
          regexp += "\\.\\w+$)|(^";
          regexp += QRegExp::escape(emoticon.attribute("file"));
          regexp += "$)";
          QStringList fileName = fileList.filter(QRegExp(regexp));
          if (!fileName.isEmpty()) {
            QStringList strings;
            QPixmap tmp;
            QImage image = tmp.toImage();
            image.fill(Qt::transparent);
            int stringCount = emoticon.childNodes().count();
            QDomElement emoticonString = emoticon.firstChild().toElement();
            for (int j = 0; j < stringCount; j++) {
              if (emoticonString.tagName() == "string") {
                if (image.isNull())
                  tmp = QPixmap(dirPath + "/" + fileName.at(0));
                int width = 16;
                int height = width;
                image = image.scaled(width, height, Qt::IgnoreAspectRatio,
                                     Qt::SmoothTransformation);
                QString text = emoticonString.text().toHtmlEscaped();
                m_urls.insert(text,
                              QString("<img src=\"%1\" width=\"%2\" "
                                      "height=\"%2\" alt=\"%1\" title=\"%1\"/>")
                                  .arg(dirPath + "/" + fileName.at(0))
                                  .arg(width)
                                  .arg(text.replace("\"", "&quot;")));
                strings.append(emoticonString.text());
              }

              emoticonString = emoticonString.nextSibling().toElement();
            }
            m_emoticon_list.insert(QString::number(i + 1) + "|" + dirPath +
                                       "/" + fileName.at(0),
                                   strings);
          }
        }
        emoticon = emoticon.nextSibling().toElement();
      }
      QStringList emoticon_keys = m_urls.keys();
      qSort(emoticon_keys.begin(), emoticon_keys.end(), lengthLessThan);
      m_emoticons.clear();
      foreach (const QString &emoticon, emoticon_keys) {
        m_emoticons << qMakePair(emoticon.toLower(), m_urls.value(emoticon));
      }
    }
  }
}
