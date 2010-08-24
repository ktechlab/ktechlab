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
#include <interfaces/simulator/isimulationmanager.h>
#include <interfaces/simulator/ielementfactory.h>
#include "pingroup.h"
#include "interfaces/simulator/ielement.h"
#include "interfaces/simulator/iwire.h"

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
    recreateElementList();
    recreateWireList();
    splitPinsInGroups();
    /*
    splitDocumentInCircuits(); 
    foreach(Circuit *c, m_circuits){ // what kind of abstraction?
        stepSimulation();
        createElementSet();
        solveElementSet();
    }
    */
}

void CircuitTransientSimulator::recreateElementList()
{
    // clear the list
    qDeleteAll(m_allElementList);
    m_allElementList.clear();
    m_idToElement.clear();
    // cache a pointer
    ISimulationManager *simMng = ISimulationManager::self();

    QVariantMap allComponents = m_doc->components();
    foreach(QVariant componentVariant, m_doc->components() ){
        if( componentVariant.type() != QVariant::Map){
            kError() << "BUG: this component is not a QVariantMap: " << componentVariant << "\n";
            // FIXME error handling
        }
        QVariantMap componentVarMap = componentVariant.toMap();
        // get the type of the component
        if(! componentVarMap.contains("type") ){
            kError() << "BUG: a component doesn't have a \"type\" field!\n";
            // FIXME error handling
        }
        QString compType = componentVarMap.value("type").toString();

        QList<IElementFactory*> elemFactList = simMng->registeredFactories("transient", compType, "application/x-circuit");
        if( elemFactList.isEmpty() ){
            kError() << "cannot create simulation model for the component type \""
                + compType + "\": unknown component type\n";
            // FIXME error handling
        }
        // pick a factory
        IElement *element = elemFactList.first()->createElement(compType);
        // place in the list
        m_allElementList.append(element);
            // is the following line efficient?
        m_idToElement.insert(m_doc->components().key(componentVariant),
                             element);
        kDebug() << componentVarMap << "\n";
    }
    kDebug() << "created " << m_allElementList.count() << " elements\n";
}

void CircuitTransientSimulator::recreateWireList()
{
    QVariantMap allConnectors = m_doc->connectors();
    // clear the list
    qDeleteAll(m_allWireList);
    m_allWireList.clear();
    m_idToWire.clear();
    // repopulate the list
    // TODO implment
    //
    kDebug() << "created " << m_allWireList.count() << " wires\n";
}

void CircuitTransientSimulator::splitPinsInGroups()
{
    // clean up
    qDeleteAll(m_pinGroups);
    m_pinGroups.clear();
    // repopulate the list
    // TODO implement
    kDebug() << "created " << m_pinGroups.count() << " pin groups\n";
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



