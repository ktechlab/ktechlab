/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "outputmethoddlg.h"
#include "docmanager.h"
#include "filemetainfo.h"
#include "microlibrary.h"
#include "microselectwidget.h"
#include "projectmanager.h"
#include "textdocument.h"

#include <KUrlRequester>

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFile>
#include <QPushButton>
#include <QTemporaryFile>
#include <QVBoxLayout>

#include <ui_outputmethodwidget.h>
#include <ktechlab_debug.h>

class OutputMethodWidget : public QWidget, public Ui::OutputMethodWidget
{
public:
    OutputMethodWidget(QWidget *parent)
        : QWidget(parent)
    {
        setupUi(this);
    }
};

// BEGIN class OutputMethodInfo
OutputMethodInfo::OutputMethodInfo()
{
    m_method = Method::Direct;
    m_bAddToProject = false;
}

void OutputMethodInfo::initialize(OutputMethodDlg *dlg)
{
    if (dlg->m_widget->displayDirectCheck->isChecked()) {
        m_method = Method::Direct;
        // K3TempFile f( QString(), dlg->m_outputExtension );
        QTemporaryFile f(QDir::tempPath() + QLatin1String("/ktechlab_XXXXXX") + dlg->m_outputExtension);
        f.setAutoRemove(false);
        if (!f.open()) {
            qCWarning(KTL_LOG) << "failed to open " << f.fileName() << " because " << f.errorString();
        }
        f.close();
        m_outputFile = QUrl::fromLocalFile(f.fileName());
        m_bAddToProject = false;
    }

    else {
        if (dlg->m_widget->loadFileCheck->isChecked())
            m_method = Method::SaveAndLoad;

        else
            m_method = Method::SaveAndForget;

        m_outputFile = dlg->m_widget->outputFileURL->url();
        m_bAddToProject = dlg->m_widget->addToProjectCheck->isChecked();
    }

    m_picID = dlg->m_widget->m_pMicroSelect->micro();
}
// END class OutputMethodInfo

// BEGIN class OutputMethodDlg

OutputMethodDlg::OutputMethodDlg(const QString &caption, const QUrl &inputURL, bool showPICSelect, QWidget *parent)
    : QDialog(parent)
{
    setModal(true);
    setWindowTitle(caption);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    m_inputURL = inputURL;
    m_widget = new OutputMethodWidget(this);

    m_widget->addToProjectCheck->setEnabled(ProjectManager::self()->currentProject());

    if (!showPICSelect) {
        m_widget->m_pMicroSelect->hide();
        m_widget->adjustSize();
    }

    m_widget->outputFileURL->setMode(KFile::File | KFile::LocalOnly);
    m_widget->outputFileURL->setAcceptMode(QFileDialog::AcceptSave);

    connect(m_widget->saveFileCheck, SIGNAL(toggled(bool)), m_widget->groupBoxSaveOptions, SLOT(setEnabled(bool)));

    fileMetaInfo()->initializeFromMetaInfo(m_inputURL, this);

    mainLayout->addWidget(m_widget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

OutputMethodDlg::~OutputMethodDlg()
{
}

void OutputMethodDlg::setOutputExtension(const QString &extension)
{
    m_outputExtension = extension;
}

void OutputMethodDlg::setFilter(const QString &filter)
{
    m_widget->outputFileURL->setFilter(filter);
}

void OutputMethodDlg::setMethod(OutputMethodInfo::Method::Type m)
{
    switch (m) {
    case OutputMethodInfo::Method::Direct:
        m_widget->displayDirectCheck->setChecked(true);
        break;

    case OutputMethodInfo::Method::SaveAndForget:
        m_widget->saveFileCheck->setChecked(true);
        m_widget->loadFileCheck->setChecked(false);
        break;

    case OutputMethodInfo::Method::SaveAndLoad:
        m_widget->saveFileCheck->setChecked(true);
        m_widget->loadFileCheck->setChecked(true);
        break;
    };
}

void OutputMethodDlg::setPicID(const QString &id)
{
    m_widget->m_pMicroSelect->setMicro(id);
}

void OutputMethodDlg::setOutputFile(const QUrl &out)
{
    m_widget->outputFileURL->setUrl(out);
}

void OutputMethodDlg::accept()
{
    m_outputMethodInfo.initialize(this);
    fileMetaInfo()->grabMetaInfo(m_inputURL, this);

    QDialog::accept();
}

MicroSelectWidget *OutputMethodDlg::microSelect() const
{
    return m_widget->m_pMicroSelect;
}
// END class OutputMethodDlg
