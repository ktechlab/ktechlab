/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2009-2010  Julian Bäume <julian@svg4all.de>

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
#include "ktlcircuitplugin.h"
#include "interfaces/component/componentmimedata.h"
#include "interfaces/component/connectoritem.h"
#include "componentitem.h"

#include <QGraphicsSceneDragDropEvent>
#include <KDebug>
#include "circuitmodel.h"
#include "pinitem.h"
#include <interfaces/component/connector.h>

using namespace KTechLab;


CircuitScene::CircuitScene ( QObject* parent, CircuitModel *model )
 : IDocumentScene ( parent ),
   m_model( model ),
   m_theme( new Theme() )
{
//    m_componentSize = QSizeF( cg.readEntry("componentWidth", "64").toInt(), cg.readEntry("componentHeight", "64").toInt() );
    setupData();
}


CircuitScene::~CircuitScene()
{
    qDeleteAll( m_components.values() );
    delete m_theme;
}

QString CircuitScene::circuitName() const
{
    return m_circuitName;
}

void CircuitScene::dropEvent ( QGraphicsSceneDragDropEvent* event )
{
    if (!event->mimeData()->hasFormat("ktechlab/x-icomponent")) {
        kDebug() << "Dropped unknown data";
        return;
    }
    const ComponentMimeData *mimeData = qobject_cast<const ComponentMimeData*>(event->mimeData());

    ComponentItem* item =
        new ComponentItem(
            m_model->createComponent(mimeData,event->scenePos()),
            m_theme
        );
    item->setVisible(true);
    addItem( item );
    m_components.insert(item->id(), item);

    event->accept();
}

void CircuitScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
    if (!event->mimeData()->hasFormat("ktechlab/x-icomponent")) {
        event->ignore();
        return;
    }
    const ComponentMimeData *mimeData = qobject_cast<const ComponentMimeData*>(event->mimeData());

    event->accept();
}

void CircuitScene::setupData()
{
    if (!m_model)
        return;

    foreach (QVariant component, m_model->components())
    {
        if (component.canConvert(QVariant::Map)) {
            ComponentItem* item = new ComponentItem( component.toMap(), m_theme );
            addItem( item );
            m_components.insert(item->id(), item);
        }
    }
    foreach (QVariant pins, m_model->nodes()){
        if (pins.canConvert(QVariant::Map)) {
            QPointF p(pins.toMap().value("x").toDouble(),pins.toMap().value("y").toDouble());
            QRectF rect(p, QSize(4,4));
            PinItem* item = new PinItem(rect, 0, this);
            item->setId(pins.toMap().value("id").toString());
            m_pins.insert(item->id(),item);
        }
    }
    foreach (QVariant connector, m_model->connectors())
    {
        if (connector.canConvert(QVariant::Map)) {
            ConnectorItem *connectorItem = new ConnectorItem(connector.toMap(),this);
        }
    }

}

void CircuitScene::updateData( const QString& name, const QVariantMap& data )
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

IComponentItem* CircuitScene::item(const QString& id) const
{
    return m_components.value(id);
}

Node* CircuitScene::node(const QString& id) const
{
    return m_pins.value(id);
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

#include "circuitscene.moc"
