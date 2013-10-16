/***************************************************************************
 *    SimulationManager implementation                                      *
 *       A controller for simulations in KTechLab                          *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef SIMULATIONMANAGER_H
#define SIMULATIONMANAGER_H

#include "interfaces/simulator/isimulationmanager.h"

#include <QMap>

namespace KTechLab {

    class SimulationManagerPrivate;

    /**
    * \brief Implementation of ISimulationManager interface
    */
class SimulationManager : public ISimulationManager {

  public:
    /** method to create the single instance of the simulation manager */
    static void initialize();

    /** destructor */
    virtual ~SimulationManager();

    virtual ISimulator *simulatorForDocument(
                                        IComponentDocument *document,
                                        const QString& simulationType = QString());

    virtual bool destroySimulatorForDocument(
                            IComponentDocument *document,
                            const QString &simulationType = QString());

    virtual void registerSimulatorFactory(ISimulatorFactory *factory);

    virtual void unregisterSimulatorFactory(ISimulatorFactory *factory);

    virtual void registerElementFactory(IElementFactory *factory);

    virtual void unregisterElementFactory(IElementFactory *factory);

    virtual QList<QString> registeredDocumentMimeTypeNames();

    virtual QList<IElementFactory*> registeredFactories(
                        const QString &simulationType = QString(),
                        const QString &elementType = QString(),
                        const QString &documentType = QString() );

    virtual QList<QString> registeredSimulationTypes();


  protected:
    // singleton, so protected consctructor
    SimulationManager();
    // private data
    SimulationManagerPrivate *d;
    // the instance
    static SimulationManager *m_self;
};

}

#endif // SIMULATIONMANAGER_H
