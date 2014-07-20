/**************************************************************************
*   Copyright (C) 2008 by Julian Bäume <julian@svg4all.de>                *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef MATHEXPORT_H
#define MATHEXPORT_H

#if KDE_ENABLED
/* needed for KDE_EXPORT macros */
#include <kdemacros.h>

#ifndef MATH_EXPORT
# ifdef MAKE_MATH_LIB
#  define MATH_EXPORT KDE_EXPORT
# else
#  define MATH_EXPORT KDE_IMPORT
# endif
#endif

#else // KDE_ENABLED

// only static libraries are built, no need for import/export
#define MATH_EXPORT

#endif // KDE_ENABLED

#endif // MATHEXPORT_H
