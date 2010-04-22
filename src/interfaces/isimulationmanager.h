/***************************************************************************
 *    ISimulationManager interface.                                        *
 *       A controller for simulations in KTechLab                          *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ISIMULATIONMANAGER_H
#define ISIMULATIONMANAGER_H

#include "ktlinterfacesexport.h"

#include <QObject>
#include <QList>

namespace KTechLab {

    class IElementFactory;
    class ISimulator;
    class ISimulatorFactory;
    class IComponentDocument;

/**
 * \brief A simulation manager interface.
 * This interface provides access to all the simulation-related
 * services.
 * Things that the simulation manager does:
 * \li registers factories for ISimulators for given simulation type
 * \li registers factories for IElements for given simulation type
 * \li creates ISimulators for IDocuments
 * \li creates IElements for ISimulators
 * \li keep track of all simulation types
 * \li keep track of all ISimulator instances
*/
class KTLINTERFACES_EXPORT ISimulationManager : public QObject {

    Q_OBJECT

public:
    /**
     * @return the static ISimulationManager instance
     */
    static ISimulationManager *self();

    virtual ~ISimulationManager();

    /**
     * Create a simulator for a document for a given simulation type
     * \param document the document for which the simulator is created
     * \param simulationType the preferred type of simulator. If the parameter is
     *      NULL, an arbitrary simulator is returned
     * \return a new simulator suitable for a given component document
     */
    virtual ISimulator *createSimulatorForDocument(
                                IComponentDocument *document,
                                QString *simulationType) = 0;

    /**
     * Return an aleardy created simulator for a given document
     * \param document the document for which the simulator should be created
     * \param simulationType a string describimg the type of the simulation
     *      or NULL in case if doesn't matter
     * \return an existing simulator for a document or NULL if there is
     * no simulator associated with the respective document.
     *  If there is no simulationType specified and more candidate simulators
     *  are available, it returns an arbityrary one
     */
    virtual ISimulator *simulatorForDocument(IComponentDocument *document,
                                            QString *simulationType) = 0;

    /**
     * registed a ISimulatorFactory. All the needed data
     * should be provided by the factory
     * \param factory the simulator factory to register
     */
    virtual void registerSimulatorFactory(ISimulatorFactory *factory) = 0;

    /**
     * register an IElementFactory in the simulation manager
     */
    virtual void registerElementFactory(IElementFactory *factory) = 0;

    /**
     * \return a list of all document mimetypes for which at least one registered
     * factory exists
     */
    virtual QList<QString> allRegisteredDocumentMimeTypeNames() =0;

    /**
     * \return a list of factories that support creation of models for different
     *  simulation types
     */
    virtual QList<IElementFactory*> factoriesForSimulationType(QString *simulationType) = 0;

    /**
     * \return all the IElement factories that have been registered
     */
    virtual QList<IElementFactory*> allRegisteredFactories() = 0;

    /**
     * @return all the simulation types for which at least one factory is registered
     */
    virtual QList<QString> allRegisteredSimulationTypes() = 0;

protected:
    ISimulationManager();

    static ISimulationManager *m_self;
};

}
#endif // ISIMULATIONMANAGER_H
