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

#ifndef PROPERTYEDITORINPUT_H
#define PROPERTYEDITORINPUT_H

#include "doublespinbox.h"

#include <QSpinBox>

#include "propertysubeditor.h"

class KLineEdit;
class QLineEdit;
class QToolButton;
class QEvent;

class Variant;
typedef Variant Property;

class PropertyEditorInput : public PropertySubEditor
{
    Q_OBJECT

public:
    PropertyEditorInput(QWidget *parent, Property *property);
    ~PropertyEditorInput() override
    {
        ;
    }

protected Q_SLOTS:
    void slotTextChanged(const QString &text);

protected:
    KLineEdit *m_lineedit;
};

class PropIntSpinBox : public QSpinBox
{
    Q_OBJECT

public:
    PropIntSpinBox(int lower, int upper, int step, int value, int base, QWidget *parent);
    ~PropIntSpinBox() override
    {
        ;
    }

    bool eventFilter(QObject *o, QEvent *e) override;
    QLineEdit *editor() const
    {
        return QSpinBox::lineEdit();
    }
};

class PropertyEditorSpin : public PropertySubEditor
{
    Q_OBJECT

public:
    PropertyEditorSpin(QWidget *parent, Property *property);
    ~PropertyEditorSpin() override
    {
        ;
    }

protected Q_SLOTS:
    void valueChange(int);

protected:
    PropIntSpinBox *m_spinBox;
};

class PropDoubleSpinBox : public DoubleSpinBox
{
    Q_OBJECT

public:
    PropDoubleSpinBox(double lower, double upper, double minAbs, double value, const QString &unit, QWidget *parent);
    ~PropDoubleSpinBox() override
    {
        ;
    }

    bool eventFilter(QObject *o, QEvent *e) override;
    QLineEdit *editor() const
    {
        return DoubleSpinBox::lineEdit();
    }
};

class PropertyEditorDblSpin : public PropertySubEditor
{
    Q_OBJECT

public:
    PropertyEditorDblSpin(QWidget *parent, Property *property);
    ~PropertyEditorDblSpin() override
    {
        ;
    }

protected Q_SLOTS:
    void valueChange(double value);

protected:
    PropDoubleSpinBox *m_spinBox;
};

class PropertyEditorBool : public PropertySubEditor
{
    Q_OBJECT

public:
    PropertyEditorBool(QWidget *parent, Property *property);
    ~PropertyEditorBool() override
    {
        ;
    }

    bool eventFilter(QObject *watched, QEvent *e) override;

protected Q_SLOTS:
    void setState(bool state);

protected:
    QToolButton *m_toggle;
};

#endif
