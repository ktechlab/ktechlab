/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "newfiledlg.h"
#include "config.h"
#include "document.h"
#include "microinfo.h"
#include "microlibrary.h"
#include "microselectwidget.h"
#include "projectmanager.h"
#include "textdocument.h"

#include <canvas.h>

#include <KLineEdit>
// #include <k3iconview.h>
#include <KIconLoader>
#include <KLocalizedString>

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
// #include <q3paintdevicemetrics.h>

#include <algorithm>

#include <ui_newfilewidget.h>
#include <ktechlab_debug.h>

class NewFileWidget : public QWidget, public Ui::NewFileWidget
{
public:
    NewFileWidget(QWidget *parent)
        : QWidget(parent)
    {
        setupUi(this);
    }
};

NewFileDlg::NewFileDlg(QWidget *parent)
    : QDialog(parent)
{
    setObjectName("newfiledlg");
    setModal(true);
    setWindowTitle(i18n("New File"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    m_pMainParent = parent;
    m_pNewFileWidget = new NewFileWidget(this);

    m_pNewFileWidget->typeIconView->setSelectionMode(QAbstractItemView::SingleSelection /*Q3IconView::Single*/);
    // m_pNewFileWidget->typeIconView->setMode(K3IconView::Select); // 2017.12.01 - convert to qlistwidget
    m_pNewFileWidget->typeIconView->setIconSize(QSize(KIconLoader::SizeHuge, KIconLoader::SizeHuge));

    QList<QListWidgetItem *> items;

    // BEGIN insert icons
    QString text = QString("%1 (.asm)").arg(i18n("Assembly Code"));
    items << new QListWidgetItem(QIcon::fromTheme("source"), text, m_pNewFileWidget->typeIconView);

    text = "C (.c)";
    items << new QListWidgetItem(QIcon::fromTheme("text-x-csrc"), text, m_pNewFileWidget->typeIconView);

    text = QString("%1 (.circuit)").arg(i18n("Circuit"));
    items << new QListWidgetItem(QIcon::fromTheme("application-x-circuit"), text, m_pNewFileWidget->typeIconView);

    items << new QListWidgetItem(QIcon::fromTheme("application-x-flowcode"), "FlowCode (.flowcode)", m_pNewFileWidget->typeIconView);

#ifdef MECHANICS
    items << new QListWidgetItem(QIcon::fromTheme(QIcon::fromTheme("exec")), "Mechanics (.mechanics)", m_pNewFileWidget->typeIconView);
#endif

    items << new QListWidgetItem(QIcon::fromTheme("application-x-microbe"), "Microbe (.microbe)", m_pNewFileWidget->typeIconView);
    // END insert icons

    int minWidth = 20 + m_pNewFileWidget->typeIconView->spacing() * items.size();
    int minHeight = 20;

    const QList<QListWidgetItem *>::iterator end = items.end();
    for (QList<QListWidgetItem *>::iterator it = items.begin(); it != end; ++it) {
        //(*it)->setDragEnabled(false); // 2017.12.01 - use qlistwidget
        Qt::ItemFlags flags = (*it)->flags();
        flags &= (~Qt::ItemIsDragEnabled);
        (*it)->setFlags(flags);

        QList<QSize> listAvSizes = (*it)->icon().availableSizes();
        if (listAvSizes.isEmpty()) {
            qCWarning(KTL_LOG) << "no available sizes for " << (*it)->text();
        } else {
            qCDebug(KTL_LOG) << "W = " << (*it)->icon().availableSizes().first().width() << " H=" << (*it)->icon().availableSizes().first().height();
            minWidth += (*it)->icon().availableSizes().first().width() + 20;
            minHeight = std::max(minHeight, (*it)->icon().availableSizes().first().height() + 20);
        }
    }
    qCDebug(KTL_LOG) << "minW = " << minWidth << " minH=" << minHeight;
    m_pNewFileWidget->typeIconView->setMinimumSize(minWidth, minHeight);
    m_pNewFileWidget->typeIconView->setCurrentItem(items[3]);
    m_pNewFileWidget->addToProjectCheck->setChecked(ProjectManager::self()->currentProject());
    m_pNewFileWidget->addToProjectCheck->setEnabled(ProjectManager::self()->currentProject());
    microSelectWidget()->setAllowedFlowCodeSupport(MicroInfo::FullSupport | MicroInfo::PartialSupport);

    mainLayout->addWidget(m_pNewFileWidget);

    // Our behaviour is to have single click selects and double click accepts the dialog
    connect(m_pNewFileWidget->typeIconView, SIGNAL(itemSelectionChanged()), this, SLOT(fileTypeChanged()));
    connect(m_pNewFileWidget->typeIconView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(accept()));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    setAcceptDrops(true);

    m_pNewFileWidget->typeIconView->adjustSize();
    m_pNewFileWidget->adjustSize();
    adjustSize();
}

void NewFileDlg::accept()
{
    QDialog::accept();

    const QString fileText = m_pNewFileWidget->typeIconView->currentItem()->text();

    if (fileText.contains(".flowcode"))
        m_fileType = Document::dt_flowcode;

    else if (fileText.contains(".circuit"))
        m_fileType = Document::dt_circuit;

    else if (fileText.contains(".mechanics"))
        m_fileType = Document::dt_mechanics;

    else if (fileText.contains(".asm")) {
        m_fileType = Document::dt_text;
        m_codeType = TextDocument::ct_asm;
    }

    else if (fileText.contains(".basic") || fileText.contains(".microbe")) {
        m_fileType = Document::dt_text;
        m_codeType = TextDocument::ct_microbe;
    }

    else if (fileText.contains(".c")) {
        m_fileType = Document::dt_text;
        m_codeType = TextDocument::ct_c;
    }

    else
        m_fileType = Document::dt_text;

    m_bAddToProject = m_pNewFileWidget->addToProjectCheck->isChecked();

    m_microID = m_pNewFileWidget->m_pMicroSelect->micro();
}

void NewFileDlg::fileTypeChanged()
{
    bool doEnableMicros = false;
    if (!m_pNewFileWidget->typeIconView->selectedItems().isEmpty()) {
        doEnableMicros = m_pNewFileWidget->typeIconView->selectedItems().first()->text().contains(".flowcode");
    }
    m_pNewFileWidget->m_pMicroSelect->setEnabled(doEnableMicros);
}

MicroSelectWidget *NewFileDlg::microSelectWidget() const
{
    return m_pNewFileWidget->m_pMicroSelect;
}
