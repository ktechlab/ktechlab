/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MICROLIBRARY_H
#define MICROLIBRARY_H

#include "asminfo.h"
#include "microinfo.h"

#include <QList>
#include <QString>
#include <QStringList>

class MicroInfo;
class MicroLibrary;
typedef QList<MicroInfo *> MicroInfoList;

inline MicroLibrary *microLibrary();

/**
@short Stores all the available PICs (info)
@author David Saxton
*/
class MicroLibrary
{
public:
    static MicroLibrary *self();

    ~MicroLibrary();

    MicroInfo *microInfoWithID(QString id);
    void addMicroInfo(MicroInfo *microInfo);

    /**
     * Returns a list of micro ids with the given properties (OR'ed
     * together).
     */
    QStringList microIDs(unsigned asmSet = AsmInfo::AsmSetAll, unsigned gpsimSupport = MicroInfo::AllSupport, unsigned flowCodeSupport = MicroInfo::AllSupport, unsigned microbeSupport = MicroInfo::AllSupport);

public:
    MicroLibrary();

private:
    // 		static MicroLibrary * m_pSelf;

    MicroInfoList m_microInfoList;
    friend MicroLibrary *microLibrary();
};

#endif
