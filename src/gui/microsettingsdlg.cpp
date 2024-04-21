/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "microsettingsdlg.h"
#include "inputdialog.h"
#include "microinfo.h"
#include "micropackage.h"
#include "microsettings.h"
#include "pinmapping.h"
#include "ui_microsettingswidget.h"

#include <KComboBox>
#include <KLineEdit>
#include <KLocalizedString>
#include <KMessageBox>

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QRegExp>
#include <QVBoxLayout>
// #include <q3table.h>

#include <ui_newpinmappingwidget.h>
#include <ktechlab_debug.h>

class MicroSettingsWidget : public QWidget, public Ui::MicroSettingsWidget
{
public:
    MicroSettingsWidget(QWidget *parent)
        : QWidget(parent)
    {
        setupUi(this);
    }
};

class NewPinMappingWidget : public QWidget, public Ui::NewPinMappingWidget
{
public:
    NewPinMappingWidget(QWidget *parent)
        : QWidget(parent)
    {
        setupUi(this);
    }
};

MicroSettingsDlg::MicroSettingsDlg(MicroSettings *microSettings, QWidget *parent)
    : QDialog(parent)
{
    setModal(true);
    setWindowTitle(i18n("PIC Settings"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    m_pMicroSettings = microSettings;
    m_pNewPinMappingWidget = nullptr;
    m_pNewPinMappingDlg = nullptr;
    m_pNewPinMappingOkButton = nullptr;
    m_pWidget = new MicroSettingsWidget(this);

    setWhatsThis(i18n("This dialog allows editing of the initial properties of the PIC"));
    m_pWidget->portsGroupBox->setWhatsThis(
        i18n("Edit the initial value of the ports here. For each binary number, the order from right-to-left is pins 0 through 7.<br><br>The \"Type (TRIS)\" edit shows the initial input/output state of the ports; 1 represents an input, "
             "and 0 an output.<br><br>The \"State (PORT)\" edit shows the initial high/low state of the ports; 1 represents a high, and 0 a low."));
    m_pWidget->variables->setWhatsThis(
        i18n("Edit the initial value of the variables here.<br><br>Note that the value of the variable can only be in the range 0->255. These variables will be initialized before any other code is executed."));

    // BEGIN Initialize initial port settings
    m_portNames = microSettings->microInfo()->package()->portNames();

    m_portTypeEdit.resize(m_portNames.size() /*, 0  - 2018.06.02 - initialized below */);
    m_portStateEdit.resize(m_portNames.size() /*, 0 - 2018.06.02 - initialized below */);

    uint row = 0;
    QStringList::iterator end = m_portNames.end();
    for (QStringList::iterator it = m_portNames.begin(); it != end; ++it, ++row) {
        // BEGIN Get current Type / State text
        QString portType = QString::number(microSettings->portType(*it), 2);
        QString portState = QString::number(microSettings->portState(*it), 2);

        QString fill;
        fill.fill('0', 8 - portType.length());
        portType.prepend(fill);
        fill.fill('0', 8 - portState.length());
        portState.prepend(fill);
        // END Get current Type / State text

        QGroupBox *groupBox = new QGroupBox(*it, m_pWidget->portsGroupBox);

        // groupBox->setColumnLayout(0, Qt::Vertical ); // 2018.06.02 - not needed
        groupBox->setLayout(new QVBoxLayout);
        groupBox->layout()->setSpacing(6);
        groupBox->layout()->setContentsMargins(11,11,11,11);
        QGridLayout *groupBoxLayout = new QGridLayout(groupBox /*groupBox->layout() */);
        groupBoxLayout->setAlignment(Qt::AlignTop);
        groupBoxLayout->setSpacing(groupBox->layout()->spacing());
        groupBox->layout()->addItem(groupBoxLayout);

        // TODO: replace this with i18n( "the type", "Type (TRIS register):" );
        groupBoxLayout->addWidget(new QLabel(i18n("Type (TRIS register):"), groupBox), 0, 0);
        groupBoxLayout->addWidget(new QLabel(i18n("State (PORT register):"), groupBox), 1, 0);

        m_portTypeEdit[row] = new KLineEdit(portType, groupBox);
        groupBoxLayout->addWidget(m_portTypeEdit[row], 0, 1);

        m_portStateEdit[row] = new KLineEdit(portState, groupBox);
        groupBoxLayout->addWidget(m_portStateEdit[row], 1, 1);

        // 		(dynamic_cast<QVBoxLayout*>(m_pWidget->portsGroupBox->layout()))->insertWidget( row, groupBox );
        (dynamic_cast<QVBoxLayout *>(m_pWidget->portsGroupBox->layout()))->addWidget(groupBox);
    }
    // END Initialize initial port settings

    // BEGIN Initialize initial variable settings
    // Hide row headers
    // m_pWidget->variables->setLeftMargin(0); // 2018.06.02 - fixed in UI file

    // Make columns as thin as possible
    // m_pWidget->variables->setColumnStretchable( 0, true );  // 2018.06.02 - to be fixed
    // m_pWidget->variables->setColumnStretchable( 1, true );  // 2018.06.02 - to be fixed
    {
        QStringList headerLabels;
        headerLabels.append(i18n("Variable name"));
        headerLabels.append(i18n("Variable value"));
        m_pWidget->variables->setHorizontalHeaderLabels(headerLabels);
    }

    QStringList variableNames = microSettings->variableNames();
    row = 0;
    end = variableNames.end();
    for (QStringList::iterator it = variableNames.begin(); it != end; ++it) {
        VariableInfo *info = microSettings->variableInfo(*it);
        if (info) {
            qCDebug(KTL_LOG) << "add var: " << *it << " val: " << info->valueAsString();
            m_pWidget->variables->insertRow(row);
            QTableWidgetItem *varNameItem = new QTableWidgetItem(*it);
            m_pWidget->variables->setItem(row, 0, varNameItem);
            QTableWidgetItem *varValItem = new QTableWidgetItem(info->valueAsString());
            m_pWidget->variables->setItem(row, 1, varValItem);
            ++row;
        }
    }
    m_pWidget->variables->insertRow(row);
    qCDebug(KTL_LOG) << "row count: " << m_pWidget->variables->rowCount();

    connect(m_pWidget->variables, &QTableWidget::cellChanged, this, &MicroSettingsDlg::checkAddVariableRow);
    // END Initialize initial variable settings

    // BEGIN Initialize pin maps
    connect(m_pWidget->pinMapAdd, &QPushButton::clicked, this, &MicroSettingsDlg::slotCreatePinMap);
    connect(m_pWidget->pinMapModify, &QPushButton::clicked, this, &MicroSettingsDlg::slotModifyPinMap);
    connect(m_pWidget->pinMapRename, &QPushButton::clicked, this, &MicroSettingsDlg::slotRenamePinMap);
    connect(m_pWidget->pinMapRemove, &QPushButton::clicked, this, &MicroSettingsDlg::slotRemovePinMap);

    m_pinMappings = microSettings->pinMappings();
    m_pWidget->pinMapCombo->insertItems(m_pWidget->pinMapCombo->count(), m_pinMappings.keys());

    updatePinMapButtons();
    // END Initialize pin maps

    mainLayout->addWidget(m_pWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
    mainLayout->addWidget(buttonBox);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &MicroSettingsDlg::slotApplyClicked);

    m_pWidget->adjustSize();
    adjustSize();
}

MicroSettingsDlg::~MicroSettingsDlg()
{
}

void MicroSettingsDlg::accept()
{
    slotSaveStuff();
    QDialog::accept();
    deleteLater();
}

void MicroSettingsDlg::slotApplyClicked()
{
    slotSaveStuff();
    emit applyClicked();
}

void MicroSettingsDlg::slotSaveStuff()
{
    for (int i = 0; i < m_portNames.size(); i++)
        savePort(i);

    m_pMicroSettings->removeAllVariables();
    for (int i = 0; i < m_pWidget->variables->rowCount(); i++)
        saveVariable(i);

    m_pMicroSettings->setPinMappings(m_pinMappings);
}

void MicroSettingsDlg::reject()
{
    QDialog::reject();
    deleteLater();
}

QValidator::State MicroSettingsDlg::validatePinMapName(QString &name) const
{
    name.replace(' ', '_');

    if (name.isEmpty())
        return QValidator::Intermediate;

    for (int i = 0; i < name.length(); ++i) {
        if (!name[i].isLetterOrNumber() && name[i] != '_')
            return QValidator::Invalid;
    }

    if (name[0].isNumber())
        return QValidator::Intermediate;

    if (m_pWidget->pinMapCombo->contains(name))
        return QValidator::Intermediate;

    return QValidator::Acceptable;
}

void MicroSettingsDlg::slotCheckNewPinMappingName(const QString &name)
{
    // Validate name might change the name so that it is valid
    QString newName = name;

    if (m_pNewPinMappingOkButton) {
        m_pNewPinMappingOkButton->setEnabled(validatePinMapName(newName) == QValidator::Acceptable);
    }

    if (newName != name)
        m_pNewPinMappingWidget->nameEdit->setText(newName);
}

void MicroSettingsDlg::slotCreatePinMap()
{
    m_pNewPinMappingDlg = new QDialog(this);
    m_pNewPinMappingDlg->setObjectName("New Pin Mapping Dlg");
    m_pNewPinMappingDlg->setModal(true);
    m_pNewPinMappingDlg->setWindowTitle(i18n("New Pin Mapping"));
    QVBoxLayout *mainLayout = new QVBoxLayout;
    m_pNewPinMappingDlg->setLayout(mainLayout);
    m_pNewPinMappingWidget = new NewPinMappingWidget(m_pNewPinMappingDlg);
    mainLayout->addWidget(m_pNewPinMappingWidget);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    m_pNewPinMappingOkButton = buttonBox->button(QDialogButtonBox::Ok);
    m_pNewPinMappingOkButton->setText(i18n("Create"));
    m_pNewPinMappingOkButton->setDefault(true);
    m_pNewPinMappingOkButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, m_pNewPinMappingDlg, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, m_pNewPinMappingDlg, &QDialog::reject);
    mainLayout->addWidget(buttonBox);

    PinMappingNameValidator *validator = new PinMappingNameValidator(this);
    m_pNewPinMappingWidget->nameEdit->setValidator(validator);

    connect(m_pNewPinMappingWidget->nameEdit, &KLineEdit::textChanged, this, &MicroSettingsDlg::slotCheckNewPinMappingName);
    slotCheckNewPinMappingName(nullptr);

    int accepted = m_pNewPinMappingDlg->exec();
    unsigned selectedType = m_pNewPinMappingWidget->typeCombo->currentIndex();
    QString name = m_pNewPinMappingWidget->nameEdit->text();

    delete m_pNewPinMappingDlg;
    delete validator;
    m_pNewPinMappingDlg = nullptr;
    m_pNewPinMappingWidget = nullptr;
    m_pNewPinMappingOkButton = nullptr;
    if (accepted != QDialog::Accepted)
        return;

    PinMapping::Type type = PinMapping::Invalid;

    switch (selectedType) {
    case 0:
        type = PinMapping::SevenSegment;
        break;

    case 1:
        type = PinMapping::Keypad_4x3;
        break;

    case 2:
        type = PinMapping::Keypad_4x4;
        break;

    default:
        qCCritical(KTL_LOG) << "Unknown selected type " << type;
        break;
    }

    m_pinMappings[name] = PinMapping(type);
    m_pWidget->pinMapCombo->insertItem(m_pWidget->pinMapCombo->count(), name);
    // m_pWidget->pinMapCombo->setCurrentItem( m_pWidget->pinMapCombo->count() - 1 );
    m_pWidget->pinMapCombo->setCurrentItem(name);

    updatePinMapButtons();
    slotModifyPinMap();
}

void MicroSettingsDlg::slotRenamePinMap()
{
    KComboBox *combo = m_pWidget->pinMapCombo;

    QString oldName = combo->currentText();
    if (oldName.isEmpty())
        return;

    PinMappingNameValidator *validator = new PinMappingNameValidator(this, oldName);

    bool ok = false;
    QString newName = InputDialog::getText(i18n("New Pin Map Name"), i18n("Name"), oldName, &ok, this, /* 0, */ validator);

    delete validator;

    if (!ok)
        return;

    if (newName == oldName)
        return;

    m_pinMappings[newName] = m_pinMappings[oldName];
    m_pinMappings.remove(oldName);

    // combo->setCurrentText( newName ); // 2018.12.02
    combo->setItemText(combo->currentIndex(), newName);
}

void MicroSettingsDlg::slotModifyPinMap()
{
    QString name = m_pWidget->pinMapCombo->currentText();
    PinMapping pinMapping = m_pinMappings[name];

    PinMapEditor *pinMapEditor = new PinMapEditor(&pinMapping, m_pMicroSettings->microInfo(), this);
    pinMapEditor->setObjectName("PinMapEditor");
    int accepted = pinMapEditor->exec();

    delete pinMapEditor;

    if (accepted != QDialog::Accepted)
        return;

    m_pinMappings[name] = pinMapping;
}

void MicroSettingsDlg::slotRemovePinMap()
{
    KComboBox *combo = m_pWidget->pinMapCombo;

    QString pinMapID = combo->currentText();
    if (pinMapID.isEmpty())
        return;

    m_pinMappings.remove(pinMapID);
    combo->removeItem(combo->currentIndex());

    updatePinMapButtons();
}

void MicroSettingsDlg::updatePinMapButtons()
{
    bool havePinMaps = (m_pWidget->pinMapCombo->count() != 0);

    m_pWidget->pinMapModify->setEnabled(havePinMaps);
    m_pWidget->pinMapRename->setEnabled(havePinMaps);
    m_pWidget->pinMapRemove->setEnabled(havePinMaps);
}

void MicroSettingsDlg::savePort(int row)
{
    QString port = m_portNames[row];

    int type, state;

    QString typeText = m_portTypeEdit[row]->text();
    bool typeOk = true;
    if (typeText.startsWith("0x", Qt::CaseInsensitive))
        type = typeText.remove(0, 2).toInt(&typeOk, 16);
    else if (typeText.contains(QRegularExpression("[^01]")))
        type = typeText.toInt(&typeOk, 10);
    else
        type = typeText.toInt(&typeOk, 2);

    if (!typeOk) {
        // 		KMessageBox::error( this, i18n("Unregnised Port Type: %1", typeText) );
        return;
    }

    QString stateText = m_portStateEdit[row]->text();
    bool stateOk = true;
    if (stateText.startsWith("0x", Qt::CaseInsensitive))
        state = stateText.remove(0, 2).toInt(&stateOk, 16);
    else if (stateText.contains(QRegularExpression("[^01]")))
        state = stateText.toInt(&stateOk, 10);
    else
        state = stateText.toInt(&stateOk, 2);

    if (!stateOk) {
        // 		KMessageBox::error( this, i18n("Unregnised Port State: %1", stateText) );
        return;
    }

    m_pMicroSettings->setPortState(port, state);
    m_pMicroSettings->setPortType(port, type);
}

void MicroSettingsDlg::saveVariable(int row)
{
    QTableWidgetItem *nameItem = m_pWidget->variables->item(row, 0);
    if (!nameItem) {
        return;
    }
    QString name = nameItem->text();
    if (name.isEmpty())
        return;

    QTableWidgetItem *valueItem = m_pWidget->variables->item(row, 1);
    QString valueText;
    if (valueItem) {
        valueText = valueItem->text();
    }
    int value;
    bool ok = true;
    if (valueText.startsWith("0x", Qt::CaseInsensitive))
        value = valueText.remove(0, 2).toInt(&ok, 16);
    else
        value = valueText.toInt(&ok, 10);

    if (!ok) {
        KMessageBox::error(this, i18n("Invalid variable value: %1", valueText));
        return;
    }

    qCDebug(KTL_LOG) << "save variable: " << name << " val: " << value;

    m_pMicroSettings->setVariable(name, value, true);
    VariableInfo *info = m_pMicroSettings->variableInfo(name);
    if (info && info->valueAsString().toInt() != value) {
        // 		info->setValue(value);
        // 		info->permanent = true;
        info->initAtStart = true;
    }
}

void MicroSettingsDlg::checkAddVariableRow()
{
    int lastRow = m_pWidget->variables->rowCount() - 1;
    if (QTableWidgetItem *lastItem = m_pWidget->variables->item(lastRow, 0)) {
        if (!lastItem->text().isEmpty()) {
            m_pWidget->variables->insertRow(lastRow + 1);
        }
    }
}

#include "moc_microsettingsdlg.cpp"
