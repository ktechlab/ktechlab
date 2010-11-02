/**************************************************************************
*   Copyright (C) 2008 by Julian Bäume <julian@svg4all.de>                *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef CIRCUITEXPORT_H
#define CIRCUITEXPORT_H

/* needed for KDE_EXPORT macros */
#include <kdemacros.h>

#ifndef CIRCUIT_EXPORT
# ifdef MAKE_CIRCUIT_LIB
#  define CIRCUIT_EXPORT KDE_EXPORT
# else
#  define CIRCUIT_EXPORT KDE_IMPORT
# endif
#endif

#endif
