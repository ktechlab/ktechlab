/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MECHANICSVIEW_H
#define MECHANICSVIEW_H

#include <itemview.h>

class MechanicsDocument;

/**
@author David Saxton
*/
class MechanicsView : public ItemView
{
	Q_OBJECT
	public:
		MechanicsView( MechanicsDocument *mechanicsDocument, ViewContainer *viewContainer, uint viewAreaId, const char *name = 0l );
		~MechanicsView() override;

	protected:
		void dragEnterEvent( QDragEnterEvent * e ) override;
		MechanicsDocument *m_pMechanicsDocument;
};

#endif
