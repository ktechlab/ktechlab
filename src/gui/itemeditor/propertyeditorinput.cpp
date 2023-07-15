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

#include "propertyeditorinput.h"
#include "doublespinbox.h"
#include "iteminterface.h"
#include "property.h"

#include <KLineEdit>
#include <KLocalizedString>

#include <QDebug>
//#include <qiconset.h>
#include <QIcon>
#include <QKeyEvent>
#include <QToolButton>

#include <limits.h>

// BEGIN class PropertyEditorInput
PropertyEditorInput::PropertyEditorInput(QWidget *parent, Property *property)
    : PropertySubEditor(parent, property)
{
    m_lineedit = new KLineEdit(this);
    m_lineedit->resize(width(), height());

    m_lineedit->setText(property->value().toString());
    m_lineedit->show();

    setWidget(m_lineedit);

    connect(m_lineedit, &KLineEdit::textChanged, this, &PropertyEditorInput::slotTextChanged);
    connect(m_property, qOverload<const QString &>(&Property::valueChanged), m_lineedit, &KLineEdit::setText);
}

void PropertyEditorInput::slotTextChanged(const QString &text)
{
    m_property->setValue(text);
    ItemInterface::self()->setProperty(m_property);
}
// END class PropertyEditorInput

// BEGIN class PropIntSpinBox
PropIntSpinBox::PropIntSpinBox(int lower, int upper, int step, int value, int base = 10, QWidget *parent = nullptr)
    : QSpinBox(parent)
{
    setMinimum(lower);
    setMaximum(upper);
    setSingleStep(step);
    setValue(value);
    setDisplayIntegerBase(base);
    lineEdit()->setAlignment(Qt::AlignLeft);
}

bool PropIntSpinBox::eventFilter(QObject *o, QEvent *e)
{
    if (o == lineEdit()) {
        if (e->type() == QEvent::KeyPress) {
            QKeyEvent *ev = static_cast<QKeyEvent *>(e);
            if ((ev->key() == Qt::Key_Up || ev->key() == Qt::Key_Down) && ev->modifiers() != Qt::ControlModifier) {
                parentWidget()->eventFilter(o, e);
                return true;
            }
        }
    }

    return QSpinBox::eventFilter(o, e);
}
// END class PropIntSpinBox

// BEGIN class PropertyEditorSpin
PropertyEditorSpin::PropertyEditorSpin(QWidget *parent, Property *property)
    : PropertySubEditor(parent, property)
{
    m_leaveTheSpaceForRevertButton = true;

    m_spinBox = new PropIntSpinBox((int)property->minValue(), (int)property->maxValue(), 1, 0, 10, this);

    m_spinBox->resize(width(), height());
    m_spinBox->setValue(property->value().toInt());
    m_spinBox->show();

    setWidget(m_spinBox, m_spinBox->editor());
    connect(m_spinBox, qOverload<int>(&PropIntSpinBox::valueChanged), this, &PropertyEditorSpin::valueChange);
    connect(m_property, qOverload<int>(&Property::valueChanged), m_spinBox, &PropIntSpinBox::setValue);
}

void PropertyEditorSpin::valueChange(int value)
{
    m_property->setValue(value);
    ItemInterface::self()->setProperty(m_property);
}
// END class PropertyEditorSpin

// BEGIN class PropDoubleSpinBox
PropDoubleSpinBox::PropDoubleSpinBox(double lower, double upper, double minAbs, double value, const QString &unit, QWidget *parent = nullptr)
    : DoubleSpinBox(lower, upper, minAbs, value, unit, parent)
{
    lineEdit()->setAlignment(Qt::AlignLeft);
}

bool PropDoubleSpinBox::eventFilter(QObject *o, QEvent *e)
{
    if (o == lineEdit()) {
        if (e->type() == QEvent::KeyPress) {
            QKeyEvent *ev = static_cast<QKeyEvent *>(e);
            if ((ev->key() == Qt::Key_Up || ev->key() == Qt::Key_Down) && ev->modifiers() != Qt::ControlModifier) {
                parentWidget()->eventFilter(o, e);
                return true;
            }
        }
    }

    return DoubleSpinBox::eventFilter(o, e);
}
// END class PropDoubleSpinBox

// BEGIN class PropertyEditorDblSpin
PropertyEditorDblSpin::PropertyEditorDblSpin(QWidget *parent, Property *property)
    : PropertySubEditor(parent, property)
{
    m_leaveTheSpaceForRevertButton = true;
    m_spinBox = new PropDoubleSpinBox(property->minValue(), property->maxValue(), property->minAbsValue(), property->value().toDouble(), property->unit(), this);
    m_spinBox->resize(width(), height());
    m_spinBox->show();

    setWidget(m_spinBox, m_spinBox->editor());
    connect(m_spinBox, qOverload<double>(&PropDoubleSpinBox::valueChanged), this, &PropertyEditorDblSpin::valueChange);
    connect(m_property, qOverload<double>(&Property::valueChanged), m_spinBox, &PropDoubleSpinBox::setValue);
}

void PropertyEditorDblSpin::valueChange(double value)
{
    m_property->setValue(value);
    ItemInterface::self()->setProperty(m_property);
}
// END class PropertyEditorDblSpin

// BEGIN class PropertyEditorBool
PropertyEditorBool::PropertyEditorBool(QWidget *parent, Property *property)
    : PropertySubEditor(parent, property)
{
    m_toggle = new QToolButton(this);
    m_toggle->setFocusPolicy(Qt::NoFocus);
    m_toggle->setCheckable(true);
    m_toggle->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    // 2018.12.02: see above
    // m_toggle->setTextPosition(QToolButton::Right); //js BesideIcon -didnt work before qt3.2);
    m_toggle->resize(width(), height());

    connect(m_toggle, &QToolButton::toggled, this, &PropertyEditorBool::setState);
    connect(m_property, qOverload<bool>(&Property::valueChanged), m_toggle, &QToolButton::setChecked);

    if (property->value().toBool())
        m_toggle->setChecked(true);
    else {
        m_toggle->toggle();
        m_toggle->setChecked(false);
    }

    m_toggle->show();
    setWidget(m_toggle);
    installEventFilter(this);
}

bool PropertyEditorBool::eventFilter(QObject *watched, QEvent *e)
{
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent *ev = static_cast<QKeyEvent *>(e);
        if (ev->key() == Qt::Key_Space) {
            m_toggle->toggle();
            return true;
        }
    }
    return PropertySubEditor::eventFilter(watched, e);
}

void PropertyEditorBool::setState(bool state)
{
    if (state) {
        m_toggle->setIcon(QIcon::fromTheme("dialog-ok"));
        m_toggle->setToolTip(i18n("Yes"));
    } else {
        m_toggle->setIcon(QIcon::fromTheme("dialog-cancel"));
        m_toggle->setToolTip(i18n("No"));
    }

    m_property->setValue(state);
    ItemInterface::self()->setProperty(m_property);
}
// END class PropertyEditorBool

#include "moc_propertyeditorinput.cpp"
