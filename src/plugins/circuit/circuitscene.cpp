/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "circuitscene.h"
#include "theme.h"
#include "interfaces/component/componentmimedata.h"
#include "componentitem.h"

#include <QGraphicsSceneDragDropEvent>
#include <KDebug>
#include "circuitmodel.h"

using namespace KTechLab;


CircuitScene::CircuitScene ( QObject* parent, CircuitModel *model )
 : QGraphicsScene ( parent ),
   m_model( model ),
   m_theme( new Theme() )
{
//    KConfigGroup cg = config( "circuit" );
//    m_componentTheme = cg.readEntry( "componentTheme", "din" );

//    m_componentSize = QSizeF( cg.readEntry("componentWidth", "64").toInt(), cg.readEntry("componentHeight", "64").toInt() );
    setupData();
}


CircuitScene::~CircuitScene()
{
    qDeleteAll( m_components.values() );
}

QString CircuitScene::circuitName() const
{
    return m_circuitName;
}

void CircuitScene::dropEvent ( QGraphicsSceneDragDropEvent* event )
{
    if (!event->mimeData()->hasFormat("application/x-icomponent")) {
        kDebug() << "Dropped unknown data";
        return;
    }
    const ComponentMimeData *mimeData = qobject_cast<const ComponentMimeData*>(event->mimeData());

    //FIXME: implement me!
    //do something with mimeData here. it should be added to the document using the document
    //DataEngine and services. use mimeData->createComponent() to create a new component.
    kDebug() << "Dropping item @"<< event->scenePos() << "type:" << mimeData->data("application/x-icomponent");
}


void CircuitScene::dragEnterEvent ( QGraphicsSceneDragDropEvent* event )
{
    if (!event->mimeData()->hasFormat("application/x-icomponent")) {
        return;
    }
    const ComponentMimeData *mimeData = qobject_cast<const ComponentMimeData*>(event->mimeData());

    kDebug() << "dragging type:" << mimeData->data("application/x-icomponent");
}

void CircuitScene::dragLeaveEvent ( QGraphicsSceneDragDropEvent* event )
{
    QGraphicsScene::dragLeaveEvent ( event );
}


void CircuitScene::setupData()
{
    if (!m_model)
        return;

    foreach (QVariant component, m_model->components())
    {
        if (component.canConvert(QVariant::Map)) {
            addItem( new ComponentItem( component.toMap(), m_theme ) );
        }
    }
}

void CircuitScene::dataUpdated( const QString &name, const QVariantList &data )
{
    //Plasma::DataEngine *docEngine = dataEngine( "ktechlabdocument" );
    //kDebug() << "isContainment() ==" << isContainment();
/*
    if ( data["mime"].toString().endsWith("circuit") ) {
        const QStringList &itemSources = data["itemList"].toStringList();
        foreach ( const QString &source, itemSources ) {
            if ( !m_components.contains(source) ) {
                ComponentApplet *component = new ComponentApplet( this, m_theme );
                kDebug() << "before connecting";
                //docEngine->connectSource( m_circuitName + "/" + source, component );
                kDebug() << "after connecting";

                //addApplet( component, position, false );
                m_components.insert( source, component );
            }
        }
    }*/
    //kDebug() << "Difference between components and applets" << (m_components.size() - applets().size());
}

void CircuitScene::setCircuitName ( const QString& name )
{
    //if name didn't change, do nothing
    if ( m_circuitName == name ) {
        return;
    }

    m_circuitName = name;
    setupData();
}
