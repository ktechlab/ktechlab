/***************************************************************************
 *    SimulationManager implmentation                                      *
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

#include "interfaces/isimulationmanager.h"

namespace KTechLab {

    class SimulationManagerPrivate;

    /**
    * \brief Implementation of SimulationManager interface
    */
class SimulationManager : public ISimulationManager {

  public:
    /** method to create the single instance of the simulation manager */
    static void initialize();
    
    /** destructor */
    virtual ~SimulationManager();

    /** @return an existing simulator for a document or NULL if there is
     * no simulator associated with the respective document */
    virtual ISimulator *simulatorForDocument(IComponentDocument *document,
                                             QString *simulationType);
    
    /** @return a new simulator suitable for a given component document */
    virtual ISimulator *createSimulatorForDocument(IComponentDocument *document,
                                    QString *simulationType);

    virtual void registerElementFactory(IElementFactory *factory) = 0;

    virtual QList<QString> allRegisteredDocumentMimeTypeNames() =0;
    
    virtual QList<IElementFactory> factoriesForSimulationType(QString *simulationType) = 0;
    
    virtual QList<IElementFactory> allRegisteredFactories() = 0;

    virtual QList<QString> allRegisteredSimulationTypes() = 0;


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
