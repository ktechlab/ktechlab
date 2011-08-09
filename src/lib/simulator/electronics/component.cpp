/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <QDebug>

#include "circuit.h"
#include "component.h"
#include "ecnode.h"
#include "elementmap.h"
#include "pin.h"
#include "simulator.h"

//BEGIN class Component

Component::Component(Circuit &ownerCircuit):
    m_circuit(ownerCircuit)
{

}


Component::~Component() {
    // pins
    foreach(ECNode *n, m_pinMap.values()){
        delete n;
    }
    
    // element maps
    foreach(ElementMap *e, m_elementMapList){
        delete e;
    }
    m_elementMapList.clear();

    // delete all properties
    foreach(Property *p, m_propertyList){
        // first disconnect
        disconnect( p, SIGNAL(valueChanged(QVariant,QVariant)),
             this, SLOT(propertyChanged(QVariant,QVariant)) );
        // then delete
        delete p;
    }
    m_propertyList.clear();
}


Circuit & Component::circuit() const
{
    return m_circuit;
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

ECNode* Component::pinByName(const QString name) const
{
    if(m_pinMap.contains(name))
        return m_pinMap.value(name);
    else
        return NULL;
}

QMap< QString, ECNode* > Component::pinMap() const
{
    return m_pinMap;
}

QList< ElementMap* > Component::elementMapList() const
{
    return m_elementMapList;
}

//END class Component

#include "component.moc"

