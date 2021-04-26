/***************************************************************************
 *   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>                     *
 *   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>                     *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "propertyeditorlist.h"
#include "iteminterface.h"
#include "property.h"

//#include <klistbox.h>
#include <KLocalizedString>

#include <QCursor>
#include <QDebug>
//#include <q3hbox.h>
#include <QHBoxLayout>
#include <QInputEvent>
#include <QLineEdit>
#include <QStringList>
#include <QToolButton>

// BEGIN class PropComboBox
PropComboBox::PropComboBox(QWidget *parent)
    : KComboBox(parent)
{
    m_eventFilterEnabled = true;
}

bool PropComboBox::eventFilter(QObject *o, QEvent *e)
{
    if (!m_eventFilterEnabled)
        return false;

    if (o == lineEdit()) {
        if (e->type() == QEvent::KeyPress) {
            QKeyEvent *ev = static_cast<QKeyEvent *>(e);
            if ((ev->key() == Qt::Key_Up || ev->key() == Qt::Key_Down) &&
                (/* ev->state()!=Qt::ControlButton */
                 (dynamic_cast<QInputEvent *>(ev))->modifiers() != Qt::ControlModifier)) {
                parentWidget()->eventFilter(o, e);
                return true;
            }
        }
    }

    return KComboBox::eventFilter(o, e);
}

void PropComboBox::hideList()
{
    lineEdit()->setFocus();
}
// END class PropComboBox

// BEGIN class PropertyEditorList
PropertyEditorList::PropertyEditorList(QWidget *parent, Property *property)
    : PropertySubEditor(parent, property)
{
    QWidget *box = new QWidget(this);
    QHBoxLayout *boxLayout = new QHBoxLayout;
    boxLayout->setMargin(0);

    m_combo = new PropComboBox(box);
    m_combo->setGeometry(frameGeometry());

    boxLayout->addWidget(m_combo);
    box->setLayout(boxLayout);

    bool isEditable = false;
    switch (property->type()) {
    case Property::Type::Port:
    case Property::Type::Pin:
    case Property::Type::PenStyle:
    case Property::Type::PenCapStyle:
    case Property::Type::SevenSegment:
    case Property::Type::KeyPad:
    case Property::Type::Select:
        isEditable = false;
        break;

    case Property::Type::String:
    case Property::Type::Multiline:
    case Property::Type::RichText:
    case Property::Type::Combo:
    case Property::Type::FileName:
    case Property::Type::VarName:
        isEditable = true;
        break;

    case Property::Type::None:
    case Property::Type::Int:
    case Property::Type::Raw:
    case Property::Type::Double:
    case Property::Type::Color:
    case Property::Type::Bool:
        // not handled by this
        break;
    }

    m_combo->setEditable(isEditable);

    m_combo->setInsertPolicy(QComboBox::InsertAtBottom);
    m_combo->setAutoCompletion(true);
    m_combo->setMinimumSize(10, 0); // to allow the combo to be resized to a small size

    m_combo->insertItems(m_combo->count(), m_property->allowed());
    // m_combo->setCurrentText( m_property->displayString() ); // 2018.12.07
    {
        QString text = m_property->displayString();
        int i = m_combo->findText(text);
        if (i != -1)
            m_combo->setCurrentIndex(i);
        else if (m_combo->isEditable())
            m_combo->setEditText(text);
        else
            m_combo->setItemText(m_combo->currentIndex(), text);
    }

    KCompletion *comp = m_combo->completionObject();
    comp->insertItems(m_property->allowed());

    setWidget(box, m_combo->lineEdit());

    connect(m_combo, qOverload<const QString &>(&PropComboBox::activated), this, &PropertyEditorList::valueChanged);
    connect(m_property, qOverload<const QString &>(&Property::valueChanged), this, [this](const QString &str) { m_combo->setCurrentItem(str); });
}

void PropertyEditorList::setList(QStringList l)
{
    // m_combo->insertStringList(l); // 2018.12.07
    m_combo->insertItems(m_combo->count(), l);
}

void PropertyEditorList::valueChanged(const QString &text)
{
    m_property->setValue(text);
    ItemInterface::self()->setProperty(m_property);
}
// END class PropertyEditorList
