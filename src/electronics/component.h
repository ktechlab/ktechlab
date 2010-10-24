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
    Component();
	virtual ~Component();

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

protected:
	/**
	 * Removes all elements and switches.
	 * @param setPinsInterIndependent whether to bother calling
	 * setPinsInterIndependent. This is false when calling from the
	 * destructor, or when the dependency information is the same.
	 */
	void removeElements();

private:

	/**
	 * List of ElementMaps; which contain information on the pins associated
	 * with the element as well as the dependence between the pins for that
	 * element.
	 * @see ElementMap
	 */
	QList<ElementMap*> m_elementMapList;

};

#endif
