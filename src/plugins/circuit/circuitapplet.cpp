/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "circuitapplet.h"

#include <Plasma/DataEngine>

CircuitApplet::CircuitApplet( QObject *parent, const QVariantList &args )
    :   Plasma::Applet( parent, args )
{
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    setBackgroundHints(DefaultBackground);
    init();
}

CircuitApplet::~CircuitApplet()
{}

void CircuitApplet::init()
{
}

void CircuitApplet::dataUpdated( const QString &name, const Plasma::DataEngine::Data &data )
{

}

void CircuitApplet::setupData()
{
    Plasma::DataEngine *docEngine = dataEngine( "ktechlabdocument" );
    if ( !docEngine ) {
        kWarning() << "No document engine found" << endl;
        return;
    }

    //FIXME: connect to the source and it's provided components
    kDebug() << docEngine->query( m_circuitName );
}

void CircuitApplet::setCircuitName( const QString &name )
{
    //if name didn't change, do nothing
    if ( m_circuitName == name ) {
        return;
    }

    m_circuitName = name;
    setupData();
}

// vim: sw=4 sts=4 et tw=100
