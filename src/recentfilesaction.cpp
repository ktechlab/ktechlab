/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "recentfilesaction.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <QDir>
#include <QMenu>
#include <QUrl>

RecentFilesAction::RecentFilesAction(const QString &configGroupName, const QString &text, const QObject *receiver, const char *slot, QObject *parent, const char *name)
    : KSelectAction(text /*, 0*/ /*pix*/, parent /*, name */)
{
    setObjectName(name);

    m_configGroupName = configGroupName;
    m_maxItems = 10;

    m_popup = new QMenu;
    connect(m_popup, SIGNAL(aboutToShow()), this, SLOT(menuAboutToShow()));
    connect(m_popup, SIGNAL(triggered(QAction *)), this, SLOT(menuItemActivated(QAction *)));
    connect(this, SIGNAL(triggered(const QString &)), this, SLOT(itemSelected(const QString &)));

    setMenuAccelsEnabled(false);

    if (receiver)
        connect(this, SIGNAL(urlSelected(QUrl)), receiver, slot);
}

RecentFilesAction::~RecentFilesAction()
{
    delete m_popup;
}

void RecentFilesAction::addUrl(const QUrl &url)
{
    if (url.isLocalFile() && url.toLocalFile().startsWith(QDir::tempPath()))
        return;

    const QString file = url.toDisplayString(QUrl::PreferLocalFile);

    QStringList lst = items();

    // remove file if already in list
    lst.removeAll(file);

    // remove last item if already maxitems in list
    if (lst.count() == m_maxItems) {
        // remove last item
        lst.removeAll(lst.last());
    }

    // add file to list
    lst.prepend(file);
    setItems(lst);

    saveEntries();
}

void RecentFilesAction::loadEntries()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();

    QString key;
    QString value;
    // QString     oldGroup;
    QStringList lst;

    // oldGroup = config->group();

    KConfigGroup grCfg = config->group(m_configGroupName);

    // read file list
    for (unsigned int i = 1; i <= m_maxItems; i++) {
        key = QString("File%1").arg(i);
        value = grCfg.readPathEntry(key, QString());

        if (!value.isNull())
            lst.append(value);
    }

    // set file
    setItems(lst);

    // config->setGroup( oldGroup );
}

void RecentFilesAction::saveEntries()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();

    QString key;
    QString value;
    // QString     oldGroup;
    QStringList lst = items();

    // oldGroup = config->group();

    config->deleteGroup(m_configGroupName /*, true */);
    KConfigGroup grCfg = config->group(m_configGroupName);

    // write file list
    for (unsigned int i = 1; i <= lst.count(); i++) {
        key = QString("File%1").arg(i);
        value = lst[i - 1];
        grCfg.writePathEntry(key, value);
    }

    // grCfg.setGroup( oldGroup );

    config->sync();
}

void RecentFilesAction::itemSelected(const QString &text)
{
    emit urlSelected(QUrl::fromUserInput(text));
}

void RecentFilesAction::menuItemActivated(QAction *action)
{
    // emit urlSelected( KUrl(m_popup->text(id)) ); // 2018.11.21
    emit urlSelected(QUrl::fromUserInput(action->text()));
}

void RecentFilesAction::menuAboutToShow()
{
    QMenu *menu = m_popup;
    menu->clear();
    QStringList list = items();
    QStringList::iterator end = list.end();
    for (QStringList::Iterator it = list.begin(); it != end; ++it) {
        // menu->insertItem(*it); // 2018.11.21
        menu->addAction(*it);
    }
}

void RecentFilesAction::slotClicked()
{
    // KAction::slotActivated(); // TODO CORRECT?
    QAction::trigger();
}

void RecentFilesAction::slotActivated(const QString &text)
{
    // KSelectAction::slotActivated(text); // TODO CORRECT?
    KSelectAction::activate(QAction::Trigger);
}

void RecentFilesAction::slotActivated(int id)
{
    // KSelectAction::slotActivated(id);
    KSelectAction::activate(QAction::Trigger); // TODO correct?
}

void RecentFilesAction::slotActivated()
{
    emit triggered(currentItem());
    // emit activated( currentText() ); // TODO how should this work?
}
