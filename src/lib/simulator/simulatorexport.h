/**************************************************************************
* Copyright (C) 2011 by Zoltan Padrah                                     *
*                  <zoltan.padrah@users.sourceforge.net>                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef SIMULATOREXPORT_H__
#define SIMULATOREXPORT_H__

// because we don't want KDE dependency here, we define these macros 
//  independently from KDE

#define __KDE_HAVE_GCC_VISIBILITY

#ifdef __KDE_HAVE_GCC_VISIBILITY
#define KDE_NO_EXPORT __attribute__ ((visibility("hidden")))
#define KDE_EXPORT __attribute__ ((visibility("default")))
#define KDE_IMPORT __attribute__ ((visibility("default")))
#elif defined(_WIN32) || defined(_WIN64)
#define KDE_NO_EXPORT
#define KDE_EXPORT __declspec(dllexport)
#define KDE_IMPORT __declspec(dllimport)
#else
#define KDE_NO_EXPORT
#define KDE_EXPORT
#define KDE_IMPORT
#endif

// actual macros
#ifndef SIMULATOR_EXPORT
# ifdef MAKE_SIMULATOR_LIB
#  define SIMULATOR_EXPORT KDE_EXPORT
# else
#  define SIMULATOR_EXPORT KDE_IMPORT
# endif
#endif

#endif

