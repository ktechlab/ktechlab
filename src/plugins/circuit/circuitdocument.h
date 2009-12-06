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

#include <shell/partdocument.h>

namespace Sublime
{
class Document;
} // namespace Sublime

namespace KDevelop
{
class Core;
} // namespace KDevelop

namespace KTechLab
{

class CircuitDocument;
class CircuitScene;
class CircuitModel;

class CircuitDocumentPrivate
{
public:
    CircuitDocumentPrivate( CircuitDocument *doc );
    ~CircuitDocumentPrivate();

    void reloadFromXml();

    CircuitScene *circuitScene;
    CircuitModel *circuitModel;

private:
    CircuitDocument *m_document;

};

/**
 * CircuitDocument handles circuit files. It provides access to 
 * visualisation and interaction
 * and other information about .circuit files.
 * @short Circuit Document
 * @author Julian Bäume
 */
class CircuitDocument : public KDevelop::PartDocument
{
    Q_OBJECT
public:
    CircuitDocument( const KUrl &url, KDevelop::Core* core );
    virtual ~CircuitDocument();

    virtual QString documentType() const;

    QVariantMap items() const;

protected:
    virtual QWidget *createViewWidget( QWidget* parent = 0 );

private:
    void init();
    CircuitDocumentPrivate *d;
};

}
#endif

