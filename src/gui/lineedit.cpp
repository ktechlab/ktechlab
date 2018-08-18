/***************************************************************************
 *   Copyright (C) 2006 by David Saxton - david@bluehaze.org               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "lineedit.h"

LineEdit::LineEdit( QWidget * parent, const char * name )
	: KLineEdit( parent )
{
    setObjectName( name );
}


LineEdit::LineEdit( const QString & text, QWidget * parent, const char * name )
	: KLineEdit( text, parent )
{
    setObjectName( name );
}


LineEdit::~LineEdit()
{
}


void LineEdit::setText( const QString & text )
{
	if ( this->text() == text )
		return;
	
	KLineEdit::setText( text );
}


#include "lineedit.moc"
