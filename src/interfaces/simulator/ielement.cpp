/***************************************************************************
 *    IElement interface implementation                                    *
 *       A common interface for all component models in KTechLab           *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ielement.h"

#include "ielementset.h"
#include "ipin.h"

#if KDE_ENABLED
#include "kdebug.h"
#endif
#include <QDebug>

using namespace KTechLab;

IElement::IElement(QVariantMap & parentInModel,
                   int numPins, int numNodes, int numVoltageSources,
                   QList<QString> pinNames
                   ) :
    m_numPins(numPins), m_numNodes(numNodes), m_numVoltageSources(numVoltageSources),
    m_parentInModel(parentInModel),
    m_pins()
{
    if( numPins != pinNames.size() ){
#if KDE_ENABLED
        kError() << "BUG: tried to created IElement with " << numPins << " pins, but with "
            << pinNames.size() << " pin names!\n";
#else
        qCritical() << "BUG: tried to created IElement with " << numPins << " pins, but with "
                    << pinNames.size() << " pin names!\n";
#endif
        qFatal("number of pin names is differeent from number of pins!\n");
    }
    // create the pins
    m_pins.clear();
    m_nameToPin.clear();
    for(int i=0; i<m_numPins; ++i){
        IPin *newPin = new IPin(parentInModel, pinNames.at(i), this);
        m_pins.append(newPin);
        m_nameToPin.insert(pinNames.at(i), newPin);
    }
    // mappings
    m_nodeIDs = new int[m_numNodes];
    // the node IDs are not defined, mark them invalid
    for(int i=0; i<m_numNodes; ++i)
        m_nodeIDs[i] = -1;
    m_voltageSourceIDs = new int[m_numVoltageSources];
    for(int i=0; i<m_numVoltageSources; ++i)
        m_voltageSourceIDs[i] = -1;
    // TODO implement: allocate m_a*, m_b* matrixes
}

IElement::~IElement(){
    delete [] m_nodeIDs;
    delete [] m_voltageSourceIDs;
}

QVariantMap KTechLab::IElement::parentInModel() const
{
    return m_parentInModel;
}

void IElement::setElementSet(IElementSet *elemSet)
{
    m_elemSet = elemSet;
}

IElementSet * IElement::elementSet() const
{
    return m_elemSet;
}

int KTechLab::IElement::pinCount() const
{
    return m_numPins;
}

KTechLab::IPin* KTechLab::IElement::pin(int number) const
{
    return m_pins.at(number);
}

IPin* IElement::pinByName(QString nodeName)
{
    return m_nameToPin.value(nodeName);
}

const QList< IPin* > IElement::pins() const
{
    return m_pins;
}


int KTechLab::IElement::numNodes() const
{
    return m_numNodes;
}

int IElement::numVoltageSources() const
{
    return m_numVoltageSources;
}


int KTechLab::IElement::nodeID(const int nodeNumber) const
{
    // TODO optimize
    if((nodeNumber < 0) || (nodeNumber >= m_numNodes)){
        qFatal("BUG: trying to get id of invalid node number: %d\n", nodeNumber);
        return -1;
    }
    return m_nodeIDs[nodeNumber];
}

void KTechLab::IElement::setNodeID(const int nodeNumber,
                                   const int position)
{
    // TODO optimize
    if((nodeNumber < 0) || (nodeNumber >= m_numNodes)){
        qFatal("BUG: trying to set the id of invalid node number: %d\n", nodeNumber);
    }
    if(position < 0){
        qFatal("BUG: trying to assign negative position in the matrix: %d\n", position);
    }
    m_nodeIDs[nodeNumber] = position;
}


int KTechLab::IElement::voltageSourceID(const int sourceNumber)
{
    // TODO optimize
    if((sourceNumber < 0) || (sourceNumber >= m_numVoltageSources)){
        qFatal("BUG: trying to get id of invalid voltage source number: %d\n", sourceNumber);
        return -1;
    }
    return m_voltageSourceIDs[sourceNumber];
}

void KTechLab::IElement::setVoltageSourceID(const int sourceNumber,
                                            const int position)
{
    // TODO optimize
    if((sourceNumber < 0) || (sourceNumber >= m_numVoltageSources)){
        qFatal("BUG: trying to set id of invalid voltage source number: %d\n", sourceNumber);
    }
    if(position < 0){
        qFatal("BUG: trying to set invalid position: %d\n", position);
    }
    m_voltageSourceIDs[sourceNumber] = position;
}

double& KTechLab::IElement::A_g(const int i,
                                const int j)
{
    // TODO add checks and optimize
    // this looks inefficient: indirect call, virtual method call, and another forwarding
    Q_ASSERT( (i<m_numNodes) && (j<m_numNodes) );
    return m_elemSet->A_g(m_nodeIDs[i], m_nodeIDs[j]);
    // better:
    //return (double &)(m_ag[i][j]);
}

double& KTechLab::IElement::A_b(const int i, const int j)
{
    // TODO optimize
    Q_ASSERT( (i<m_numNodes) && (j<m_numVoltageSources) );
    return m_elemSet->A_b(m_nodeIDs[i], m_voltageSourceIDs[j]);
}

double& KTechLab::IElement::A_c(const int i, const int j)
{
    // TODO optimize
    Q_ASSERT( (i<m_numVoltageSources) && (j<m_numNodes) );
    return m_elemSet->A_c(m_voltageSourceIDs[i], m_nodeIDs[j]);
}

double& KTechLab::IElement::A_d(const int i, const int j)
{
    // TODO optimize
    Q_ASSERT( (i<m_numVoltageSources) && (j<m_numVoltageSources) );
    return m_elemSet->A_d(m_voltageSourceIDs[i], m_voltageSourceIDs[j]);
}

double& KTechLab::IElement::b_i(const int i)
{
    // TODO optimize
    Q_ASSERT(i<m_numNodes);
    return m_elemSet->b_i(m_nodeIDs[i]);
}

double& KTechLab::IElement::b_v(const int i)
{
    // TODO optimize
    Q_ASSERT(i<m_numVoltageSources);
    return m_elemSet->b_v(m_voltageSourceIDs[i]);
}

