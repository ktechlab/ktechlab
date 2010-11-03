/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

// #include <cmath>
#include <kdebug.h>
// #include <qpainter.h>
//Added by qt3to4:
// #include <Q3PointArray>

// #include "circuitdocument.h"
#include "component.h"
#include "pin.h"
#include "simulator.h"

//BEGIN class Component

Component::Component()
{

}


Component::~Component() {
	removeElements();
    // delete all properties
    foreach(Property *p, m_propertyList){
        if(p)
            delete p;
    }
    m_propertyList.clear();
}

void Component::removeElements() {
	m_elementMapList.clear();
}

void Component::addProperty(Property* theProperty)
{
    if(NULL == theProperty){
        qCritical("Component::addProperty: trying to add NULL property!\n");
    }
    m_propertyList.append(theProperty);
    connect( theProperty, SIGNAL(valueChanged(QVariant,QVariant)),
             this, SLOT(propertyChanged(QVariant,QVariant)) );
}

void Component::propertyChanged(QVariant newValue, QVariant oldValue)
{
    Property *theSender = dynamic_cast<Property*>(sender());
    if(!theSender){
        qCritical() << "the sender of propertyChanged is null "
            <<" or it's not a property!";
        return;
    }
    propertyChanged(*theSender,newValue,oldValue);
}


QList< Property* > Component::properties() const
{
    return m_propertyList;
}

Property* Component::propertyByName(const QString& name) const
{
    foreach(Property *ret, m_propertyList){
        if(ret->name() == name)
            return ret;
    }
    return 0;
}


//END class Component

#include "component.moc"

