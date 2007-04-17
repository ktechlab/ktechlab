/***************************************************************************
 *   Copyright (C) 2003-2006 David Saxton <david@bluehaze.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ORIENTATIONWIDGET_H
#define ORIENTATIONWIDGET_H

#include <qguardedptr.h>
#include <qwidget.h>

class CNItem;
class CNItemGroup;
class Component;
class FlowPart;

class QPushButton;

/**
@author David Saxton
*/
class OrientationWidget : public QWidget
{
	Q_OBJECT
	public:
		OrientationWidget( QWidget *parent = 0l, const char *name = 0l );
		~OrientationWidget();

	public slots:
		void slotUpdate( CNItemGroup * itemGroup );
		void slotClear();
	
	signals:
		void orientationSelected( uint orientation );
		
	protected slots:
		void slotButtonClicked();
		/**
		 * Updates which button is indented depending on the current orientation
		 * of the item(s) being edited.
		 */
		void updateShownOrientation();
	
	protected:
		void initFromComponent( Component * component );
		void initFromFlowPart( FlowPart * flowPart );
	
		QPushButton *m_toolBtn[2][4];
		QGuardedPtr<FlowPart> m_pFlowPart;
		QGuardedPtr<Component> m_pComponent;
		QGuardedPtr<CNItem> m_pCNItem; // Either the flowpart or component
		bool m_bHaveSameOrientation; // Whether the items had the same orientation when last updated
};

#endif
