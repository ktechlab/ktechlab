/***************************************************************************
 *   Copyright (C) 2009 by Julian Bäume <julian@svg4all.de>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CIRCUITDOCUMENT_H
#define CIRCUITDOCUMENT_H

#include "shell/partdocument.h"

namespace Sublime
{
class Document;
} // namespace Sublime

namespace KTechLab
{
class Core;
} // namespace KTechLab

/**
 * CircuitDocument handles circuit files. It provides access to 
 * visualisation and interaction
 * and other information about .circuit files.
 * @short Circuit Document
 * @author Julian Bäume
 */
class CircuitDocument : public KTechLab::PartDocument
{
    Q_OBJECT
public:
    CircuitDocument( const KUrl &url, KTechLab::Core* core );
    virtual ~CircuitDocument();

    virtual QString documentType() const;

protected:
    virtual QWidget *createViewWidget( QWidget* parent = 0 );

private:
    void init();
};

#endif

