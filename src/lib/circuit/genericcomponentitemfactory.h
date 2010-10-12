/*
    Copyright (C) 2010 Julian Bäume <julian@svg4all.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#ifndef KTECHLAB_GENERICCOMPONENTITEMFACTORY_H
#define KTECHLAB_GENERICCOMPONENTITEMFACTORY_H

#include <interfaces/component/icomponentplugin.h>


namespace KTechLab {

class GenericComponentItemFactory : public KTechLab::IComponentFactory
{

public:
    virtual KTechLab::ComponentItem* createItem(const QVariantMap& data, KTechLab::Theme* theme = 0);
    virtual KTechLab::IComponent* create(const QString& name);

    /**
     * Find the name of the file containing the graphical representation of
     * the given component. The default implementation will simply normalize the
     * components name to contain no slashes and add .svgz as the default suffix.
     * You are encouraged to override this method in your own implementation to support
     * multiple components per file.
     *
     * \param component - the name of the component
     * \returns the filename containing the component
     */
    virtual QString fileNameForComponent( const QString &component );
};

}

#endif // KTECHLAB_GENERICCOMPONENTITEMFACTORY_H
