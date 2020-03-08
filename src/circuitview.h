/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CIRCUITVIEW_H
#define CIRCUITVIEW_H

#include <icnview.h>

class CircuitDocument;

/**
@author David Saxton
*/
class CircuitView : public ICNView
{
Q_OBJECT
public:
	CircuitView( CircuitDocument *circuitDocument, ViewContainer *viewContainer, uint viewAreaId, const char *name = nullptr );
	~CircuitView() override;

public slots:
	virtual void slotUpdateRunningStatus( bool isRunning );

protected:
	void dragEnterEvent( QDragEnterEvent * e ) override;
	CircuitDocument *p_circuitDocument;
};

#endif
