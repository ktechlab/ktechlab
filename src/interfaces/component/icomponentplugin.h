/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ICOMPONENTPLUGIN_H
#define ICOMPONENTPLUGIN_H

#include "../ktlinterfacesexport.h"
#include "icomponent.h"

#if KDE_ENABLED
#include <interfaces/iplugin.h>
#endif

namespace KTechLab
{

class ComponentItem;
class Theme;

class IComponent;
class IDocumentPlugin;

/**
 * \short Factory to create IComponentItem objects
 * When writing a new \class IComponentPlugin this class must also
 * be implemented to create visual representations of components,
 * that are exported by the plugin and to provide meta-data about
 * what components are exported.
 *
 * \author Julian Bäume
 */
class KTLINTERFACES_EXPORT IComponentItemFactory
{
public:
    IComponentItemFactory();
    virtual ~IComponentItemFactory() {}

    /**
     * Return a list containing all meta-data
     * for all registered components within this plugin.
     * \return a \class QList containing all meta-data
     */
    QList<ComponentMetaData> allMetaData();

    /**
     * Create a KTechLab::ComponentItem for the given \param data and drawn with
     * the given \param theme.
     */
    virtual ComponentItem* createItem( const QVariantMap& data, KTechLab::Theme* theme=0 )=0;
protected:
    /**
     * Call this method from the implementation to register a
     * new component represented by \param data. This will add
     * the component to the list of supported components.
     * \param data - \class ComponentMetaData containing
     * all information about the component
     */
    void addSupportedComponent( const ComponentMetaData & data );

    /**
     * Load all components found in the file referenced by \param file.
     * This will add each components meta-data found in the file to the
     * internal list.
     *
     * \param file - file-name containing the components
     * including their meta-data
     */
    void loadComponentsFromFile( const QString &file );

private:
    QList<ComponentMetaData> m_componentDataList;
};

/**
 * \short base-class for component plugins
 * To create a new plugin providing a set of components,
 * this interface must be sub-classed. Implementing this class
 * will make it possible to load it as a plugin by KDevPlatform
 * and therefore make it accessible to your software.
 *
 * \author Julian Bäume
 */
#if KDE_ENABLED
class KTLINTERFACES_EXPORT IComponentPlugin: public KDevelop::IPlugin
#else
class KComponentData;
class KTLINTERFACES_EXPORT IComponentPlugin
#endif
{
public:
    /**
     * Default constructor see \class KDevelop::IPlugin
     */
    IComponentPlugin(const KComponentData &data, QObject *parent = 0 );

};

} // namespace KTechLab

#endif
