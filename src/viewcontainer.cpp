/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "docmanager.h"
#include "document.h"
#include "ktechlab.h"
#include "view.h"
#include "viewcontainer.h"

#include <kconfig.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <ktabwidget.h>
#include <qobjectlist.h>

ViewContainer::ViewContainer( const QString & caption, KTechlab * ktechlab, QWidget * parent )
	: QWidget( ktechlab ? ktechlab->tabWidget() : parent )
{
	b_deleted = false;
	p_ktechlab = ktechlab;
	if (ktechlab)
		connect( ktechlab, SIGNAL(needUpdateCaptions()), this, SLOT(updateCaption()) );
	
	QHBoxLayout *layout = new QHBoxLayout(this);
	m_baseViewArea = new ViewArea( this, this, 0, "viewarea_0" );
	connect( m_baseViewArea, SIGNAL(destroyed(QObject* )), this, SLOT(baseViewAreaDestroyed(QObject* )) );
	
	layout->addWidget(m_baseViewArea);
	
	m_activeViewArea = 0;
	b_focused = false;
	
	if (ktechlab)
		ktechlab->tabWidget()->addTab( this, caption );
	
	show();
	
	if (ktechlab)
		ktechlab->tabWidget()->setCurrentPage( ktechlab->tabWidget()->indexOf(this) );
}


ViewContainer::~ViewContainer()
{
	b_deleted = true;
}


void ViewContainer::setFocused()
{
	if (b_focused)
		return;
	b_focused = true;
	
	View *view = activeView();
	if (view)
		view->setFocused();
}


void ViewContainer::setUnfocused()
{
	if (!b_focused)
		return;
	b_focused = false;
	
	View *view = activeView();
	if (view)
		view->setUnfocused();
}


void ViewContainer::setActiveViewArea( uint id )
{
	if ( m_activeViewArea == int(id) )
		return;
	
	View *oldView = view(m_activeViewArea);
	if (oldView)
		oldView->setUnfocused();
	
	m_activeViewArea = id;
	
	ViewArea *va = viewArea(id);
	if ( va && b_focused )
		va->setFocus();
	
	View *newView = view(id);
	if ( newView && b_focused );
	{
		if (newView)
		{
			setCaption( newView->caption() );
			newView->setFocused();
		}
	}
}


View *ViewContainer::view( uint id ) const
{
	ViewArea *va = viewArea(id);
	if (!va) return 0;

	// We do not want a recursive search as ViewAreas also hold other ViewAreas
	QObjectList *l = va->queryList( "View", 0, false, false );
	View *view = 0;
	if ( !l->isEmpty() )
		view = dynamic_cast<View*>(l->first());
	delete l;

	return view;
}


ViewArea *ViewContainer::viewArea( uint id ) const
{
	if ( !m_viewAreaMap.contains(id) )
		return 0;
	
	return m_viewAreaMap[id];
}


bool ViewContainer::closeViewContainer()
{
	bool didClose = true;
	while ( didClose && !m_viewAreaMap.isEmpty() )
	{
		didClose = closeViewArea( m_viewAreaMap.begin().key() );
	}
	
	return m_viewAreaMap.isEmpty();
}


bool ViewContainer::closeViewArea( uint id )
{
	ViewArea *va = viewArea(id);
	if ( !va )
		return true;
	
	bool doClose = false;
	View *v = view(id);
	if ( v && v->document() )
	{
		doClose = v->document()->numberOfViews() > 1;
		if (!doClose)
			doClose = v->document()->fileClose();
	}
	else
		doClose = true;
	
	if (!doClose)
		return false;
	
	m_viewAreaMap.remove(id);
	va->deleteLater();
	
	if ( m_activeViewArea == int(id) )
	{
		m_activeViewArea = -1;
		findActiveViewArea();
	}
	
	return true;
}


int ViewContainer::createViewArea( int relativeViewArea, ViewArea::Position position )
{
	if ( relativeViewArea == -1 )
		relativeViewArea = activeViewArea();
	
	ViewArea *relative = viewArea(relativeViewArea);
	if (!relative)
	{
		kdError() << k_funcinfo << "Could not find relative view area" << endl;
		return -1;
	}
	
	uint id = uniqueNewId();
// 	setActiveViewArea(id);
	
	ViewArea *viewArea = relative->createViewArea( position, id );
// 	ViewArea *viewArea = new ViewArea( m_splitter, id, (const char*)("viewarea_"+QString::number(id)) );
	viewArea->show(); // remove?
	
	return id;
}


void ViewContainer::setViewAreaId( ViewArea *viewArea, uint id )
{
	m_viewAreaMap[id] = viewArea;
	m_usedIDs.append(id);
}


void ViewContainer::setViewAreaRemoved( uint id )
{
	if (b_deleted)
		return;
	
	ViewAreaMap::iterator it = m_viewAreaMap.find(id);
	if ( it == m_viewAreaMap.end() )
		return;
	
	m_viewAreaMap.erase(it);
	
	if ( m_activeViewArea == int(id) )
		findActiveViewArea();
}


void ViewContainer::findActiveViewArea()
{
	if ( m_viewAreaMap.isEmpty() )
		return;
	
	setActiveViewArea( (--m_viewAreaMap.end()).key() );
}


void ViewContainer::baseViewAreaDestroyed( QObject *obj )
{
	if (!obj)
		return;
	
	if (!b_deleted)
	{
		b_deleted = true;
		close();
		deleteLater();
	}
}


ViewContainer * ViewContainer::duplicateViewContainer()
{
	ViewContainer *viewContainer = new ViewContainer( caption(), p_ktechlab );
	copyViewContainerIntoExisting(viewContainer);
	
	p_ktechlab->addWindow(viewContainer);
	
	return viewContainer;
}


void ViewContainer::copyViewContainerIntoExisting( ViewContainer *viewContainer )
{
	if (!viewContainer)
		return;
	
	const ViewAreaMap::iterator end = m_viewAreaMap.end();
	for ( ViewAreaMap::iterator it = m_viewAreaMap.begin(); it != end; ++it )
	{
		View *oldView = view(it.key());
		if (!oldView)
			continue;
			
		// See if there is an empty view container to be inserted into with id 0...
		uint newId;
		if ( viewContainer->view(0) )
			newId = viewContainer->createViewArea( 0, ViewArea::Right );
		else
			newId = 0;
				
		oldView->document()->createView( viewContainer, newId );
	}
}


bool ViewContainer::canSaveUsefulStateInfo() const
{
	return m_baseViewArea && m_baseViewArea->canSaveUsefulStateInfo();
}


void ViewContainer::saveState( KConfig *config )
{
	if (!m_baseViewArea)
		return;
	
	config->writeEntry( "BaseViewArea", m_baseViewArea->id() );
	m_baseViewArea->saveState(config);
}


void ViewContainer::restoreState( KConfig *config, const QString &groupName )
{
	config->setGroup(groupName);
	int baseAreaId = config->readNumEntry("BaseViewArea");
	m_baseViewArea->restoreState( config, baseAreaId, groupName );
}


int ViewContainer::uniqueParentId()
{
	int lowest = -1;
	const IntList::iterator end = m_usedIDs.end();
	for ( IntList::iterator it = m_usedIDs.begin(); it != end; ++it )
	{
		if ( *it < lowest )
			lowest = *it;
	}
	int newId = lowest-1;
	m_usedIDs.append(newId);
	return newId;
}


int ViewContainer::uniqueNewId()
{
	int highest = 0;
	const IntList::iterator end = m_usedIDs.end();
	for ( IntList::iterator it = m_usedIDs.begin(); it != end; ++it )
	{
		if ( *it > highest )
			highest = *it;
	}
	int newId = highest+1;
	m_usedIDs.append(newId);
	return newId;
}


void ViewContainer::setIdUsed( int id )
{
	m_usedIDs.append(id);
}


void ViewContainer::updateCaption()
{
	QString caption;
	
	if ( !activeView() || !activeView()->document() )
		caption = i18n("(empty)");
	
	else
	{
		Document * doc = activeView()->document();
		caption = doc->url().isEmpty() ? doc->caption() : doc->url().fileName();
		if ( viewCount() > 1 )
			caption += " ...";
	}
	
	setCaption(caption);
	p_ktechlab->tabWidget()->setTabLabel( this, caption );
}


void ViewContainer::setKTechlabDeleted()
{
	p_ktechlab = 0;
	ViewAreaMap::iterator end = m_viewAreaMap.end();
	for ( ViewAreaMap::iterator it = m_viewAreaMap.begin(); it != end; ++it )
	{
		if ( *it )
			(*it)->setKTechlabDeleted();
	}
}





ViewArea::ViewArea( QWidget *parent, ViewContainer *viewContainer, int id, const char *name )
	: QSplitter( parent, name )
{
	p_viewContainer = viewContainer;
	m_id = id;
	p_view = 0;
	p_viewArea1 = 0;
	p_viewArea2 = 0;
	if (id >= 0)
		p_viewContainer->setViewAreaId( this, uint(id) );
	p_viewContainer->setIdUsed(id);
	setOpaqueResize(KGlobalSettings::opaqueResize());
}


ViewArea::~ViewArea()
{
	if ( m_id >= 0 )
		p_viewContainer->setViewAreaRemoved( uint(m_id) );
}


ViewArea *ViewArea::createViewArea( Position position, uint id )
{
	if (p_viewArea1 || p_viewArea2)
	{
		kdError() << k_funcinfo << "Attempting to create ViewArea when already containing ViewAreas!" << endl;
		return 0;
	}
	if (!p_view)
	{
		kdError() << k_funcinfo << "We don't have a view yet, so creating a new ViewArea is redundant" << endl;
		return 0;
	}
	
	setOrientation( ( position == Right ) ? Qt::Horizontal : Qt::Vertical );
	
	p_viewArea1 = new ViewArea( this, p_viewContainer, m_id, (const char*)("viewarea_"+QString::number(m_id)) );
	p_viewArea2 = new ViewArea( this, p_viewContainer, id, (const char*)("viewarea_"+QString::number(id)) );
	
	connect( p_viewArea1, SIGNAL(destroyed(QObject* )), this, SLOT(viewAreaDestroyed(QObject* )) );
	connect( p_viewArea2, SIGNAL(destroyed(QObject* )), this, SLOT(viewAreaDestroyed(QObject* )) );
	
	p_view->reparent( p_viewArea1, QPoint(), true );
	p_viewArea1->setView(p_view);
	p_view = 0;
	m_id = p_viewContainer->uniqueParentId();
	
	QValueList<int> splitPos;
	int pos = ((orientation() == Qt::Horizontal) ? width()/2 : height()/2);
	splitPos << pos << pos;
	setSizes(splitPos);
	
	p_viewArea1->show();
	p_viewArea2->show();
	return p_viewArea2;
}


void ViewArea::viewAreaDestroyed( QObject *obj )
{
	ViewArea *viewArea = static_cast<ViewArea*>(obj);
	
	if ( viewArea == p_viewArea1 )
		p_viewArea1 = 0;
	
	if ( viewArea == p_viewArea2 )
		p_viewArea2 = 0;
	
	if ( !p_viewArea1 && !p_viewArea2 )
		deleteLater();
}


void ViewArea::setView( View *view )
{
	if (!view)
		return;
	if (p_view)
	{
		kdError() << k_funcinfo << "Attempting to set already contained view!" << endl;
		return;
	}
	p_view = view;
	connect( view, SIGNAL(destroyed()), this, SLOT(viewDestroyed()) );
}


void ViewArea::viewDestroyed()
{
	if ( !p_view && !p_viewArea1 && !p_viewArea2 )
		deleteLater();
}


void ViewArea::setKTechlabDeleted()
{
	if ( p_view )
		p_view->setKTechlabDeleted();
}


bool ViewArea::canSaveUsefulStateInfo() const
{
	if ( p_viewArea1 && p_viewArea1->canSaveUsefulStateInfo() )
		return true;
	
	if ( p_viewArea2 && p_viewArea2->canSaveUsefulStateInfo() )
		return true;
	
	if ( p_view && p_view->document() && !p_view->document()->url().isEmpty() )
		return true;
	
	return false;
}


void ViewArea::saveState( KConfig *config )
{
	bool va1Ok = p_viewArea1 && p_viewArea1->canSaveUsefulStateInfo();
	bool va2Ok = p_viewArea2 && p_viewArea2->canSaveUsefulStateInfo();
	
	if ( va1Ok || va2Ok )
	{
		config->writeEntry( orientationKey(m_id), (orientation() == Qt::Horizontal) ? "LeftRight" : "TopBottom" );
		
		QValueList<int> contains;
		if (va1Ok)
			contains << p_viewArea1->id();
		if (va2Ok)
			contains << p_viewArea2->id();
		
		config->writeEntry( containsKey(m_id), contains );
		if (va1Ok)
			p_viewArea1->saveState(config);
		if (va2Ok)
			p_viewArea2->saveState(config);
	}
	else if ( p_view && !p_view->document()->url().isEmpty() )
	{
		config->writePathEntry( fileKey(m_id), p_view->document()->url().prettyURL() );
	}
}


void ViewArea::restoreState( KConfig *config, int id, const QString &groupName )
{
	if (!config)
		return;
	
	if ( id != m_id )
	{
		if ( m_id >= 0 )
			p_viewContainer->setViewAreaRemoved( uint(m_id) );
		
		m_id = id;
		
		if ( m_id >= 0 )
			p_viewContainer->setViewAreaId( this, uint(m_id) );
		
		p_viewContainer->setIdUsed(id);
	}
	
	config->setGroup(groupName);
	if ( config->hasKey( orientationKey(id) ) )
	{
		QString orientation = config->readEntry( orientationKey(m_id) );
		setOrientation( (orientation == "LeftRight") ? Qt::Horizontal : Qt::Vertical );
	}
	
	config->setGroup(groupName);
	if ( config->hasKey( containsKey(m_id) ) )
	{
		typedef QValueList<int> IntList;
		IntList contains = config->readIntListEntry( containsKey(m_id) );
		
		if ( contains.isEmpty() || contains.size() > 2 )
			kdError() << k_funcinfo << "Contained list has wrong size of " << contains.size() << endl;
		
		else
		{
			if ( contains.size() >= 1 )
			{
				int viewArea1Id = contains[0];
				p_viewArea1 = new ViewArea( this, p_viewContainer, viewArea1Id, (const char*)("viewarea_"+QString::number(viewArea1Id)) );
				connect( p_viewArea1, SIGNAL(destroyed(QObject* )), this, SLOT(viewAreaDestroyed(QObject* )) );
				p_viewArea1->restoreState( config, viewArea1Id, groupName );
				p_viewArea1->show();
			}
			
			if ( contains.size() >= 2 )
			{
				int viewArea2Id = contains[1];
				p_viewArea2 = new ViewArea( this, p_viewContainer, viewArea2Id, (const char*)("viewarea_"+QString::number(viewArea2Id)) );
				connect( p_viewArea2, SIGNAL(destroyed(QObject* )), this, SLOT(viewAreaDestroyed(QObject* )) );
				p_viewArea2->restoreState( config, viewArea2Id, groupName );
				p_viewArea2->show();
			}
		}
	}
	
	config->setGroup(groupName);
	if ( config->hasKey( fileKey(m_id) ) )
	{
		bool openedOk = DocManager::self()->openURL( config->readPathEntry( fileKey(m_id) ), this );
		if (!openedOk)
			deleteLater();
	}
}

QString ViewArea::fileKey( int id )
{
	return QString("ViewArea ") + QString::number(id) + QString(" file");
}
QString ViewArea::containsKey( int id )
{
	return QString("ViewArea ") + QString::number(id) + QString(" contains");
}
QString ViewArea::orientationKey( int id )
{
	return QString("ViewArea ") + QString::number(id) + QString(" orientation");
}




ViewContainerDrag::ViewContainerDrag( ViewContainer *viewContainer )
	: QStoredDrag( "dontcare", viewContainer)
{
	p_viewContainer = viewContainer;
}

#include "viewcontainer.moc"
