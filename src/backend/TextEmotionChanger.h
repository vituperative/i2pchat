/***************************************************************************
 *   Copyright (C) 2008 by I2P-Messenger                                   *
 *   Messenger-Dev@I2P-Messenger                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// based on qutim-0.2.0 abstractemoticonslayer.h
/*
   AbstractEmoticonsLayer

   Copyright (c) 2009 by Rustam Chakin <qutim.develop@gmail.com>

***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************
*/

#ifndef TEXTEMOTIONCHANGER_H
#define TEXTEMOTIONCHANGER_H

#include <QDomDocument>
#include <QDomElement>
#include <QHash>
#include <QtGui>

class CTextEmotionChanger {
public:
  enum HtmlState { OutsideHtml = 0, FirstTag, TagText, SecondTag };

  static CTextEmotionChanger *exemplar();
  ~CTextEmotionChanger();

  // forbid some operators
  CTextEmotionChanger &operator=(const CTextEmotionChanger &) = delete;

  QHash<QString, QStringList> getEmoticonsList() const {
    return m_emoticon_list;
  }
  void checkMessageForEmoticons(QString &message);
  static bool lengthLessThan(const QString &s1, const QString &s2);
  QString getEmoticonsPath() const { return m_dir_path; }

private:
  static CTextEmotionChanger *instanz;
  CTextEmotionChanger();
  CTextEmotionChanger(const CTextEmotionChanger &);
  void setEmoticonPath(const QString &path);
  void appendEmoticon(QString &text, const QString &url,
                      const QStringRef &emo) const;
  bool compareEmoticon(const QChar *c, const QString &smile) const;

  QString m_profile_name;
  QString m_emoticons_path;
  QHash<QString, QStringList> m_emoticon_list;
  QHash<QString, QString> m_urls;
  QList<QPair<QString, QString>> m_emoticons;
  QString m_dir_path;
  bool m_check_space;
};
#endif
