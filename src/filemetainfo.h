/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef FILEMETAINFO_H
#define FILEMETAINFO_H

#include "outputmethoddlg.h"

#include <QList>
#include <QMap>
#include <QUrl>

class TextDocument;
class TextView;
class KConfig;
class KConfigGroup;
typedef QList<int> IntList;

class MetaInfo
{
public:
    MetaInfo();

    /**
     * Returns true if all the data stored is default; and therefore does
     * not need saving.
     */
    bool hasDefaultData() const;
    /**
     * Writes to the given config the data stored in here. Does not set the
     * group.
     */
    void save(KConfigGroup *conf);
    /**
     * Reads in the data from the config. Does not set the group.
     */
    void load(KConfigGroup *conf);

    IntList bookmarks() const
    {
        return m_bookmarks;
    }
    void setBookmarks(IntList bookmarks)
    {
        m_bookmarks = bookmarks;
    }

    IntList breakpoints() const
    {
        return m_breakpoints;
    }
    void setBreakpoints(IntList breakpoints)
    {
        m_breakpoints = breakpoints;
    }

    OutputMethodInfo &outputMethodInfo()
    {
        return m_outputMethodInfo;
    }
    void setOutputMethodInfo(OutputMethodInfo info)
    {
        m_outputMethodInfo = info;
    }

    unsigned cursorLine() const
    {
        return m_cursorLine;
    }
    void setCursorLine(unsigned line)
    {
        m_cursorLine = line;
    }

    unsigned cursorColumn() const
    {
        return m_cursorColumn;
    }
    void setCursorColumn(unsigned column)
    {
        m_cursorColumn = column;
    }

protected:
    /**
     * Convert the id (e.g. "Direct") to a method, used when reading in the
     * config file.
     */
    OutputMethodInfo::Method::Type toMethod(const QString &id);
    /**
     * Conver the method (e.g. OutputMethodInfo::Method::Direct) to an id
     * that can be saved in the config file.
     */
    QString toID(OutputMethodInfo::Method::Type method);

    IntList m_bookmarks;
    IntList m_breakpoints;
    OutputMethodInfo m_outputMethodInfo;
    unsigned m_cursorLine;
    unsigned m_cursorColumn;
};
typedef QMap<QUrl, MetaInfo> MetaInfoMap;

/**
Looks after per-file metainfo; e.g. bookmarks, breakpoints, compiling options, etc

@author David Saxton
*/
class FileMetaInfo : public QObject
{
    Q_OBJECT
public:
    ~FileMetaInfo() override;

    /**
     * Initialize the TextDocument with the appropriate stored metainfo - e.g.
     * setting the appopriate bookmarks, etc
     */
    void initializeFromMetaInfo(const QUrl &url, TextDocument *textDocument);
    /**
     * Initialize the TextView with the appropriate stored metainfo - e.g.
     * setting the appopriate cursor position, etc.
     */
    void initializeFromMetaInfo(const QUrl &url, TextView *textView);
    /**
     * Initialize the OutputMethodDlg with the options the user had selected
     * for the last time it was used for the given url.
     */
    void initializeFromMetaInfo(const QUrl &url, OutputMethodDlg *outputMethodDlg);
    /**
     * Get the bookmarks, etc from the given TextDocument, and save them
     */
    void grabMetaInfo(const QUrl &url, TextDocument *textDocument);
    /**
     * Get the cursor position, etc from the given TextView, and save them.
     */
    void grabMetaInfo(const QUrl &url, TextView *textView);
    /**
     * Get the output method et al from the given OutputMethodDlg, and save
     * them.
     */
    void grabMetaInfo(const QUrl &url, OutputMethodDlg *outputMethodDlg);
    /**
     * Save all metainfo to disk.
     */
    void saveAllMetaInfo();

protected:
    /**
     * Load all metainfo from disk (combining that read in with those already
     * loaded)
     */
    void loadAllMetaInfo();

    KConfig *m_metaInfoConfig;

private:
    FileMetaInfo();
    friend inline FileMetaInfo *fileMetaInfo();

    MetaInfoMap m_metaInfoMap;
};

inline FileMetaInfo *fileMetaInfo()
{
    static FileMetaInfo *fmi = new FileMetaInfo();
    return fmi;
}

#endif
