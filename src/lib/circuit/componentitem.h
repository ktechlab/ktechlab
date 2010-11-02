/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef COMPONENTAPPLET_H
#define COMPONENTAPPLET_H

#include <QDomDocument>
#include "interfaces/component/icomponentitem.h"
#include "circuitexport.h"

namespace KTechLab
{

class Theme;

class CIRCUIT_EXPORT ComponentItem: public IComponentItem
{
    Q_OBJECT
public:
    ComponentItem ( const QVariantMap& data, Theme *theme, QGraphicsItem* parentItem = 0 );
    ~ComponentItem();

    virtual QPainterPath shape() const;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);

    virtual QVariantMap data() const;
public slots:
    virtual void updateData( const QString &name, const QVariantMap &data );

signals:
    void dataUpdated( const QString &name, const QVariantMap &data );

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:

    /**
     * Add all pins defined in the svg file to this item.
     * Information is extracted from the DOM.
     */
    void initPins();

    QSvgRenderer *m_renderer;
    Theme *m_theme;
    QDomDocument m_svgDocument;

    QPainterPath m_shape;
};

}
#endif

