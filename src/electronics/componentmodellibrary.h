/***************************************************************************
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef COMPONENTMODELLIBRARY_H
#define COMPONENTMODELLIBRARY_H

#include <QMap>
#include <QObject>
#include <QStringList>

typedef QMap<QString, double> DoubleMap;

class ComponentModel
{
public:
    ComponentModel();
    ~ComponentModel();

    /**
     * Sets the description.
     */
    void setDescription(const QString &description)
    {
        m_description = description;
    }
    /**
     * @return A short description.
     */
    QString description() const
    {
        return m_description;
    }
    /**
     * @return the value of the property with the given @p name . If the
     * property does not exist, then the value 0.0 is returned.
     */
    double property(const QString &name) const;
    /**
     * Set the property called @p name to have the value @p value .
     */
    void setProperty(const QString &name, double value);

protected:
    QString m_name;
    QString m_description;
    DoubleMap m_property;
};

typedef QMultiHash<QString, ComponentModel> ComponentModelDict;

/**
@author David Saxton <david@bluehaze.org>
*/
class ComponentModelLibrary : public QObject
{
    Q_OBJECT
public:
    enum ModelType { None, NPN, PNP };
    typedef QMap<ModelType, ComponentModelDict> ComponentModelDictMap;
    typedef QMap<ModelType, QStringList> ModelStringListMap;

    /**
     * @return The list of IDs for the given ModelType.
     * @see modelNames
     */
    QStringList modelIDs(ModelType modelType) const
    {
        return m_componentModelIDs[modelType];
    }

    static ComponentModelLibrary *self();
    ~ComponentModelLibrary() override;

protected:
    /**
     * Reads in the component models from disk.
     */
    void loadModels();

    ComponentModelDictMap m_componentModels;
    ModelStringListMap m_componentModelIDs;

private:
    ComponentModelLibrary();
    static ComponentModelLibrary *m_pSelf;
};

#endif
