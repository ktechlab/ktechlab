/***************************************************************************
*   Copyright (C) 2003-2005 by David Saxton <david@bluehaze.org>          *
*   Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

//#include "itemselector.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <KUrl>
#include <QPointer>
#include <QList>

class Document;
class KTechlab;
class Options;
class ProcessOptions;
class ProjectInfo;
class ProjectItem;
class ProjectManager;
class QDomDocument;
class QDomElement;
class QStringList;

typedef QList<ProcessOptions> ProcessOptionsList;
typedef QList<ProjectItem*> ProjectItemList;

/**
@short Contains info about item for ItemSelector
@author David Saxton
*/
class ILVItem : public QObject, public QTreeWidgetItem
{
    public:
        ILVItem( QTreeWidget *parent, const QString &id );
        ILVItem( QTreeWidgetItem *parent, const QString &id );

        void setProjectItem( ProjectItem * projectItem ) { m_pProjectItem = projectItem; }
        ProjectItem * projectItem() const { return m_pProjectItem; }

        QString id() const { return m_id; }

        QString key( int, bool ) const { return m_id; }
        /**
        * Set whether the item can be removed from the listview by the user
        */
        void setRemovable( bool isRemovable ) { b_isRemovable = isRemovable; }
        /**
        * Whether the item can be removed from the listview by the user
        */
        bool isRemovable() const { return b_isRemovable; }

    protected:
        QString m_id;
        bool b_isRemovable;
        ProjectItem * m_pProjectItem;
};

/** Base-class for different optional options of a ProjectItem
 *
 * Use this class to define optional options for a ProjectItem. All possible optional options
 * are stored in a OptionsList in ProjectItem class.
 *
 * This is mainly an interface to provide the correct toDomElement and fromDomElement methods
 * for any sub-classes.
 *
 * @author Julian Bäume
**/
class Options
{
    public:
        Options() {};
        virtual ~Options() {};

        virtual QDomElement toDomElement( QDomDocument & doc, const KUrl & baseURL ) const =0;
        virtual bool fromDomElement( const QDomElement & element, const KUrl & baseURL ) =0;

};

/** Provide options for an external linker program.
 *
 * These options are used by the projects c-files and provide options for the linker.
 *
**/
class LinkerOptions : Options
{
    public:
        LinkerOptions();

        class HexFormat
        {
            public:
                enum type { inhx32, inhx8m, inhx8s, inhx16 };
        };

        HexFormat::type hexFormat() const { return m_hexFormat; }
        void setHexFormat( HexFormat::type hexFormat ) { m_hexFormat = hexFormat; }

        bool outputMapFile() const { return m_bOutputMapFile; }
        void setOutputMapFile( bool outputMapFile ) { m_bOutputMapFile = outputMapFile; }

        QString libraryDir() const { return m_libraryDir; }
        void setLibraryDir( const QString & libraryDir ) { m_libraryDir = libraryDir; }

        QString linkerScript() const { return m_linkerScript; }
        void setLinkerScript( const QString & linkerScript ) { m_linkerScript = linkerScript; }

        QString linkerOther() const { return m_other; }
        void setLinkerOther( const QString & other ) { m_other = other; }

        /**
        * Used for linkable ProjectItems. Returns a list of urls of files
        * inside the project to link against. Each url is relative to the
        * project directory.
        */
        QStringList linkedInternal() const { return m_linkedInternal; }
        void setLinkedInternal( const QStringList & linkedInternal ) { m_linkedInternal = linkedInternal; }

        /**
        * Used for linkable ProjectItems. Returns a list of urls of files
        * outside the project to link against. Each url is absolute.
        */
        QStringList linkedExternal() const { return m_linkedExternal; }
        void setLinkedExternal( const QStringList & linkedExternal ) { m_linkedExternal = linkedExternal; }

        static QString hexFormatToString( HexFormat::type format );
        static HexFormat::type stringToHexFormat( const QString & hexFormat );

        virtual QDomElement toDomElement( QDomDocument & doc, const KUrl & baseURL ) const;
        virtual bool fromDomElement( const QDomElement & element, const KUrl & baseURL );

    private:

        QStringList m_linkedInternal;
        QStringList m_linkedExternal;
        HexFormat::type m_hexFormat;
        bool m_bOutputMapFile;
        QString m_libraryDir;
        QString m_linkerScript;
        QString m_other;
};

/** Provide processing options for a ProjectItem
 *
 * For some sub-classes of ProjectItem KTechLab provides ways to
 * process these items in a certain way.
 * This class stores options for these processes.
**/
class ProcessingOptions : Options
{
    public:
        ProcessingOptions();
        virtual ~ProcessingOptions();

        /**
        * Sets the output url that this item will be built into (if this is a
        * buildable item).
        */
        void setOutputURL( const KUrl & url ) { m_outputURL = url; }
        KUrl outputURL() const { return m_outputURL; }

        /**
        * Set the microprocessor id that this project item is being built for
        * (when applicable).
        */
        virtual void setMicroID( const QString & id ) { m_microID = id; }
        virtual QString microID() const { return m_microID; }


        void setUseParentMicroID( bool useParentMicroID ) { m_bUseParentMicroID = useParentMicroID; }
        bool useParentMicroID() const { return m_bUseParentMicroID; }

        virtual QDomElement toDomElement( QDomDocument & doc, const KUrl & baseURL ) const;
        virtual bool fromDomElement( const QDomElement & element, const KUrl & baseURL );

    private:

        KUrl m_outputURL;
        QString m_microID;
        bool m_bUseParentMicroID;
};

/** Base-class for all kind of items that can be part of a project.
 *
 *@author David Saxton
**/
class ProjectItem
{
    public:
        enum Type
        {
            ProjectType     = 1 << 0,
            FileType        = 1 << 1,
            ProgramType     = 1 << 2,
            LibraryType     = 1 << 3
        };
        enum { AllTypes = ProjectType | FileType | ProgramType | LibraryType };

        enum OutputType
        {
            ProgramOutput   = 1 << 0,
            ObjectOutput    = 1 << 1,
            LibraryOutput   = 1 << 2,
            UnknownOutput   = 1 << 3
        };
        enum { AllOutputs = ProgramOutput | ObjectOutput | LibraryOutput | UnknownOutput };

        static ProjectItem * createProjectItem(ProjectItem * parent, QString type, ProjectManager * manager);
        virtual ~ProjectItem();

        void setILVItem( ILVItem * ilvItem );

        /**
        * Adds the child to the list of children, and creates an ILVItem for it
        * in the project tree view.
        */
        void addChild( ProjectItem * child );
        ProjectItemList children() const { return m_children; }

        void setName( const QString & name );
        QString name() const { return m_name; }

        /**
        * Sets the (input) url that this project item refers to. If the output
        * url has not yet been set, then this project item will set the output
        * url based on this (input) url.
        */
        void setURL( const KUrl & url );
        KUrl url() const { return m_url; }

        /**
         * Returns the QString representation of this object.
         * For a ProjectItem this is pure virtual.
         * @return a QString representation of the type
        **/
        virtual QString typeToString() const =0;

        /**
        * Returns a list of output urls of the children and their recursively
        * contained children (does not include the url for this project item).
        * @param types An OR'ed list of ProjectItem::Type values for the
        * children.
        * @param outputTypes An OR'ed list of ProjectItem::OutputType values
        * for the children.
        */
        KUrl::List childOutputURLs( unsigned types = AllTypes, unsigned outputTypes = AllOutputs ) const;

        virtual QString outputExtension() const { return QString(); };
        virtual void setOutputExtension( QString & ext ) { Q_UNUSED(ext); };

        /**
        * Creates a new ProjectItem for the given url and adds it as a child.
        */
        void addFile( const KUrl & url );
        /**
        * Queries the user for a list of urls to add, and then calls addFile
        * for each url.
        */
        void addFiles();

        void addCurrentFile();
        bool closeOpenFiles();

        virtual bool build( ProcessOptionsList * pol );
        virtual void upload( ProcessOptionsList * pol );

        virtual void setMicroID( const QString & id );
        virtual QString microID() const;

        void setProcessingOptions( ProcessingOptions * options ) { m_processingOptions = options; };
        ProcessingOptions * processingOptions() { return m_processingOptions; };

        void setLinkerOptions( LinkerOptions * options ) { m_linkerOptions = options; };
        LinkerOptions * linkerOptions() { return m_linkerOptions; };

        /**
        * Searches this item and the children for an item for the given url,
        * return null if no such item could be found.
        */
        ProjectItem * findItem( const KUrl & url );

        virtual QDomElement toDomElement( QDomDocument & doc, const KUrl & baseURL ) const;
        virtual bool fromDomElement( const QDomElement & element, const KUrl & baseURL );

    protected:
        ProjectItem( ProjectItem * parent, ProjectManager * projectManager );

        /** Update the pixmap of the item in the tree
         *
         * Pure virtual function to be overwritten in the ProjectItemTypes sub-classes.
        **/
        virtual void updateILVItemPixmap()=0;
        virtual void updateControlChildMicroIDs();
        virtual inline bool shouldUpdateControlChildMicroIDs() { return false; };

        QPointer<ILVItem> m_pILVItem;
    private:
        KUrl m_url;
        QString m_name;

        ProjectManager * m_pProjectManager;
        ProjectItem * m_pParent;
        ProjectItemList m_children;

        ProcessingOptions * m_processingOptions;
        LinkerOptions * m_linkerOptions;
};

namespace ProjectItemTypes {

class Project : public ProjectItem
{
    public:
        Project( ProjectItem * parent, ProjectManager * projectManager )
            :   ProjectItem(parent,projectManager) {};
    private:
        //Does nothing, projects don't have a pixmap
        void updateILVItemPixmap() {};
        QString typeToString() const { return QString("Project"); };
};

class File : public ProjectItem
{
    public:
        File( ProjectItem * parent, ProjectManager * projectManager )
            :   ProjectItem(parent,projectManager) {};
    private:
        virtual QString outputExtension() const { return m_outputExtension; };
        virtual void setOutputExtension( QString & ext ) { m_outputExtension = ext; };

        void setURL( const KUrl & url );
        void updateILVItemPixmap();
        inline bool shouldUpdateControlChildMicroIDs() { return true; };
        QString typeToString() const { return QString("File"); };

        QString m_outputExtension;
};

class Program : public ProjectItem
{
    public:
        Program( ProjectItem * parent, ProjectManager * projectManager )
            :   ProjectItem(parent,projectManager) {};
    private:
        void updateILVItemPixmap();
        inline bool shouldUpdateControlChildMicroIDs() { return !microID().isEmpty(); };
        QString typeToString() const { return QString("Program"); };
};

class Library : public ProjectItem
{
    public:
        Library( ProjectItem * parent, ProjectManager * projectManager )
            :   ProjectItem(parent,projectManager) {};
    private:
        void updateILVItemPixmap();
        QString typeToString() const { return QString("Library"); };
};

};

/** Public interface to a project.
*
*   This class provides a public interface for a project to the project manager.
*   It implements save and load functionality for a project.
*
@author David Saxton
*/
class ProjectInfo
{
    public:
        ProjectInfo( ProjectManager * projectManager );
        ~ProjectInfo();

        /**
        * Returns the directory that the project is saved in
        */
        QString directory() const { return m_project->url().directory(false); }

        /**
        * Saves the project information to file, and attempts to close all
        * open project files.
        * @return true if successful
        */
        bool saveAndClose();
        bool save();

        bool open( const KUrl & url );

        void setName(QString name) { m_project->setName( name ); };
        QString name() { return m_project->name(); };

        void setURL(KUrl url) { m_project->setURL( url ); };
        KUrl url() { return m_project->url(); };

        void addChild(ProjectItem * child) { m_project->addChild(child); };

        ProjectItem * project() { return m_project; };

    private:
        ProjectItem * m_project;
};

/** Project Management
 *
 * This class is the controller part for project management.
 * It handles the communication between view and the model.
 *   TODO: say some more about that class here...
 *
@author David Saxton
*/
class ProjectManager : public QObject
{
    Q_OBJECT
    public:
        ~ProjectManager();
        static ProjectManager * self( QWidget * parent = 0l );

        static QString toolViewIdentifier() { return "ProjectManager"; }

        /**
        * @return the currently open project, or NULL if no project is open.
        */
        ProjectInfo * currentProject() const { return m_pCurrentProject; }

        QTreeWidget * treeWidget() { return m_treeWidget; }

        void updateActions();

    signals:
        /**
        * Emitted when an existing project is opened.
        */
        void projectOpened();
        /**
        * Emitted when the current project is closed.
        */
        void projectClosed();
        /**
        * Emitted when a new project is created.
        */
        void projectCreated();
        /**
        * Emitted when a subproject is created.
        */
        void subprojectCreated();
        /**
        * Emitted when file(s) are added to the project or a subproject.
        */
        void filesAdded();
        /**
        * Emitted when file(s) are removed from the project or a subproject.
        */
        void filesRemoved();

    public slots:
        void slotNewProject();
        void slotOpenProject();
        void slotOpenProject( const KUrl &url );
        bool slotCloseProject();
        void slotCreateSubproject();
        void slotAddFile();
        void slotAddCurrentFile();
        void slotSubprojectAddExistingFile();
        void slotSubprojectAddCurrentFile();
        void slotItemBuild();
        void slotItemUpload();
        void slotItemProcessingOptions();
        void slotRemoveSelected();
        void slotExportToMakefile();
        void slotSubprojectLinkerOptions();
        /**
        * Pops ups a project configuration dialog
        */
        void slotProjectOptions();

    private slots:
        void slotContextMenuRequested( QTreeWidgetItem *item, const QPoint &pos, int col );
        /**
        * Called when a user clicks on any item in the project view
        */
        void slotItemClicked( QTreeWidgetItem * item );

    protected:
        ProjectInfo * m_pCurrentProject;

    private:
        ProjectManager( QWidget * parent );
        static ProjectManager * m_pSelf;

        QTreeWidget * m_treeWidget;
};

#endif
