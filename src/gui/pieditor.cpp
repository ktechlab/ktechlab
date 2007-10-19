/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "colorcombo.h"
#include "doublespinbox.h"
#include "pieditor.h"
#include "variant.h"

#include <kcombobox.h>
#include <klineedit.h>
#include <klocale.h>
#include <knuminput.h>
#include <kurlrequester.h>
#include <qlayout.h>

//BEGIN class PIEditor
PIEditor::PIEditor(QString id, Variant *data, QWidget *parent, const char *name)
 : QWidget(parent, name)
{
	m_id = id;
	m_data = data;
	connect(m_data,SIGNAL(valueChanged(QVariant, QVariant)),this,SLOT(valueChanged(QVariant)));
	setFocus();
	update();
	//show();
}

PIEditor::~PIEditor()
{
}

void PIEditor::valueChanged( QVariant /*variant*/)
{
}
//END class PIEditor


//BEGIN class PIBool
PIBool::PIBool(QString id, Variant *data, QWidget *parent, const char *name)
	: PIEditor( id, data, parent, name)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setAutoAdd(true);
	
	m_comboBox = new KComboBox(this);
	m_comboBox->insertItem( i18n("True"), 0);
	m_comboBox->insertItem( i18n("False"), 1);
	m_comboBox->setCurrentItem( m_data->value().toBool() ? 0 : 1);
	
	connect( m_comboBox, SIGNAL(activated(int)), this, SLOT(selectChanged(int)));
}

PIBool::~PIBool()
{
}

void PIBool::popup()
{
	m_comboBox->popup();
}

void PIBool::selectChanged( int index)
{
	emit editorDataChanged( m_id, QVariant( index == 0));
}

void PIBool::valueChanged( QVariant /*variant*/)
{
	m_comboBox->setCurrentItem( m_data->value().toBool() ? 0 : 1);
}
//END class PIBool


//BEGIN class PIColor
PIColor::PIColor(QString id, Variant *data, QWidget *parent, const char *name)
	: PIEditor(id,data,parent, name)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setAutoAdd(true);
	
	m_combo = new ColorCombo( (ColorCombo::ColorScheme)m_data->colorScheme(), this);
	m_combo->setColor(m_data->value().toColor());
	
	connect(m_combo,SIGNAL(activated(const QColor&)),this,SLOT(colorChanged(const QColor&)));
// 	connect(m_combo,SIGNAL(highlighted(const QColor&)),this,SLOT(colorChanged(const QColor&)));
}

PIColor::~PIColor()
{
}

void PIColor::popup()
{
	m_combo->popup();
}

void PIColor::colorChanged(const QColor &col)
{
	emit editorDataChanged(m_id,QVariant(col));
}

void PIColor::valueChanged( QVariant /*variant*/)
{
	m_combo->setColor(m_data->value().toColor());
}
//END class PIColor


//BEGIN class PIDouble
PIDouble::PIDouble(QString id, Variant *data, QWidget *parent, const char *name)
	: PIEditor(id,data,parent, name)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setAutoAdd(true);
	
	spin = new DoubleSpinBox(m_data->minValue(),m_data->maxValue(),m_data->minAbsValue(),m_data->value().toDouble(),m_data->unit(),this);
	
	connect(spin,SIGNAL(valueChanged(double)),this,SLOT(spinValueChanged(double)));
}

PIDouble::~PIDouble()
{
}

void PIDouble::spinValueChanged(double value)
{
	emit editorDataChanged(m_id,QVariant(value));
}

void PIDouble::valueChanged( QVariant /*variant*/)
{
	spin->setValue(m_data->value().toDouble());
}
//END class PIDouble


//BEGIN class PIFileName
PIFilename::PIFilename(QString id, Variant *data, QWidget *parent, const char *name)
	: PIEditor(id,data,parent, name)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setAutoAdd(true);
	
	m_combo = 0;
	if( m_data->allowed().count() == 0)
	{
		m_combo = new KComboBox(this);
		m_combo->insertStringList( m_data->allowed());
		m_urlreq = new KURLRequester(m_combo, this);
	}
	else m_urlreq = new KURLRequester(this);
	
	m_urlreq->setURL( m_data->value().toString());
	m_urlreq->setFilter( m_data->filter());
	
	connect(m_urlreq,SIGNAL(urlSelected(const QString&)),this,SLOT(slotURLChanged(const QString&)));
	//connect(m_urlreq,SIGNAL(openFileDialog(KURLRequester*)),this,SLOT(slotOpenFileDialog(KURLRequester*)));
}

PIFilename::~PIFilename()
{
}

void PIFilename::slotURLChanged(const QString &url)
{
	emit editorDataChanged(m_id,QVariant(url));
}

void PIFilename::valueChanged( QVariant /*variant*/)
{
	if(m_combo) m_combo->setCurrentItem( m_data->value().toString());
	m_urlreq->setURL( m_data->value().toString());
}

/* //FIXME Reintroduce this code if deciding not to go with dropping cod files
   onto the PIC componenent ?? */
//
// void PIFilename::slotOpenFileDialog(KURLRequester *kurlreq)
// {
// 	// If no file has been selected so far then it seems
// 	// to make most sense to open the dialog at the directory
// 	// of the current project if open.
// 	if(kurlreq->url() == m_data->defaultValue().toString() && !ProjectManager::self()->directory().isEmpty()) kurlreq->setURL(ProjectManager::self()->directory());
// }
//END class PIFileName


//BEGIN class PIInt
PIInt::PIInt( const QString &id, Variant *data, QWidget *parent, const char *name)
	: PIEditor( id, data, parent, name)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setAutoAdd(true);
	
	spin = new KIntSpinBox( (int)m_data->minValue(), (int)m_data->maxValue(), 1, m_data->value().toInt(), 10, this);
	
	connect( spin, SIGNAL(valueChanged(int)), this, SLOT(spinValueChanged(int)));
}

PIInt::~PIInt()
{
}

void PIInt::spinValueChanged( int value)
{
	emit editorDataChanged( m_id, QVariant(value));
}

void PIInt::valueChanged( QVariant /*variant*/)
{
	spin->setValue( m_data->value().toInt());
}
//END class PIInt


//BEGIN class PILineEdit
PILineEdit::PILineEdit(QString id, Variant *data, QWidget *parent, const char *name)
	: PIEditor( id, data, parent, name)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setAutoAdd(true);
	m_edit = new KLineEdit( m_data->value().toString() , this);
	connect(m_edit,SIGNAL(textChanged(const QString&)),this,SLOT(slotEditTextChanged()));
}


PILineEdit::~PILineEdit()
{
}

void PILineEdit::slotEditTextChanged()
{
	emit editorDataChanged(m_id,QVariant(m_edit->text()));
}

void PILineEdit::valueChanged( QVariant /*variant*/)
{
	m_edit->setText(m_data->value().toString());
}
//END class PILineEdit


//BEGIN class PIStringCombo
PIStringCombo::PIStringCombo(QString id, Variant *data, QWidget *parent, const char *name)
	: PIEditor( id, data, parent, name)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setAutoAdd(true);
	
	m_combo = new KComboBox( this);
	m_combo->insertStringList(m_data->allowed());
	m_combo->setCurrentItem(m_data->value().toString());
	const Variant::Type::Value type = m_data->type();
	m_combo->setEditable( type == Variant::Type::Combo ||
			type == Variant::Type::FileName || 
			type == Variant::Type::VarName);
	
	connect(m_combo,SIGNAL(highlighted(const QString&)),this,SLOT(slotComboChanged()));
	connect(m_combo,SIGNAL(activated(const QString&)),this,SLOT(slotComboChanged()));
}


PIStringCombo::~PIStringCombo()
{
}

void PIStringCombo::popup()
{
	m_combo->popup();
}

void PIStringCombo::slotComboChanged()
{
	emit editorDataChanged(m_id,QVariant(m_combo->currentText()));
}

void PIStringCombo::valueChanged( QVariant /*variant*/)
{
	m_combo->setCurrentItem(m_data->value().toString());
}
//END class PIStringCombo




#include "pieditor.moc"

