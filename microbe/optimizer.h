/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "instruction.h"


/// Used for debugging; returns the uchar as a binary string (e.g. 01101010).
QString binary( uchar val);


/**
@author David Saxton
*/
class Optimizer
{
	public:
		Optimizer();
		~Optimizer();

		void optimize( Code * code);
		
	protected:
		/**
		 * Repeatedly generates links and states for the instructions and
		 * refining their input states, until equilibrium in the input states
		 * is reached.
		 */
		void propagateLinksAndStates();
		/**
		 * Tell the instructions about their input states.
		 * @return whether any input states changed from the previous value
		 * stored in the instruction (if checkChanged is true - else returns
		 * true).
		 */
		bool giveInputStates();
		/**
		 * Remove instructions without any input links (and the ones that are
		 * only linked to from a removed instruction).
		 * @return whether any instructions were removed
		 */
		bool pruneInstructions();
		/**
		 * Perform optimizations (code cropping, modification, assembly, etc)
		 * based on instruction linkage and processor states.
		 * @return whether anything was changed
		 */
		bool optimizeInstructions();
		/**
		 * Redirects any GOTOs that point at the given instruction to the given
		 * label.
		 * @return whether any GOTOs were redirected
		 */
		bool redirectGotos( Instruction * current, const QString & label);
		/**
		 * Find out if the given instruction or any of its outputs overwrite
		 * any of the bits of the given register before they are used.
		 */
		uchar generateRegisterDepends( Instruction * current, const Register & reg);
		/**
		 * This function should only be used from generateRegisterDepends.
		 * Recursively looks at the output links of the given instruction, and
		 * returns which bits are eventually used before being overwritten.
		 */
		uchar registerDepends( Instruction * current, const Register & reg);
		/**
		 * We often need to know whether removing an instruction will affect the
		 * future processor state. This function looks are all possible future
		 * dependencies of the given register, and returns true if the removal
		 * of the instruction will have no critical effect.
		 * @param bitMask only look at the given bits of the register
		 */
		bool canRemove( Instruction * ins, const Register & reg, uchar bitMask = 0xff);
		
		Code * m_pCode;
};

#endif
