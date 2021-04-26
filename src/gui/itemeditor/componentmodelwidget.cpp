/***************************************************************************
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "componentmodelwidget.h"
#include "component.h"
#include "componentmodellibrary.h"

#include <KLineEdit>
#include <KLocalizedString>
#include <KToolBar>
//#include <ktoolbarbutton.h> // converted to QToolButton

#include <QDebug>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>
// #include <q3header.h> // needed?
#include <QPainter>
#include <QPalette>
#include <QToolButton>

// BEGIN class ComponentModelWidget
ComponentModelWidget::ComponentModelWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vlayout = new QVBoxLayout(this /*, 0, 6  - 2018.12.07 */);
    vlayout->setMargin(0);
    vlayout->setSpacing(6);

    // parts of the following code are stolen from amarok/src/playlistwindow.cpp :)
    // BEGIN Filter lineedit
    QHBoxLayout *h1Layout = new QHBoxLayout;
    h1Layout->setMargin(0);
    KToolBar *bar = new KToolBar(QStringLiteral("ComponentModelSearch"), this);
    bar->setIconSize(QSize(22, 22) /*, false  ?? */); // looks more sensible
    // bar->setFlat( true ); //removes the ugly frame
    bar->setMovable(false); // removes the ugly frame
    // bar->setMovingEnabled( false ); //removes the ugly frame // removed, apparently

    // QWidget * button = new QToolButton( "locationbar_erase", 1, bar );
    QWidget *button = new QToolButton(bar);
    button->setObjectName("locationbar_erase"); // TODO what is: "locationbar_erase", 1,
                                                // button: locationbar_erase is the name of the icon to be displayed on it
    m_pSearchEdit = new KLineEdit(bar);
    m_pSearchEdit->setPlaceholderText(i18n("Filter here..."));

    // bar->setStretchableWidget( m_pSearchEdit ); // TODO removed, investigate
    m_pSearchEdit->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
    m_pSearchEdit->setFrame(true /* 2019.01.19: was QFrame::Sunken */);
    connect(m_pSearchEdit, &QLineEdit::textChanged, this, &ComponentModelWidget::setFilter);

    // TODO Because plain QWidget* type doesn't have a clicked signal to connect, we cast
    connect(qobject_cast<QToolButton *>(button), &QToolButton::clicked, m_pSearchEdit, &QLineEdit::clear);

    button->setToolTip(i18n("Clear filter"));
    QString filtertip = i18n("Enter space-separated terms to filter the component library.");

    m_pSearchEdit->setToolTip(filtertip);

    h1Layout->addWidget(m_pSearchEdit);
    h1Layout->addWidget(button);
    // END Filter lineedit

    m_pList = new QListWidget(this);
    // 	m_pList->setItemMargin( 3 );
    // m_pList->addColumn( "model" ); // 2018.06.02 - should not be needed
    // m_pList->setFullWidth( true );    // 2018.06.02 - is it fixed?
    m_pList->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    // m_pList->header()->hide();
    m_pList->setToolTip(i18n("Select a predefined component configuration from this list."));

    vlayout->addWidget(bar);
    // vlayout->addWidget( m_pSearchEdit );
    vlayout->addLayout(h1Layout);
    vlayout->addWidget(m_pList);
}

ComponentModelWidget::~ComponentModelWidget()
{
}

void ComponentModelWidget::reset()
{
    m_pList->clear();
    m_pSearchEdit->clear();
}

void ComponentModelWidget::init(Component *component)
{
    // for testing purposes
    reset();
    if (!component)
        return;

    QStringList types;
    if (component->type() == "ec/npnbjt") {
        //		types = ComponentModelLibrary::self()->modelIDs( ComponentModelLibrary::NPN );
    } else if (component->type() == "ec/pnpbjt") {
        //		types = ComponentModelLibrary::self()->modelIDs( ComponentModelLibrary::PNP );
    } else
        return;

    QStringList::iterator end = types.end();
    for (QStringList::iterator it = types.begin(); it != end; ++it) {
        QListWidgetItem *newItem = new QListWidgetItem(m_pList);
        newItem->setText(*it);
    }
}

void ComponentModelWidget::setFilter(const QString &filter)
{
    QString lower = filter.toLower();

    for (int itemNr = 0; itemNr < m_pList->count(); ++itemNr) {
        QListWidgetItem *item = m_pList->item(itemNr);
        bool hasText = item->text().toLower().contains(lower);
        item->setHidden(!hasText);
    }
}
// END class ComponentModelWidget
