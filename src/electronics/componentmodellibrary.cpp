/***************************************************************************
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "componentmodellibrary.h"

#include <QFile>
#include <QStandardPaths>
#include <QTime>

#include <cassert>

#include <ktechlab_debug.h>

// A prime number slightly larger than the number of models for any particular type
// const int maxComponentModels = 101;

// BEGIN class ComponentModel
ComponentModel::ComponentModel()
{
}

ComponentModel::~ComponentModel()
{
}

double ComponentModel::property(const QString &name) const
{
    return m_property[name];
}

void ComponentModel::setProperty(const QString &name, double value)
{
    m_property[name] = value;
}
// END class ComponentModel

// BEGIN class ComponentModelLibrary
ComponentModelLibrary *ComponentModelLibrary::m_pSelf = nullptr;

// static
ComponentModelLibrary *ComponentModelLibrary::self()
{
    if (!m_pSelf)
        m_pSelf = new ComponentModelLibrary;

    return m_pSelf;
}

ComponentModelLibrary::ComponentModelLibrary()
{
    loadModels();
}

ComponentModelLibrary::~ComponentModelLibrary()
{
}

void ComponentModelLibrary::loadModels()
{
    QTime ct;
    ct.start();

    QStringList files;
    files << "transistors_lib.txt";

    // Used to check that maxComponentModels isn't too small
    typedef QMap<ModelType, int> IntMap;
    IntMap modelCount;

    QStringList::iterator end = files.end();
    for (QStringList::iterator it = files.begin(); it != end; ++it) {
        QString fileName = QStandardPaths::locate(QStandardPaths::AppDataLocation, "models/" + *it);
        if (fileName.isEmpty()) {
            qCWarning(KTL_LOG) << "Could not find library file \"" << *it << "\".";
            continue;
        }

        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            qCWarning(KTL_LOG) << "Could not open library file \"" << fileName << "\" for reading.";
            continue;
        }

        QString id;
        QString typeString;
        ComponentModel *model = nullptr;

        QTextStream stream(&file);
        while (!stream.atEnd()) {
            QString line = stream.readLine();

            if (line.isEmpty())
                continue;

            if (line == "[/]") {
                // End of previous model

                assert(model);

                ModelType type = None;
                if (typeString == "NPN")
                    type = NPN;
                else if (typeString == "PNP")
                    type = PNP;
                else
                    qCCritical(KTL_LOG) << "Unknown type \"" << typeString << "\".";

                if (m_componentModelIDs[type].contains(id))
                    qCCritical(KTL_LOG) << "Already have model with id=\"" << id << "\" for type=\"" << typeString << "\".";

                if (!m_componentModels.contains(type)) {
                    m_componentModels[type] = ComponentModelDict(/* maxComponentModels */);
                    // m_componentModels[type].setAutoDelete( true ); // 2018.08.14 - not needed
                }

                m_componentModels[type].insert(id, *model);
                m_componentModelIDs[type] << id;

                /* if ( int(modelCount[type] * 1.2) > maxComponentModels )  // 2018.08.14 - not needed
                {
                    qCWarning(KTL_LOG) << "There are "<<modelCount[type]<<" models for component type \""<<typeString<<"\". Consider enlarging the dictionary.";
                } */

                // Reset the model
                model = nullptr;
                id = QString();
                typeString = QString();

                modelCount[type]++;
            } else if (line.startsWith("[")) {
                // Already handled the case with "[/]", so must be beginning of
                // new model

                // Check that their isn't a previous model that hasn't saved
                assert(!model);

                model = new ComponentModel;
                id = line.mid(1, line.length() - 2); // extract the text between the square brackets
            } else {
                // Setting a property of the model
                assert(model);

                int pos = line.indexOf('=');
                assert(pos != -1);

                QString name = line.left(pos);
                QString value = line.mid(pos + 1);

                if (name == "Description")
                    model->setDescription(value);
                else if (name == "Type")
                    typeString = value;
                else {
                    bool ok;
                    double realValue = value.toDouble(&ok);

                    if (!ok)
                        qCCritical(KTL_LOG) << "Could not convert \"" << value << "\" to a real number (for property \"" << name << "\".";
                    else
                        model->setProperty(name, realValue);
                }
            }
        }
    }

    qCDebug(KTL_LOG) << "It took " << ct.elapsed() << " milliseconds to read in the component models.";
}
// END class ComponentModelLibrary
