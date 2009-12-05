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
    :   m_document(doc)
{
    QVariantMap args;
    args.insert( "circuitName", doc->url().prettyUrl() );
    circuit = new CircuitScene( doc, QVariantList() << args );
    reloadFromXml();
}

CircuitDocumentPrivate::~CircuitDocumentPrivate()
{
    delete circuit;
}

void CircuitDocumentPrivate::reloadFromXml()
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
    QDomDocument doc( "KTechlab" );
    if ( !doc.setContent( &file, &errorMessage ) ) {
        KMessageBox::sorry( 0, i18n("Couldn't parse xml:\n%1").arg(errorMessage) );
        file.close();
        KIO::NetAccess::removeTempFile(tempFile);
        return;
    }
    file.close();

    QDomElement root = doc.documentElement();
    QDomNode node = root.firstChild();
    while ( !node.isNull() ) {
        QDomElement element = node.toElement();
        if ( !element.isNull() ) {
            const QString tagName = element.tagName();
            if ( tagName == "item" ) {
                QDomNamedNodeMap attribs = element.attributes();
                Item item;
                for ( int i=0; i<attribs.count(); ++i ) {
                    item[ attribs.item(i).nodeName() ] = attribs.item(i).nodeValue();
                }
                items[ element.attribute("id") ] = item;
            }
        }
        node = node.nextSibling();
    }
}

CircuitDocument::CircuitDocument( const KUrl &url, KDevelop::Core* core )
    :   KDevelop::PartDocument( url, core ),
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

QWidget* CircuitDocument::createViewWidget( QWidget* parent )
{
    CircuitView *view = new CircuitView( d->circuit, parent);

    return view;
}

#include "circuitdocument.moc"
