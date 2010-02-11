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

#include <QGraphicsScene>
#include <QVariantList>

namespace KTechLab
{
class ComponentItem;
class Theme;
class CircuitModel;
class KTLCircuitPlugin;

class CircuitScene : public QGraphicsScene
{
  public:
    CircuitScene ( QObject* parent = 0, CircuitModel *model = 0 );
    ~CircuitScene();

  public slots:
    void dataUpdated( const QString &name, const QVariantList &data );

  protected:
    virtual void dropEvent ( QGraphicsSceneDragDropEvent* event );
    virtual void dragEnterEvent ( QGraphicsSceneDragDropEvent* event );
    virtual void dragLeaveEvent ( QGraphicsSceneDragDropEvent* event );

  private:
    QSizeF m_componentSize;
    void setCircuitName( const QString &name );
    QString circuitName() const;

    void setupData();
    QString m_circuitName;
    QString m_componentTheme;
    QMap<QString,ComponentItem*> m_components;
    CircuitModel *m_model;
    Theme *m_theme;
};

}
#endif // CIRCUITSCENE_H
