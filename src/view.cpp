/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "document.h"
#include "iteminterface.h"
#include "ktechlab.h"
#include "view.h"
#include "viewiface.h"
#include "viewcontainer.h"

#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <ksqueezedtextlabel.h>
#include <qpainter.h>

//BEGIN KVSSBSep
// Taken from kdebase-3.4.0/kate/app/kateviewspace.cpp, Copyright Anders Lund <anders.lund@lund.tdcadsl.dk>
/*
   "KateViewSpaceStatusBarSeparator"
   A 2 px line to separate the statusbar from the view.
   It is here to compensate for the lack of a frame in the view,
   I think Kate looks very nice this way, as QScrollView with frame
   looks slightly clumsy...
   Slight 3D effect. I looked for suitable QStyle props or methods,
   but found none, though maybe it should use QStyle::PM_DefaultFrameWidth
   for height (TRY!).
   It does look a bit funny with flat styles (Light, .Net) as is,
   but there are on methods to paint panel lines separately. And,
   those styles tends to look funny on their own, as a light line
   in a 3D frame next to a light contents widget is not functional.
   Also, QStatusBar is up to now completely ignorant to style.
   -anders
*/
class KVSSBSep : public QWidget {
	public:
		KVSSBSep( View * parent=0) : QWidget(parent)
		{
			setFixedHeight( 2 );
		}
	protected:
		void paintEvent( QPaintEvent *e )
		{
			QPainter p( this );
			p.setPen( colorGroup().shadow() );
			p.drawLine( e->rect().left(), 0, e->rect().right(), 0 );
			p.setPen( ((View*)parentWidget())->isFocused() ? colorGroup().light() : colorGroup().midlight() );
			p.drawLine( e->rect().left(), 1, e->rect().right(), 1 );
		}
};
//END KVSSBSep



//BEGIN class View
View::View( Document *document, ViewContainer *viewContainer, uint viewAreaId, const char *name )
	: QWidget( viewContainer->viewArea(viewAreaId), name ? name : (const char *)("view_" + QString::number(viewAreaId)) ),
	  KXMLGUIClient()
{
	m_dcopID = 0;
	m_viewAreaId = viewAreaId;
	m_pDocument = document;
	p_ktechlab = document->ktechlab();
	p_viewContainer = viewContainer;
	m_pViewIface = 0;

// intentional assignment. 
	if ( ViewArea *viewArea = viewContainer->viewArea(viewAreaId) )
		viewArea->setView(this);
	else	kdDebug() << k_funcinfo << " viewArea = " << viewArea <<endl;
	
	b_isFocused = false;
	m_layout = new QVBoxLayout(this);
	
	// Don't bother creating statusbar if no ktechlab as we are not a main ktechlab tab
	if (p_ktechlab)
	{
		m_statusBar = new ViewStatusBar(this);
	
		m_layout->addWidget( new KVSSBSep(this) );
		m_layout->addWidget( m_statusBar );
	
		connect( p_ktechlab, SIGNAL(configurationChanged()), this, SLOT(slotUpdateConfiguration()) );
	}
}


View::~View()
{
	if (p_ktechlab)
		p_ktechlab->factory()->removeClient(this);
}


KAction * View::action( const QString & name ) const
{
	KAction * action = actionCollection()->action(name);
	if ( !action )
		kdError() << k_funcinfo << "No such action: " << name << endl;
	return action;
}


DCOPObject * View::dcopObject( ) const
{
	return m_pViewIface;
}


bool View::closeView()
{
	return p_viewContainer->closeViewArea( viewAreaId() );
}


void View::setFocused()
{
	b_isFocused = true;
	p_viewContainer->setActiveViewArea( viewAreaId() );
	
	if ( p_ktechlab )
	{
		p_ktechlab->action("file_save")->setEnabled(true);
		p_ktechlab->action("file_save_as")->setEnabled(true);
		p_ktechlab->action("file_close")->setEnabled(true);
		p_ktechlab->action("file_print")->setEnabled(true);
		p_ktechlab->action("edit_paste")->setEnabled(true);
		p_ktechlab->action("view_split_leftright")->setEnabled(true);
		p_ktechlab->action("view_split_topbottom")->setEnabled(true);
		
		ItemInterface::self()->updateItemActions();
	}
	
	emit viewFocused(this);
}


void View::setUnfocused()
{
	b_isFocused = false;
	emit viewUnfocused();
}


void View::setDCOPID( unsigned id )
{
	if ( m_dcopID == id )
		return;
	
	m_dcopID = id;
	if ( m_pViewIface )
	{
		QCString docID;
		docID.setNum( document()->dcopID() );
		
		QCString viewID;
		viewID.setNum( dcopID() );
		
		m_pViewIface->setObjId( "View#" + docID + "." + viewID );
	}
}
//END class View



//BEGIN class ViewStatusBar
ViewStatusBar::ViewStatusBar( View *view )
	: KStatusBar(view)
{
	p_view = view;
	
	m_modifiedLabel = new QLabel(this);
	addWidget( m_modifiedLabel, 0, false );
	m_fileNameLabel = new KSqueezedTextLabel(this);
	addWidget( m_fileNameLabel, 1, false );
	
	m_modifiedPixmap = KGlobal::iconLoader()->loadIcon( "filesave", KIcon::Small );
	m_unmodifiedPixmap = KGlobal::iconLoader()->loadIcon( "null", KIcon::Small );
	
	connect( view->document(), SIGNAL(modifiedStateChanged()), this, SLOT(slotModifiedStateChanged()) );
	connect( view->document(), SIGNAL(fileNameChanged(const KURL& )), this, SLOT(slotFileNameChanged(const KURL& )) );
	
	connect( view, SIGNAL(viewFocused(View* )), this, SLOT(slotViewFocused(View* )) );
	connect( view, SIGNAL(viewUnfocused()), this, SLOT(slotViewUnfocused()) );
	
	slotModifiedStateChanged();
	slotFileNameChanged( view->document()->url() );
}


void ViewStatusBar::slotModifiedStateChanged()
{
	m_modifiedLabel->setPixmap( p_view->document()->isModified() ? m_modifiedPixmap : m_unmodifiedPixmap );
}


void ViewStatusBar::slotFileNameChanged( const KURL &url )
{
	m_fileNameLabel->setText( url.isEmpty() ? i18n("Untitled") : url.fileName(true) );
}


void ViewStatusBar::slotViewFocused( View * )
{
	setPalette(p_view->palette());
}


void ViewStatusBar::slotViewUnfocused()
{
	QPalette pal( p_view->palette() );
	pal.setColor( QColorGroup::Background, pal.active().mid() );
	pal.setColor( QColorGroup::Light, pal.active().midlight() );
	setPalette(pal);
}
//END class ViewStatusBar

#include "view.moc"
