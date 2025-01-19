/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MICROSETTINGS_H
#define MICROSETTINGS_H

#include <QObject>
#include <QVariant>

class QString;
class QVariant;
class MicroData;
class MicroInfo;

class VariableInfo
{
public:
    VariableInfo();

    // Returns the value as a string
    QString valueAsString() const;

    // MicroSettings::VariableType (don't rely on this just yet...)
    int type;

    // Sets the value
    void setValue(const QVariant &value);

    // If true, the variable will be initialised at the start of the FlowCode
    // to the given value
    bool initAtStart;

    // True if the variable was "created" by the user in the variable dialog,
    // as opposed to being from a variable name entry box
    bool permanent;

private:
    QVariant value;
};

typedef QMap<QString, VariableInfo> VariableMap; // Variable name, variable info

/**
@short Stores pic pin settings - type/state
@author David Saxton
*/
class PinSettings : public QObject
{
    Q_OBJECT
public:
    enum pin_type { pt_input, pt_output };

    enum pin_state { ps_on, ps_off };

    PinSettings();
    PinSettings(PinSettings::pin_type _type, PinSettings::pin_state _state, const QString &id, const QString &port);

    PinSettings::pin_type type() const
    {
        return m_type;
    }
    PinSettings::pin_state state() const
    {
        return m_state;
    }
    QString id() const
    {
        return m_id;
    }
    QString port() const
    {
        return m_port;
    }

    void setType(PinSettings::pin_type type);
    void setState(PinSettings::pin_state state);

signals:
    /**
     * Emitted when either the type or the state is changed.
     */
    void settingsChanged();

private:
    PinSettings::pin_type m_type;
    PinSettings::pin_state m_state;
    QString m_id;
    QString m_port;
};
typedef QList<PinSettings *> PinSettingsList;

class PinMapping;
typedef QMap<QString, PinMapping> PinMappingMap;
typedef QMap<QString, PinSettingsList> PortList;

/**
This class stores PIC settings that are specific to the PIC program being developed.
This includes such things as port settings and variable settings.
This is different from PIC info, which includes stuff such as PIC pin names

@short Stores Pic settings - pin settings
@author David Saxton
*/
class MicroSettings : public QObject
{
    Q_OBJECT
public:
    enum VariableType { vt_signedInteger, vt_unsignedInteger, vt_unknown };
    MicroSettings(MicroInfo *microInfo);
    ~MicroSettings() override;
    /**
     * Returns microdata to describe the microsettings.
     * This includes ports settings and variable settings
     */
    MicroData microData() const;
    void restoreFromMicroData(const MicroData &microData);
    /**
     * Returns a pointer to the MicroInfo object for the PIC in use
     */
    MicroInfo *microInfo() const
    {
        return _microInfo;
    }
    /**
     * Set the pin with the given id to the given initial type (input/output)
     */
    void setPinType(const QString &id, PinSettings::pin_type type);
    /**
     * Set the pin with the given id to the given initial state (on/off)
     */
    void setPinState(const QString &id, PinSettings::pin_state state);
    /**
     * Returns a pointer to the PinSettings for the pin with the given id,
     * or null if no such pin exists.
     */
    PinSettings *pinWithID(const QString &id);
    /**
     * Returns the initial port state (on/off) for the given port.
     * Each pin state occupies one bit of the returned integer.
     */
    int portState(const QString &port);
    /**
     * Sets the port with the given name to the given state
     */
    void setPortState(const QString &port, int state);
    /**
     * Sets the port with the given name to the given type
     */
    void setPortType(const QString &port, int type);
    /**
     * Returns the initial port type (input/output) for the given port.
     * Each pin type occupies one bit of the returned integer.
     */
    int portType(const QString &port);
    /**
     * Sets the variable "name" to the initial value "value. If the variable
     * already exists, its value will be changed. Else, the variable will be
     * created.
     */
    void setVariable(const QString &name, QVariant value, bool permanent = true);
    /**
     * Returns the list of initial variables as a QStringList, just the names
     * without the values. Generated from the VariableMap m_variables.
     */
    QStringList variableNames();
    /**
     * Returns a pointer to the variable info with the given name, or nullptr
     * if the variable is not found
     */
    VariableInfo *variableInfo(const QString &name);
    /**
     * Deletes the variable with the given name, returns true if successful
     * (i.e. a variable with that name existed), or false if not
     */
    bool deleteVariable(const QString &name);
    /**
     * Removes all variables
     */
    void removeAllVariables();
    /**
     * Sets the list of Pin Mappings to that given.
     */
    void setPinMappings(const PinMappingMap &pinMappings);
    /**
     * Returns the pic pin mapping with the given id.
     */
    PinMapping pinMapping(const QString &id) const;
    /**
     * Returns the list of different Pin Mappings;
     */
    PinMappingMap pinMappings() const;

signals:
    void pinMappingsChanged();

private:
    PinMappingMap m_pinMappings;
    PinSettingsList m_pinSettingsList;
    MicroInfo *_microInfo;
    VariableMap m_variableMap;
    PortList m_ports;
};

#endif
