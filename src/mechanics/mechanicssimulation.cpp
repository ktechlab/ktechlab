/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "mechanicsdocument.h"
#include "mechanicsitem.h"
#include "mechanicssimulation.h"

#include <cmath>
#include <Qt/qtimer.h>

MechanicsSimulation::MechanicsSimulation( MechanicsDocument *mechanicsDocument )
	: QObject(mechanicsDocument)
{
	p_mechanicsDocument = mechanicsDocument;
	m_advanceTmr = new QTimer(this);
	connect( m_advanceTmr, SIGNAL(timeout()), this, SLOT(slotAdvance()) );
	m_advanceTmr->start(20); // it is not implemented anyway, so don't hog the CPU
}


MechanicsSimulation::~MechanicsSimulation()
{
}


void MechanicsSimulation::slotAdvance()
{
// 	if ( p_mechanicsDocument && p_mechanicsDocument->canvas() )
// 		p_mechanicsDocument->canvas()->advance();
}


RigidBody::RigidBody( MechanicsDocument *mechanicsDocument )
{
	p_mechanicsDocument = mechanicsDocument;
	p_overallParent = 0l;
}


RigidBody::~RigidBody()
{
}


bool RigidBody::addMechanicsItem( MechanicsItem *item )
{
	if ( !item || m_mechanicsItemList.contains(item) )
		return false;
	
	m_mechanicsItemList.append(item);
	findOverallParent();
	return true;
}


void RigidBody::moveBy( double dx, double dy )
{
	if (overallParent())
		overallParent()->moveBy( dx, dy );
}


void RigidBody::rotateBy( double dtheta )
{
	if (overallParent())
		overallParent()->rotateBy(dtheta);
}


bool RigidBody::findOverallParent()
{
	p_overallParent = 0l;
	if ( m_mechanicsItemList.isEmpty() )
		return false;
	
	m_mechanicsItemList.remove(0l);
	
	const MechanicsItemList::iterator end = m_mechanicsItemList.end();
	for ( MechanicsItemList::iterator it = m_mechanicsItemList.begin(); it != end; ++it )
	{
		MechanicsItem *parentItem = *it;
		MechanicsItem *parentCandidate = dynamic_cast<MechanicsItem*>((*it)->parentItem());
		
		while (parentCandidate)
		{
			parentItem = parentCandidate;
			parentCandidate = dynamic_cast<MechanicsItem*>(parentItem->parentItem());
		}
		
		if ( !p_overallParent )
			// Must be the first item to test
			p_overallParent = parentItem;
		
		if ( p_overallParent != parentItem )
		{
			p_overallParent = 0l;
			return false;
		}
	}
	return true;
}


void RigidBody::updateRigidBodyInfo()
{
	if (!p_overallParent)
		return;
	
	m_mass = p_overallParent->mechanicsInfoCombined()->mass;
	m_momentOfInertia = p_overallParent->mechanicsInfoCombined()->momentOfInertia;
}


Vector2D::Vector2D()
{
	x = 0.;
	y = 0.;
}

double Vector2D::length() const
{
	return std::sqrt( x*x + y*y );
}

RigidBodyState::RigidBodyState()
{
	angularMomentum = 0.;
}



#include "mechanicssimulation.moc"
