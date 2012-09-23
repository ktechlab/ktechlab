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

    // FIXME these are obsolete / need rework?

// referenced classes
class IComponentDocument;
class IDocumentModel;

/**
 * simulator for electronic circuits, in time domain
 *
 * For now it is only a proxy to the simulator located in
 * src/lib/simulator, but this simulator provides a nice interface to the
 * simulator located in the simulator library
 */
class CircuitTransientSimulator : public ISimulator
{
    Q_OBJECT
public:
    /**
     * create a simulator and associate it with a document
     */
    CircuitTransientSimulator(IDocumentModel *doc);

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
    // virtual IElement *getModelForComponent(QVariantMap *component);

    // TODO set the real:simuated time ratio and the max. time step
//
public slots:
    /**
     * Slot activate in case of the document structure has changed.
     * The simulator should rebuild its data structures.
     */
    virtual void documentStructureChanged(const QModelIndex &, const QModelIndex &);

    /**
     * Slot to be activated my the document in case of the parameters
     * of one model changes. (for example, a resistance).
     *
     * The optional parameter indicates the model for which the change
     * occured. It is 0 (NULL) in case of the component is not specified.
     */

    virtual void componentParameterChanged(QVariantMap * component = NULL);

#if 0
    /**
      Slot to be activated when some time has passed and the simulation time
      should advance
      */
    virtual void simulationTimerTicked();
#endif

    /**
     Print all the equations from the simulation
     */
    void dumpDebugInfo() const;

private:
    #if 0
    /** create a list of elements to be simulated, based on the document model
     \return true if it's successful and false un faliure
     */
    bool recreateElementList();
    /** creata a list of wires to be simulated, based on the document model
     \return true if it's successful and false un faliure
     */
    bool recreateWireList();
    /** creates a list of pins that don't belong to any element
     \return true if it's successful and false un faliure
     */
    bool recreateNodeList();
    /** creates a list containing all the pins in the current document
     \return true, if it's successful and false on faliure
     */
    bool createAllPinList();
    /** split the groups into connected groups
     \return true if it's successful and false un faliure
     */
    bool splitPinsInGroups();
    /** split the document in ElementSets, so the matrixes of these
     ElementSets can be solved independently
     \return true, if succeeds, false otherwise
     */
    bool splitDocumentInElementSets();

    // utilitary methods
    /** convert a variant containing string containing a boolean (0/1) value
        to boolean. Has error checking */
    bool variantToBool(const QVariant variant, bool& success);
    /** convert a variant containing a string to a string.
        Has error checking */
    QString variantToString(const QVariant &string, bool &success);

    /// the document model associated with the simulator
    IDocumentModel *m_doc;

    /** flag to indicate if the creation of internal data structures was successful.
        the flag is used by simulationTimerTicked, in order to try to simulate,
        or not */
    bool m_canBeSimulated;

    /** the simulation time quantity that passes between two
        simulation steps.

        Lowering this value can fix convergence problems, but it decreases
        simulation speed
        */
    double m_timeStepPerTick;
    /** number of steps to take in one tick
        */
    int m_stepsPerTick;
    /** the current time in simulation */
    double m_timeInSimulation;

    /// list of all elements in the document; it's the primary reference to the elements
    QList<IElement*> m_allElementList;
    /// list of all wires in the document; it's the primary reference to the wires
    QList<IWire *> m_allWireList;
    /// map of string ID to element
    QMap<QString, IElement*> m_idToElement;
    /// map of string ID to wire
    QMap<QString, IWire *> m_idToWire;

    /** list of pins not associated to any element;
        it's the primary reference to unassociated nodes */
    QList<IPin*> m_nodeList;
    /// map of string ID to nodes
    QMap<QString, IPin*> m_idToNode;

    /** list of all nodes in the document;
        these pointers are not primary */
    QList<IPin*> m_allPinList;

    /// list of all the groups of pins
    QList<PinGroup *> m_pinGroups;

    /** list of all ElementSets in the document */
    QList<ElementSet*> m_allElementSetsList;
    #endif
};

}

#endif // CIRCUITTRANSIENTSIMULATOR_H
