/******************************************************************************
 * DocumentEngine Class - Model representing documents                        *
 *                                                                            *
 * Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                        *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License as             *
 * published by the Free Software Foundation; either version 2 of             *
 * the License, or (at your option) any later version.                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.      *
 ******************************************************************************/

#ifndef DOCUMENTENGINE_H
#define DOCUMENTENGINE_H

#include <Plasma/DataEngine>

namespace KTechLab
{
    class Core;
} // namespace KTechLab{

/**
 * Handle all opened documents in KTechLab via this Engine.
 */
class DocumentEngine : public Plasma::DataEngine
{
    Q_OBJECT

    public:
        DocumentEngine( QObject* parent, const QVariantList& args );
        ~DocumentEngine();

        /**
         * Implementation of virtual Plasma::DataEngine::sources() method.
         * This provides a list of all documents sorted within projects.
         */
        QStringList sources() const;

    protected:
        /**
         * Do some initialization for the DocumentEngine
         */
        void init();

        /**
         * Handle a request to a source named \param{name}
         */
        bool sourceRequestEvent( const QString &name );

        /**
         * Update the internal data of a source named \param{name}
         * List of sources, this DataEngine provides:
         *
         * opened
         *  Some information from the DocumentController, including:
         *  - documentCount:    the number of open documents
         *  - documentList:     a QStringList containing a url for each open document
         *
         * Besides that, there are DataSources for each document in documentList providing
         * all data that is needed to visualize a document.
         */
        bool updateSourceEvent( const QString &source );

    private:
        //keep track of opened documents which in fact are our sources
        QStringList m_sources;
        //keep the pointer to our core
        KTechLab::Core *m_core;
        //we are disabled, if m_core isn't initialized (KTechLab isn't running)
        //updateSourceEvent will return false in that case
        bool m_disabled;
};

#endif
