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

#include <simulator.h>

#include "lib/simulator/simulator.h"

using namespace KTechLab;

CircuitTransientSimulator::CircuitTransientSimulator(IDocumentModel* doc):
    ISimulator(doc)
{
//    doc->
//    Simulator::self()->attachCircuit();
    printf("transient simulator created\n");
}

// FIXME quick hack for start/pause/tooglePause: all circuits run, or none

void CircuitTransientSimulator::start()
{
    kDebug() << "start\n";
    Simulator::self()->slotSetSimulating(true);
}

void CircuitTransientSimulator::pause()
{
    kDebug() << "pause\n";
    Simulator::self()->slotSetSimulating(false);
}

void CircuitTransientSimulator::tooglePause()
{
    kDebug() << "togglePause\n";
    Simulator::self()->slotSetSimulating(!Simulator::self()->isSimulating());
}

/*
IElement* CircuitTransientSimulator::getModelForComponent(QVariantMap* component)
{
    kDebug() << "getModelForComponent\n";
    return NULL;
}
*/

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

}

void CircuitTransientSimulator::dumpDebugInfo() const
{

}

#if 0
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
    m_canBeSimulated = false;
    if( ! recreateElementList() ){
        kError() << "failed to recreate element list\n";
        return;
    }
    if( ! recreateNodeList() ){
        kError() << "failed to recreate node list\n";
        return;
    }
    if( ! recreateWireList() ){
        kError() << "failed to recreate the wire list\n";
        return;
    }
    if( ! createAllPinList() ){
        kError() << "failed to recreate the list of all pins\n";
        return;
    }
    if( ! splitPinsInGroups() ){
        kError() << "failed to split pins in groups\n";
        return;
    }
    if( ! splitDocumentInElementSets() ){
        kError() << "failed to split document in element sets\n";
        return;
    }
    /*
    search for logic nodes

    foreach(Circuit *c, m_circuits){ // what kind of abstraction?
        stepSimulation();
        createElementSet();
        solveElementSet();
    }
    */
    m_canBeSimulated = true;
}

bool CircuitTransientSimulator::recreateElementList()
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
            return false;
        }
        QVariantMap componentVarMap = componentVariant.toMap();
        // get the type of the component
        if(! componentVarMap.contains("type") ){
            kError() << "BUG: a component doesn't have a \"type\" field!\n";
            return false;
        }
        QString compType = componentVarMap.value("type").toString();

        QList<IElementFactory*> elemFactList = simMng->registeredFactories("transient", compType, "application/x-circuit");
        if( elemFactList.isEmpty() ){
            kError() << "cannot create simulation model for the component type \""
                + compType + "\": unknown component type\n";
            return false;
        }
        if( elemFactList.size() > 1){
            kWarning() << "more than one model factory for component type " << compType
                << ", selecting first one \n";
        }
        // pick a factory
        IElement *element = elemFactList.first()->createElement(compType.toUtf8(), componentVarMap);
        // place in the list
        m_allElementList.append(element);
            // is the following line efficient?
        m_idToElement.insert(m_doc->components().key(componentVariant),
                             element);
        kDebug() << "created model for: " << componentVarMap << "\n";
    }
    kDebug() << "created " << m_allElementList.size() << " elements\n";
    return true;
}

bool CircuitTransientSimulator::recreateNodeList()
{
    // clear the existing list of nodes
    qDeleteAll(m_nodeList);
    m_nodeList.clear();
    m_idToNode.clear();
    // for each node...
    QVariantMap allNodes = m_doc->nodes();
    foreach(QString id, allNodes.keys()){
        QVariant nodeVariant = allNodes.value(id);
        if( nodeVariant.type() != QVariant::Map){
            kError() << "node variant doesn't contain variant map!\n";
            return false;
        }
        QVariantMap nodeMap = nodeVariant.toMap();
        // create the node
        IPin *pin = new IPin(nodeMap, id, NULL);
        // insert in the list
        m_nodeList.append(pin);
        m_idToNode.insert(id, pin);
    }
    // some statistics
    kDebug() << "created " << m_nodeList.size() << " nodes\n";
    return true;
}

bool CircuitTransientSimulator::recreateWireList()
{
    bool success;
    // clear the list
    qDeleteAll(m_allWireList);
    m_allWireList.clear();
    m_idToWire.clear();
    // repopulate the list
    QVariantMap allConnectors = m_doc->connectors();
    foreach(QString id, allConnectors.keys()){
        // test for conversion
        QVariant connectorVariant = allConnectors.value(id);
        if( connectorVariant.type() != QVariant::Map){
            kError() << "connector variant doesn't contain variant map!\n";
            return false;
        }
        QVariantMap connectorMap = connectorVariant.toMap();
        // get the endpoints
        IPin *startPin = 0;
        IPin *endPin = 0;
        bool startIsChild = variantToBool(connectorMap.value("start-node-is-child"), success);
        // kDebug() << "startIsChild: " << startIsChild;
        if( !success ){
            kError() << "can't get start-node-is-child property\n";
            return false;
        }
        if( startIsChild ){
            QString startParent = variantToString(connectorMap.value("start-node-parent"), success);
            if( !success ){
                kError() << "can't get start-node-parent property\n";
                return false;
            }
            QString startParentNodeID = variantToString(connectorMap.value("start-node-cid"), success);
            if( !success ){
                kError() << "can't get start-node-cid property\n";
                return false;
            }
            startPin = m_idToElement.value(startParent)->pinByName(startParentNodeID);
            if( startPin == 0){
                kError() << "can't get specified pin id '" << startParentNodeID <<
                    "' on element with id '" << startParent << "'\n";
                return false;
            }
        } else {
            QString startNodeID = variantToString(connectorMap.value("start-node-id"), success);
            if(!success){
                kError() << "can't get start-node-id property\n";
                return false;
            }
            if(! m_idToNode.contains(startNodeID)){
                kError() << "node list doesn't contain ID " << startNodeID << "\n";
                return false;
            }
            startPin = m_idToNode.value(startNodeID);
        }
        bool endIsChild = variantToBool(connectorMap.value("end-node-is-child"), success);
        if( !success ){
            kError() << "can't get end-node-is-child property\n";
            return false;
        }
        if(endIsChild){
            QString endParent = variantToString(connectorMap.value("end-node-parent"), success);
            if( !success ){
                kError() << "can't get start-node-parent property\n";
                return false;
            }
            QString endParentNodeID = variantToString(connectorMap.value("end-node-cid"), success);
            if( !success ){
                kError() << "can't get start-node-cid property\n";
                return false;
            }
            endPin = m_idToElement.value(endParent)->pinByName(endParentNodeID);
            if( endPin == 0){
                kError() << "can't get specified pin id '" << endParentNodeID <<
                    "' on element with id '" << endParent << "'\n";
                return false;
            }
        } else {
            QString endNodeID = variantToString(connectorMap.value("end-node-id"), success);
            if(!success){
                kError() << "can't get start-node-id property\n";
                return false;
            }
            if(! m_idToNode.contains(endNodeID)){
                kError() << "node list doesn't contain ID " << endNodeID << "\n";
                return false;
            }
            endPin = m_idToNode.value(endNodeID);
        }
        // create the wire
        IWire *wire = new IWire(startPin, endPin, connectorMap);
        // connect the nodes to the wire
        startPin->connectWire(wire);
        endPin->connectWire(wire);
        // add the wire to the list
        m_allWireList.append(wire);
        m_idToWire.insert(id, wire);
    }
    //
    kDebug() << "created " << m_allWireList.size() << " wires\n";
    return true;
}

bool CircuitTransientSimulator::createAllPinList()
{
    m_allPinList.clear();
    // pins belonging to elements
    foreach(IElement *elem, m_allElementList){
        int n = elem->pinCount();
        for(int i = 0; i<n; i++)
                m_allPinList.append(elem->pin(i));
    }
    // pins not belonging to the element
    foreach(IPin *pin, m_nodeList){
        m_allPinList.append(pin);
    }
    // stats
    kDebug() << "in total " << m_allPinList.size() << " pins are in the document\n";
    return true;
}

bool CircuitTransientSimulator::splitPinsInGroups()
{
    // clean up
    qDeleteAll(m_pinGroups);
    m_pinGroups.clear();
    // repopulate the list
        // mark all pins, initially none
    QMap<IPin*, PinGroup*> pinInGroup;
    IPin *currentPin;
    pinInGroup.clear();
    int pinNumber = m_allPinList.size();
    for(int i=0; i<pinNumber; i++){
        currentPin = m_allPinList.at(i);
        if(!pinInGroup.contains(currentPin)){
            // pin is not in any group, create a new one
            PinGroup *group = new PinGroup(currentPin);
            // add the pingroup to the list
            m_pinGroups.append(group);
            // mark all pins that belong to the group
            QList<IPin*> pinsInNewGroup = group->pins();
            foreach(IPin *otherPin, pinsInNewGroup){
                if(pinInGroup.contains(otherPin)){
                    kError() << "BUG: a pin cannot belong to two groups!\n";
                    return false;
                }
                pinInGroup.insert(otherPin, group);
            }
        }
    }
    // check for all pins
    foreach(IPin *pin, m_allPinList){
        if( ! pinInGroup.contains(pin) ){
            kError() << "BUG: found pin without a group!\n";
            return false;
        }
    }
    // status
    kDebug() << "created " << m_pinGroups.size() << " pin groups\n";
    return true;
}

bool CircuitTransientSimulator::splitDocumentInElementSets()
{
    // cleanup..
    qDeleteAll(m_allElementSetsList);
    m_allElementSetsList.clear();
    // split ...
    QMap<IElement*, ElementSet*> elementInSet;
    elementInSet.clear();
    IElement *currElement;
    int numberOfElements = m_allElementList.size();
    for(int i=0; i<numberOfElements; i++){
        currElement = m_allElementList.at(i);
        if( !elementInSet.contains(currElement) ){
            // create a new set
            ElementSet *newSet = new ElementSet(currElement,
                                                m_allElementList,
                                                m_pinGroups);
            m_allElementSetsList.append(newSet);
            //
            QList<IElement*> elementsInNewSet = newSet->elements();
            foreach(IElement* e, elementsInNewSet ){
                if( elementInSet.contains(e) ){
                    kError() << "BUG: Element in two ElementSets. Impossible!\n";
                    return false;
                }
                elementInSet.insert(e, newSet);
            }
        }
    }
    // check for unassigned elements
    foreach(IElement *e, m_allElementList){
        if(! elementInSet.contains(e) ){
            kDebug() << "BUG: Element not assigned to any element set\n";
            return false;
        }
    }
    // stats
    kDebug() << "created " << m_allElementSetsList.size() << " element sets\n";
    return true;
}

void CircuitTransientSimulator::simulationTimerTicked()
{
    kDebug() << "simulationTimerTicked\n";
    if( ! m_canBeSimulated ){
        // kDebug() << "shouldn't do anything\n";
        return;
    }
    for(int step=0; step<m_stepsPerTick; ++step){
        // advance the time
        m_timeInSimulation += m_timeStepPerTick;
        // notify the elements about the new simulation step
        for(int i=0; i<m_allElementList.size(); ++i){
            m_allElementList[i]->actOnSimulationStep(m_timeInSimulation);
        }
        // solve the new equations
        for(int i=0; i<m_allElementSetsList.size(); ++i){
            m_allElementSetsList[i]->solveEquations();
        }
        // calculate the remaining unkonwn quantities
        for(int i=0; i<m_pinGroups.size(); ++i){
            m_pinGroups[i]->calculateInternalCurrents();
        }
    }
    // save the results into the model
    for(int i=0; i<m_allPinList.size(); ++i){
        m_allPinList[i]->transferStatusToModel();
    }
}

bool CircuitTransientSimulator::variantToBool(const QVariant variant, bool &success)
{
    if(!variant.isValid()){
        kError() << "cannot convert invalid variant to bool!\n";
        success = false;
        return false;
    }
    if(variant.type() != QVariant::String){
        // XML for the looser: everything is a string...
        kError() << "cannot convert non-string variant to bool: "
            << variant << "\n";
        success = false;
        return false;
    }
    if(variant.toString() == "0"){
        success = true;
        return false;
    }
    if(variant.toString() == "1"){
        success = true;
        return true;
    }
    // got here, so it must be junk
    kError() << "junk instead of boolean value: " << variant << "\n";
    success = false;
    return false;
}

QString CircuitTransientSimulator::variantToString(const QVariant& string, bool& success)
{
    if(!string.isValid()){
        kError() << "cannot convert invalid variant to string!\n";
        success = false;
        return false;
    }
    if(string.type() != QVariant::String){
        // XML for the loose: everything is a string...
        kError() << "cannot convert non-string variant to bool: "
            << string << "\n";
        success = false;
        return QString();
    }
    success = true;
    return string.toString();
}

void KTechLab::CircuitTransientSimulator::dumpDebugInfo() const
{
    qDebug() << "\n------\n";
    qDebug() << "equations for " << m_allElementSetsList.size() << " elementSets:\n";
    foreach(ElementSet *set, m_allElementSetsList){
        set->dumpEquations();
    }
    qDebug() << "\n--!!--\n";
}
#endif

#include "circuittransientsimulator.moc"
