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

using namespace KTechLab;

IElement::IElement(QVariantMap* parentInModel,
                   IElementSet* elementSet,
                   int numNodes, int numVoltageSources)
{
    m_numNodes = numNodes;
    m_numVoltageSources = numVoltageSources;
    m_elemSet = elementSet;
    m_parentInModel = parentInModel;
    // TODO implement: allocate nodeIDs, m_a*, m_b* matrixes
    // elementSet.addElement(this);
}

IElement::~IElement(){
    // TODO delete everything allocated
}

QVariantMap* KTechLab::IElement::parentInModel() const
{
    return m_parentInModel;
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

int KTechLab::IElement::numNodes() const
{
    return m_numNodes;
}

int IElement::numVoltageSources() const
{
    return m_numVoltageSources;
}


unsigned int KTechLab::IElement::nodeID(const unsigned int nodeNumber) const
{
    // TODO implement
}

void KTechLab::IElement::setNodeID(const unsigned int nodeNumber,
                                   const unsigned int position)
{
    // TODO implement
}


unsigned int KTechLab::IElement::voltageSourceID(const unsigned int sourceNumber)
{
    // TODO implement
}

void KTechLab::IElement::setVoltageSourceID(const unsigned int sourceNumber,
                                            const unsigned int position)
{
    // TODO implement
}

double& KTechLab::IElement::A_g(const unsigned int i,
                                const unsigned int j)
{
    // TODO add checks and implement
    // this looks inefficient: indirect call, virtual method call, and another forwarding
    return m_elemSet->A_g(m_nodeIDs[i], m_nodeIDs[j]);
    // better:
    //return (double &)(m_ag[i][j]);
}

double& KTechLab::IElement::A_b(const unsigned int i, const unsigned int j)
{
    // TODO implement
}

double& KTechLab::IElement::A_c(const unsigned int i, const unsigned int j)
{
    // TODO implement
}

double& KTechLab::IElement::A_d(const unsigned int i, const unsigned int j)
{
    // TODO implement
}

double& KTechLab::IElement::b_i(const unsigned int i)
{
    // TODO implement
}

double& KTechLab::IElement::b_v(const unsigned int i)
{
    // TODO implement
}

