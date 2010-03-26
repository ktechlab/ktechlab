/***************************************************************************
 *   Copyright (C) 2005 by Fredy Yanardi                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MAGNITUDECOMPARATOR_H
#define MAGNITUDECOMPARATOR_H

#include <vector>

#include "dipcomponent.h"
#include "logic.h"

/**
@author Fredy Yanardi
 */
class MagnitudeComparator : public CallbackClass, public DIPComponent {

public:
	MagnitudeComparator(ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~MagnitudeComparator();

	static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
	static LibraryItem *libraryItem();

protected:
	void initPins();
	virtual void dataChanged();
	void inStateChanged();

	int m_oldABLogicCount;
	bool firstTime;

	std::vector<LogicIn> m_aLogic;
	std::vector<LogicIn> m_bLogic;

	LogicIn *m_cLogic[3];
	LogicOut *m_output[3];
};

#endif
