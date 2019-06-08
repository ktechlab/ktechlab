/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef SERIALPORTCOMPONENT_H
#define SERIALPORTCOMPONENT_H

#include "logic.h"
#include "component.h"

class SerialPort;

/**
@author David Saxton
*/
class SerialPortComponent : public CallbackClass, public Component
{
	public:
		SerialPortComponent( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
		~SerialPortComponent() override;
	
		static Item * construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem * libraryItem();
		
		void stepNonLogic() override;
		bool doesStepNonLogic() const override { return true; }

	protected:
		/**
		 * @param baudRate as defined in <bits/termios.h>
		 */
		void initPort( const QString & port, unsigned baudRate );
		void dataChanged() override;
		void drawShape( QPainter & p ) override;

		void tdCallback( bool isHigh );
		void dtrCallback( bool isHigh );
		void dsrCallback( bool isHigh );
		void rtsCallback( bool isHigh );

		LogicIn * m_pTD;
		LogicIn * m_pDTR;
// 		LogicIn * m_pDSR;
// 		LogicIn * m_pRTS;

		LogicOut * m_pCD;
// 		LogicOut * m_pRD;
		LogicOut * m_pCTS;
		LogicOut * m_pRI;

		SerialPort * m_pSerialPort;
};

#endif
