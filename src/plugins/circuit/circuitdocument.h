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

#include <interfaces/icomponentdocument.h>

#include <QtGui/QIcon>

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

class CircuitDocumentPrivate;

/**
 * CircuitDocument handles circuit files. It provides access to
 * visualisation and interaction
 * and other information about .circuit files.
 * @short Circuit Document
 * @author Julian Bäume
 */
class CircuitDocument : public IComponentDocument
{
    Q_OBJECT
public:
#if KDE_ENABLED
    CircuitDocument( const KUrl &url, KDevelop::Core* core );
#else
	CircuitDocument( const QUrl &url);
#endif
    virtual ~CircuitDocument();

// #if !KDE_ENABLED
//	QUrl url();
//#endif

    /**
     * see \ref Sublime::UrlDocument
     */
    virtual QString documentType() const;

    /**
     * see \ref KTechLab::IComponentDocument
     */
    virtual IDocumentModel* documentModel() const;

    /**
     * see \ref KTechLab::IComponentDocument
     */
    virtual IDocumentScene* documentScene() const;

#if KDE_ENABLED
    /**
     * see \ref KDevelop::IDocument
     */
    virtual DocumentState state() const;
#endif

#if KDE_ENABLED
    /**
     * see \ref KDevelop::IDocument
     */
    virtual bool save(DocumentSaveMode mode = Default);
#endif

protected:
    /**
     * see \ref KDevelop::PartDocument
     */
    virtual QWidget *createViewWidget( QWidget* parent = 0 );

#if !KDE_ENABLED
	/* Method stubs for KDE methods */
protected:
	void notifyStateChanged() { }
public:
	void setStatusIcon(QIcon) { }
#endif

public slots:
    // should be private slot, but the private slot has issues with incomplete types,
    // so working around here
    void slotUpdateStatePrivate();

private:
    // this will fail to compile because CircuitDocumentPrivate is not completely defined
    // Q_PRIVATE_SLOT(d, void slotUpdateState())
    void init();
    friend class CircuitDocumentPrivate;
    CircuitDocumentPrivate *d;
};

}
#endif

