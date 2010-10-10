/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

// #include "docmanager.h"
// #include "filemetainfo.h"
// #include "textdocument.h"
#include "outputmethoddlg.h"
// #include "microlibrary.h"
// #include "projectmanager.h"

#include <kcombobox.h>
#include <kconfigskeleton.h>
#include <kdebug.h>
// #include <ktempfile.h>
#include <kurlrequester.h>
#include <QTemporaryFile>


//BEGIN class OutputMethodInfo
OutputMethodInfo::OutputMethodInfo()
{
    m_method = Method::Direct;
    m_bAddToProject = false;
}



//END class OutputMethodInfo




#include "outputmethoddlg.moc"
