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
         */
        bool updateSourceEvent( const QString &source );

    private:
        //keep track of opened documents which in fact are our sources
        QStringList m_sources;
};

#endif
