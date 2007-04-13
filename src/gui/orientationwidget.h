/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
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
class FlowPart;
class QPushButton;

/**
@author David Saxton
*/
class OrientationWidget : public QWidget
{
Q_OBJECT
public:
	OrientationWidget( QWidget *parent = 0, const char *name = 0 );
	~OrientationWidget();

public slots:
	void slotUpdate( CNItem *item );
	void slotClear();
	
	void set_cio_noflip_0();
	void set_cio_noflip_90();
	void set_cio_noflip_180();
	void set_cio_noflip_270();
	void set_cio_flip_0();
	void set_cio_flip_90();
	void set_cio_flip_180();
	void set_cio_flip_270();
	
signals:
	void orientationSelected( uint orientation );
	
protected:
	void initFromComponent( Component *component );
	void initFromFlowPart( FlowPart *flowPart );
	
	QPushButton *m_toolBtn[2][4];
	QGuardedPtr<FlowPart> p_activeFlowPart;
};

#endif
