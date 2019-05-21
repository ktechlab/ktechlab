/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DEPENDENTSOURCE_H
#define DEPENDENTSOURCE_H

#include "component.h"

/**
@author David Saxton
*/
class DependentSource : public Component
{
	public:
		DependentSource( ICNDocument *icnDocument, bool newItem, const char *id );
		~DependentSource() override;

	protected:
		void drawOutline( QPainter & p );
		void drawTopArrow( QPainter & p );
		void drawBottomArrow( QPainter & p );
};

/**
@short Current Controlled Current Source
@author David Saxton
*/
class ECCCCS : public DependentSource
{
	public:
		ECCCCS( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
		~ECCCCS() override;
	
		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();
	
	protected:
		void dataChanged() override;
		void drawShape( QPainter &p ) override;
	
		CCCS *m_cccs;
};

/**
@short Current Controlled Voltage Source
@author David Saxton
*/
class ECCCVS : public DependentSource
{
	public:
		ECCCVS( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
		~ECCCVS() override;
	
		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();
	
	protected:
		void dataChanged() override;
		void drawShape( QPainter &p ) override;
	
		CCVS *m_ccvs;
};

/**
@short Voltage Controlled Current Source
@author David Saxton
*/
class ECVCCS : public DependentSource
{
	public:
		ECVCCS( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
		~ECVCCS() override;
	
		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();
	
	protected:
		void dataChanged() override;
		void drawShape( QPainter &p ) override;
	
		VCCS *m_vccs;
};

/**
@short Voltage Controlled Voltage Source
@author David Saxton
*/
class ECVCVS : public DependentSource
{
	public:
		ECVCVS( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
		~ECVCVS() override;
	
		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();
	
	protected:
		void dataChanged() override;
		void drawShape( QPainter &p ) override;
	
		VCVS *m_vcvs;
};

#endif
