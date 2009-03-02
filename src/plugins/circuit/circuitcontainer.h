/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CIRCUITCONTAINER_H
#define CIRCUITCONTAINER_H

#include <Plasma/DataContainer>

namespace KDevelop
{
class IDocument;
} // namespace KDevelop

class CircuitContainer: public Plasma::DataContainer
{
    Q_OBJECT
public:
    CircuitContainer( KDevelop::IDocument *document, QObject *parent=0 );

    void setComponent( const QString &component );
};

#endif

