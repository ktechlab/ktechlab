/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef FLOWCODEDOCUMENT_H
#define FLOWCODEDOCUMENT_H

#include "icndocument.h"

#include <qguardedptr.h>

class KTechlab;
class FlowCode;
class MicroInfo;
class PicItem;
class FlowPart;
class MicroSettings;
class TextDocument;
class QString;

typedef QValueList<FlowPart*> FlowPartList;
typedef QMap<QString, int > StringIntMap;

/**
@short View for editing FlowCode
@author David Saxton
*/
class FlowCodeDocument : public ICNDocument
{
	Q_OBJECT
	public:
		FlowCodeDocument( const QString &caption, const char *name = 0L);
		~FlowCodeDocument();
	
		virtual View *createView( ViewContainer *viewContainer, uint viewAreaId, const char *name = 0l );

		/**
		 * Returns a pointer used for the MicroSettings in this FlowCode document
		 */
		MicroSettings *microSettings() const { return m_microSettings; }
		/**
		 * Sets the type of PIC to be used. FlowCodeDocument se
		virtual void convertToMicrobe();ts the internal MicroInfo pointer to that
		 * returned by MicroLibrary for the given id. The pic type must be set before anything useful
		 * (such as compilage) can be done.
		 */
		void setPicType( const QString &id );

		enum ConvertToTarget
		{
			MicrobeOutput,
			AssemblyOutput,
			HexOutput,
			PICOutput
		};

#define protected public
	signals:
		void picTypeChanged();
#undef protected

	signals:
		void pinMappingsChanged();
	
	public slots:
		/**
		 * @param target as ConvertToTarget
		 */
		void slotConvertTo( int target );
		void convertToMicrobe();
		void convertToAssembly();
		void convertToHex();
		void convertToPIC();
		/**
		 * Called when a variable name has changed (from an entry box)
		 */
		void varNameChanged( const QString &newValue, const QString &oldValue );

	protected:
		virtual bool isValidItem( Item *item );
		virtual bool isValidItem( const QString &itemId );
		
	private slots:
		void setLastTextOutputTarget( TextDocument * target );

	private:
		QGuardedPtr<TextDocument> m_pLastTextOutputTarget;
		MicroInfo *m_microInfo; // Stores information about the PIC
		MicroSettings *m_microSettings; // Stores initial settings of the PIC
		PicItem *m_picItem; // Allows the user to change the PIC settings
		StringIntMap m_varNames;
};

#endif
