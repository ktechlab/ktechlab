/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "plvitem.h"
#include "variant.h"

#include <klistview.h>
#include <qpainter.h>
#include <qvariant.h>

//BEGIN class PLVItem
PLVItem::PLVItem(KListView *listview, const QString &id, Variant *data)
	: KListViewItem(listview, data->editorCaption())
{
	p_data = data;
	m_id = id;
	setText(1,p_data->displayString() );
	
	connect(data,SIGNAL(valueChanged(QVariant, QVariant )),this,SLOT(updateData(QVariant )));
	
	//setHeight(100);
}


PLVItem::~PLVItem()
{
}

void PLVItem::updateData(QVariant /*value*/)
{
	if (!p_data)
		return;
	setText(1,p_data->displayString() );
}


int PLVItem::width ( const QFontMetrics & fm, const QListView * lv, int c ) const
{
	if ( c == 0 )
		return 100;
// 		return KListViewItem::width( fm, lv, c );
	else
		return 200;
}
//END class PLVitem



//BEGIN class PLVColorItem
PLVColorItem::PLVColorItem(KListView *listview, const QString &id, Variant *data)
	: PLVItem(listview,id,data)
{
}


PLVColorItem::~PLVColorItem()
{
}

void PLVColorItem::paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align )
{
	if (!p_data)
		return;
	// we only draw column 1 "Data" ourselves, otherwise
	// we leave it up to KDE
	if (column == 1)
	{
		p->setBackgroundColor( p_data->value().toColor() );
		QBrush brush( p_data->value().toColor() );
		p->fillRect(QRect(0,0,listView()->columnWidth(1),KListViewItem::height()),brush);
	}
	else
		KListViewItem::paintCell(p,cg,column,width,align);
}

void PLVColorItem::updateData(QVariant value)
{
	listView()->triggerUpdate();
	PLVItem::updateData(value);
}
//END class PLVColorItem

#include "plvitem.moc"
