/**************************************************************************
*   Copyright (C) 2008 by Julian Bäume <julian@svg4all.de>                *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KTLINTERFACESEXPORT_H
#define KTLINTERFACESEXPORT_H

#if KDE_ENABLED

/* needed for KDE_EXPORT macros */
#include <kdemacros.h>

#ifndef KTLINTERFACES_EXPORT
# ifdef MAKE_KTLINTERFACES_LIB
#  define KTLINTERFACES_EXPORT KDE_EXPORT
# else
#  define KTLINTERFACES_EXPORT KDE_IMPORT
# endif
#endif

#else

// only build simple static libraries, no need for exporting
#define KTLINTERFACES_EXPORT

#endif // KDE_ENABLED

#endif // KTLINTERFACESEXPORT_H
