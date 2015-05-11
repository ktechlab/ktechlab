/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "circuitdocument.h"
#include "connector.h"
#include "cnitem.h"
#include "documentiface.h"
#include "flowcodedocument.h"
#include "itemlibrary.h"
#include "libraryitem.h"
#include "mechanicsdocument.h"
#include "textdocument.h"
#include "view.h"


//BEGIN class DocumentIface
DocumentIface::DocumentIface( Document * document )
	: DCOPObject( /* TODO "Document" */ )
{
	m_pDocument = document;
}


DocumentIface::~DocumentIface()
{
}

void DocumentIface::selectAll( )
{
	m_pDocument->selectAll();
}

void DocumentIface::redo( )
{
	m_pDocument->redo();
}

void DocumentIface::undo( )
{
	m_pDocument->undo();
}

void DocumentIface::paste( )
{
	m_pDocument->paste();
}

void DocumentIface::copy( )
{
	m_pDocument->copy();
}

void DocumentIface::cut( )
{
	m_pDocument->cut();
}

void DocumentIface::print( )
{
	m_pDocument->print();
}


bool DocumentIface::close( )
{
	return m_pDocument->fileClose();
}

void DocumentIface::saveAs( )
{
	m_pDocument->fileSaveAs();
}

void DocumentIface::save( )
{
	m_pDocument->fileSave();
}

bool DocumentIface::isRedoAvailable( )
{
	return m_pDocument->isRedoAvailable();
}

bool DocumentIface::isUndoAvailable( )
{
	return m_pDocument->isUndoAvailable();
}

bool DocumentIface::isModified( )
{
	return m_pDocument->isModified();
}

bool DocumentIface::openURL( const QString & url )
{
	return m_pDocument->openURL(url);
}

QString DocumentIface::url( )
{
	return m_pDocument->url().url();
}

uint DocumentIface::numberOfViews( )
{
	return m_pDocument->numberOfViews();
}

DCOPRef DocumentIface::activeView( )
{
	return viewToRef( m_pDocument->activeView() );
}

QString DocumentIface::caption( ) const
{
	return m_pDocument->caption();
}

DCOPRef DocumentIface::viewToRef( View * view )
{
	return DCOPRef(); //  view->dcopObject() ); TODO
}
//END class DocumentIface



//BEGIN class FlowCodeDocumentIface
FlowCodeDocumentIface::FlowCodeDocumentIface( FlowCodeDocument * document )
	: ICNDocumentIface(document)
{
	m_pFlowCodeDocument = document;
}

void FlowCodeDocumentIface::setPicType( const QString & id )
{
	m_pFlowCodeDocument->setPicType(id);
}

void FlowCodeDocumentIface::convertToMicrobe()		
{
	m_pFlowCodeDocument->convertToMicrobe();
}

void FlowCodeDocumentIface::convertToHex()		
{
	m_pFlowCodeDocument->convertToHex();
}

void FlowCodeDocumentIface::convertToPIC()
{
	m_pFlowCodeDocument->convertToPIC();
}

void FlowCodeDocumentIface::convertToAssembly()
{
	m_pFlowCodeDocument->convertToAssembly();
}
//END class FlowCodeDocumentIface



//BEGIN class CircuitDocumentIface
CircuitDocumentIface::CircuitDocumentIface( CircuitDocument * document )
	: ICNDocumentIface(document)
{
	m_pCircuitDocument = document;
}

void CircuitDocumentIface::setOrientation0( )
{
	m_pCircuitDocument->setOrientation0();
}

void CircuitDocumentIface::setOrientation90( )
{
	m_pCircuitDocument->setOrientation90();
}

void CircuitDocumentIface::setOrientation180( )
{
	m_pCircuitDocument->setOrientation180();
}

void CircuitDocumentIface::setOrientation270( )
{
	m_pCircuitDocument->setOrientation270();
}

void CircuitDocumentIface::rotateCounterClockwise( )
{
	m_pCircuitDocument->rotateCounterClockwise();
}

void CircuitDocumentIface::rotateClockwise( )
{
	m_pCircuitDocument->rotateClockwise();
}

void CircuitDocumentIface::flipHorizontally()
{
	m_pCircuitDocument->flipHorizontally();
}

void CircuitDocumentIface::flipVertically()
{
	m_pCircuitDocument->flipVertically();
}

void CircuitDocumentIface::displayEquations( )
{
	m_pCircuitDocument->displayEquations();
}

void CircuitDocumentIface::createSubcircuit( )
{
	m_pCircuitDocument->createSubcircuit();
}
//END class CircuitDocumentIface



//BEGIN class ICNDocumentIface
ICNDocumentIface::ICNDocumentIface( ICNDocument * document )
	: ItemDocumentIface(document)
{
	m_pICNDocument = document;
}

void ICNDocumentIface::exportToImage( )
{
	m_pICNDocument->exportToImage();
}

QStringList ICNDocumentIface::nodeIDs( const QString & id )
{
	CNItem * item = m_pICNDocument->cnItemWithID(id);
	
	QStringList ids;
	if ( !item )
		return ids;
	
	const NodeInfoMap nm = item->nodeMap();
	const NodeInfoMap::const_iterator end = nm.end();
	for ( NodeInfoMap::const_iterator it = nm.begin(); it != end; ++it )
		ids.append( it.key().ascii() );
	
	return ids;
}

QString ICNDocumentIface::makeConnection( const QString & item1, const QString & node1, const QString & item2, const QString & node2 )
{
	CNItem * i1 = m_pICNDocument->cnItemWithID(item1);
	CNItem * i2 = m_pICNDocument->cnItemWithID(item2);
	
	if ( !i1 || !i2 )
		return QString::null;
	
	Node * n1 = m_pICNDocument->nodeWithID( i1->nodeId(node1) );
	Node * n2 = m_pICNDocument->nodeWithID( i2->nodeId(node2) );
	
	if ( !n1 || !n2 )
		return QString::null;
	
	Connector * connector = m_pICNDocument->createConnector( n1, n2 );
	return connector ? connector->id() : QString::null;
}

void ICNDocumentIface::selectConnector( const QString & id )
{
	m_pICNDocument->select( m_pICNDocument->connectorWithID(id) );
}

void ICNDocumentIface::unselectConnector( const QString & id )
{
	m_pItemDocument->unselect( m_pICNDocument->connectorWithID(id) );
}
//END class ICNDocumentIface



//BEGIN class ItemDocumentIface
ItemDocumentIface::ItemDocumentIface( ItemDocument * document )
	: DocumentIface(document)
{
	m_pItemDocument = document;
}

QStringList ItemDocumentIface::validItemIDs( )
{
	QStringList validIDs;
	
	LibraryItemList * allItems = itemLibrary()->items();
	const LibraryItemList::iterator end = allItems->end();
	for ( LibraryItemList::iterator it = allItems->begin(); it != end; ++it )
	{
		QString id = (*it)->activeID();
		if ( m_pItemDocument->isValidItem(id) )
			validIDs << id.utf8();
	}
	return validIDs;
}

QString ItemDocumentIface::addItem( const QString & id, int x, int y )
{
	Item * item = m_pItemDocument->addItem( id, QPoint( x, y ), true );
	return item ? item->id() : QString::null;
}

void ItemDocumentIface::selectItem( const QString & id )
{
	m_pItemDocument->select( m_pItemDocument->itemWithID(id) );
}

void ItemDocumentIface::unselectItem( const QString & id )
{
	m_pItemDocument->unselect( m_pItemDocument->itemWithID(id) );
}

void ItemDocumentIface::deleteSelection( )
{
	m_pItemDocument->deleteSelection();
}

void ItemDocumentIface::clearHistory( )
{
	m_pItemDocument->clearHistory();
}

void ItemDocumentIface::unselectAll( )
{
	m_pItemDocument->unselectAll();
}

void ItemDocumentIface::alignHorizontally( )
{
	m_pItemDocument->alignHorizontally();
}

void ItemDocumentIface::alignVertically( )
{
	m_pItemDocument->alignVertically();
}

void ItemDocumentIface::distributeHorizontally( )
{
	m_pItemDocument->distributeHorizontally();
}

void ItemDocumentIface::distributeVertically( )
{
	m_pItemDocument->distributeVertically();
}
//END class ItemDocumentIface



//BEGIN class TextDocumentIface
TextDocumentIface::TextDocumentIface( TextDocument * document )
	: DocumentIface(document)
{
	m_pTextDocument = document;
}

void TextDocumentIface::debugStepOver( )
{
	m_pTextDocument->debugStepOver();
}

void TextDocumentIface::debugStepOut( )
{
	m_pTextDocument->debugStepOut();
}

void TextDocumentIface::debugStep( )
{
	m_pTextDocument->debugStep();
}

void TextDocumentIface::debugStop( )
{
	m_pTextDocument->debugStop();
}

void TextDocumentIface::debugInterrupt( )
{
	m_pTextDocument->debugInterrupt();
}

void TextDocumentIface::debugRun( )
{
	m_pTextDocument->debugRun();
}

bool TextDocumentIface::isDebugging( )
{
#ifndef NO_GPSIM
	return m_pTextDocument->debuggerIsRunning();
#else
	return false;
#endif
}

void TextDocumentIface::clearBookmarks( )
{
	m_pTextDocument->clearBookmarks();
}

void TextDocumentIface::convertToAssembly( )
{
	m_pTextDocument->convertToAssembly();
}

void TextDocumentIface::convertToPIC( )
{
	m_pTextDocument->convertToPIC();
}

void TextDocumentIface::convertToHex( )
{
	m_pTextDocument->convertToHex();
}

void TextDocumentIface::convertToMicrobe( )
{
	m_pTextDocument->convertToMicrobe();
}

void TextDocumentIface::formatAssembly( )
{
	m_pTextDocument->formatAssembly();
}
//END class TextDocumentIface



//BEGIN class MechanicsDocumentIface
MechanicsDocumentIface::MechanicsDocumentIface( MechanicsDocument * document )
	: ItemDocumentIface(document)
{
	m_pMechanicsDocument = document;
}
//END class MechanicsDocumentIface

