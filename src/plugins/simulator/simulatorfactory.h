/***************************************************************************
 *    KTechLab simulator plugin                                            *
 *       Factory for the simulator instance                                *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CIRCUIT_SIMULATORFACTORY_H
#define CIRCUIT_SIMULATORFACTORY_H


#include "interfaces/isimulatorfactory.h"


namespace KTechLab {

class SimulatorFactory : public ISimulatorFactory {
    Q_OBJECT

public:

    SimulatorFactory();
    
    virtual ~SimulatorFactory();

    /**
     * Get the simulation type. In this case, simulation is "transient"
     */
    virtual QString simulationType() const;

    /**
     * Document  type: only for circuits
     */
    virtual QString supportedDocumentMimeTypeName() const;

    /**
     * create circuit simulator
     * @param doc document for which a simulator is needed
     * @return the simulator associated with the document
     */
    virtual ISimulator *create(IComponentDocument *doc);

    private:
        QString m_simType;
        QString m_documentType;;
    
};

}


#endif // CIRCUIT_SIMULATORFACTORY_H
