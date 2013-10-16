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

#include "../ktlinterfacesexport.h"

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
     * Return an aleardy created simulator of a given type for a given
     * document, or create a new one if it doesn't exist
     * \param document the document for which the simulator is created
     * \param simulationType the preferred type of simulator. If the parameter is
     *      NULL, an arbitrary simulator is returned
     * \return a new simulator suitable for a given component document
     */
    virtual ISimulator *simulatorForDocument(
                            IComponentDocument *document,
                            const QString &simulationType = QString()) = 0;

    /**
     * Destroy a simulator created for a document and simulation type.
     * @return true if a simulator has been deleted,
     *          false if no simulator has been deleted.
     */
    virtual bool destroySimulatorForDocument(
                            IComponentDocument *document,
                            const QString &simulationType = QString()) = 0;

    /**
     * registed a ISimulatorFactory. All the needed data
     * should be provided by the factory
     * \param factory the simulator factory to register
     */
    virtual void registerSimulatorFactory(ISimulatorFactory *factory) = 0;

    /**
     * unregisted a ISimulatorFactory. it should do nothing if the factory
     * is not registered or the parameter is null
     * \param factory the simulator factory to unregister
     */
    virtual void unregisterSimulatorFactory(ISimulatorFactory *factory) = 0;

    /**
     * register an IElementFactory in the simulation manager
     * \param factory the factory to register
     */
    virtual void registerElementFactory(IElementFactory *factory) = 0;

    /**
     * unregister an IElementFactory in the simulation manager
     * \param factory to register. it should be no problem if the factory is
     * null or it's not registered
     */
    virtual void unregisterElementFactory(IElementFactory *factory) = 0;

    /**
     * \return a list of all document mimetypes for which at least one registered
     * factory exists
     */
    virtual QList<QString> registeredDocumentMimeTypeNames() =0;


    /**
     * \return all the IElement factories that have been registered for a given
     *      simulation type or for all simulation types,
     *      or if they can create a specific type of element
     * \param simulationType the type of simulation for which the factory list is
     *      queried; this parameter can be optional -- in that case all the factories
     *      are returned
     * \param elementType the type of element that should be created by the
     *      returned factories
     */
    virtual QList<IElementFactory*> registeredFactories(
                            const QString &simulationType = QString(),
                            const QString &elementType = QString(),
                            const QString &documentType = QString() ) = 0;

    /**
     * @return all the simulation types for which at least one factory is registered
     */
    virtual QList<QString> registeredSimulationTypes() = 0;


protected:
    ISimulationManager();

    static ISimulationManager *m_self;
};

}
#endif // ISIMULATIONMANAGER_H
