/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "itemselector.h"

#include <kurl.h>
#include <qpointer.h>
#include <qlist.h>

class Document;
class ILVItem;
class KTechlab;
class ProcessOptions;
class ProjectInfo;
class ProjectItem;
class ProjectManager;
class QDomDocument;
class QDomElement;
class QStringList;
namespace KateMDI { class ToolView; }

typedef QList<ProcessOptions> ProcessOptionsList;
typedef QList< QPointer<ProjectItem> > ProjectItemList;


class LinkerOptions
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
		
		QDomElement toDomElement( QDomDocument & doc, const KUrl & baseURL ) const;
		
		static QString hexFormatToString( HexFormat::type format );
		static HexFormat::type stringToHexFormat( const QString & hexFormat );
		
	protected:
		void domElementToLinkerOptions( const QDomElement & element, const KUrl & baseURL );
		
		QStringList m_linkedInternal;
		QStringList m_linkedExternal;
		HexFormat::type m_hexFormat;
		bool m_bOutputMapFile;
		QString m_libraryDir;
		QString m_linkerScript;
		QString m_other;
};


class ProcessingOptions
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
		
		QDomElement toDomElement( QDomDocument & doc, const KUrl & baseURL ) const;
		
		void setUseParentMicroID( bool useParentMicroID ) { m_bUseParentMicroID = useParentMicroID; }
		bool useParentMicroID() const { return m_bUseParentMicroID; }
		
	protected:
		void domElementToProcessingOptions( const QDomElement & element, const KUrl & baseURL );
		
		KUrl m_outputURL;
		QString m_microID;
		bool m_bUseParentMicroID;
};


/**
@author David Saxton
*/
class ProjectItem : public QObject, public LinkerOptions, public ProcessingOptions
{
	public:
		enum Type
		{
			ProjectType		= 1 << 0,
			FileType		= 1 << 1,
			ProgramType		= 1 << 2,
			LibraryType		= 1 << 3
		};
		enum { AllTypes = ProjectType | FileType | ProgramType | LibraryType };
		
		enum OutputType
		{
			ProgramOutput	= 1 << 0,
			ObjectOutput	= 1 << 1,
			LibraryOutput	= 1 << 2,
			UnknownOutput	= 1 << 3
		};
		enum { AllOutputs = ProgramOutput | ObjectOutput | LibraryOutput | UnknownOutput };
		
		ProjectItem( ProjectItem * parent, Type type, ProjectManager * projectManager );
		virtual ~ProjectItem();
		
		Type type() const { return m_type; }
		QString typeToString() const;
		static Type stringToType( const QString & type );
		
		void setILVItem( ILVItem * ilvItem );
		
		/**
		 * Adds the child to the list of children, and creates an ILVItem for it
		 * in the project tree view.
		 */
		void addChild( ProjectItem * child );
		ProjectItemList children() const { return m_children; }
		
		void setObjectName( const QString & name );
		QString name() const { return m_name; }
		
		/**
		 * Sets the (input) url that this project item refers to. If the output
		 * url has not yet been set, then this project item will set the output
		 * url based on this (input) url.
		 */
		void setURL( const KUrl & url );
		KUrl url() const { return m_url; }
		
		OutputType outputType() const;
		
		/**
		 * Returns a list of output urls of the children and their recursively
		 * contained children (does not include the url for this project item).
		 * @param types An OR'ed list of ProjectItem::Type values for the
		 * children.
		 * @param outputTypes An OR'ed list of ProjectItem::OutputType values
		 * for the children.
		 */
		KUrl::List childOutputURLs( unsigned types = AllTypes, unsigned outputTypes = AllOutputs ) const;
		
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
		QDomElement toDomElement( QDomDocument & doc, const KUrl & baseURL ) const;
		
		bool build( ProcessOptionsList * pol );
		void upload( ProcessOptionsList * pol );
		
		virtual void setMicroID( const QString & id );
		virtual QString microID() const;
		
		/**
		 * Searches this item and the children for an item for the given url,
		 * return null if no such item could be found.
		 */
		ProjectItem * findItem( const KUrl & url );
		
	protected:
		void domElementToItem( const QDomElement & element, const KUrl & baseURL );
		void updateILVItemPixmap();
		void updateControlChildMicroIDs();
		
		KUrl m_url;
		QString m_name;
		ProjectItemList m_children;
		Type m_type;
		
		QPointer<ILVItem> m_pILVItem;
		ProjectManager * m_pProjectManager;
		ProjectItem * m_pParent;
};


/**
@author David Saxton
*/
class ProjectInfo : public ProjectItem
{
	Q_OBJECT

	public:
		ProjectInfo( ProjectManager * projectManager );
		~ProjectInfo();
	
  	 	/**
		 * Returns the directory that the project is saved in
		 */
		QString directory() const { return m_url.directory(KUrl::IgnoreTrailingSlash); }
		
		/**
		 * Saves the project information to file, and attempts to close all
		 * open project files.
		 * @return true iff succesful
		 */
		bool saveAndClose();
		bool save();
		
		bool open( const KUrl & url );
};

/**
@short Project Management
@author David Saxton
*/
class ProjectManager : public ItemSelector
{
	Q_OBJECT
	public:
		~ProjectManager();
		static ProjectManager * self( KateMDI::ToolView * parent = 0l );
	
		static QString toolViewIdentifier() { return "ProjectManager"; }
	
		/**
		 * @return the currently open project, or NULL if no project is open.
		 */
		ProjectInfo * currentProject() const { return m_pCurrentProject; }
		
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
		void slotContextMenuRequested( const QPoint &pos );
		/**
		 * Called when a user clicks on any item in the project view
		 */
		void slotItemClicked( QTreeWidgetItem* item, int );
		
	protected:
		ProjectInfo * m_pCurrentProject;
	
	private:
		ProjectManager( KateMDI::ToolView * parent );
		static ProjectManager * m_pSelf;
};

#endif
