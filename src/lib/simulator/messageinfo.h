/*
    KTechLab, and IDE for electronics

    Copyright (C) 2003-2005 by David Saxton
    david@bluehaze.org

    Copyright (C) 2010 Zoltan Padrah

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef MESSAGEINFO_H
#define MESSAGEINFO_H

#include "simulatorexport.h"

#include <QObject>
#include <QString>

class SIMULATOR_EXPORT MessageInfo : public QObject
{
    Q_OBJECT

    public:
        MessageInfo( QString fileURL  = "", int fileLine = -1);
        MessageInfo(const MessageInfo &orig);

        QString fileURL() const { return m_fileURL; }
        int fileLine() const { return m_fileLine; }

    protected:
        QString m_fileURL;
        int m_fileLine;
};

#endif // MESSAGEINFO_H
