/**************************************************************************
*   Copyright (C) 2008 by Julian Bäume <julian@svg4all.de>                *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KTL_CORE_PRIVATE_H
#define KTL_CORE_PRIVATE_H

#include "core.h"

#include <kcomponentdata.h>
#include <QtCore/QPointer>

namespace KTechLab
{
class RunController;
class PartController;
class LanguageController;
class SessionController;
class SourceFormatterController;
class SelectionController;
class ProjectController;
class PluginController;
class UiController;
class DocumentController;

class KTLSHELL_EXPORT CorePrivate
{
public:
    CorePrivate(Core *core);
    ~CorePrivate();

    void initialize();
    QPointer<PluginController> pluginController;
    QPointer<UiController> uiController;
    QPointer<ProjectController> projectController;
    QPointer<DocumentController> documentController;
    QPointer<PartController> partController;
    QPointer<SessionController> sessionController;

    KComponentData m_componentData;
    Core *m_core;
    bool m_cleanedUp;
};

} // namespace KTechLab

#endif
