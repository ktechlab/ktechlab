/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef FLOWCODEVIEW_H
#define FLOWCODEVIEW_H

#include <icnview.h>

class FlowCodeDocument;

/**
@author David Saxton
*/
class FlowCodeView : public ICNView
{
	Q_OBJECT
	public:
		FlowCodeView( FlowCodeDocument *flowCodeDocument, ViewContainer *viewContainer, uint viewAreaId, const char *name = 0l );
		~FlowCodeView();
	
	protected:
		virtual void dragEnterEvent( QDragEnterEvent * e );
		FlowCodeDocument *p_flowCodeDocument;
};

#endif
