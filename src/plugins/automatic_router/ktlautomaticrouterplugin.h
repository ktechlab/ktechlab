/*
    Copyright (C) 2003-2004  David Saxton <david@bluehaze.org>
    Copyright (C) 2010 Julian Bäume <julian@svg4all.de>

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

#ifndef KTLAUTOMATICROUTERPLUGIN_H
#define KTLAUTOMATICROUTERPLUGIN_H

#include <interfaces/iplugin.h>
#include <interfaces/irouterplugin.h>
#include <QVariantList>

class AutomaticRouter : public KDevelop::IPlugin, public KTechLab::IRouterPlugin
{
    Q_OBJECT
    Q_INTERFACES( KTechLab::IRouterPlugin )
public:
    AutomaticRouter(QObject* parent = 0, const QVariantList& args = QVariantList());

protected slots:
    virtual void generateRoutingInfo(KTechLab::IDocumentScene* scene);
};

#endif // KTLAUTOMATICROUTERPLUGIN_H
