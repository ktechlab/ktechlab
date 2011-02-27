/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef COMPONENT_H
#define COMPONENT_H

#include <list>
#include <set>

#include <QObject>
#include <variant.h>

class Circuit;
class ECNode;
class ElementMap;

// typedef std::list<Pin*> PinList;
// typedef std::list<PinSet> PinSetList;
// typedef std::list<ElementMap> ElementMapList;

/**
@short Base class for all electrical components
@author David Saxton

  In the new version, the component should not behave as a
  controller or view, but only as a model.
*/
class Component : public QObject {
	Q_OBJECT

public:
	// Component(ICNDocument *icnDocument, bool newItem, const QString &id);
    /**
     * Create a component on a given circuit
     * @param ownerCircuit the circuit in which the component belong
     */
    Component(Circuit &ownerCircuit);

    /**
     * Destructor for component.
     */
	virtual ~Component();

    /**
     * @return the circuit in which the component is located
     */
    Circuit & circuit() const;

	/**
	 * If reinherit (and use) the stepNonLogic function, then you must also
	 * reinherit this function so that it returns true. Else your component
	 * will not get called.
     * By default, the component doesn't step non-logic.
     * @return true, if the steopNonLogic() method should be called or
     *      false, if it shouldn't be called
	 */
	virtual bool doesStepNonLogic() const {	return false; }

    /**
     * Default implmenetation of the stepNonLogic() method.
     * It does nothing.
     * This method should be overridden of the implemented component needs
     * non-logic stepping, together with \ref doesStepNonLogic.
     */
	virtual void stepNonLogic() {};

    /**
     * \return the list of all the properties of the component
     */
    QList<Property*> properties() const;

    /**
     * \return a property of this component, having a given name/
     *  If the property is not found, this method returns null.
     * \param name the name of the property
     */
    Property* propertyByName(const QString& name) const;

    /**
     * \return pin of the component, an ECNode, identified by a name
     *   If no such pin exsits, the method returns NULL
     * \param name the idenfidier of the pin, as seen by the component
     */
    ECNode* pinByName(const QString name) const;

    /**
     * \return all mapping of pin IDs to ECNodes in the current component
     */
    QMap<QString, ECNode*> pinMap() const;

    /**
     * \return the list of ElementMaps contained in this component.
     *      This is needed by the Circuit class.
     */
    QList<ElementMap*> elementMapList() const;

protected:

    /**
     * associate a property with this component
     * \param theProperty the property object
     */
    void addProperty(Property *theProperty);

    /**
     * Method that will be called when a property of a component has changed.
     * By default, this method does nothing, but it should be overridden
     * in the derived classes.
     * \param theProperty the property that has changes
     * \param newValue the new value of the property
     * \param oldValue the old value of the property
     */
    virtual void propertyChanged(Property& theProperty,
                                 QVariant newValue, QVariant oldValue )
                {
                    Q_UNUSED(theProperty);
                    Q_UNUSED(newValue);
                    Q_UNUSED(oldValue);
                }

    /**
     * all the pins owned by this component;
     * these pins will not be automatically deleted when the component is deleted,
     * because they might belong to an element
     */
    QMap<QString, ECNode*> m_pinMap;

    /**
     * List of ElementMaps; which contain information on the pins associated
     * with the element as well as the dependence between the pins for that
     * element.
     * @see ElementMap
     */
    QList<ElementMap*> m_elementMapList;

private slots:
    /**
     * Slot used by the properties, in order to signal changes of their values
     */
    void propertyChanged(QVariant newValue, QVariant oldValue );

private:


    /**
     * List of all properties of the component.
     * <br>
     * Note: because a component should have only a few (at most about 10)
     * properties, a simple list should be enough.
     */
    QList<Property*> m_propertyList;

    /// the circuit where the component is located
    Circuit &m_circuit;
};

#endif
