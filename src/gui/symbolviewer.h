/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "config.h"
#ifndef NO_GPSIM

#ifndef SYMBOLVIEWER_H
#define SYMBOLVIEWER_H

#include <klistview.h>

class KComboBox;
class RegisterInfo;
class RegisterSet;
class SymbolViewer;
namespace KateMDI { class ToolView; }


/**
@author David Saxton
*/
class SymbolViewer : public QWidget
{
	Q_OBJECT
	public:
		static SymbolViewer * self( KateMDI::ToolView * parent = 0l );
		static QString toolViewIdentifier() { return "SymbolViewer"; }
		~SymbolViewer();
		
		enum Radix
		{
			Binary = 2,
			Octal = 8,
			Decimal = 10,
			Hexadecimal = 16
		};
		
		Radix valueRadix() const { return m_valueRadix; }
		
		KListView * symbolList() const { return m_pSymbolList; }
		/**
		 * Write the current properties (such as currently selected radix) to
		 * the config.
		 */
		void saveProperties( KConfig * config );
		/**
		 * Reads the properties (such as the last selected radix) from the
		 * config file.
		 */
		void readProperties( KConfig * config );
		
		void setContext( GpsimProcessor * gpsim );
		/** 
		 * Converts the value to a string for display according to the currently
		 * selected radix.
		 */
		QString toDisplayString( unsigned value ) const;
		
	signals:
		void valueRadixChanged( SymbolViewer::Radix newRadix );
		
	public slots:
		void selectRadix( int selectIndex );
		
	protected:
		GpsimProcessor *m_pGpsim;
		RegisterSet *m_pCurrentContext;
		KListView *m_pSymbolList;
		Radix m_valueRadix;

	private:
		SymbolViewer( KateMDI::ToolView * parent );
		static SymbolViewer * m_pSelf;
		KComboBox * m_pRadixCombo;
};


class SymbolViewerItem : public QObject, public KListViewItem
{
	Q_OBJECT
	public:
		SymbolViewerItem( SymbolViewer * symbolViewer, RegisterInfo * registerInfo );
		
	public slots:
		void valueChanged( unsigned newValue );
		void radixChanged();
		
	protected:
		RegisterInfo * m_pRegisterInfo;
		SymbolViewer * m_pSymbolViewer;
};

#endif

#endif
