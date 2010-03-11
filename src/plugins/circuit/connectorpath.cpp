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

#include "connectorpath.h"

#include <KDebug>
#include <QStringList>

using namespace KTechLab;


KTechLab::ConnectorPath::ConnectorPath ( const QVariantMap& path )
    : QPainterPath ( )
{
    QStringList route = path.value( "route" ).toString().split(',');
    //remove last entry, if it is empty
    if (route.last().isEmpty())
        route.removeLast();

    QStringListIterator it(route);
    QPointF p;
    p.setX(it.next().toDouble()*8);
    p.setY(it.next().toDouble()*8);
    moveTo( p );
    while (it.hasNext())
    {
        p.setX(it.next().toDouble());
        p.setY(it.next().toDouble());
        p*=8;
        lineTo( p );
    }
}
