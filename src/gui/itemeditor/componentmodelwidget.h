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
@author David Saxton
*/
class ComponentModelWidget : public QWidget
{
    Q_OBJECT
public:
    ComponentModelWidget(QWidget *parent = nullptr);
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
    KLineEdit *m_pSearchEdit;
};

#endif
