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

#ifndef CIRCUITSCENE_H
#define CIRCUITSCENE_H


#include <QGraphicsScene>
#include <QVariantList>

namespace KTechLab
{
class ComponentItem;
class Theme;

class CircuitScene : public QGraphicsScene
{
  public:
    CircuitScene ( QObject* parent = 0, const QVariantList &args = QVariantList() );
    ~CircuitScene();
  public slots:
    void dataUpdated( const QString &name, const QVariantList &data );

  protected:
    virtual void dropEvent ( QGraphicsSceneDragDropEvent* event );
//    virtual void dragEnterEvent ( QGraphicsSceneDragDropEvent* event );
//    virtual void dragLeaveEvent ( QGraphicsSceneDragDropEvent* event );

  private:
    QSizeF m_componentSize;
    void setCircuitName( const QString &name );
    QString circuitName() const;

    void setupData();
    QString m_circuitName;
    QString m_componentTheme;
    QMap<QString,ComponentItem*> m_components;

    Theme *m_theme;
};

}
#endif // CIRCUITSCENE_H
