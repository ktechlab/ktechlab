/*
    Copyright (C) Julian Bäume <julian@svg4all.de>

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


#ifndef KTECHLAB_SIMPLEROUTERPLUGIN_H
#define KTECHLAB_SIMPLEROUTERPLUGIN_H

#include <interfaces/irouterplugin.h>
#include <interfaces/iplugin.h>
#include <QVariantList>

namespace KTechLab {

/**
 * This plugin provides simple routing between two points using an 8x8 grid.
 * It will connect these two points using a maximum of one horizontal and
 * one vertical line.
 **/
class SimpleRouterPlugin : public KDevelop::IPlugin, public KTechLab::IRouterPlugin
{
    Q_OBJECT
    Q_INTERFACES( KTechLab::IRouterPlugin )
public:
    SimpleRouterPlugin(QObject* parent = 0, const QVariantList& args = QVariantList());
protected:
    virtual void generateRoutingInfo(KTechLab::IDocumentScene* scene);
};

}

#endif // KTECHLAB_SIMPLEROUTERPLUGIN_H
