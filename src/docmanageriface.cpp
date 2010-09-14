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
#include "docmanageriface.h"
#include "document.h"


DocManagerIface::DocManagerIface( DocManager * docManager )
	: DCOPObject("DocumentManager")
{
	m_pDocManager = docManager;
}


DocManagerIface::~DocManagerIface()
{
}

bool DocManagerIface::closeAll( )
{
	return m_pDocManager->closeAll();
}

DCOPRef DocManagerIface::openURL( const QString & url )
{
	return docToRef( m_pDocManager->openURL(url) );
}

void DocManagerIface::gotoTextLine( const QString & url, int line )
{
	m_pDocManager->gotoTextLine( url, line );
}

DCOPRef DocManagerIface::createTextDocument( )
{
	return docToRef( (Document*)m_pDocManager->createTextDocument() );
}

DCOPRef DocManagerIface::createCircuitDocument( )
{
	return docToRef( (Document*)m_pDocManager->createCircuitDocument() );
}

DCOPRef DocManagerIface::createFlowCodeDocument( )
{
	return docToRef( (Document*)m_pDocManager->createFlowCodeDocument() );
}

DCOPRef DocManagerIface::createMechanicsDocument( )
{
	return docToRef( (Document*)m_pDocManager->createMechanicsDocument() );
}

DCOPRef DocManagerIface::docToRef( Document * document )
{
	if (document)
		return DCOPRef(document->dcopObject());
	return DCOPRef();
}


