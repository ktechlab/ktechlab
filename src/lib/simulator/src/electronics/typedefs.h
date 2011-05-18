/***************************************************************************
 *   Copyright (C) 2010 by Zoltan Padrah                                   *
 *   zoltan_padrah@users.sourceforge.net                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

/**
 \brief File containgin definitions of various list/vector/set typedefs
 This is useful because:
 - the typedefs are defined only once
 - the typedefs are defined on one central place
 - compilation speed will be good, because this file is not very long
    or complex
*/

#ifndef _ELECTR_TYPEDEFS_H_
#define _ELECTR_TYPEDEFS_H_

#include <QList>

#include <set>

class Pin;

typedef QList<Pin*> PinList;
typedef std::set<Pin *> PinSet;

class Wire;

typedef QList<Wire*> WireList;
// typedef std::set<Wire *> WireList;

class Connector;

typedef QList<Connector*> ConnectorList;

class Element;

typedef std::set<Element*> ElementList;


#endif // _ELECTR_TYPEDEFS_H_
