/***************************************************************************
 *   Copyright (C) 2002 by Lucijan Busch <lucijan@gmx.at>                  *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <qvariant.h>
#include <qevent.h>

#include <kdebug.h>

#include "propertysubeditor.h"
#include "propertyeditor.h"

PropertySubEditor::PropertySubEditor( QWidget * parent, Property * property, const char * name )
 : QWidget( parent /*, name */ )
{
    setObjectName( name );
	m_childWidget = 0;
	m_property = property;
	m_leaveTheSpaceForRevertButton = false;
}

bool
PropertySubEditor::eventFilter(QObject* /*watched*/, QEvent* e)
{
	if ( e->type() == QEvent::KeyPress ) // || e->type()==QEvent::AccelOverride)
	{
		QKeyEvent * ev = static_cast<QKeyEvent*>(e);
		PropertyEditor *list = dynamic_cast<PropertyEditor*>( parentWidget()->parentWidget() );
		if (!list)
			return false; //for sanity
		return list->handleKeyPress(ev);
	}
	return false;
}

void
PropertySubEditor::resizeEvent(QResizeEvent *ev)
{
	if(m_childWidget)
	{
		m_childWidget->resize(ev->size());
	}
}

void
PropertySubEditor::setWidget(QWidget *w, QWidget* focusProxy)
{
	if (m_childWidget)
		m_childWidget->removeEventFilter(this);

	m_childWidget = w;

	if(!m_childWidget)
		return;
	if (focusProxy && focusProxy->focusPolicy()!=Qt::NoFocus) {
		setFocusProxy(focusProxy);
		focusProxy->installEventFilter(this);
	}
	else if (m_childWidget->focusPolicy()!=Qt::NoFocus)
		setFocusProxy(m_childWidget);

	m_childWidget->installEventFilter(this);
//	if (m_childWidget->inherits("QFrame")) {
//		static_cast<QFrame*>(m_childWidget)->setFrameStyle( QFrame::Box | QFrame::Plain );
//	}
}


PropertySubEditor::~PropertySubEditor()
{
}


#include "propertysubeditor.moc"
