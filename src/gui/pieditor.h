/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PIEDITOR_H
#define PIEDITOR_H

#include <qwidget.h>
#include <qstring.h>
#include <qvariant.h>

class DoubleSpinBox;
class ColorCombo;
class KComboBox;
class KIntSpinBox;
class KLineEdit;
class KURLRequester;
class Variant;


/**
@author Daniel Clarke
*/
class PIEditor : public QWidget
{
	Q_OBJECT
	public:
		PIEditor(QString id, Variant *data, QWidget *parent = 0, const char *name = 0);
		~PIEditor();
		
	public slots:
		virtual void valueChanged( QVariant variant);
		
	signals:
		void editorDataChanged(const QString &id, QVariant data);
		
	protected:
		QString m_id;
		Variant *m_data;
};


/**
@author David Saxton
 */
class PIBool : public PIEditor
{
	Q_OBJECT
	public:
		PIBool( QString id, Variant *data, QWidget *parent = 0, const char *name = 0);
		~PIBool();
		
		void popup();
		
	protected slots:
		void selectChanged( int index);
		virtual void valueChanged( QVariant variant);
		
	protected:
		KComboBox *m_comboBox;
};

/**
@author Daniel Clarke
 */
class PIColor : public PIEditor
{
	Q_OBJECT
	public:
		PIColor(QString id, Variant *data, QWidget *parent = 0, const char *name = 0);
		~PIColor();
		void popup();
		
	protected slots:
		void colorChanged(const QColor &col);
		virtual void valueChanged( QVariant variant);
		
	protected:
		ColorCombo *m_combo;
};


/**
Allows the editing of double precision numerical values, using the DoubleNum widget
@author Daniel Clarke
 */
class PIDouble : public PIEditor
{
	Q_OBJECT
	public:
		PIDouble(QString id, Variant *data, QWidget *parent = 0, const char *name = 0);
		~PIDouble();
		
	protected slots:
		void spinValueChanged(double value);
		virtual void valueChanged( QVariant variant);
		
	protected:
		DoubleSpinBox *spin;
};


/**
@author Daniel Clarke
 */
class PIFilename : public PIEditor
{
	Q_OBJECT
	public:
		PIFilename(QString id, Variant *data, QWidget *parent = 0, const char *name = 0);
		~PIFilename();
		
	protected slots:
		void slotURLChanged(const QString &url);
		virtual void valueChanged( QVariant variant);
	// see comments in implementation.
	//void slotOpenFileDialog(KURLRequester *kurlreq);
		
	protected:
		KURLRequester *m_urlreq;
		KComboBox *m_combo;
};

/**
@author David Saxton
 */
class PIInt : public PIEditor
{
	Q_OBJECT
	public:
		PIInt( const QString &id, Variant *data, QWidget *parent = 0, const char *name = 0);
		~PIInt();
	
	protected slots:
		void spinValueChanged( int value);
		virtual void valueChanged( QVariant variant);
	
	protected:
		KIntSpinBox *spin;
};


/**
@author Daniel Clarke
 */
class PILineEdit : public PIEditor
{
	Q_OBJECT
	public:
		PILineEdit(QString id, Variant *data, QWidget *parent = 0, const char *name = 0);
		~PILineEdit();
		
	protected slots:
		void slotEditTextChanged();
		virtual void valueChanged( QVariant variant);
		
	protected:
		KLineEdit *m_edit;
};


/**
@author Daniel Clarke
 */
class PIStringCombo : public PIEditor
{
	Q_OBJECT
	public:
		PIStringCombo(QString id, Variant *data, QWidget *parent, const char *name = 0);
		~PIStringCombo();
		void popup();
		
	public slots:
		virtual void valueChanged( QVariant variant);
		void slotComboChanged();
		
	protected:
		KComboBox *m_combo;
	
};

#endif
