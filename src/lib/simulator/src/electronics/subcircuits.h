/***************************************************************************
 *   Copyright (C) 2004 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef SUBCIRCUITS_H
#define SUBCIRCUITS_H

#include <qobject.h>

class CircuitDocument;
class ECSubcircuit;
class Subcircuits;
inline Subcircuits *subcircuits();

/**
Interface for dealing with loading / saving / etc of subcircuits
@author David Saxton
*/
class Subcircuits : public QObject
{
Q_OBJECT
public:
	~Subcircuits();
	/**
	 * Handles subcircuit creation when the user selects the subcircuit to be
	 * created.
	 * @param id Id of subcircuit; e.g. "sc/10"
	 */
	static ECSubcircuit* createSubcircuit( int id, CircuitDocument *circuitDocument, bool newItem, const char *newId );
	/**
	 * Loads a subcircuit into a subcircuit component
	 */
	static void initECSubcircuit( int subcircuitId, ECSubcircuit *ecSubcircuit );
	/**
	 * Reads in the config entries and adds the subcircuits found to the
	 * component selector
	 */
	static void loadSubcircuits();
	/**
	 * Saves the given subcircuit to the appdata dir, updates the appropriate
	 * config entries, and adds the subcircuit to the component selector.
	 */
	static void addSubcircuit( const QString &name, const QString &subcircuitXml );
	/**
	 * returns a path to the appdata dir, e.g. genFileName(2) might return
	 * ~/.kde/share/apps/ktechlab/subcircuit_2.circuit
	 */
	static QString genFileName( const int nextId );
	/**
	 * Adds the given entry to the component selector
	 */
	static void updateComponentSelector( int id, const QString &name );
	
protected slots:
	void slotItemRemoved( const QString &id );
	
private:
	Subcircuits();
	
	friend Subcircuits* subcircuits();
};


inline Subcircuits* subcircuits()
{
	static Subcircuits *_subcircuits = new Subcircuits();
	return _subcircuits;
}

#endif
