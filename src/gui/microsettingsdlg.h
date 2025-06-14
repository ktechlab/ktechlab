/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MICROSETTINGSDLG_H
#define MICROSETTINGSDLG_H

#include <QDialog>
#include <QMap>
#include <QValidator>
// #include <q3valuevector.h>

class KLineEdit;
class MicroSettings;
class MicroSettingsWidget;
class NewPinMappingWidget;
class PinMapping;
class QPushButton;

typedef QMap<QString, PinMapping> PinMappingMap;

/**
@author David Saxton
*/
class MicroSettingsDlg : public QDialog
{
    Q_OBJECT
public:
    MicroSettingsDlg(MicroSettings *_microSettings, QWidget *parent = nullptr);
    ~MicroSettingsDlg() override;

    void reject() override;
    void accept() override;

    /**
     * @param pinMapName the pinMapName; may be changed to make it valid
     * (e.g. spaces replaced with underscores).
     * @returns Invalid for a pinMapName containing a non-variable name,
     * Intermediate for a pinMapName that starts with a number or is already
     * in use, and Acceptable otherwise.
     */
    QValidator::State validatePinMapName(QString &pinMapName) const;

public slots:
    /**
     * Saves the port details in the given row to the MicroSettings class.
     * Usually called when the value is changed, or on 'Apply' of the
     * dialog.
     */
    void savePort(int row);
    /**
     * Saves the variable details to the MicroSettings class.
     */
    void saveVariable(int row);
    /**
     * Adds an extra row to the list of variable if one is required.
     */
    void checkAddVariableRow();
    /**
     * Called when the pinMapAdd button is pressed.
     */
    void slotCreatePinMap();
    /**
     * Called when the pinMapModify button is pressed.
     */
    void slotModifyPinMap();
    /**
     * Called when the pinMapRename button is pressed.
     */
    void slotRenamePinMap();
    /**
     * Called when the pinMapRemove button is pressed.
     */
    void slotRemovePinMap();
    /**
     * Called when the dialog is Applied or OK'd.
     */
    void slotSaveStuff();

Q_SIGNALS:
    void applyClicked();

protected slots:
    void slotCheckNewPinMappingName(const QString &name);
    void slotApplyClicked();

protected:
    /**
     * Set each button enabled / disabled as appropriate.
     */
    void updatePinMapButtons();

    NewPinMappingWidget *m_pNewPinMappingWidget; // Used for checking that the variable name is ok
    QDialog *m_pNewPinMappingDlg;
    QPushButton *m_pNewPinMappingOkButton;
    MicroSettingsWidget *m_pWidget;
    MicroSettings *m_pMicroSettings;
    PinMappingMap m_pinMappings;
    QVector<KLineEdit *> m_portTypeEdit;
    QVector<KLineEdit *> m_portStateEdit;
    QStringList m_portNames;
};

class PinMappingNameValidator : public QValidator
{
public:
    /**
     * Create a validator. If oldName is not empty, then the input is
     * allowed to be oldName.
     */
    PinMappingNameValidator(MicroSettingsDlg *dlg, const QString &oldName = nullptr)
        : QValidator(nullptr)
    {
        m_pDlg = dlg;
        m_oldName = oldName;
    }

    State validate(QString &input, int &) const override

    {
        if ((!m_oldName.isEmpty()) && (input == m_oldName))
            return QValidator::Acceptable;

        return m_pDlg->validatePinMapName(input);
    }

protected:
    MicroSettingsDlg *m_pDlg;
    QString m_oldName;
};

#endif
