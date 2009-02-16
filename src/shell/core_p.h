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

namespace KDevelop
{
class RunController;
class PartController;
class LanguageController;
class ProjectController;
class SessionController;
class SourceFormatterController;
class SelectionController;
}

namespace KTechLab
{

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
    QPointer<KDevelop::ProjectController> projectController;
    QPointer<DocumentController> documentController;

    KComponentData m_componentData;
    Core *m_core;
    bool m_cleanedUp;
};

} // namespace KTechLab

#endif
