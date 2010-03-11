/***************************************************************************
 *   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>                     *
 *   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>                     *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "iteminterface.h"
#include "propertyeditorlist.h"
#include "property.h"

#include <kdebug.h>
#include <kiconloader.h>
#include <klistbox.h>
#include <klocale.h>

#include <qcursor.h>
#include <qhbox.h>
#include <qstringlist.h>
#include <qtoolbutton.h>


//BEGIN class PropComboBox
PropComboBox::PropComboBox( QWidget *parent )
   : KComboBox(parent)
{
	m_eventFilterEnabled = true;
}


bool PropComboBox::eventFilter(QObject *o, QEvent *e)
{
	if (!m_eventFilterEnabled)
		return false;

	if(o == lineEdit())
	{
		if(e->type() == QEvent::KeyPress)
		{
			QKeyEvent* ev = static_cast<QKeyEvent*>(e);
			if((ev->key()==Key_Up || ev->key()==Key_Down) && ev->state()!=ControlButton)
			{
				parentWidget()->eventFilter(o, e);
				return true;
			}
		}
	}

	return KComboBox::eventFilter(o, e);
}


void PropComboBox::hideList()
{
	lineEdit()->setFocus();
}
//END class PropComboBox



//BEGIN class PropertyEditorList
PropertyEditorList::PropertyEditorList( QWidget * parent, Property * property, const char * name )
	: PropertySubEditor( parent, property, name )
{
	QHBox *box = new QHBox(this);

	m_combo = new PropComboBox( box );
	m_combo->setGeometry(frameGeometry());
	
	bool isEditable = false;
	switch ( property->type() )
	{
		case Property::Type::Port:
		case Property::Type::Pin:
		case Property::Type::PenStyle:
		case Property::Type::PenCapStyle:
		case Property::Type::SevenSegment:
		case Property::Type::KeyPad:
		case Property::Type::Select:
			isEditable = false;
			break;
			
		case Property::Type::String:
		case Property::Type::Multiline:
		case Property::Type::RichText:
		case Property::Type::Combo:
		case Property::Type::FileName:
		case Property::Type::VarName:
			isEditable = true;
			break;
				
		case Property::Type::None:
		case Property::Type::Int:
		case Property::Type::Raw:
		case Property::Type::Double:
		case Property::Type::Color:
		case Property::Type::Bool:
			// not handled by this
			break;
	}
	
	m_combo->setEditable( isEditable );
	
	m_combo->setInsertionPolicy(QComboBox::NoInsertion);
	m_combo->setAutoCompletion(true);
	m_combo->setMinimumSize(10, 0); // to allow the combo to be resized to a small size

	m_combo->insertStringList( m_property->allowed() );
	m_combo->setCurrentText( m_property->displayString() );
	KCompletion *comp = m_combo->completionObject();
	comp->insertItems( m_property->allowed() );

	setWidget(box, m_combo->lineEdit());
	
	connect( m_combo, SIGNAL(activated( const QString & )), this, SLOT(valueChanged( const QString & )) );
	connect( m_property, SIGNAL(valueChanged( const QString& )), m_combo, SLOT(setCurrentItem( const QString & )) );
}


void PropertyEditorList::setList(QStringList l)
{
	m_combo->insertStringList(l);
}


void PropertyEditorList::valueChanged( const QString & text )
{
	m_property->setValue( text );
	ItemInterface::self()->setProperty( m_property );
}
//END class PropertyEditorList


#include "propertyeditorlist.moc"
