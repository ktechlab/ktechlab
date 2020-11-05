/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef COLORUTILS_H
#define COLORUTILS_H

class QWidget;
class KColorCombo;

namespace ColorUtils
{

enum ColorScheme {
    QtStandard = 0,
    LED = 1,
};

KColorCombo *createColorCombo(ColorScheme colorScheme, QWidget *parent = nullptr);

}

#endif
