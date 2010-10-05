/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "circuitdocument.h"

#include "circuitview.h"
#include "circuitscene.h"
#include "circuitmodel.h"

#include <shell/core.h>
#include <KDebug>
#include <KLocale>
#include <QDomDocument>
#include <QDomElement>
#include <KMessageBox>
#include <QFile>
#include <QMap>
#include <KIO/NetAccess>

using namespace KTechLab;

CircuitDocumentPrivate::CircuitDocumentPrivate( CircuitDocument *doc )
    :    m_document(doc)
{
    initCircuitModel();
    circuitScene = new CircuitScene( doc, circuitModel );
}

void CircuitDocumentPrivate::initCircuitModel()
{
    QString errorMessage, tempFile;
    if ( !KIO::NetAccess::download( m_document->url(), tempFile, 0 ) ) {
        errorMessage = KIO::NetAccess::lastErrorString();
        KMessageBox::sorry( 0, i18n("Couldn't parse xml:\n%1").arg(errorMessage) );
        return;
    }
    QFile file(tempFile);
    if (!file.open(QIODevice::ReadOnly)) {
        KMessageBox::sorry( 0, i18n("Couldn't parse xml:\n%1").arg(errorMessage) );
        return;
    }
    if (file.size() == 0) {
        //return in case the file is empty
        return;
    }
    QDomDocument dom( "KTechlab" );
    if ( !dom.setContent( &file, &errorMessage ) ) {
        KMessageBox::sorry( 0, i18n("Couldn't parse xml:\n%1").arg(errorMessage) );
        file.close();
        KIO::NetAccess::removeTempFile(tempFile);
        return;
    }
    file.close();
    circuitModel = new CircuitModel( dom );
}

CircuitDocumentPrivate::~CircuitDocumentPrivate()
{
    delete circuitScene;
    delete circuitModel;
}

void CircuitDocumentPrivate::reloadFromXml()
{

}

CircuitDocument::CircuitDocument( const KUrl &url, KDevelop::Core* core )
    :   IComponentDocument( url, core ),
        d(new CircuitDocumentPrivate(this))
{

    init();
}

CircuitDocument::~CircuitDocument()
{
    delete d;
}

void CircuitDocument::init()
{
}

QString CircuitDocument::documentType() const
{
    return "Circuit";
}

IDocumentModel* CircuitDocument::documentModel() const
{
    return d->circuitModel;
}
IDocumentScene* CircuitDocument::documentScene() const
{
    return d->circuitScene;
}

QWidget* CircuitDocument::createViewWidget( QWidget* parent )
{
    CircuitView *view = new CircuitView( d->circuitScene, parent);

    return view;
}

#include "circuitdocument.moc"
