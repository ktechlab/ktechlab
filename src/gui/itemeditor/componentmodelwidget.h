/***************************************************************************
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef COMPONENTMODELWIDGET_H
#define COMPONENTMODELWIDGET_H

#include <KLineEdit>

class Component;
class QListWidget;

/**
  This class provides a KLineEdit which contains a greyed-out hinting
  text as long as the user didn't enter any text

  @short LineEdit with customizable "Click here" text
  @author Daniel Molkentin
*/
class ClickLineEdit : public KLineEdit
{
    Q_OBJECT
    Q_PROPERTY(QString clickMessage READ clickMessage WRITE setClickMessage)
public:
    ClickLineEdit(const QString &msg, QWidget *parent, const char *name = nullptr);

    void setClickMessage(const QString &msg);
    QString clickMessage() const
    {
        return mClickMessage;
    }

    void setText(const QString &txt) override;

protected:
    virtual void drawContents(QPainter *p);
    void focusInEvent(QFocusEvent *ev) override;
    void focusOutEvent(QFocusEvent *ev) override;

private:
    QString mClickMessage;
    bool mDrawClickMsg;
};

/**
@author David Saxton
*/
class ComponentModelWidget : public QWidget
{
    Q_OBJECT
public:
    ComponentModelWidget(QWidget *parent = nullptr, const char *name = nullptr);
    ~ComponentModelWidget() override;
    /**
     * Clears the list of component models.
     */
    void reset();
    /**
     * Fills the list with models appropriate for the given component.
     */
    void init(Component *component);

public slots:
    /**
     * The filter is applied against the list of component model names.
     */
    void setFilter(const QString &filter);

protected:
    QListWidget *m_pList;
    ClickLineEdit *m_pSearchEdit;
};

#endif
