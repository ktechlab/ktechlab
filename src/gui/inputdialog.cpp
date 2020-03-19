/***************************************************************************
 *   Copyright (C) 2020 by Friedrich W. H. Kossebau - kossebau@kde.org     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "inputdialog.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QValidator>
#include <QString>
#include <QValidator>

namespace InputDialog
{

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(const QString &windowTitle, const QString &labelText,
           const QString &value, QWidget *parent,
           QValidator *validator);

    QString text() const;

private Q_SLOTS:
    void handleTextChanged(const QString &text);

private:
    QValidator *m_validator;
    QLineEdit *m_lineEdit;
    QDialogButtonBox *m_buttonBox;
};

Dialog::Dialog(const QString &windowTitle, const QString &labelText,
               const QString &value, QWidget *parent,
               QValidator *validator)
    : QDialog(parent)
    , m_validator(validator)
{
    setWindowTitle(windowTitle);
    setModal(true);

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);
    QHBoxLayout *editLayout = new QHBoxLayout;

    QLabel *label = new QLabel(labelText, this);
    label->setWordWrap(true);
    editLayout->addWidget(label);

    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setText(value);
    m_lineEdit->setClearButtonEnabled(true);
    m_lineEdit->setFocus();
    if (validator) {
        m_lineEdit->setValidator(validator);
    }
    label->setBuddy(m_lineEdit);
    editLayout->addWidget(m_lineEdit);

    layout->addLayout(editLayout);

    layout->addStretch();

    m_buttonBox = new QDialogButtonBox(this);
    m_buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(m_buttonBox);

    connect(m_lineEdit, &QLineEdit::textChanged,
            this, &Dialog::handleTextChanged);

    handleTextChanged(value);
}

QString Dialog::text() const
{
    return m_lineEdit->text();
}

void Dialog::handleTextChanged(const QString &text)
{
    bool acceptable;

    if (m_validator) {
        QString validatedText(text);
        int index = m_lineEdit->cursorPosition();
        acceptable = (m_validator->validate(validatedText, index) == QValidator::Acceptable);
    } else {
        acceptable = !text.trimmed().isEmpty();
    }

    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(acceptable);
}

QString getText(const QString &windowTitle, const QString &labelText,
                const QString &value, bool *ok, QWidget *parent,
                QValidator *validator)
{
    Dialog *dialog = new Dialog(windowTitle, labelText, value, parent, validator);

    const bool accepted = (dialog->exec() == QDialog::Accepted);

    const QString result = accepted ? dialog->text() : QString();
    if (ok) {
        *ok = accepted;
    }

    delete dialog;

    return result;
}

}

#include "inputdialog.moc"
