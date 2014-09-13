/***************************************************************************
 *   Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef KTLCIRCUITPLUGIN_QT_H_
#define KTLCIRCUITPLUGIN_QT_H_

#include "interfaces/idocumentplugin.h"
#include "interfaces/component/componentmodel.h"

#include "fakecomponentitemfactory.h"

namespace KTechLab {

class KTLCircuitPluginQt : public KTechLab::IDocumentPlugin
{
public:
    KTLCircuitPluginQt(const KComponentData &componentData, QObject *parent, const QVariantList& args );
    virtual ~KTLCircuitPluginQt();
    virtual void unload();


    /**
     * @return the component model representing all components
     */
    ComponentModel * componentModel();
    /**
     * Register the component- @param{factory} to the plugin so it knows about all provided components
     * and can provide the user with entries in the browser to drag it into their circuit.
     */
    void registerComponentFactory( KTechLab::IComponentItemFactory *factory );

    /**
     * Deregister the component-factory from the plugin.
     */
    void deregisterComponentFactory( KTechLab::IComponentItemFactory *factory );

    /**
     * Get a factory to create KTechLab::ComponentItem instances.
     * \sa KTechLab::IDocumentPlugin
     */
    IComponentItemFactory* componentItemFactory( const QString& name, Theme* theme = 0 );

protected:
    ComponentModel *m_componentModel;
    FakeComponentItemFactory* m_fakeComponentItemFactory;
};

}

#endif // KTLCIRCUITPLUGIN_QT_H_
