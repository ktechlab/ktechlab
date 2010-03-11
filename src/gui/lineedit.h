/***************************************************************************
 *   Copyright (C) 2006 by David Saxton - david@bluehaze.org               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <klineedit.h>

/**
Workaround bug in QLineEdit - actually check to see if the text we have is the
same as when setText is called - and if so, don't reset it.
@author David Saxton
*/
class LineEdit : public KLineEdit
{
	Q_OBJECT
	public:
		LineEdit( QWidget * parent = 0, const char * name = 0 );
		LineEdit( const QString & text, QWidget * parent = 0, const char * name = 0 );
		~LineEdit();
		
	public slots:
		virtual void setText( const QString & text );
};

#endif
