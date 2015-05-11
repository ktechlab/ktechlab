/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PINMAPPING_H
#define PINMAPPING_H

#include "component.h"
#include "circuiticndocument.h"
#include "icnview.h"

#include <kdialog.h>

class ECKeyPad;
class ECSevenSegment;
class MicroInfo;
class PIC_IC;
class PinMapDocument;
class PinMapView;


/**
Stores a pin mapping Pic <--> [component] where component is set by the Type
(e.g. Keypad or Seven Segment). Used for FlowCode.
@author David Saxton
*/
class PinMapping
{
	public:
		enum Type
		{
			SevenSegment,
			Keypad_4x3,
			Keypad_4x4,
			Invalid
		};
		
		/**
		 * Creates an invalid PinMapping, required by Qt templates.
		 */
		PinMapping();
		/**
		 * Creates a PinMapping with the given type.
		 */
		PinMapping( Type type );
		~PinMapping();
		
		Type type() const { return m_type; }
		
		QStringList pins() const { return m_pins; }
		void setPins( const QStringList & pins ) { m_pins = pins; }
		
	protected:
		QStringList m_pins;
		Type m_type;
};
typedef QMap< QString, PinMapping > PinMappingMap;


/**
Dialog for editing a Pin Mapping
@author David Saxton
*/
class PinMapEditor : public KDialog
{
	Q_OBJECT
	public:
		PinMapEditor( PinMapping * PinMapping, MicroInfo * Info, QWidget * parent, const char * name );
		
	protected:
		virtual void slotApply();
		virtual void slotOk();
		void savePinMapping();
		
		PinMapping * m_pPinMapping;
		PinMapDocument * m_pPinMapDocument;
		PinMapView * m_pPinMapView;
};


/**
For use with FlowParts that require a pin map (e.g. Keypad and Seven Segment).
@author David Saxton
To see a document like this: put down a PIC, clik advanced on it, then down in the pin map definitions dialog
clik add/modifify, select something in the appearing dialog, and there is is.
It's similar to a circuit.
*/
// because this document is similar to a circuit, the nodes are the electronic nodes
class PinMapDocument : public CircuitICNDocument
{
	Q_OBJECT
	public:
		PinMapDocument();
		~PinMapDocument();

		void init( const PinMapping & PinMapping, MicroInfo *microInfo );

		virtual bool isValidItem( Item *item );
		virtual bool isValidItem( const QString &itemId );

		PinMapping pinMapping() const;

		virtual void deleteSelection();

	protected:
		PinMapping::Type m_pinMappingType;
		ECKeyPad *m_pKeypad;
		ECSevenSegment *m_pSevenSegment;
		PIC_IC *m_pPicComponent;
};


/**
@author David Saxton
*/
class PinMapView : public ICNView
{
	Q_OBJECT
	public:
		PinMapView( PinMapDocument *pinMapDocument, ViewContainer *viewContainer, uint viewAreaId, const char *name = 0l );
		~PinMapView();
};


class PIC_IC : public Component
{
	public:
		PIC_IC( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
		virtual ~PIC_IC();
		
		static Item *construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();
	
		void initPackage(MicroInfo *info);
};

#endif
