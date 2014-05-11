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
#include <interfaces/component/icomponentplugin.h>

using namespace KTechLab;


CircuitScene::CircuitScene ( QObject* parent, CircuitModel *model, KTLCircuitPlugin* plugin )
 : IDocumentScene ( parent ),
   m_model( model ),
   m_theme( new Theme() ),
   m_plugin( plugin )
{
//    m_componentSize = QSizeF( cg.readEntry("componentWidth", "64").toInt(), cg.readEntry("componentHeight", "64").toInt() );
    setupData();

    connect(this,SIGNAL(transactionAborted()),model,SLOT(revert()));
    connect(this,SIGNAL(transactionCompleted()),model,SLOT(submit()));

    connect(this,SIGNAL(componentsMoved(QList<KTechLab::IComponentItem*>)),
            this,SLOT(updateModel(QList<KTechLab::IComponentItem*>)));
    connect(this,SIGNAL(routeCreated(KTechLab::ConnectorItem*)),
            this,SLOT(addConnector(KTechLab::ConnectorItem*)));
    connect(this,SIGNAL(routed(QList<KTechLab::ConnectorItem*>)),
            this,SLOT(updateModel(QList<KTechLab::ConnectorItem*>)));
    connect(this,SIGNAL(itemAdded(KTechLab::IDocumentItem*)),
            this,SLOT(addItem(KTechLab::IDocumentItem*)));
    connect(this,SIGNAL(itemRemoved(KTechLab::IDocumentItem*)),
            this,SLOT(removeItem(KTechLab::IDocumentItem*)));
}


CircuitScene::~CircuitScene()
{
    delete m_theme;
}

QString CircuitScene::circuitName() const
{
    return m_circuitName;
}

QVariantMap CircuitScene::createItemData(const KTechLab::ComponentMimeData* data, const QPointF& pos) const
{
    QVariantMap comp;
    // type in the xml files is name in the component mimedata :/
    comp.insert("type", data->name());
    comp.insert("x", pos.x());
    comp.insert("y", pos.y());
    comp.insert("id", m_model->generateUid(data->name()));

    return comp;
}

void CircuitScene::dropEvent ( QGraphicsSceneDragDropEvent* event )
{
    if (!event->mimeData()->hasFormat("ktechlab/x-icomponent")) {
#if KDE_ENABLED
        kDebug() << "Dropped unknown data";
#else
		qDebug() << "Dropped unknown data";
#endif
        return;
    }
    const ComponentMimeData *mimeData = qobject_cast<const ComponentMimeData*>(event->mimeData());

    const QVariantMap& itemData = createItemData(mimeData, event->scenePos());
    m_model->addComponent(itemData);
    ComponentItem* item = mimeData->factory()->createItem(itemData, m_theme);
    item->setVisible(true);
    addItem( item );
    m_components.insert(item->id(), item);
    item->setPos(alignToGrid(item->scenePos()));
    QList<IComponentItem*> list;
    list << item;
    emit IDocumentScene::componentsMoved(list);
    emit transactionCompleted();
    event->accept();
}

void CircuitScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
    if (!event->mimeData()->hasFormat("ktechlab/x-icomponent")) {
        event->ignore();
        return;
    }
    event->accept();
}

void CircuitScene::setupData()
{
    if (!m_model && !m_plugin)
        return;

    foreach (QVariant component, m_model->components())
    {
        if (component.canConvert(QVariant::Map)) {
            const QVariantMap& map(component.toMap());
            IComponentItemFactory* componentFactory =
                m_plugin->componentItemFactory( map.value("type").toString() );

            Q_ASSERT(componentFactory);
            ComponentItem* item = componentFactory->createItem( map, m_theme );
            Q_ASSERT(item);
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
            new ConnectorItem(connector.toMap(),this);
        }
    }

}

void CircuitScene::updateModel(QList< IComponentItem* > components)
{
    foreach(IDocumentItem* item, components){
        updateModel(item);
    }
}
void CircuitScene::updateModel(QList< ConnectorItem* > connectors)
{
    foreach(IDocumentItem* item, connectors){
        updateModel(item);
    }
}
void CircuitScene::updateModel(IDocumentItem* item)
{
    const QVariantMap& data = item->data();
    m_model->updateData(data.value("id").toString(),data);
}

void CircuitScene::addConnector(ConnectorItem* item)
{
    QVariantMap data = item->data();
    if (!data.contains("id"))
        data.insert("id", m_model->generateUid(((IDocumentItem*)item)->type()));

    m_model->addConnector(data);
}

void CircuitScene::removeItem(IDocumentItem* item)
{
    QModelIndex index = m_model->index(item->data());
    if (!m_model->removeRow(index.row(),index.parent())) {
#if KDE_ENABLED
        kWarning() << "Could not remove item:" << item->data();
#else
		qWarning() << "Could not remove item:" << item->data();
#endif
	}
}

void CircuitScene::updateData( const QString& /* name */, const QVariantMap& /* data */ )
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

void CircuitScene::rotateSelectedComponents(qreal angle)
{
    QList<IComponentItem*> components;
    QList<ConnectorItem*> connectors;

    foreach(QGraphicsItem* item, selectedItems()){
        IComponentItem* c = qgraphicsitem_cast<IComponentItem*>(item);
        if (!c) continue;
        components.append(c);
        scheduleForRerouting(collidingItems(c));
        c->setRotation(c->rotation()+angle);
    }
    emit componentsMoved(components);
    performRerouting();
    emit transactionCompleted();
}

void CircuitScene::flipSelectedComponents(Qt::Axis axis)
{
    QList<IComponentItem*> components;

    foreach(QGraphicsItem* item, selectedItems()){
        IComponentItem* c = qgraphicsitem_cast<IComponentItem*>(item);
        if (!c) continue;
        components.append(c);
        scheduleForRerouting(collidingItems(c));
        QPointF center = c->boundingRect().center();
        QTransform t;
        int sx = (axis == Qt::XAxis) ? 1 : -1;
        int sy = (axis == Qt::YAxis) ? 1 : -1;
        t.translate(center.x(),center.y()).scale(sx,sy).translate(-center.x(),-center.y());
        c->setTransform(t,true);
    }
    emit componentsMoved(components);
    performRerouting();
    emit transactionCompleted();
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
