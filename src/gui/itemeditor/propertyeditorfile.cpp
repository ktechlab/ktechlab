/***************************************************************************
 *   Copyright (C) 2003 Cedric Pasteur <cedric.pasteur@free.fr>            *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "iteminterface.h"
#include "propertyeditorfile.h"
#include "property.h"

#include <klineedit.h>
#include <kpushbutton.h>
#include <kfiledialog.h>
#include <klocalizedstring.h>
#include <kdebug.h>

#include <Qt/qstring.h>
#include <Qt/qpixmap.h>
#include <Qt/qvariant.h>
#include <Qt/qevent.h>
#include <Qt/qlabel.h>
#include <Qt/qcursor.h>


PropertyEditorFile::PropertyEditorFile( QWidget * parent, Property * property, const char * name )
	: PropertySubEditor( parent, property, name )
{
	m_lineedit = new KLineEdit(this);
	m_lineedit->resize(width(), height()-2);

	m_button = new KPushButton(i18n(" ... "), this);
	m_button->resize(height(), height()-10);
	m_button->move(width() - m_button->width() -1, 1);

	m_lineedit->setText(property->value().toString());
	m_lineedit->show();
	m_button->show();

	setWidget(m_lineedit);

	connect( m_button, SIGNAL(clicked()), this, SLOT(selectFile()) );
	connect( property, SIGNAL(valueChanged( const QString& )), m_lineedit, SLOT(setText(const QString &)) );
}


void PropertyEditorFile::selectFile()
{
	KUrl url = KFileDialog::getOpenFileName( KUrl() /* QString::null */, m_property->filter(), this, i18n("Choose File") );
	if ( !url.isValid() )
		return;
	
	m_property->setValue( url.path() );
	ItemInterface::self()->setProperty( m_property );
}


void PropertyEditorFile::resizeEvent(QResizeEvent *ev)
{
	m_lineedit->resize(ev->size());
	m_button->move(ev->size().width() - m_button->width()-1, 1);
}


bool PropertyEditorFile::eventFilter(QObject* watched, QEvent* e)
{
	if(e->type() == QEvent::KeyPress)
	{
		QKeyEvent* ev = static_cast<QKeyEvent*>(e);
		if((ev->key() == Qt::Key_Enter) || (ev->key()== Qt::Key_Space) || (ev->key() == Qt::Key_Return))
		{
			m_button->animateClick();
			return true;
		}
	}
	return PropertySubEditor::eventFilter(watched, e);
}


#include "propertyeditorfile.moc"

