/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DEPENDENTSOURCE_H
#define DEPENDENTSOURCE_H

// #include "simplecomponent.h"
#include "component.h"

#include "cccs.h"
#include "ccvs.h"
#include "vccs.h"
#include "vcvs.h"

class Circuit;

/**
@author David Saxton
*/
class DependentSource : public Component {

public:
	DependentSource(Circuit &ownerCircuit);
	~DependentSource();

};

/**
@short Current Controlled Current Source
@author David Saxton
*/
class ECCCCS : public DependentSource {

public:
	ECCCCS(Circuit &ownerCircuit);
	~ECCCCS();

protected:
    virtual void propertyChanged(Property& theProperty,
                                 QVariant newValue, QVariant oldValue );
    //	virtual void dataChanged();
	CCCS m_cccs;
};

/**
@short Current Controlled Voltage Source
@author David Saxton
*/
class ECCCVS : public DependentSource {

public:
	ECCCVS(Circuit &ownerCircuit);
	~ECCCVS();

protected:
	//virtual void dataChanged();
    virtual void propertyChanged(Property& theProperty,
                                 QVariant newValue, QVariant oldValue );

	CCVS m_ccvs;
};

/**
@short Voltage Controlled Current Source
@author David Saxton
*/
class ECVCCS : public DependentSource {

public:
	ECVCCS(Circuit &ownerCircuit);
	~ECVCCS();


protected:
    virtual void propertyChanged(Property& theProperty,
                                 QVariant newValue, QVariant oldValue );
//	virtual void dataChanged();

	VCCS m_vccs;
};

/**
@short Voltage Controlled Voltage Source
@author David Saxton
*/
class ECVCVS : public DependentSource {

public:
	ECVCVS(Circuit &ownerCircuit);
	~ECVCVS();

protected:
    virtual void propertyChanged(Property& theProperty,
                                 QVariant newValue, QVariant oldValue );
//	virtual void dataChanged();

	VCVS m_vcvs;
};

#endif
