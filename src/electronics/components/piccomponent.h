/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PICCOMPONENT_H
#define PICCOMPONENT_H

#include "config.h"
#ifndef NO_GPSIM

#include "component.h"

#include <qpointer.h>
#include <qmap.h>

class Document;
class ECNode;
class GpsimProcessor;
class IOPIN;
class KTechlab;
class MicroInfo;
class MicroPackage;
class PIC_IOPORT;
class PICComponent;
class PICComponentPin;
class PicPin;
class TextDocument;

typedef QMap< int, PICComponentPin * > PICComponentPinMap;

/**
@short Electronic PIC device
@author David Saxton
*/
class PICComponent : public Component
{
	Q_OBJECT
	public:
		PICComponent( ICNDocument * icnDocument, bool newItem, const char *id = 0L );
		~PICComponent();
	
		static Item * construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem * libraryItem();
	
		virtual void buttonStateChanged( const QString &id, bool state );
		virtual bool mouseDoubleClickEvent( const EventInfo &eventInfo );
	
		void programReload();
		/**
		 * Sets up the pins, text, etc for the given PIC type. If info is null,
		 * then a generic rectangle is displayed (used when no file has been
		 * loaded yet).
		 */
		void initPackage( MicroInfo * info );
	
	public slots:
		void slotUpdateFileList();
		void slotUpdateBtns();
	
	protected slots:
		void slotCODCreationSucceeded();
		void slotCODCreationFailed();
	
	protected:
		/**
		 * Attaches all PICComponentPins to the current instance of gpsim.
		 */
		void attachPICComponentPins();
		void deletePICComponentPins();
		/**
		 * Attempts to compile the program to a symbol file, and connects the assembly
		 * finish signal to loadGpsim
		 */
		QString createSymbolFile();
		virtual void dataChanged();
		/**
		 * Initializes the PIC from the options the user has selected.
		 */
		void initPIC( bool forceReload );
	
		QPointer<GpsimProcessor> m_pGpsim;
		QString m_picFile; ///< The input program that the user selected
		QString m_symbolFile; ///< The symbol file that was generated from m_picFile
		bool m_bLoadingProgram; ///< True between createSymbolFile being called and the file being created
		PICComponentPinMap m_picComponentPinMap;
		bool m_bCreatedInitialPackage; ///< Set true once the initial package is loaded; until then, will load a package from the lastPackage data
		static QString _def_PICComponent_fileName;
};

#endif
#endif
