/***************************************************************************
 *   Copyright (C) 2003 Cedric Pasteur <cedric.pasteur@free.fr>            *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "colorcombo.h"
#include "iteminterface.h"
#include "property.h"
#include "propertyeditorcolor.h"

#include <klocale.h>
#include <kfontdialog.h>
#include <kpushbutton.h>
#include <kdebug.h>

#include <qlabel.h>
#include <qstring.h>
#include <qevent.h>

//BEGIN class PropertyEditorColor
PropertyEditorColor::PropertyEditorColor( QWidget * parent, Property * property, const char * name )
	: PropertySubEditor( parent, property, name )
{
	m_pColorCombo = new ColorCombo( (ColorCombo::ColorScheme)property->colorScheme(), this );
	m_pColorCombo->setColor( property->value().toColor() );
	m_pColorCombo->resize(width(), height());
	m_pColorCombo->show();
	setWidget(m_pColorCombo);
	
	connect( m_pColorCombo, SIGNAL(activated(const QColor&)), this, SLOT(valueChanged(const QColor&)) );
	connect( property, SIGNAL(valueChanged( const QColor& )), m_pColorCombo, SLOT(setColor(const QColor &)) );
}


void PropertyEditorColor::valueChanged( const QColor & color )
{
	m_property->setValue( color );
	ItemInterface::self()->setProperty( m_property );
}


bool PropertyEditorColor::eventFilter(QObject* watched, QEvent* e)
{
	if(e->type() == QEvent::KeyPress)
	{
		QKeyEvent* ev = static_cast<QKeyEvent*>(e);
		if((ev->key() == Key_Enter) | (ev->key()== Key_Space) || (ev->key() == Key_Return))
		{
// 			m_pColorCombo->animateClick();
			m_pColorCombo->popup();
			return true;
		}
	}
	return PropertySubEditor::eventFilter(watched, e);
}
//END class PropertyEditorColor

#include "propertyeditorcolor.moc"

