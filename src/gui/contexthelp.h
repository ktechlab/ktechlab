/***************************************************************************
 *   Copyright (C) 2003-2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CONTEXTHELP_H
#define CONTEXTHELP_H

#include <ui_contexthelpwidget.h>

#include <Qt/qpointer.h>
#include <ktextedit.h>

class Item;
class ContextHelp;
class RichTextEditor;

class KHTMLPart;
class KHTMLView;
class KUrl;
class QLabel;
class QTextBrowser;
class QWidgetStack;

namespace KateMDI { class ToolView; }
namespace KParts { class URLArgs; }

/**
Sidebar that provides context-sensitive help for whatever the user is currently
helping (e.g. pinouts, command references, etc). Not to be confused with
ItemEditor, which which allows editing of data specific to the selected CNItem
in a ICNDocument.

@author David Saxton
*/
class ContextHelp : public QWidget, Ui::ContextHelpWidget
{
	Q_OBJECT
	public:
		static ContextHelp * self( KateMDI::ToolView * parent = 0l );
		static QString toolViewIdentifier() { return "ContextHelp"; }
		
		~ContextHelp();
		/**
		 * Replace special tags with appropriate html formatting code.
		 */
		void parseInfo( QString &info );
		/**
		 * Used as an event filter in context help.
		 */
		virtual bool eventFilter( QObject * watched, QEvent * e );
	
	public slots:
		void slotClear();
		void slotMultipleSelected();
		void slotUpdate( Item * item );
		void setContextHelp( QString name, QString help );
		/**
		 * Set the help browser to the given location.
		 */
		void setBrowserItem( const QString & type );
		void openURL( const KUrl& url /*, const KParts::OpenUrlArguments& */ );
		
	protected slots:
		/**
		 * Called when the user clicks the "Edit" button.
		 */
		void slotEdit();
		/**
		 * Called when the user clicks the "Save" button.
		 */
		void slotSave();
		/**
		 * Called when the user clicks the "Reset" button.
		 */
		void slotEditReset();
		/**
		 * Called from the language select combo when the current selection
		 * changes.
		 */
		void setCurrentLanguage( const QString & language );
		/**
		 * Request a directory from the user for storing the context help in.
		 */
		void requestItemDescriptionsDirectory();
		
	protected:
		enum LinkType
		{
			HelpLink, ///< Context help item (that exists)
			NewHelpLink, ///< Context help item that doesn't yet exist
			ExampleLink, ///< Example circuit or flowcode
			ExternalLink  ///< External website, etc
		};
		/**
		 * Looks at url and tries to determine the link type. Will return
		 * ExternalLink if the url can not be identified as any other type.
		 */
		static LinkType extractLinkType( const KUrl & url );
		/**
		 * Adjusts the appearance of links depending on their LinkType (e.g
		 * external links are given an "external" icon, and new help links are 
		 * colored in red.
		 */
		static void addLinkTypeAppearances( QString * html );
		/**
		 * @return the physical location of the example file from an example url
		 * (e.g. "/mosfets/and.circuit" might return
		 * "/usr/share/apps/ktechlab/mosfets/and.circuit").
		 */
		static QString examplePathToFullPath( QString path );
		
		/**
		 * Saves the current editor text for the given language.
		 * @return if all ok (e.g. if the file could be written successfully).
		 */
		bool saveDescription( const QString & language );
		bool isEditChanged();
		QString m_currentLanguage;
		QString m_lastItemType;
		KHTMLPart * m_pBrowser;
		KHTMLView * m_pBrowserView;
		RichTextEditor * m_pEditor;
		
	private slots:
		/**
		 * This has to be called after itemlibrary has constructed itself
		 * and a list of languages (i.e. a little bit after the constructor).
		 */
		void slotInitializeLanguageList();
	
	private:
		ContextHelp( KateMDI::ToolView * parent );
		
		static ContextHelp * m_pSelf;
};

#endif

