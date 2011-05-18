/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef BIDIRLED_H
#define BIDIRLED_H

#include "component.h"

class Circuit;
class Diode;

/**
@author David Saxton
*/
class BiDirLED : public Component
{
	public:
		BiDirLED(Circuit &ownerCircuit);
		~BiDirLED();

		// virtual void dataChanged();
		virtual void stepNonLogic();
		virtual bool doesStepNonLogic() const { return true; }

        /**
         * \return the average brightness of a given led
         * \param ledNumber the number of the LED. It should be
         *  0 for one LED, and 1 for the other

         FIXME the current implementation will restun bogous results if this
            method is called too often
         */
		double averageBrightness(int ledNumber);

	private:

		double avg_brightness[2];
		double lastUpdatePeriod;
		Diode *m_pDiode[2];
};

#endif
