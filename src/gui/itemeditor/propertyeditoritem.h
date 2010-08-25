/***************************************************************************
 *   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>                     *
 *   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>            *
 *   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>                     *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PROPERTYEDITORITEM_H
#define PROPERTYEDITORITEM_H

#include <klistview.h>
#include <qasciidict.h>
#include <qptrlist.h>
#include <qdict.h>

#include "property.h"

#define PropertyEditorItem_BranchBoxSize 9

/** This class is a subclass of KListViewItem which is associated to a property.
    It also takes care of drawing custom contents.
 **/
 //! An item in PropertyEditorItem associated to a property
class PropertyEditorItem : public QObject, public KListViewItem
{
	Q_OBJECT
	
	public:
		typedef QAsciiDict<PropertyEditorItem> Dict;

		/**
		 * Creates a PropertyEditorItem child of \a parent, associated to
		 * \a property. Within property editor, items are created in
		 * PropertyEditor::fill(), every time the buffer is updated. It
		 * \a property has not desctiption set, its name (i.e. not i18n'ed) is
		 * reused.
		 */
		PropertyEditorItem( PropertyEditorItem *parent, Property *property );

		/**
		 * Creates PropertyEditor Top Item which is necessary for drawing all
		 * branches.
		 */
		PropertyEditorItem(KListView *parent, const QString &text);

		~PropertyEditorItem();

		/**
		 * \return property's name.
		 */
		QString name() const { return m_property->id(); }
		/**
		 * \return properties's type.
		 */
		Variant::Type::Value	type() { return m_property->type(); }
		/**
		 * \return a pointer to the property associated to this item.
		 */
		Property*	property() { return m_property;}
		/**
		 * Updates text on of this item, for current property value. If
		 * \a alsoParent is true, parent item (if present) is also updated.
		 */
		virtual void updateValue(bool alsoParent = true);

		virtual void paintFocus ( QPainter * p, const QColorGroup & cg, const QRect & r );
		
	protected slots:
		virtual void propertyValueChanged();

	protected:
		/**
		 * Reimplemented from KListViewItem to draw custom contents. Properties
		 * names are wriiten in bold if modified. Also takes care of drawing
		 * borders around the cells as well as pixmaps or colors if necessary.
		 */
		virtual void paintCell(QPainter *p, const QColorGroup & cg, int column, int width, int align);
		/**
		 * Reimplemented from KListViewItem to hide the top item.
		 */
		virtual void setup();

	private:
		Property *m_property;
};

#endif
