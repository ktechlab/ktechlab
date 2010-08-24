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

#ifndef CIRCUITTRANSIENTSIMULATOR_H
#define CIRCUITTRANSIENTSIMULATOR_H

#include "interfaces/simulator/isimulator.h"

namespace KTechLab {

class PinGroup;

class IWire;

    // referenced classes
    class IComponentDocument;
    class IDocumentModel;
    
/**
 * simulator for electronic circuits, in time domain
 */
class CircuitTransientSimulator : public ISimulator
{
    Q_OBJECT
public:
    /**
     * create a simulator and associate it with a document
     */
    CircuitTransientSimulator(IComponentDocument *doc);

    /**
     * start the simulation
     */
    virtual void start();

    /**
     * pause the simulation. if it's paused, do nothing
     */
    virtual void pause();

    /**
     * change the state of paused/running of the simulator
     */
    virtual void tooglePause();

    /**
     * @return the IElement associated with a component
     */
    virtual IElement *getModelForComponent(QVariantMap *component);

    // TODO set the real:simuated time ratio and the max. time step
//
public slots:
    /**
     * Slot activate in case of the document structure has changed.
     * The simulator should rebuild its data structures.
     */
    virtual void documentStructureChanged();

    /**
     * Slot to be activated my the document in case of the parameters
     * of one model changes. (for example, a resistance).
     *
     * The optional parameter indicates the model for which the change
     * occured. It is 0 (NULL) in case of the component is not specified.
     */

    virtual void componentParameterChanged(QVariantMap * component = NULL);

    /**
      Slot to be activated when some time has passed and the simulation time
      should advance
      */
    virtual void simulationTimerTicked();
    
private:
    /** create a list of elements to be simulated, based on the document model */
    void recreateElementList();
    /** creata a list of wires to be simulated, based on the document model */
    void recreateWireList();
    /** split the groups into connected groups */
    void splitPinsInGroups();
    // utilitary methods
    /** convert a variant containing string containing a boolean (0/1) value
        to boolean. Has error checking */
    bool variantToBool(const QVariant &variant, bool &success);
    /** convert a variant containing a string to a string.
        Has error checking */
    QString variantToString(const QVariant &string, bool &success);
    
    /// the document model associated with the simulator
    IDocumentModel *m_doc;
    
    /// list of all elements in the document
    QList<IElement*> m_allElementList;
    /// list of all wires in the document
    QList<IWire *> m_allWireList;
    /// map of string ID to element
    QMap<QString, IElement*> m_idToElement;
    /// map of string ID to wire
    QMap<QString, IWire *> m_idToWire;
    
    /// list of all the groups of pins
    QList<PinGroup *> m_pinGroups;
};

}

#endif // CIRCUITTRANSIENTSIMULATOR_H
