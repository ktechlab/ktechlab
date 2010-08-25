/***************************************************************************
 *   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>                     *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PROPERTYSUBEDITOR_H
#define PROPERTYSUBEDITOR_H

#include <qwidget.h>

class Variant;
typedef Variant Property;

//! The base class for all editors used in PropertyEditor.

class PropertySubEditor : public QWidget
{
	Q_OBJECT

	public:
		PropertySubEditor( QWidget * parent, Property * property, const char * name = 0 );
		virtual ~PropertySubEditor();

		virtual bool eventFilter( QObject * watched, QEvent * e );
		Property * property() const { return m_property; }

		/**
		 * Sets \a w as editor 's widget, ie the widget which events are
		 * filtered and which is resized. If \a focusProxy is not 0, it will be
		 * used as focus proxy instead of \a w.
		*/
		void setWidget(QWidget *w, QWidget* focusProxy = 0);
		/**
		 * \sa m_leaveTheSpaceForRevertButton description.
		 */
		bool leavesTheSpaceForRevertButton() const { return m_leaveTheSpaceForRevertButton; }

	protected:
		virtual void resizeEvent(QResizeEvent *ev);

		Property * m_property;
		QWidget * m_childWidget;
		/**
		 * true if there should be left space at the right hand for the Revert
		 * Button. false by default. Integer editor (spinbox) sets this to true
		 * to avoid spin arrows clicking inconvenience.
		 */
		bool m_leaveTheSpaceForRevertButton;
};

#endif
