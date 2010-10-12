/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DOCUMENTPLUGIN_H
#define DOCUMENTPLUGIN_H

#include "ktlinterfacesexport.h"

#include <interfaces/iplugin.h>
#include <QObject>
#include <QString>
#include <QVariantMap>

namespace KDevelop
{
class IDocument;
} // namespace KDevelop

namespace KTechLab
{

class Theme;
class ComponentItem;
class IComponentFactory;

/**
 * \short Base-class for Plugins handling documents
 * Sub-class this interface to support a new document-class for
 * KTechLab. KTechLab documents are composed of different components.
 * This interface unifies this and provides basic handling of these components.
 *
 * \author Julian Bäume
 */
class KTLINTERFACES_EXPORT IDocumentPlugin: public KDevelop::IPlugin
{
    Q_OBJECT
public:
    IDocumentPlugin( KComponentData data, QObject *parent = 0 );
    virtual ~IDocumentPlugin() {};

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

    /**
     * Each component factory can register itself at the document plugin, to make
     * it aware of it's presence. It will then determine which components the
     * factory has to offer and can handle them accordingly.
     *
     * \param factory - the component factory
     */
    virtual void registerComponentFactory( KTechLab::IComponentFactory * factory )=0;
    /**
     * When component plugins are unloaded, they can remove the components, they
     * provide from the document plugin, so these components won’t be available for
     * use any longer.
     *
     * \sa registerComponentFactory
     *
     * \param factory - the component factory
     */
    virtual void deregisterComponentFactory( KTechLab::IComponentFactory* factory )=0;

    /**
     * Create a KTechLab::ComponentItem from the given meta-data. The implementation should
     * find a component factory that can handle the meta-data and produce the item.
     */
    virtual ComponentItem* createComponentItem( const QVariantMap& data, KTechLab::Theme* theme = 0 )=0;
};

} // namespace KTechLab

#endif

