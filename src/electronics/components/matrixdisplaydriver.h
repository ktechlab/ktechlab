/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MATRIXDISPLAYDRIVER_H
#define MATRIXDISPLAYDRIVER_H

#include "matrixdisplay.h"

/**
@author David Saxton
 */
class MatrixDisplayDriver : public Component
{
	public:
		MatrixDisplayDriver( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
		~MatrixDisplayDriver();

		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();

		virtual void stepNonLogic() override;
		virtual bool doesStepNonLogic() const override { return true; }

	protected:
		QVector<LogicIn*> m_pValueLogic;
		QVector<LogicOut*> m_pRowLogic;
		QVector<LogicOut*> m_pColLogic;

		unsigned m_prevCol;
		unsigned m_nextCol;
		unsigned m_scanCount;
};

#endif
