/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "programmerdlg.h"
#include "ktlconfig.h"
#include "language.h"
#include "microselectwidget.h"
#include "picprogrammer.h"
#include "port.h"
#include "ui_programmerwidget.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

#include <KLocalizedString>

class ProgrammerWidget : public QWidget, public Ui::ProgrammerWidget
{
public:
    ProgrammerWidget(QWidget *parent)
        : QWidget(parent)
    {
        setupUi(this);
    }
};

ProgrammerDlg::ProgrammerDlg(const QString &picID, QWidget *parent, const char *name)
    : QDialog(parent)
{
    setObjectName(name);
    setModal(true);
    setWindowTitle(i18n("PIC Programmer"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    m_pProgrammerWidget = new ProgrammerWidget(this);
    m_pProgrammerSettings = new PicProgrammerSettings;

    // Setup the list of programmers
    QComboBox *programmerCombo = m_pProgrammerWidget->m_pProgrammerProgram;
    QStringList programmerNames = m_pProgrammerSettings->configNames(false);
    programmerCombo->insertItems(programmerCombo->count(), programmerNames);
    // programmerCombo->setSizeLimit( programmerNames.size() );
    programmerCombo->setMaxCount(programmerNames.size());
    // programmerCombo->setCurrentText( KTLConfig::picProgrammerProgram() ); // 2018.12.07
    {
        QComboBox *c = programmerCombo;
        QString text = KTLConfig::picProgrammerProgram();
        int i = c->findText(text);
        if (i != -1)
            c->setCurrentIndex(i);
        else if (c->isEditable())
            c->setEditText(text);
        else
            c->setItemText(c->currentIndex(), text);
    }
    // Sets up the list of ports
    m_pProgrammerWidget->m_pPicProgrammerPort->insertItems(m_pProgrammerWidget->m_pPicProgrammerPort->count(), Port::ports(Port::ExistsAndRW));
    // m_pProgrammerWidget->m_pPicProgrammerPort->setCurrentText( KTLConfig::picProgrammerPort() ); // 2018.12.07
    {
        QComboBox *c = m_pProgrammerWidget->m_pPicProgrammerPort;
        QString text = KTLConfig::picProgrammerPort();
        int i = c->findText(text);
        if (i != -1)
            c->setCurrentIndex(i);
        else if (c->isEditable())
            c->setEditText(text);
        else
            c->setItemText(c->currentIndex(), text);
    }

    // Set the pic type to the one requested
    if (!picID.isEmpty())
        m_pProgrammerWidget->m_pMicroSelect->setMicro(picID);

    mainLayout->addWidget(m_pProgrammerWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel);
    QPushButton *burnButton = new QPushButton(QIcon::fromTheme(QStringLiteral("dialog-ok")), i18n("Burn"), buttonBox);
    buttonBox->addButton(burnButton, QDialogButtonBox::AcceptRole);
    burnButton->setDefault(true);
    burnButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    mainLayout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

ProgrammerDlg::~ProgrammerDlg()
{
}

void ProgrammerDlg::initOptions(ProcessOptions *options)
{
    if (!options)
        return;

    options->m_picID = m_pProgrammerWidget->m_pMicroSelect->micro();
    options->m_port = m_pProgrammerWidget->m_pPicProgrammerPort->currentText();
    options->m_program = m_pProgrammerWidget->m_pProgrammerProgram->currentText();
}

MicroSelectWidget *ProgrammerDlg::microSelect() const
{
    return m_pProgrammerWidget->m_pMicroSelect;
}
