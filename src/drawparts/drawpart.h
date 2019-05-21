/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DRAWPART_H
#define DRAWPART_H

#include <item.h>

class ICNDocument;
class ItemDocument;
class LibraryItem;
class RectangularOverlay;

/**
@author David Saxton
*/
class DrawPart : public Item
{
	public:
		enum DrawAction
		{
		// Note: values are used for popup menu
			da_text = 0,
			da_line = 1,
			da_arrow = 2,
			da_rectangle = 3,
			da_ellipse = 4,
			da_image = 5
		};
		
		DrawPart( ItemDocument *itemDocument, bool newItem, const char *id );
		~DrawPart() override;

		bool canResize() const override { return true; }
		
		Variant * createProperty( const QString & id, Variant::Type::Value type ) override;
		
		Qt::PenStyle getDataPenStyle( const QString & id );
		Qt::PenCapStyle getDataPenCapStyle( const QString & id );
		
		void setDataPenStyle( const QString & id, Qt::PenStyle value );
		void setDataPenCapStyle( const QString & id, Qt::PenCapStyle value );
		
		ItemData itemData() const override;
		void restoreFromItemData( const ItemData &itemData ) override;
		
		// Convention for following functions: name is i18n'd name of style, id is the english one
		
		static QString penStyleToID( Qt::PenStyle style );
		static Qt::PenStyle idToPenStyle( const QString & id );
		static QString penCapStyleToID( Qt::PenCapStyle style );
		static Qt::PenCapStyle idToPenCapStyle( const QString & id );
		
		static QString penStyleToName( Qt::PenStyle style );
		static Qt::PenStyle nameToPenStyle( const QString & name );
		static QString penCapStyleToName( Qt::PenCapStyle style );
		static Qt::PenCapStyle nameToPenCapStyle( const QString & name );
};

#endif
