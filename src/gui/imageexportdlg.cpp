/***************************************************************************
 *   Copyright (C) 2020 by Friedrich W. H. Kossebau - kossebau@kde.org     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "imageexportdlg.h"

#include <KComboBox>
#include <KUrlRequester>
#include <KLocalizedString>

#include <QMimeDatabase>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QString>


ImageExportDialog::ImageExportDialog(QWidget *parent)
    : QDialog(parent)
    , m_mimeTypeNames({
        QStringLiteral("image/png"),
        QStringLiteral("image/bmp"),
        QStringLiteral("image/svg+xml"),
    })
{
    setWindowTitle(i18n("Export As Image"));
    setModal(true);

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    QFormLayout *formLayout = new QFormLayout;

    m_formatSelect = new KComboBox(this);
    QMimeDatabase mimeDb;
    for (auto& mimeTypeName : qAsConst(m_mimeTypeNames)) {
        m_formatSelect->addItem(mimeDb.mimeTypeForName(mimeTypeName).comment());
    }
    formLayout->addRow(i18n("Format:"), m_formatSelect);

    m_filePathEdit = new KUrlRequester(QUrl(), this);
    m_filePathEdit->setAcceptMode(QFileDialog::AcceptSave);
    m_filePathEdit->setMode(KFile::File | KFile::LocalOnly);

    formLayout->addRow(i18n("File name:"), m_filePathEdit);

    m_cropCheck = new QCheckBox(this);
    m_cropCheck->setObjectName( "cropCheck" );
    m_cropCheck->setChecked(true); // yes by default?

    formLayout->addRow(i18n("Crop image:"), m_cropCheck);
    layout->addLayout(formLayout);

    layout->addStretch();

    m_buttonBox = new QDialogButtonBox(this);
    m_buttonBox->setStandardButtons(QDialogButtonBox::Cancel);
    m_exportButton = m_buttonBox->addButton(i18n("Export"), QDialogButtonBox::AcceptRole);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(m_buttonBox);

    connect(m_formatSelect, QOverload<int>::of(&KComboBox::currentIndexChanged),
            this, &ImageExportDialog::handleFormatIndexChanged);
    connect(m_filePathEdit, &KUrlRequester::textChanged,
            this, &ImageExportDialog::updateExportButton);

    handleFormatIndexChanged(m_formatSelect->currentIndex());
}

QString ImageExportDialog::filePath() const
{
    return m_filePathEdit->text();
}

QString ImageExportDialog::formatType() const
{
    const int formatIndex = m_formatSelect->currentIndex();
    return
        (formatIndex == 0) ? QStringLiteral("PNG") :
        (formatIndex == 1) ? QStringLiteral("BMP") :
        (formatIndex == 2) ? QStringLiteral("SVG") :
        QString();
}

bool ImageExportDialog::isCropSelected() const
{
    return m_cropCheck->isChecked();
}

void ImageExportDialog::handleFormatIndexChanged(int index)
{
    m_filePathEdit->setMimeTypeFilters((index != -1) ? QStringList{m_mimeTypeNames.at(index)} : QStringList());

    updateExportButton();
}

void ImageExportDialog::updateExportButton()
{
    const bool acceptable = !m_filePathEdit->text().isEmpty() && (m_formatSelect->currentIndex() != -1);

    m_exportButton->setEnabled(acceptable);
}
