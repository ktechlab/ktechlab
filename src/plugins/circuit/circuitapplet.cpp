/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "circuitapplet.h"
#include "interfaces/component/componentmimedata.h"

#include <Plasma/DataEngine>
#include <QGraphicsSceneDragDropEvent>
#include <KDebug>

CircuitApplet::CircuitApplet( QObject *parent, const QVariantList &args )
    :   Plasma::Applet( parent, args )
{
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    setBackgroundHints(DefaultBackground);
    setAcceptDrops( true );
    init();
}

CircuitApplet::~CircuitApplet()
{}

void CircuitApplet::init()
{
}

void CircuitApplet::dropEvent( QGraphicsSceneDragDropEvent *event )
{
    if (!event->mimeData()->hasFormat("application/x-icomponent")) {
        return;
    }
    const KTechLab::ComponentMimeData *mimeData = qobject_cast<const KTechLab::ComponentMimeData*>(event->mimeData());

    //FIXME: implement me!
    //do something with mimeData here. it should be added to the document using the document
    //DataEngine and services. use mimeData->createComponent() to create a new component.
    kDebug() << "Dropping item @"<< event->scenePos() << "type:" << mimeData->data("application/x-icomponent");
}

void CircuitApplet::dataUpdated( const QString &name, const Plasma::DataEngine::Data &data )
{
    Plasma::DataEngine *docEngine = dataEngine( "ktechlabdocument" );

    if ( data["mime"].toString().endsWith("circuit") ) {
        const QStringList &itemSources = data["itemList"].toStringList();
        foreach ( const QString &source, itemSources ) {
            if ( !m_components.contains(source) ) {
                m_components << source;
                docEngine->connectSource( m_circuitName + "/" + source, this );
            }
        }
    } else if ( data["mime"].toString().endsWith("component") ) {
        //TODO: implement component handling
    }
}

void CircuitApplet::setupData()
{
    Plasma::DataEngine *docEngine = dataEngine( "ktechlabdocument" );
    if ( !docEngine ) {
        kWarning() << "No document engine found" << endl;
        return;
    }

    docEngine->connectSource( m_circuitName, this );
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
