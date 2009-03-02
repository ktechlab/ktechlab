/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "circuitcontainer.h"

#include "circuitdocument.h"

CircuitContainer::CircuitContainer( KDevelop::IDocument *document, QObject *parent )
    :   Plasma::DataContainer(parent),
        m_document(dynamic_cast<CircuitDocument*>( document )) //do we really need dynamic_cast here? we know that this is a CircuitDocument*
{

}

void CircuitContainer::setComponent( const QString &component )
{
    if ( component.isEmpty() ) {
        setCircuitData();
    } else {
        setComponentData( component );
    }
}

void CircuitContainer::setComponentData( const QString &component )
{
    //TODO: implement me
}

void CircuitContainer::setCircuitData()
{
    setObjectName( m_document->url().prettyUrl() );
    setData( "itemList", QVariant(m_document->items().keys()) );
}

// vim: sw=4 sts=4 et tw=100
