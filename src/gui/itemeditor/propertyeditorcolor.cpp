/***************************************************************************
 *   Copyright (C) 2003 Cedric Pasteur <cedric.pasteur@free.fr>            *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "propertyeditorcolor.h"
#include "colorutils.h"
#include "iteminterface.h"
#include "property.h"

#include <KColorCombo>
#include <KLocalizedString>

#include <QDebug>
#include <QKeyEvent>
#include <QLabel>
#include <QString>

// BEGIN class PropertyEditorColor
PropertyEditorColor::PropertyEditorColor(QWidget *parent, Property *property)
    : PropertySubEditor(parent, property)
{
    m_pColorCombo = ColorUtils::createColorCombo((ColorUtils::ColorScheme)property->colorScheme(), this);
    m_pColorCombo->setColor(property->value().value<QColor>());
    m_pColorCombo->resize(width(), height());
    m_pColorCombo->show();
    setWidget(m_pColorCombo);

    connect(m_pColorCombo, &KColorCombo::activated, this, &PropertyEditorColor::valueChanged);
    connect(property, qOverload<const QColor &>(&Property::valueChanged), m_pColorCombo, &KColorCombo::setColor);
}

void PropertyEditorColor::valueChanged(const QColor &color)
{
    m_property->setValue(color);
    ItemInterface::self()->setProperty(m_property);
}

bool PropertyEditorColor::eventFilter(QObject *watched, QEvent *e)
{
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent *ev = static_cast<QKeyEvent *>(e);
        if ((ev->key() == Qt::Key_Enter) | (ev->key() == Qt::Key_Space) || (ev->key() == Qt::Key_Return)) {
            // 			m_pColorCombo->animateClick();
            m_pColorCombo->showPopup();
            return true;
        }
    }
    return PropertySubEditor::eventFilter(watched, e);
}
// END class PropertyEditorColor
