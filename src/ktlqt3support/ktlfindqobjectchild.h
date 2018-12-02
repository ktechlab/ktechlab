/***************************************************************************
 *   Copyright (C) 2018 by Zoltan Padrah                                   *
 *   zoltan_padrah@users.sf.net                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KTL_FIND_QOBJECT_CHILD_H__
#define KTL_FIND_QOBJECT_CHILD_H__

class QObject;

/** find first instance of type className which is a child or indirect child
 *  of the parent QObject
 */
QObject *ktlFindQObjectChild(QObject *parent, const char *className);

#endif // KTL_FIND_QOBJECT_CHILD_H__
