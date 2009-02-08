 /**************************************************************************
 *   Copyright (C) 2008 by Julian Bäume <julian@svg4all.de>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KTECHLABCORE_H
#define KTECHLABCORE_H

#include "ktlshellexport.h"

#include <shell/core.h>

class KTLSHELL_EXPORT KTechlabCore : public KDevelop::Core
{
public:
    static void initialize( KDevelop::Core::Setup mode = KDevelop::Core::Default );

    virtual KDevelop::Core::Setup setupFlags() const;
protected:
    static KTechlabCore *m_self;
    KDevelop::CorePrivate *d;

private:
    KTechlabCore( QObject *parent = 0 );
};

#endif // KTL_CORE_H
