/***************************************************************************
 *   Copyright (C) 2005 by David Saxton <david@bluehaze.org>               *
 *   Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "config.h"
//#include "document.h"
//#include "microinfo.h"
#include "newfiledlg.h"
#include "ui_newfilewidget.h"
//#include "microlibrary.h"
//#include "microselectwidget.h"
//#include "projectmanager.h"
//#include "textdocument.h"

#include <KComboBox>
#include <KDebug>
#include <KLineEdit>
#include <KLocale>
#include <KIconLoader>

#include <QListWidgetItem>
#include <QCheckBox>
#include <QDir>
#include <QFile>
#include <QLabel>
#include <QStyle>

using namespace KTechLab;

NewFileDlg::NewFileDlg( QWidget *parent )
    : KDialog( parent )
{
    m_pMainParent = parent;
    m_bAccepted = false;

    setObjectName( "newfiledlg" );
    setWindowTitle( "New File" );

    m_pNewFileWidget = new Ui::NewFileWidget();
    m_pNewFileWidget->setupUi( mainWidget() );

    KIconLoader *loader = KIconLoader::global();

    QList<QListWidgetItem *> items;

    //BEGIN insert icons
    QString text = QString("%1 (.asm)").arg( i18n("Assembly Code") );
    items << new QListWidgetItem( loader->loadIcon( "source", KIconLoader::NoGroup, KIconLoader::SizeHuge ), text, m_pNewFileWidget->typeIconView );

    text = "C (.c)";
    items << new QListWidgetItem( loader->loadIcon( "source_c", KIconLoader::NoGroup, KIconLoader::SizeHuge ), text, m_pNewFileWidget->typeIconView );

    text = QString("%1 (.circuit)").arg( i18n("Circuit") );
    items << new QListWidgetItem( loader->loadIcon( "ktechlab_circuit", KIconLoader::NoGroup, KIconLoader::SizeHuge ), text, m_pNewFileWidget->typeIconView );

    text = QString("%1 (.flowcode)").arg( i18n("FlowCode") );
    items << new QListWidgetItem( loader->loadIcon( "ktechlab_flowcode", KIconLoader::NoGroup, KIconLoader::SizeHuge ), text, m_pNewFileWidget->typeIconView );

#ifdef MECHANICS
    text = QString("%1 (.mechanics)").arg( i18n("Mechanics") );
    items << new QListWidgetItem( loader->loadIcon( "exec", KIconLoader::NoGroup, KIconLoader::SizeHuge ), text , m_pNewFileWidget->typeIconView );
#endif

    text = QString("%1 (.microbe)").arg( i18n("Microbe") );
    items << new QListWidgetItem( loader->loadIcon( "ktechlab_microbe", KIconLoader::NoGroup, KIconLoader::SizeHuge ), text, m_pNewFileWidget->typeIconView );
    //END insert icons

    const QList<QListWidgetItem *>::iterator end = items.end();
    for ( QList<QListWidgetItem *>::iterator it = items.begin(); it != end; ++it )
    {
        QFlags<Qt::ItemFlag> flags = (*it)->flags();
        (*it)->setFlags( flags & ~( Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled ) );
    }

    m_pNewFileWidget->typeIconView->setCurrentItem(items[3]);
//    microSelectWidget()->setAllowedFlowCodeSupport( MicroInfo::FullSupport | MicroInfo::PartialSupport );

    // Our behaviour is to have single click selects and double click accepts the dialog
    connect( m_pNewFileWidget->typeIconView, SIGNAL( itemSelectionChanged() ), this, SLOT( fileTypeChanged() ) );
    connect( m_pNewFileWidget->typeIconView, SIGNAL( executed( QListWidgetItem* ) ), this, SLOT( accept() ) );

    setAcceptDrops(true);

    m_pNewFileWidget->typeIconView->adjustSize();
    adjustSize();
}

void NewFileDlg::accept()
{
    hide();
    m_bAccepted = true;

    const QString fileText = m_pNewFileWidget->typeIconView->currentItem()->text();

    m_bAddToProject = m_pNewFileWidget->addToProjectCheck->isChecked();

//     m_microID = m_pNewFileWidget->m_pMicroSelect->micro();
}


void NewFileDlg::reject()
{
    m_bAccepted = false;
}


void NewFileDlg::fileTypeChanged()
{
    QListWidgetItem *item = m_pNewFileWidget->typeIconView->currentItem();
    m_pNewFileWidget->microSelect->setEnabled(
            item->text().contains(".flowcode") );
}


MicroSelectWidget * NewFileDlg::microSelectWidget() const
{
//     return m_pNewFileWidget->m_pMicroSelect;
    return 0;
}

#include "newfiledlg.moc"
