/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECMOSFET_H
#define ECMOSFET_H

#include "component.h"
#include "mosfet.h"

/**
@short Simulates a MOSFET
@author David Saxton
 */
class ECMOSFET : public Component
{
	public:
		ECMOSFET( int MOSFET_type);
		~ECMOSFET();
	
	protected:
        virtual void propertyChanged(Property& theProperty, QVariant newValue,
                                     QVariant oldValue);

		
		bool m_bHaveBodyPin;
		int m_MOSFET_type;
		MOSFET m_pMOSFET;
};
#endif
