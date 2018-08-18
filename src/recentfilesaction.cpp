/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "recentfilesaction.h"

#include <kconfig.h>
#include <kmenu.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kglobal.h>
#include <ksharedconfig.h>

RecentFilesAction::RecentFilesAction( const QString & configGroupName, const QString& text, const QObject* receiver, const char* slot, QObject* parent, const char* name )
	: KSelectAction( text /*, 0*/ /*pix*/, parent /*, name */ )
{
    setObjectName(name);

	m_configGroupName = configGroupName;
	m_maxItems = 10;
	
	m_popup = new KMenu;
	connect(m_popup, SIGNAL(aboutToShow()), this, SLOT(menuAboutToShow()));
	connect(m_popup, SIGNAL(activated(int)), this, SLOT(menuItemActivated(int)));
	connect( this, SIGNAL(triggered(const QString&)),
			 this, SLOT( itemSelected( const QString& ) ) );

	setMenuAccelsEnabled( false );
	
	if ( receiver )
		connect( this, SIGNAL(urlSelected(const KUrl &)), receiver, slot );
}


RecentFilesAction::~RecentFilesAction()
{
	delete m_popup;
}

void RecentFilesAction::addURL( const KUrl& url )
{
	if ( url.isLocalFile() && !KGlobal::dirs()->relativeLocation("tmp", url.path()).startsWith("/"))
		return;
	
	QString file;
	if ( url.isLocalFile() && url.ref().isNull() && url.query().isNull() )
		file = url.path();
	else
		file = url.prettyUrl();
	
	QStringList lst = items();

    // remove file if already in list
	lst.remove( file );

    // remove last item if already maxitems in list
	if( lst.count() == m_maxItems )
	{
        // remove last item
		lst.remove( lst.last() );
	}

    // add file to list
	lst.prepend( file );
	setItems( lst );
	
	saveEntries();
}


void RecentFilesAction::loadEntries()
{
	KConfig * config = KGlobal::config().data();
	
	QString     key;
	QString     value;
	//QString     oldGroup;
	QStringList lst;

	//oldGroup = config->group();

	KConfigGroup grCfg = config->group( m_configGroupName );

    // read file list
	for( unsigned int i = 1 ; i <= m_maxItems ; i++ )
	{
		key = QString( "File%1" ).arg( i );
		value = grCfg.readEntry( key, "");

		if (!value.isNull())
			lst.append( value );
	}

    // set file
	setItems( lst );

	//config->setGroup( oldGroup );
}

void RecentFilesAction::saveEntries()
{
	KConfig * config = KGlobal::config().data();
	
	QString     key;
	QString     value;
	//QString     oldGroup;
	QStringList lst = items();

	//oldGroup = config->group();

	config->deleteGroup( m_configGroupName /*, true */ );
	KConfigGroup grCfg = config->group( m_configGroupName );

    // write file list
	for( unsigned int i = 1 ; i <= lst.count() ; i++ )
	{
		key = QString( "File%1" ).arg( i );
		value = lst[ i - 1 ];
		grCfg.writePathEntry( key, value );
	}

	//grCfg.setGroup( oldGroup );
	
	config->sync();
}

void RecentFilesAction::itemSelected( const QString& text )
{
	emit urlSelected( KUrl( text ) );
}

void RecentFilesAction::menuItemActivated( int id )
{
	emit urlSelected( KUrl(m_popup->text(id)) );
}

void RecentFilesAction::menuAboutToShow()
{
	KMenu *menu = m_popup;
	menu->clear();
	QStringList list = items();
	QStringList::iterator end = list.end();
	for ( QStringList::Iterator it = list.begin(); it != end; ++it )
		menu->insertItem(*it);
}

void RecentFilesAction::slotClicked()
{
	//KAction::slotActivated(); // TODO CORRECT?
    KAction::trigger();
}

void RecentFilesAction::slotActivated(const QString& text)
{
	//KSelectAction::slotActivated(text); // TODO CORRECT?
    KSelectAction::activate( QAction::Trigger);
}


void RecentFilesAction::slotActivated(int id)
{
	//KSelectAction::slotActivated(id);
    KSelectAction::activate(QAction::Trigger); // TODO correct?
}


void RecentFilesAction::slotActivated()
{
	emit activated( currentItem() );
	//emit activated( currentText() ); // TODO how should this work?
}


#include "recentfilesaction.moc"
