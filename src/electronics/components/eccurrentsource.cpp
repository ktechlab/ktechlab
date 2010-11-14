/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "eccurrentsource.h"

#include "variant.h"

#include "qdebug.h"

ECCurrentSource::ECCurrentSource()
	: Component(),
        m_currentSource(0.02)
{
	// m_pNNode[0]->pin().setGroundType(Pin::gt_low);

    Property * current = new Property("current", Variant::Type::Double);
	current->setCaption(tr("Current"));
	current->setUnit("A");
	current->setMinValue(-1e12);
	current->setMaxValue(1e12);
	current->setValue(0.02);
	addProperty(current);
}

ECCurrentSource::~ECCurrentSource()
{
}

void ECCurrentSource::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    Q_UNUSED(oldValue);

    if(theProperty.name() == "current"){
        m_currentSource.setCurrent(newValue.asDouble());
    } else
        qCritical() << "ECCurrentSource: unknown property has changed!" << theProperty.name();
}
