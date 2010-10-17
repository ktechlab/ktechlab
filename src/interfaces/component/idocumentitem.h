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

#ifndef KTECHLAB_IDOCUMENTITEM_H
#define KTECHLAB_IDOCUMENTITEM_H
#include <QVariantMap>
#include "../ktlinterfacesexport.h"

class QLatin1String;

namespace KTechLab {

/**
 * \short Interface to provide identification for items
 *
 */
class KTLINTERFACES_EXPORT IDocumentItem
{
public:
    IDocumentItem();
    virtual ~IDocumentItem();
    /**
     * Get an identification string for this item.
     */
    QString id() const;
    /**
     * Get a name for this item.
     */
    QString name() const;
    /**
     * Get the name of the type of this item.
     */
    QString type() const;
    /**
     * Provide some internal data as QVariantMap, so it can easily be stored to disk.
     * Default \returns all information known to this class, like id, name and type.
     *
     * You should override this method and provide more data, that you want to be
     * stored by the KTechLab::IDocumentModel.
     *
     */
    virtual QVariantMap data() const;

protected:
    /**
     * Get an identification string for this item.
     */
    void setId(const QString& id);
    /**
     * Get a name for this item.
     */
    void setName(const QString& name);
    /**
     * Get the name of the type of this item.
     */
    void setType(const QString& type);

private:
    class IDocumentItemPrivate;
    IDocumentItemPrivate* d;
};

}

#endif // KTECHLAB_IDOCUMENTITEM_H
