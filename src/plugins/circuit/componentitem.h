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

#include <QGraphicsView>
#include <QGraphicsSvgItem>

namespace KTechLab
{

class Theme;

class ComponentItem: public QGraphicsSvgItem
{
public:
    ComponentItem ( const QVariantMap& data, Theme *theme, QGraphicsItem* parentItem = 0 );
    ~ComponentItem();

public slots:
    void dataUpdated( const QString &name, const QVariantMap &data );

protected:
    QString imagePathForComponent( const QVariantMap &map ) const;

private:
    QSvgRenderer *m_renderer;
    Theme *m_theme;
};

}
#endif

