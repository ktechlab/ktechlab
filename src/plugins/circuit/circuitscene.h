/*
    The graphical representation of a circuit.
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

#ifndef CIRCUITSCENE_H
#define CIRCUITSCENE_H

#include <interfaces/idocumentscene.h>
#include <QVariantList>

namespace KTechLab
{

class IDocumentItem;
class KTLCircuitPlugin;

class ComponentItem;
class ComponentMimeData;
class PinItem;
class Theme;
class CircuitModel;
class KTLCircuitPlugin;

class CircuitScene : public IDocumentScene
{
  Q_OBJECT
  public:
    CircuitScene ( QObject* parent = 0, CircuitModel *model = 0, KTLCircuitPlugin* plugin = 0 );
    virtual ~CircuitScene();

    virtual IComponentItem* item(const QString& id) const;
    virtual Node* node(const QString& id) const;

  public slots:
    virtual void updateData( const QString &name, const QVariantMap &data );

  protected:
    virtual void dropEvent ( QGraphicsSceneDragDropEvent* event );
    virtual void dragMoveEvent ( QGraphicsSceneDragDropEvent* event );

  protected slots:
    /**
     * update the items in the list withen the model
     */
    void updateModel(QList<KTechLab::IComponentItem*> components);
    void updateModel(QList<KTechLab::ConnectorItem*> connectors);
    void updateModel(KTechLab::IDocumentItem* item);

  private:
    QSizeF m_componentSize;
    void setCircuitName( const QString &name );
    QString circuitName() const;

    void setupData();
    QVariantMap createItemData(const KTechLab::ComponentMimeData* data, const QPointF& pos) const;
    QString m_circuitName;
    QString m_componentTheme;
    QMap<QString,ComponentItem*> m_components;
    QMap<QString,PinItem*> m_pins;
    CircuitModel *m_model;
    Theme *m_theme;
    KTLCircuitPlugin* m_plugin;
};

}
#endif // CIRCUITSCENE_H
