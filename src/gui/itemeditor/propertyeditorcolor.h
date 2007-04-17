/***************************************************************************
 *   Copyright (C) 2003 Cedric Pasteur <cedric.pasteur@free.fr>            *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PROPERTYEDITORFONT_H
#define PROPERTYEDITORFONT_H

#include "propertysubeditor.h"

class ColorCombo;
class KPushButton;
class QLabel;
class QResizeEvent;

class PropertyEditorColor : public PropertySubEditor
{
	Q_OBJECT

	public:
		PropertyEditorColor( QWidget * parent, Property * property, const char * name = 0 );
		~PropertyEditorColor() {;}

		bool eventFilter( QObject * watched, QEvent * e );
		
	protected slots:
		void valueChanged(const QColor &color);

	protected:
		ColorCombo * m_pColorCombo;
		QColor m_color;
};

#endif

