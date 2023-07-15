/***************************************************************************
 *   Copyright (C) 2003 Cedric Pasteur <cedric.pasteur@free.fr>            *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "propertyeditorfile.h"
#include "iteminterface.h"
#include "property.h"

#include <KLineEdit>
#include <KLocalizedString>

#include <QCursor>
#include <QFileDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QString>
#include <QVariant>

#include <ktechlab_debug.h>

PropertyEditorFile::PropertyEditorFile(QWidget *parent, Property *property)
    : PropertySubEditor(parent, property)
{
    m_lineedit = new KLineEdit(this);
    m_lineedit->resize(width(), height() - 2);

    m_button = new QPushButton(i18n(" ... "), this);
    m_button->resize(height(), height() - 10);
    m_button->move(width() - m_button->width() - 1, 1);

    m_lineedit->setText(property->value().toString());
    m_lineedit->show();
    m_button->show();

    setWidget(m_lineedit);

    connect(m_button, &QPushButton::clicked, this, &PropertyEditorFile::selectFile);
    connect(property, qOverload<const QString &>(&Property::valueChanged), m_lineedit, &KLineEdit::setText);
}

void PropertyEditorFile::selectFile()
{
    const QString filePath = QFileDialog::getOpenFileName(this, i18n("Choose File"), QString(), m_property->fileFilters().toQtStyleString());
    qCDebug(KTL_LOG) << "got QString: " << filePath;
    if (filePath.isEmpty()) {
        qCDebug(KTL_LOG) << "url is not valid, not setting it";
        return;
    }

    m_property->setValue(filePath);
    ItemInterface::self()->setProperty(m_property);
}

void PropertyEditorFile::resizeEvent(QResizeEvent *ev)
{
    m_lineedit->resize(ev->size());
    m_button->move(ev->size().width() - m_button->width() - 1, 1);
}

bool PropertyEditorFile::eventFilter(QObject *watched, QEvent *e)
{
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent *ev = static_cast<QKeyEvent *>(e);
        if ((ev->key() == Qt::Key_Enter) || (ev->key() == Qt::Key_Space) || (ev->key() == Qt::Key_Return)) {
            m_button->animateClick();
            return true;
        }
    }
    return PropertySubEditor::eventFilter(watched, e);
}

#include "moc_propertyeditorfile.cpp"
