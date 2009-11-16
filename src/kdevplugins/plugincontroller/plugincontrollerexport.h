/**************************************************************************
*   Copyright (C) 2009 by Julian Bäume <julian@svg4all.de>                *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KTLPLUGINCONTROLLEREXPORT_H
#define KTLPLUGINCONTROLLEREXPORT_H

/* needed for KDE_EXPORT macros */
#include <kdemacros.h>

#ifndef PLUGINCONTROLLER_EXPORT
# ifdef MAKE_PLUGINCONTROLLER_LIB
#  define PLUGINCONTROLLER_EXPORT KDE_EXPORT
# else
#  define PLUGINCONTROLLER_EXPORT KDE_IMPORT
# endif
#endif

#endif
