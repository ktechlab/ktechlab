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

#include <qapplication.h>


#include <cassert>

//BEGIN class View
View::View( Document *document, ViewContainer *viewContainer, uint viewAreaId, const char *name )
	: QWidget( viewContainer->viewArea(viewAreaId), name ? name : (const char *)("view_" + QString::number(viewAreaId)) ),
	  KXMLGUIClient()
{
	m_pFocusWidget = 0l;
	m_dcopID = 0;
	m_viewAreaId = viewAreaId;
	m_pDocument = document;
	p_viewContainer = viewContainer;
	m_pViewIface = 0l;
	
	setFocusPolicy( ClickFocus );
	
	if ( ViewArea * viewArea = viewContainer->viewArea(viewAreaId) )
		viewArea->setView(this);
	
	else
		kdDebug() << k_funcinfo << " viewArea = " << viewArea <<endl;
	
	m_layout = new QVBoxLayout(this);
	
	// Don't bother creating statusbar if no ktechlab as we are not a main ktechlab tab
	if ( KTechlab::self() )
	{
		m_statusBar = new ViewStatusBar(this);
	
		m_layout->addWidget( new KVSSBSep(this) );
		m_layout->addWidget( m_statusBar );
	
		connect( KTechlab::self(), SIGNAL(configurationChanged()), this, SLOT(slotUpdateConfiguration()) );
	}
}


View::~View()
{
	if ( KTechlab::self() )
		KTechlab::self()->factory()->removeClient(this);
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


void View::setFocusWidget( QWidget * focusWidget )
{
	assert( focusWidget );
	assert( !m_pFocusWidget );
	
	if ( hasFocus() )
		focusWidget->setFocus();
	
	m_pFocusWidget = focusWidget;
	setFocusProxy( m_pFocusWidget );
	m_pFocusWidget->installEventFilter( this );
	m_pFocusWidget->setFocusPolicy( ClickFocus );
}


bool View::eventFilter( QObject * watched, QEvent * e )
{
// 	kdDebug() << k_funcinfo << e->type() << endl;
	
	if ( watched != m_pFocusWidget )
		return false;
	
	switch ( e->type() )
	{
		case QEvent::FocusIn:
		{
			p_viewContainer->setActiveViewArea( viewAreaId() );
	
			if ( KTechlab * ktl = KTechlab::self() )
			{
				ktl->action("file_save")->setEnabled(true);
				ktl->action("file_save_as")->setEnabled(true);
				ktl->action("file_close")->setEnabled(true);
				ktl->action("file_print")->setEnabled(true);
				ktl->action("edit_paste")->setEnabled(true);
				ktl->action("view_split_leftright")->setEnabled(true);
				ktl->action("view_split_topbottom")->setEnabled(true);
		
				ItemInterface::self()->updateItemActions();
			}
	
// 			kdDebug() << k_funcinfo << "Focused In\n";
			emit focused(this);
			break;
		}
		
		case QEvent::FocusOut:
		{
// 			kdDebug() << k_funcinfo << "Focused Out.\n";
			
			if ( QWidget * fw = qApp->focusWidget() )
			{
				QString fwClassName( fw->className() );
// 				kdDebug() << "New focus widget is \""<<fw->name()<<"\" of type " << fwClassName << endl;
				
				if ( (fwClassName != "KateViewInternal") &&
								  (fwClassName != "QViewportWidget") )
				{
// 					kdDebug() << "Returning as a non-view widget has focus.\n";
					break;
				}
			}
			else
			{
// 				kdDebug() << "No widget currently has focus.\n";
			}
			
			if ( QFocusEvent::reason() == QFocusEvent::Popup )
			{
// 				kdDebug() << k_funcinfo << "Ignoring focus-out event as was a popup.\n";
				break;
			}
			
			if ( QFocusEvent::reason() == QFocusEvent::ActiveWindow )
			{
// 				kdDebug() << k_funcinfo << "Ignoring focus-out event as main window lost focus.\n";
				break;
			}
			
			emit unfocused();
			break;
		}
		
		default:
			break;
	}
	
	return false;
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
	
	connect( view, SIGNAL(focused(View* )), this, SLOT(slotViewFocused(View* )) );
	connect( view, SIGNAL(unfocused()), this, SLOT(slotViewUnfocused()) );
	
	slotModifiedStateChanged();
	slotFileNameChanged( view->document()->url() );
	
	slotViewUnfocused();
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
