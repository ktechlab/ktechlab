/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECBJT_H
#define ECBJT_H

#include "component.h"
#include "bjt.h"

/**
@short Simulates a BJT
@author David Saxton
*/
class ECBJT : public Component
{
	public:
		ECBJT(bool isNPN);
		~ECBJT();
	
	
	protected:
        virtual void propertyChanged(Property& theProperty, QVariant newValue,
                                     QVariant oldValue);
		
		bool m_bIsNPN;
		BJT m_pBJT;
};

#endif
