/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PICINFO_H
#define PICINFO_H

#include "microinfo.h"

/**
This is the base class for all 12bit, 14bit and 16bit PICs.
Thanks go to gpsim & Scott Dattalo for some of the hierachal PIC class
structure used here :-)
@short Base class for all PICMicros
@author David Saxton
*/
class PicInfo : public MicroInfo
{
public:
    PicInfo();
    ~PicInfo() override;
    AsmInfo *instructionSet() override = 0;
};

#endif
