/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MECHANICSSIMULATION_H
#define MECHANICSSIMULATION_H

#include <qpointer.h>
#include <qobject.h>
#include <qlist.h>

class MechanicsItem;
class MechanicsDocument;
typedef QList<MechanicsItem*> MechanicsItemList;


/**
@short 2 dimensional vector with associated length functions et al
@author David Saxton
*/
class Vector2D
{
public:
	Vector2D();
	
	double length() const;
	double lengthSquared() const { return x*x + y*y; }
	
	double x;
	double y;
};


/**
@short State of a rigid body, in an inertial MechanicsDocument frame
@author David Saxton
*/
class RigidBodyState
{
public:
	RigidBodyState();
	
	Vector2D linearMomentum;
	double angularMomentum;
	Vector2D position; // Position of center of mass
};
		

/**
@author David Saxton
*/
class MechanicsSimulation : public QObject
{
Q_OBJECT
public:
    MechanicsSimulation( MechanicsDocument *mechanicsDocument );
    ~MechanicsSimulation();
	
	MechanicsDocument* mechanicsDocument() const { return p_mechanicsDocument; }

protected slots:
	void slotAdvance();
	
protected:
	QPointer<MechanicsDocument> p_mechanicsDocument;
	QTimer *m_advanceTmr;
};


/**
Rigid body with mass, inertia, etc. Collection of mechanics items with
functionality for moving them about, rotating, etc. Only one mother-parent
(has no parent itself, all other items are descendents) allowed.
@short Rigid body, handles MechanicsItems
@author David Saxton
*/
class RigidBody
{
public:
	RigidBody( MechanicsDocument *mechanicsDocument );
	~RigidBody();
	
	/**
	 * 
	 */
	void advance( int phase, double delta );
	/**
	 * Add the MechanicsItem to the entity.
	 * @returns true iff successful in adding
	 */
	bool addMechanicsItem( MechanicsItem *item );
	/**
	 * Pointer to the mother MechanicsItem.
	 */
	MechanicsItem *overallParent() const { return p_overallParent; }
	/**
	 * Updates the mass and the moment of inertia info
	 */
	void updateRigidBodyInfo();
	
protected:
	/**
	 * Attempt to find the overall parent.
	 * @returns false iff unsucessful (including if there are no MechanicsItems present)
	 */
	bool findOverallParent();
	/**
	 * Move the set of MechanicsItems by the given amount
	 */
	void moveBy( double dx, double dy );
	/**
	 * Rotate the set of MechanicsItems by the given amount about the center of
	 * mass.
	 * @param angle Rotate amount in radians
	 */
	void rotateBy( double dtheta );
	
	MechanicsItemList m_mechanicsItemList;
	MechanicsItem *p_overallParent;
	MechanicsDocument *p_mechanicsDocument;
	
	RigidBodyState m_rigidBodyState;
	double m_mass;
	double m_momentOfInertia;
};

#endif
