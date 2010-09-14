/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ICNVIEW_H
#define ICNVIEW_H

#include <itemview.h>

class ICNDocument;
class KRadioAction;
class KToolBarPopupAction;

/**
@author David Saxton
*/
class ICNView : public ItemView
{
	Q_OBJECT
	public:
		ICNView( ICNDocument * icnDocument, ViewContainer *viewContainer, uint viewAreaId, const char * name = 0l );
		~ICNView();
		
	protected slots:
		void slotSetRoutingMode( int mode ); // 0 = auto, 1 = manual
		void slotSetRoutingAuto();
		void slotSetRoutingManual();
		void slotUpdateRoutingMode( bool manualRouting );
		void slotUpdateRoutingToggles( bool manualRouting );
		
	protected:
		KToolBarPopupAction * m_pRoutingModeToolbarPopup;
		KRadioAction * m_pManualRoutingAction;
		KRadioAction * m_pAutoRoutingAction;
};

#endif
