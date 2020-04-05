/***************************************************************************
 *   Copyright (C) 2020 by Friedrich W. H. Kossebau - kossebau@kde.org     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef IMAGEEXPORTDLG_H
#define IMAGEEXPORTDLG_H

#include <QDialog>
#include <QStringList>

class KComboBox;
class KUrlRequester;
class QCheckBox;
class QPushButton;
class QDialogButtonBox;
class QString;

class ImageExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageExportDialog(QWidget *parent);

public:
    QString filePath() const;
    QString formatType() const;
    bool isCropSelected() const;

private Q_SLOTS:
    void handleFormatIndexChanged(int index);
    void updateExportButton();

private:
    const QStringList m_mimeTypeNames;
    KComboBox *m_formatSelect;
    KUrlRequester *m_filePathEdit;
    QCheckBox *m_cropCheck;
    QDialogButtonBox *m_buttonBox;
    QPushButton *m_exportButton;
};

#endif
