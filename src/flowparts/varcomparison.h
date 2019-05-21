/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef VARCOMPARISON_H
#define VARCOMPARISON_H

#include "flowpart.h"

/**
@short FlowPart that compares two values
@author David Saxton
*/
class VarComparison : public FlowPart
{
public:
	VarComparison( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~VarComparison() override;
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
	void generateMicrobe( FlowCode *code ) override;

protected:
	void dataChanged() override;
	/**
	 * Use this to find the logically opposite comparison (e.g. "==" returns "!=",
	 * ">=" returns "<", etc). Supoorted ops: != == <= >= < > 
	 */
	QString oppOp( const QString &op );
};

#endif
