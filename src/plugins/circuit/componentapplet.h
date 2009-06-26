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

#include <Plasma/Applet>
#include <Plasma/Svg>
#include <Plasma/DataEngine>

namespace Plasma
{
class Theme;
} // namespace Plasma

class ComponentApplet: public Plasma::Applet
{
    Q_OBJECT

public:
    ComponentApplet( QObject *parent, Plasma::Theme *theme = 0, const QVariantList &args = QVariantList() );

    void paintInterface(QPainter *painter,
            const QStyleOptionGraphicsItem *option,
            const QRect& contentsRect);

public slots:
    void dataUpdated( const QString &name, const Plasma::DataEngine::Data &data );

protected:
    QString imagePathForComponent( const QVariantMap &map ) const;

private:
    Plasma::Svg m_icon;
    Plasma::Theme *m_theme;

    QVariantMap m_itemData;
};

#endif

