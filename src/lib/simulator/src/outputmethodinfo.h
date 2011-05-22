/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef OUTPUTMETHODDLG_H
#define OUTPUTMETHODDLG_H

#include "simulatorexport.h"

#include <qurl.h>

class SIMULATOR_EXPORT OutputMethodInfo
{
    public:
        class Method
        {
            public:
                enum Type
                {
                    Direct,
                    SaveAndForget,
                    SaveAndLoad
                };
        };

        OutputMethodInfo();

        Method::Type method() const { return m_method; }
        void setMethod( Method::Type method ) { m_method = method; }

        bool addToProject() const { return m_bAddToProject; }
        void setAddToProject( bool add ) { m_bAddToProject = add; }

        QString picID() const { return m_picID; }
        void setPicID( const QString & id ) { m_picID = id; }

        QUrl outputFile() const { return m_outputFile; }
        void setOutputFile( const QUrl & outputFile ) { m_outputFile = outputFile; }

    protected:
        Method::Type m_method;
        bool m_bAddToProject;
        QString m_picID;
        QUrl m_outputFile;
};

#endif
