/***************************************************************************
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "component.h"
#include "componentmodellibrary.h"
#include "componentmodelwidget.h"

#include <kdebug.h>
#include <klineedit.h>
#include <klistview.h>
#include <klocale.h>
#include <ktoolbar.h>
#include <ktoolbarbutton.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qheader.h>
#include <qpainter.h>
#include <qtooltip.h>


//BEGIN class ComponentModelWidget
ComponentModelWidget::ComponentModelWidget( QWidget *parent, const char *name )
	: QWidget(parent, name)
{
	QVBoxLayout * vlayout = new QVBoxLayout( this, 0, 6 );
	
	
	// parts of the following code are stolen from amarok/src/playlistwindow.cpp :)
	//BEGIN Filter lineedit
	KToolBar * bar = new KToolBar( this, "ComponentModelSearch" );
	bar->setIconSize( 22, false ); //looks more sensible
	bar->setFlat( true ); //removes the ugly frame
	bar->setMovingEnabled( false ); //removes the ugly frame

	QWidget * button = new KToolBarButton( "locationbar_erase", 1, bar );
	m_pSearchEdit = new ClickLineEdit( i18n( "Filter here..." ), bar );

	bar->setStretchableWidget( m_pSearchEdit );
	m_pSearchEdit->setFrame( QFrame::Sunken );
	connect( m_pSearchEdit, SIGNAL(textChanged( const QString & )), this, SLOT(setFilter( const QString& )) );

	connect( button, SIGNAL(clicked()), m_pSearchEdit, SLOT(clear()) );

	QToolTip::add( button, i18n( "Clear filter" ) );
	QString filtertip = i18n( "Enter space-separated terms to filter the component library." );
	
	QToolTip::add( m_pSearchEdit, filtertip );
	//END Filter lineedit
	
	m_pList = new KListView( this );
// 	m_pList->setItemMargin( 3 );
	m_pList->addColumn( "model" );
	m_pList->setFullWidth( true );
	m_pList->header()->hide();
	QToolTip::add( m_pList, i18n( "Select a predefined component configuration from this list." ) );
	
	vlayout->addWidget( bar );
	vlayout->addWidget( m_pList );
}


ComponentModelWidget::~ComponentModelWidget()
{
}


void ComponentModelWidget::reset()
{
	m_pList->clear();
	m_pSearchEdit->clear();
}


void ComponentModelWidget::init( Component * component )
{
	// for testing purposes
	reset();
	if ( !component )
		return;
	
	QStringList types;
	if ( component->type() == "ec/npnbjt" )
	{
		types = ComponentModelLibrary::self()->modelIDs( ComponentModelLibrary::NPN );
	}
	else if ( component->type() == "ec/pnpbjt" )
	{
		types = ComponentModelLibrary::self()->modelIDs( ComponentModelLibrary::PNP );
	}
	else
		return;
	
	QStringList::iterator end = types.end();
	for ( QStringList::iterator it = types.begin(); it != end; ++it )
	{
		new KListViewItem( m_pList, *it );
	}
}


void ComponentModelWidget::setFilter( const QString & filter )
{
	QString lower = filter.lower();
	
	for ( QListViewItemIterator it( m_pList ); it.current(); ++it )
	{
		QListViewItem * item = *it;
		bool hasText = item->text(0).lower().contains( lower );
		item->setVisible( hasText );
	}
}
//END class ComponentModelWidget



//BEGIN class ClickLineEdit
ClickLineEdit::ClickLineEdit( const QString &msg, QWidget *parent, const char* name ) :
		KLineEdit( parent, name )
{
	mDrawClickMsg = true;
	setClickMessage( msg );
}


void ClickLineEdit::setClickMessage( const QString &msg )
{
	mClickMessage = msg;
	repaint();
}


void ClickLineEdit::setText( const QString &txt )
{
	mDrawClickMsg = txt.isEmpty();
	repaint();
	KLineEdit::setText( txt );
}


void ClickLineEdit::drawContents( QPainter *p )
{
	KLineEdit::drawContents( p );

	if ( mDrawClickMsg == true && !hasFocus() ) {
		QPen tmp = p->pen();
		p->setPen( palette().color( QPalette::Disabled, QColorGroup::Text ) );
		QRect cr = contentsRect();

        //p->drawPixmap( 3, 3, SmallIcon("filter") );

        // Add two pixel margin on the left side
		cr.rLeft() += 3;
		p->drawText( cr, AlignAuto | AlignVCenter, mClickMessage );
		p->setPen( tmp );
	}
}


void ClickLineEdit::focusInEvent( QFocusEvent *ev )
{
	if ( mDrawClickMsg == true ) {
		mDrawClickMsg = false;
		repaint();
	}
	QLineEdit::focusInEvent( ev );
}


void ClickLineEdit::focusOutEvent( QFocusEvent *ev )
{
	if ( text().isEmpty() ) {
		mDrawClickMsg = true;
		repaint();
	}
	QLineEdit::focusOutEvent( ev );
}
//END class ClickLineEdit

#include "componentmodelwidget.moc"
