/*
    Copyright (C) 2010 Julian Bäume <julian@svg4all.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef ICOMPONENTDOCUMENT_H
#define ICOMPONENTDOCUMENT_H

#include "ktlinterfacesexport.h"

#include <QObject>
#include <QUrl>

#if KDE_ENABLED
#include <shell/partdocument.h>
#endif

namespace KTechLab
{

class IDocumentScene;

class IDocumentModel;


/**
 * \short Documents containing some form of components
 *
 * This base class provides access to the \ref KTechLab::IDocumentModel representing
 * the components available in the document.
 *
 */
#if KDE_ENABLED
class KTLINTERFACES_EXPORT IComponentDocument : public KDevelop::PartDocument
#else
class KTLINTERFACES_EXPORT IComponentDocument : public QObject
#endif
{
    Q_OBJECT
public:

#if KDE_ENABLED
    IComponentDocument(const KUrl& url, KDevelop::ICore* core, const QString& preferredPart = QString());
#else
	IComponentDocument(const QUrl& url);
#endif

    /**
     * @return the IDocumentModel for this document
     */
    virtual IDocumentModel* documentModel() const =0;

    /**
     * Return the scene of the document. This is, where all
     * routing takes place.
     *
     * \return the IDocumentScene for this document
     */
    virtual IDocumentScene* documentScene() const =0;

#if !KDE_ENABLED
	/* add a fake url() method */
public:
	QUrl & url() { return m_url; }
protected:
	QUrl m_url;
#endif
};

}
#endif // ICOMPONENTDOCUMENT_H
