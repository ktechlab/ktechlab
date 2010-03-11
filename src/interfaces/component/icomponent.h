/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ICOMPONENT_H
#define ICOMPONENT_H

#include "../ktlinterfacesexport.h"

#include <QString>
#include <KIcon>

class KConfig;
namespace KTechLab
{

/**
 * MetaData for a component
 */
struct ComponentMetaData
{
    /** unique name to identify the component */
    QString name;
    /** a title visible to the user */
    QString title;
    /** category for the component */
    QString category;
    // FIXME: I'm not sure if this is the right place
    /** an icon shown to the user */
    KIcon icon;
    /** type of the component */
    QString type;
};

/**
 * Base class for all components for KTechLab.
 */
class KTLINTERFACES_EXPORT IComponent
{
public:
    /**
     * Pure virtual function should return meta-data with information how to handle this component
     */
    static ComponentMetaData metaData();
    /**
     * Read ComponentMetaData for a component with a given \param name from a KConfig object.
     */
    static ComponentMetaData metaData( const QString &name, const KConfig &metaData );
};

} // namespace KTechLab

#endif

