/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CIRCUITAPPLET_H
#define CIRCUITAPPLET_H

#include <QGraphicsView>
#include <QVariantList>

class QString;
class QStringList;
class QGraphicsSceneDragDropEvent;

namespace KTechLab
{

class ComponentApplet;
class Theme;

class CircuitApplet: public QGraphicsView
{
    Q_OBJECT
public:
    CircuitApplet( QObject *parent, const QVariantList &args = QVariantList() );
    ~CircuitApplet();

    void init();

    virtual void dropEvent( QGraphicsSceneDragDropEvent *event );

    void paintInterface(QPainter *painter,
            const QStyleOptionGraphicsItem *option,
            const QRect& contentsRect);

public slots:
    void dataUpdated( const QString &name, const QVariantList &data );

private:
    void setCircuitName( const QString &name );
    QString circuitName() const;

    void setupData();
    QString m_circuitName;
    QString m_componentTheme;
    QMap<QString,ComponentApplet*> m_components;

    QSizeF m_componentSize;

    Theme *m_theme;
};

}
#endif

