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

#include <qdebug.h>
#include <kiconloader.h>
#include <klocalizedstring.h>
#include <ksqueezedtextlabel.h>
#include <kxmlguifactory.h>
#include <kactioncollection.h>

#include <qapplication.h>
#include <qboxlayout.h>

#include <cassert>

//BEGIN class View
View::View( Document *document, ViewContainer *viewContainer, uint viewAreaId, const char *name )
	: QWidget( viewContainer->viewArea(viewAreaId)
        /*, name ? name : ("view_" + QString::number(viewAreaId)).toLatin1().data() */ ),
	  KXMLGUIClient()
{
    setObjectName(name ? name : ("view_" + QString::number(viewAreaId)).toLatin1().data());
	m_pFocusWidget = nullptr;
	m_dcopID = 0;
	m_viewAreaId = viewAreaId;
	m_pDocument = document;
	p_viewContainer = viewContainer;
	m_pViewIface = nullptr;
	
	setFocusPolicy( Qt::ClickFocus );
	
	if ( ViewArea * viewArea = viewContainer->viewArea(viewAreaId) )
		viewArea->setView(this);
	
	else
		qDebug() << Q_FUNC_INFO << " viewArea = " << viewArea <<endl;
	
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
	//if ( KTechlab::self() )   // 2015.09.13 - remove the XMLGUIClient from the factory, even at program close
	//	KTechlab::self()->factory()->removeClient(this);
    // 2017.01.09: do not crash on document close. factory has its clients removed in TextDocument::~TextDocument()
    //if ( factory() ) {
    //    factory()->removeClient( this );
    //} else {
    //    qWarning() << Q_FUNC_INFO << "Null factory";
    //}
}


QAction* View::actionByName( const QString& name ) const
{
	QAction * action = actionCollection()->action(name);
	if ( !action )
		qCritical() << Q_FUNC_INFO << "No such action: " << name << endl;
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
	m_pFocusWidget->setFocusPolicy( Qt::ClickFocus );
}


bool View::eventFilter( QObject * watched, QEvent * e )
{
// 	qDebug() << Q_FUNC_INFO << e->type() << endl;
	
	if ( watched != m_pFocusWidget )
		return false;
	
	switch ( e->type() )
	{
		case QEvent::FocusIn:
		{
			p_viewContainer->setActiveViewArea( viewAreaId() );
	
			if ( KTechlab * ktl = KTechlab::self() )
			{
				ktl->actionByName("file_save")->setEnabled(true);
				ktl->actionByName("file_save_as")->setEnabled(true);
				ktl->actionByName("file_close")->setEnabled(true);
				ktl->actionByName("file_print")->setEnabled(true);
				ktl->actionByName("edit_paste")->setEnabled(true);
				ktl->actionByName("view_split_leftright")->setEnabled(true);
				ktl->actionByName("view_split_topbottom")->setEnabled(true);
		
				ItemInterface::self()->updateItemActions();
			}
	
// 			qDebug() << Q_FUNC_INFO << "Focused In\n";
			emit focused(this);
			break;
		}
		
		case QEvent::FocusOut:
		{
// 			qDebug() << Q_FUNC_INFO << "Focused Out.\n";
            QFocusEvent *fe = static_cast<QFocusEvent*>(e);
			
			if ( QWidget * fw = qApp->focusWidget() )
			{
				QString fwClassName( fw->metaObject()->className() );
// 				qDebug() << "New focus widget is \""<<fw->name()<<"\" of type " << fwClassName << endl;
				
				if ( (fwClassName != "KateViewInternal") &&
								  (fwClassName != "QViewportWidget") )
				{
// 					qDebug() << "Returning as a non-view widget has focus.\n";
					break;
				}
			}
			else
			{
// 				qDebug() << "No widget currently has focus.\n";
			}
			
			if ( fe->reason() == Qt::PopupFocusReason )
			{
// 				qDebug() << Q_FUNC_INFO << "Ignoring focus-out event as was a popup.\n";
				break;
			}
			
			if ( fe->reason() == Qt::ActiveWindowFocusReason )
			{
// 				qDebug() << Q_FUNC_INFO << "Ignoring focus-out event as main window lost focus.\n";
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
		QString docID;
		docID.setNum( document()->dcopID() );
		
		QString viewID;
		viewID.setNum( dcopID() );
		
		m_pViewIface->setObjId( "View#" + docID + "." + viewID );
	}
}
//END class View



//BEGIN class ViewStatusBar
ViewStatusBar::ViewStatusBar( View *view )
	: QStatusBar(view)
{
	p_view = view;
	
	m_modifiedLabel = new QLabel(this);
	addWidget( m_modifiedLabel, 0 /*, false */ );
	m_fileNameLabel = new KSqueezedTextLabel(this);
	addWidget( m_fileNameLabel, 1 /*, false */ );
    m_statusLabel = new QLabel(this);
    m_statusLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    addPermanentWidget(m_statusLabel, 0);
	
	m_modifiedPixmap = KIconLoader::global()->loadIcon( "document-save", KIconLoader::Small );
	m_unmodifiedPixmap = KIconLoader::global()->loadIcon( "null", KIconLoader::Small );
	
	connect( view->document(), SIGNAL(modifiedStateChanged()), this, SLOT(slotModifiedStateChanged()) );
	connect( view->document(), SIGNAL(fileNameChanged(const KUrl& )), this, SLOT(slotFileNameChanged(const KUrl& )) );
	
	connect( view, SIGNAL(focused(View* )), this, SLOT(slotViewFocused(View* )) );
	connect( view, SIGNAL(unfocused()), this, SLOT(slotViewUnfocused()) );
	
	slotModifiedStateChanged();
	slotFileNameChanged( view->document()->url() );
	
	slotViewUnfocused();
}

void ViewStatusBar::setStatusText(const QString &statusText)
{
    m_statusLabel->setText(statusText);
}


void ViewStatusBar::slotModifiedStateChanged()
{
	m_modifiedLabel->setPixmap( p_view->document()->isModified() ? m_modifiedPixmap : m_unmodifiedPixmap );
}


void ViewStatusBar::slotFileNameChanged( const KUrl &url )
{
	m_fileNameLabel->setText( url.isEmpty() ? i18n("Untitled") : url.fileName(KUrl::IgnoreTrailingSlash) );
}


void ViewStatusBar::slotViewFocused( View * )
{
	setPalette(p_view->palette());
}


void ViewStatusBar::slotViewUnfocused()
{
	QPalette pal( p_view->palette() );
	pal.setColor( QPalette::Window /*QColorGroup::Background */ , pal.mid().color() );
	pal.setColor( QPalette::Light, pal.midlight().color() );
	setPalette(pal);
}
//END class ViewStatusBar

//BEGIN class KVSSBSep
void KVSSBSep::paintEvent( QPaintEvent *e )
{
    //QPainter p( this );
    QPainter p;
    const bool beginSuccess = p.begin( this );
    if (!beginSuccess) {
        qWarning() << Q_FUNC_INFO << " painter is not active";
    }
    //p.setPen( colorGroup().shadow() );
    //QColorGroup colorGroup(palette()); // 2018.12.02
    p.setPen( palette().shadow().color() /* colorGroup.shadow() */ );
    p.drawLine( e->rect().left(), 0, e->rect().right(), 0 );
    //p.setPen( ((View*)parentWidget())->hasFocus() ? colorGroup.light() : colorGroup.midlight() );
    View * parentView = dynamic_cast<View*>(parentWidget());
    if (!parentView) {
        qWarning() << "parent not View for this=" << this << ", parent=" << parentWidget();
        return;
    }
    p.setPen( parentView->hasFocus() ? palette().light().color() : palette().midlight().color() );
    p.drawLine( e->rect().left(), 1, e->rect().right(), 1 );
}
//END  class KVSSBSep
