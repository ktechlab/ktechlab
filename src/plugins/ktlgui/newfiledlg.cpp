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

    printf("new file dialog accepted\n");

    m_bAccepted = true;

    const QString fileText = m_pNewFileWidget->typeIconView->currentItem()->text();

    m_bAddToProject = m_pNewFileWidget->addToProjectCheck->isChecked();

//     m_microID = m_pNewFileWidget->m_pMicroSelect->micro();

    // check which signal to emit
    qDebug() << "selection: " << m_pNewFileWidget->typeIconView->currentIndex();
    qDebug() << "selection count: " << m_pNewFileWidget->typeIconView->currentIndex().row();

    switch(m_pNewFileWidget->typeIconView->currentIndex().row()){
        case 0:
            // assembly
            emit signalFileNewAssembly();
            break;
        case 1:
            // C
            emit signalFileNewC();
            break;
        case 2:
            // circuit
            emit signalFileNewCircuit();
            break;
        case 3:
            // flowcode
            emit signalFileNewFlowCode();
            break;
        case 4:
            // microbe
            emit signalFileNewMicrobe();
            break;
        default:
            qCritical("unexpected item selected in new file widget!\n");
            printf("item number: %d\n",
                   m_pNewFileWidget->typeIconView->currentIndex().row());
    }
}


void NewFileDlg::reject()
{
    hide();
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

void NewFileDlg::addFiletypeToNewFileDialog(QListWidgetItem* item, const QObject* receiver, const char* slot)
{
	Q_ASSERT(item);
	Q_ASSERT(receiver);
	Q_ASSERT(slot);
	Q_ASSERT(m_receiverList.size() == m_slotList.size());

	m_pNewFileWidget->typeIconView->addItem(item);
	m_receiverList.append(receiver);
	m_slotList.append(slot);
}

int NewFileDlg::removeFiletypeFromNewFileDialog(const QObject* receiver, const char* slot)
{
	Q_ASSERT(receiver);
	Q_ASSERT(slot);

	int n = m_receiverList.size();;
	bool anyFound = false;

	for(int i=0; i<n; i++){
		if((m_receiverList[i] == receiver) && (!strcmp(m_slotList[i],slot))){
			// it matches, so remove
			m_receiverList.removeAt(i);
			m_slotList.removeAt(i);
			anyFound = true;
		}
	}
	if(anyFound) {
		return 0;
	} else {
		return -1;
	}
}

#include "newfiledlg.moc"
