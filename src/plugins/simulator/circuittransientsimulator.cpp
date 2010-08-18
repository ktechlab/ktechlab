/***************************************************************************
 *    KTechLab Circuit Simulator in Transient Analysis                     *
 *       Simulates circuit documents in time domain                        *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "circuittransientsimulator.h"

#include "interfaces/icomponentdocument.h"
#include "interfaces/idocumentmodel.h"


#include <kdebug.h>

using namespace KTechLab;

CircuitTransientSimulator::CircuitTransientSimulator(IComponentDocument* doc):
    ISimulator(doc)
{
    // check for the correct document type
    if(doc->documentType() != "Circuit")
    {
        kError() << "BUG: trying to simulate a non-circuit document as a circuit!\n";
        // FIXME what to do here? save all and exit?
    }
    m_doc = doc->documentModel();
    // TODO connect the dataUpdated from the document model to the simulator
}

void CircuitTransientSimulator::start()
{
    kDebug() << "start\n";
}

void CircuitTransientSimulator::pause()
{
    kDebug() << "pause\n";
}

void CircuitTransientSimulator::tooglePause()
{
    kDebug() << "togglePause\n";

}

IElement* CircuitTransientSimulator::getModelForComponent(QVariantMap* component)
{
    kDebug() << "getModelForComponent\n";
    return NULL;
}

void CircuitTransientSimulator::componentParameterChanged(QVariantMap* component)
{
    kDebug() << "componentParameterChanged signaled\n";
    /*
    the circuit state values have to be reset here, and the new state
    has to be recalculated
    */
}

void CircuitTransientSimulator::documentStructureChanged()
{
    kDebug() << "documentStructureChanged\n";
    /*
    in case of document structure change, the simulator data
    structures should be updated
    - generate Pins and Elements from Components
    - based on Connectors, connect the Pins with Wires; possibly create
        new Pins, if needed (between 2 connectors)
    - split the Circuit into smaller connected units, for example ElementSets
    - for each ElementSet:
        - group the connected Pins in CNodes
        - count the number of node in an ElementSet
        - count the number of independent sources in element set
        - allocate matrix for equations
        - define voltage and current-solution chains in order to
            find all the voltages and currents
        - define chanins to find voltages and currents on all Pins and Wires
    */
}

void CircuitTransientSimulator::simulationTimerTicked()
{
    kDebug() << "simulationTimerTicked\n";
    /*
    general algorithm for circuit simulation:
    - notify the elements about the new simulation time
    - run all the logic circuits
    - for all ElementSets
        - while not converged:
            - ask for coefficients (A and z matrix)
            - solve equations
            - back-substitute the results
    */
}



