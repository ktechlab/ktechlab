/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "circuitdocument.h"
#include "ecsubcircuit.h"
#include "itemdocumentdata.h"
#include "itemlibrary.h"
#include "itemselector.h"
#include "subcircuits.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <qfile.h>
#include <q3textstream.h>
//Added by qt3to4:
#include <Q3ValueList>

Subcircuits::Subcircuits()
	: QObject()
{
	connect( ComponentSelector::self(), SIGNAL(itemRemoved(const QString& )), this, SLOT(slotItemRemoved(const QString& )) );
}


Subcircuits::~Subcircuits()
{
}


void Subcircuits::initECSubcircuit( int subcircuitId, ECSubcircuit *ecSubcircuit )
{
	const QString fileName = genFileName(subcircuitId);
	if ( !QFile::exists(fileName) )
	{
		kdDebug() << "Subcircuits::createSubcircuit: Subcircuit \""<<fileName<<"\" was not found."<<endl;
		return;
	}
	
	SubcircuitData subcircuit;
	if (!subcircuit.loadData( genFileName(subcircuitId) ) )
		return;
	
	subcircuit.initECSubcircuit(ecSubcircuit);
}


ECSubcircuit* Subcircuits::createSubcircuit( int id, CircuitDocument *circuitDocument, bool newItem, const char *newId )
{
	//I pass finishCreation = false here because the subcircuit was getting
	//finished twice, causing a segfault in CircuitDocument::assignCircuits()
	//--electronerd
	ECSubcircuit *ecSubcircuit = static_cast<ECSubcircuit*>(itemLibrary()->createItem( "ec/subcircuit", circuitDocument, newItem, newId, false ));
	ecSubcircuit->property("id")->setValue(id);
	return ecSubcircuit;
}


void Subcircuits::loadSubcircuits()
{
	KConfig *config = kapp->config();
	config->setGroup("Subcircuits");
	
	Q3ValueList<int> idList = config->readIntListEntry("Ids");
	const Q3ValueList<int>::iterator idListEnd = idList.end();
	for ( Q3ValueList<int>::iterator it = idList.begin(); it != idListEnd; ++it )
	{
		QFile file( genFileName(*it) );
		if ( file.open(QIODevice::ReadOnly) == false )
		{
			// File has mysteriously disappeared....
			*it = -1;
		}
		else
		{
			config->setGroup("Subcircuit_"+QString::number(*it));
			updateComponentSelector( *it, config->readEntry("Name") );
		}
		file.close();
	}
	idList.remove(-1);
	
	// Update the config file if any ids have been removed
	config->setGroup("Subcircuits");
	config->writeEntry( "Ids", idList );
}


QString Subcircuits::genFileName( const int nextId )
{
	return locateLocal( "appdata", "subcircuit_"+QString::number(nextId)+".circuit" );
}


void Subcircuits::updateComponentSelector( int id, const QString &name )
{
	if ( name.isEmpty() )
		return;
	
	ComponentSelector::self()->addItem( name, "sc/"+QString::number(id), i18n("Subcircuits"), KGlobal::iconLoader()->loadIcon( "ktechlab_circuit", KIcon::Small ), true );
}


void Subcircuits::addSubcircuit( const QString &name, const QString &subcircuitXml )
{
	KConfig *config = kapp->config();
	config->setGroup("Subcircuits");
	
	int nextId = config->readNumEntry( "NextId", 0 );
	
	while ( QFile::exists( genFileName(nextId) ) ) {
		nextId++;
	}
	
	const int id = nextId;
	
	const QString fileName = genFileName(id);
	QFile file(fileName);
	
	if ( file.open(QIODevice::WriteOnly) == false )
	{
		kdError() << "Subcircuits::addSubcircuit: couldn't open subcircuit save file: "<<fileName<<endl;
		return;
	}
	
	Q3TextStream stream(&file);
	stream << subcircuitXml;
	file.close();
	
	Q3ValueList<int> idList = config->readIntListEntry("Ids");
	idList += id;
	config->writeEntry( "Ids", idList );
	config->writeEntry( "NextId", ++nextId );
	
	config->setGroup("Subcircuit_"+QString::number(id));
	config->writeEntry( "Name", name );
	
	// It's important that we write the configuration *now*, lest the subcircuits be lost
	config->sync();
	
	updateComponentSelector( id, name );
}


void Subcircuits::slotItemRemoved( const QString &id )
{
	if ( !id.startsWith("sc/") ) {
		return;
	}
	
	QString temp = id;
	temp.remove("sc/");
	const int id_num = temp.toInt();
	const QString fileName = genFileName(id_num);
	QFile file(fileName);
	file.remove();
	
	KConfig *config = kapp->config();
	config->setGroup("Subcircuits");
	Q3ValueList<int> idList = config->readIntListEntry("Ids");
	idList.remove(id_num);
	config->writeEntry( "Ids", idList );
}


#include "subcircuits.moc"


