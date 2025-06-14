/***************************************************************************
 *   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>                     *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PROPERTYEDITORLIST_H
#define PROPERTYEDITORLIST_H

#include <KComboBox>

#include "propertysubeditor.h"

// class QStringList;
class KListBox;
class QToolButton;

class Variant;
typedef Variant Property;

class PropComboBox : public KComboBox
{
    Q_OBJECT

public:
    PropComboBox(QWidget *parent);
    ~PropComboBox() override
    {
        ;
    }

    bool eventFilter(QObject *o, QEvent *e) override;

public Q_SLOTS:
    void hideList();

protected:
    bool m_eventFilterEnabled : 1;
};

class PropertyEditorList : public PropertySubEditor
{
    Q_OBJECT

public:
    PropertyEditorList(QWidget *parent, Property *property);
    ~PropertyEditorList() override
    {
        ;
    }

    void setList(QStringList l);

protected Q_SLOTS:
    void valueChanged(const QString &text);

protected:
    PropComboBox *m_combo;
    QToolButton *m_button;
};

#endif
