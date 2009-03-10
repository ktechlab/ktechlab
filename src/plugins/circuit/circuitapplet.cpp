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
#include <Plasma/Theme>
#include <Plasma/FrameSvg>
#include <QGraphicsSceneDragDropEvent>
#include <QPainter>
#include <KDebug>

CircuitApplet::CircuitApplet( QObject *parent, const QVariantList &args )
    :   Plasma::Applet( parent, args ),
        m_theme( new Plasma::Theme() ),
        m_bg( new Plasma::FrameSvg() )
{
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    setBackgroundHints(NoBackground);
    setAcceptDrops( true );
    init();
}

CircuitApplet::~CircuitApplet()
{
    delete m_theme;
    qDeleteAll(m_componentSvgs);
}

void CircuitApplet::init()
{
    m_theme->setThemeName( "default" );
    KConfigGroup cg( KGlobal::config(), "circuit" );
    m_componentTheme = cg.readEntry( "componentTheme", "din" );

    m_componentSize = QSizeF( cg.readEntry("componentWidth", "64").toInt(), cg.readEntry("componentHeight", "64").toInt() );

    if ( !m_theme->currentThemeHasImage( "ktechlab/circuit-background" ) ) {
        kDebug() << "no background image for circuit found in theme" << m_theme->themeName();
    }
    m_bg.setImagePath( "ktechlab/circuit-background" );
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

void CircuitApplet::paintInterface( QPainter *p, const QStyleOptionGraphicsItem *option, const QRect &contentsRect )
{
    //draw background
    if ( m_bg.frameSize() != contentsRect.size() ) {
        m_bg.resizeFrame( contentsRect.size() );
    }
    m_bg.paintFrame(p, QPointF( contentsRect.left(), contentsRect.top() ));

    // repaint components
    foreach ( const QString &component, m_components.keys() ) {
        Plasma::Svg * itemSvg = m_componentSvgs.value( component );
        //resize if needed
        if ( itemSvg->size() != m_componentSize ) {
            itemSvg->resize( m_componentSize );
        }

        const QVariantMap &itemData = m_components.value( component );
        itemSvg->paint(p, QPointF( itemData[ "x" ].toString().toInt(), itemData[ "y" ].toString().toInt() ));
    }
}

void CircuitApplet::dataUpdated( const QString &name, const Plasma::DataEngine::Data &data )
{
    Plasma::DataEngine *docEngine = dataEngine( "ktechlabdocument" );

    if ( data["mime"].toString().endsWith("circuit") ) {
        const QStringList &itemSources = data["itemList"].toStringList();
        foreach ( const QString &source, itemSources ) {
            if ( !m_components.contains(source) ) {
                docEngine->connectSource( m_circuitName + "/" + source, this );
            }
        }
    } else if ( data["mime"].toString().endsWith("component") ) {
        QVariantMap item = data[ "item" ].toMap();

        //check if a new component is added
        if ( !m_componentSvgs.contains(item[ "id" ].toString()) ) {
            Plasma::Svg *itemSvg = new Plasma::Svg( this );
            itemSvg->setImagePath( imagePathForComponent( item ) );
            m_componentSvgs.insert( item[ "id" ].toString(), itemSvg );
        }

        //update data
        m_components.insert( item[ "id" ].toString(), item );
        //added new component, so repaint
        update();
    }
}

QString CircuitApplet::imagePathForComponent( const QVariantMap &map ) const
{
    QString path = m_theme->imagePath( "ktechlab/components/"+m_componentTheme+"/"+map[ "type" ].toString().replace("/","_") );
    if ( path.isEmpty() ) {
        //default to unknown component
        path = m_theme->imagePath( "ktechlab/components/unknown" );
    }
    kDebug()<< "finding path for" << "ktechlab/components/"+m_componentTheme+"/"+map[ "type" ].toString().replace("/","_")
            << path;
    return path;
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
