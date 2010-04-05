/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *            (C) 2010 Zoltan Padrah <zoltan_padrah@users.sourceforge.net> *
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
    :   circuitModel( new CircuitModel() ),
        m_document(doc)
{
    // load only from xml file if the url is not a special url, denoting
    // empty documents
    if(!isEmptyDocumentUrl(m_document->url()))
        reloadFromXml();
    circuitScene = new CircuitScene( doc, circuitModel );
}

CircuitDocumentPrivate::~CircuitDocumentPrivate()
{
    delete circuitScene;
    delete circuitModel;
}

bool CircuitDocumentPrivate::isEmptyDocumentUrl(KUrl url)
{
    return (url.prettyUrl().compare( EMPTY_CIRCUIT_DOCUMENT_URL ) == 0);
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
            QDomNamedNodeMap attribs = element.attributes();
            QVariantMap item;
            for ( int i=0; i<attribs.count(); ++i ) {
                item[ attribs.item(i).nodeName() ] = attribs.item(i).nodeValue();
            }
            if ( tagName == "item" ) {
                circuitModel->addComponent( item );
            } else if ( tagName == "connector" ) {
                circuitModel->addConnector( item );
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

QMap< QString, QVariant > CircuitDocument::items() const
{
    return d->circuitModel->components();
}

QString CircuitDocument::documentType() const
{
    return "Circuit";
}

QWidget* CircuitDocument::createViewWidget( QWidget* parent )
{
    CircuitView *view = new CircuitView( d->circuitScene, parent);

    return view;
}

//#include "circuitdocument.moc"
