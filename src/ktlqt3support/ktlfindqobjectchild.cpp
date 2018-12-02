/***************************************************************************
 *   Copyright (C) 2018 by Zoltan Padrah                                   *
 *   zoltan_padrah@users.sf.net                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktlfindqobjectchild.h"

#include <qobject.h>

#include <string.h>

QObject *ktlFindQObjectChild(QObject *parent, const char *className) {
    QObjectList children = parent->children();
    Q_FOREACH(QObject *child, children) {
        if (0 == strcmp( child->metaObject()->className(), className )) {
            return child;
        }
        QObject *ret = ktlFindQObjectChild(child, className);
        if (ret) {
            return ret;
        }
    }
    return NULL;
}
