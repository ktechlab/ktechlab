/***************************************************************************
 *   Copyright (C) 2023 by Friedrich W. H. Kossebau                        *
 *   kossebau@kde.org                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef FILEFILTER_H
#define FILEFILTER_H

#include <kio_version.h>

#include <QString>
#include <QList>

//class QStringList;

struct FileFilter
{
    QString name;
    QString patterns;
};

class FileFilters : public QList<FileFilter>
{
public:
    FileFilters() = default;
    FileFilters(const FileFilters& other) = default;
    FileFilters(std::initializer_list<FileFilter> list) : QList<FileFilter>(list) {}

    FileFilters &operator=(const QList<FileFilter> &other)
    { QList<FileFilter>::operator=(other); return *this; }

public:
    QString toQtStyleString() const;
    QStringList toQtStyleStringList() const;
};


#endif

