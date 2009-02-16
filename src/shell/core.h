 /**************************************************************************
 *   Copyright (C) 2008 by Julian Bäume <julian@svg4all.de>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KTL_CORE_H
#define KTL_CORE_H

#include "ktlshellexport.h"

#include <interfaces/icore.h>

namespace KTechLab
{
class SourceFormatterController;
class SelectionController;
class CorePrivate;
class RunController;
class SessionController;
class PartController;
class UiController;
class PluginController;
class ProjectController;
class LanguageController;
class DocumentController;
class CorePrivate;

class KTLSHELL_EXPORT Core: public KDevelop::ICore
{
public:
    static Core *self();
    /**
     * initialize core components
     */
    static void initialize( );

    /** @copydoc ICore::uiController() */
    virtual KDevelop::IUiController *uiController();
    
    /** @copydoc ICore::pluginController() */
    virtual KDevelop::IPluginController *pluginController();
    
    /** @copydoc ICore::projectController() */
    virtual KDevelop::IProjectController *projectController();
    
    /** @copydoc ICore::languageController() */
    virtual KDevelop::ILanguageController *languageController();
    
    /** @copydoc ICore::partController() */
    virtual KParts::PartManager *partController();
    
    /** @copydoc ICore::documentController() */
    virtual KDevelop::IDocumentController *documentController();
    
    /** @copydoc ICore::runController() */
    virtual KDevelop::IRunController *runController();
    
    /** @copydoc ICore::sourceFormatterController() */
    virtual KDevelop::ISourceFormatterController* sourceFormatterController();
    
    /** @copydoc ICore::selectionController() */
    virtual KDevelop::ISelectionController* selectionController();
    
    /**
     * @return session controller
     */
    virtual SessionController* sessionController();
    /** @copydoc ICore::activeSession() */
    virtual KDevelop::ISession *activeSession();
    
    virtual KComponentData componentData() const;

    
    virtual PluginController *pluginControllerInternal();
    virtual DocumentController *documentControllerInternal();
    virtual UiController *uiControllerInternal();
    virtual PartController *partControllerInternal();
    virtual ProjectController *projectControllerInternal();
    virtual void cleanup();
protected:
    static Core *m_self;
    CorePrivate *d;

private:
    Core( QObject *parent = 0 );
};

}

#endif // KTL_CORE_H
