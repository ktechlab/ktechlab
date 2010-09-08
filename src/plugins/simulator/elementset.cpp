/*
    KTechLab, an IDE for electronics
    Copyright (C) 2010 Zoltan Padrah

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "elementset.h"

#include "math/matrix.h"
#include "math/quickmatrix.h"
#include <interfaces/simulator/ielement.h>
#include "pingroup.h"
#include <qset.h>
#include <qlinkedlist.h>
#include <interfaces/simulator/ipin.h>
#include <kdebug.h>

using namespace KTechLab;

KTechLab::ElementSet::ElementSet(IElement *start,
                   QList<IElement*> elements,
                   QList<PinGroup*> pinGroups)
    : IElementSet()
{
    // FIXME proper error handling
    buildElementList(start, elements, pinGroups);
    allocateMatrixes();
    assignNodeAndSourceIds();
}

void ElementSet::buildElementList(IElement *start, QList<IElement*> elements,
                   QList<PinGroup*> pinGroups )
{
    m_pinToGroup.clear();
    foreach(PinGroup *group, pinGroups){
        foreach(IPin *pin, group->pins()){
            m_pinToGroup.insert(pin, group);
        }
    }

    m_pinToElement.clear();
    foreach(IElement *elem, elements){
        foreach(IPin *pin, elem->pins()){
            m_pinToElement.insert(pin, elem);
        }
    }

    // breath first on all the elements
    QSet<IElement*> takenElements;
    QSet<PinGroup*> takenGroups;
    QLinkedList<IElement*> toBeTaken;

    takenElements.clear();
    takenElements.insert(start);
    takenGroups.clear();
    toBeTaken.clear();
    toBeTaken.append(start);

    m_elements.clear();
    m_elements.append(start);
    m_pinGroups.clear();

    while( !toBeTaken.isEmpty() ){
        IElement *currentElement = toBeTaken.takeFirst();
        // for all pins of the current element...
        int pinCount = currentElement->pinCount();
        for(int currentPin = 0; currentPin < pinCount; ++currentPin){
            PinGroup *group = m_pinToGroup.value(currentElement->pin(currentPin));
            // for all pins in the group of the current pin...
            foreach(IPin *otherPin, group->pins()){
                // get the other element
                IElement *otherElement = m_pinToElement.value(otherPin);
                if( !takenElements.contains(otherElement) ){
                    // take the other element
                    takenElements.insert(otherElement);
                    toBeTaken.append(otherElement);
                    // own data structures
                    m_elements.append(otherElement);
                    if( !takenGroups.contains(group) ){
                        // insert the group, if it's not taken already
                        takenGroups.insert(group);
                        m_pinGroups.append(group);
                    }
                }
            }
        }
    }
    // assign the element set to the elements
    foreach(IElement *elem, m_elements){
        elem->setElementSet(this);
    }
    // statistics
    kDebug() << "created elementset with " << m_elements.size() << " elements and "
        << m_pinGroups.size() << " pin groups\n";
}

void ElementSet::allocateMatrixes()
{
    m_numNodes = 0;
    m_numVoltageSources = 0;
    foreach(IElement *elem, m_elements){
        m_numNodes += elem->numNodes();
        m_numVoltageSources += elem->numVoltageSources();
    }
    // status
    qDebug() << "nodes: " << m_numNodes << " ; voltage sources: " << m_numVoltageSources << "\n";
    //
    m_a = new Matrix(m_numNodes + m_numVoltageSources);
    m_b = new QuickVector(m_numNodes + m_numVoltageSources);
    m_x = new QuickVector(m_numNodes + m_numVoltageSources);
}

void ElementSet::assignNodeAndSourceIds()
{
    // nodes: 0 .. numNodes-1
    // sources: numNodes .. numNodes + numSources - 1
    int nextNodeId = 0;
    int nextSourceId = m_numNodes;
    foreach(IElement *elem, m_elements){
        // nodes
        for(int pinNr = 0; pinNr < elem->pinCount(); ++pinNr){
            elem->setNodeID(pinNr, nextNodeId);
            ++nextNodeId;
        }
        //
        for(int sourceNr = 0; sourceNr < elem->numVoltageSources(); ++sourceNr){
            elem->setVoltageSourceID(sourceNr, nextSourceId);
            ++nextSourceId;
        }
    }
}


KTechLab::ElementSet::~ElementSet()
{
    delete m_a;
    delete m_b;
    delete m_x;
}

void KTechLab::ElementSet::solveEquations()
{
    m_a->fillWithZeroes();
    m_b->fillWithZeros();
    m_x->fillWithZeros();
    foreach(IElement *elem, m_elements){
        elem->fillMatrixCoefficients();
    }
    m_a->performLU();
    m_a->fbSub(m_b,m_x);
}

double& KTechLab::ElementSet::A_g(const unsigned int i, const unsigned int j)
{
    Q_ASSERT((i<m_numNodes) && (j<m_numNodes));
    return m_a->g(i,j);
}

double& KTechLab::ElementSet::A_b(const unsigned int i, const unsigned int j)
{
    Q_ASSERT((i<m_numNodes) && (j<m_numVoltageSources));
    return m_a->g(i,j+m_numNodes);
}

double& KTechLab::ElementSet::A_c(const unsigned int i, const unsigned int j)
{
    Q_ASSERT((i<m_numVoltageSources) && (j<m_numNodes));
    return m_a->g(i+m_numNodes,j);
}

double& KTechLab::ElementSet::A_d(const unsigned int i, const unsigned int j)
{
    Q_ASSERT((i<m_numVoltageSources) && (j<m_numVoltageSources));
    return m_a->g(i+m_numNodes, j+m_numNodes);
}

double& KTechLab::ElementSet::b_i(const unsigned int i)
{
    Q_ASSERT(i<m_numNodes);
    return (*m_b)[i];
}

double& KTechLab::ElementSet::b_v(const unsigned int i)
{
    Q_ASSERT(i<m_numVoltageSources);
    return (*m_b)[i]; // m_numNodes);
}

double& KTechLab::ElementSet::x_j(const unsigned int i)
{
    Q_ASSERT(i<m_numVoltageSources);
    return (*m_x)[i+m_numNodes];
}

double& KTechLab::ElementSet::x_v(const unsigned int i)
{
    Q_ASSERT(i<m_numNodes);
    return (*m_x)[i];
}
