/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PLVITEM_H
#define PLVITEM_H

#include <klistview.h>
#include <qstring.h>
#include <qguardedptr.h>
#include <qobject.h>

class Variant;

/**
@author David Saxton

PropertiesListView Item
Basic item, which holds the Variant data and Id for an item
*/
class PLVItem : public QObject, public KListViewItem
{
	Q_OBJECT
	public:
		PLVItem( KListView *listview, const QString &id, Variant * data);
		~PLVItem();
		
		QString id() const { return m_id; }
		Variant * data() const { return p_data; }
		
		virtual int width ( const QFontMetrics & fm, const QListView * lv, int c) const;
	
	public slots:
		 /**
		  * Call to change the data held by an item, and update the display
		  * accordingly.
		  */
		virtual void updateData(QVariant value);
	
	protected:
		QString m_id;
		QGuardedPtr<Variant> p_data;
};

/**
@author Daniel Clarke
 */
class PLVColorItem : public PLVItem
{
	public:
		PLVColorItem( KListView *listview, const QString &id, Variant *data);
		~PLVColorItem();
		
		void updateData(QVariant value);
		
	protected:
		virtual void paintCell( QPainter * p, const QColorGroup & cg, int column, int width, int align);
};

#endif
