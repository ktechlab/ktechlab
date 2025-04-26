/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "itemdocumentdata.h"
#include "microinfo.h"
#include "micropackage.h"
#include "pinmapping.h"

// BEGIN class VariableInfo
VariableInfo::VariableInfo()
{
    type = MicroSettings::vt_unknown;
    value = QVariant(0);
    initAtStart = false;
    permanent = false;
}

void VariableInfo::setValue(const QVariant &_value)
{
    value = _value;
}

QString VariableInfo::valueAsString() const
{
    if (value.canConvert(QMetaType(QMetaType::QString)))
        return value.toString();
    if (value.canConvert(QMetaType(QMetaType::Int)))
        return QString::number(value.toInt());
    return "0";
}
// END class VariableInfo

// BEGIN class PinSettings
PinSettings::PinSettings()
    : QObject()
{
    m_type = PinSettings::pt_input;
    m_state = PinSettings::ps_off;
    m_id = "pin";
}

PinSettings::PinSettings(PinSettings::pin_type _type, PinSettings::pin_state _state, const QString &id, const QString &port)
{
    m_type = _type;
    m_state = _state;
    m_id = id;
    m_port = port;
}

void PinSettings::setType(PinSettings::pin_type type)
{
    if (m_type == type)
        return;
    m_type = type;
    emit settingsChanged();
}

void PinSettings::setState(PinSettings::pin_state state)
{
    if (m_state == state)
        return;
    m_state = state;
    emit settingsChanged();
}
// END class PinSettings

// BEGIN class MicroSettings
MicroSettings::MicroSettings(MicroInfo *microInfo)
{
    _microInfo = microInfo;

    QStringList portNames = _microInfo->package()->portNames();
    const QStringList::iterator portNamesEnd = portNames.end();
    for (QStringList::iterator it = portNames.begin(); it != portNamesEnd; ++it) {
        PinSettingsList portPins;
        QStringList pinIDs;
        pinIDs = _microInfo->package()->pinIDs(PicPin::type_bidir | PicPin::type_open, *it);
        pinIDs.sort();
        const int numPins = pinIDs.size();
        for (int i = 0; i < numPins; i++) {
            PinSettings *pinSettings = new PinSettings(PinSettings::pt_input, PinSettings::ps_off, pinIDs[i], *it);
            m_pinSettingsList.append(pinSettings);
            portPins.append(pinSettings);
        }
        m_ports[*it] = portPins;
    }
}

MicroSettings::~MicroSettings()
{
    qDeleteAll(m_pinSettingsList);
    // 	delete m_variableList;
}

void MicroSettings::setPinType(const QString &id, PinSettings::pin_type type)
{
    PinSettings *pin = pinWithID(id);
    if (pin)
        pin->setType(type);
}

void MicroSettings::setPinState(const QString &id, PinSettings::pin_state state)
{
    PinSettings *pin = pinWithID(id);
    if (pin)
        pin->setState(state);
}

PinSettings *MicroSettings::pinWithID(const QString &id)
{
    const PinSettingsList::iterator pinListEnd = m_pinSettingsList.end();
    for (PinSettingsList::iterator it = m_pinSettingsList.begin(); it != pinListEnd; ++it) {
        if ((*it)->id() == id)
            return *it;
    }
    return nullptr;
}

int MicroSettings::portState(const QString &port)
{
    if (microInfo()->package()->portNames().indexOf(port) == -1)
        return -1;

    int pinPower = 1;
    int num = 0;

    const PinSettingsList::iterator pinListEnd = m_pinSettingsList.end();
    for (PinSettingsList::iterator it = m_pinSettingsList.begin(); it != pinListEnd; ++it) {
        if ((*it)->port() == port) {
            if ((*it)->state() == PinSettings::ps_on)
                num += pinPower;
            // 			cout << "(*it)->state()="<<(*it)->state()<<endl;
            pinPower *= 2;
        }
    }

    return num;
}

int MicroSettings::portType(const QString &port)
{
    if (microInfo()->package()->portNames().indexOf(port) == -1)
        return -1;

    int pinPower = 1;
    int num = 0;

    const PinSettingsList::iterator pinListEnd = m_pinSettingsList.end();
    for (PinSettingsList::iterator it = m_pinSettingsList.begin(); it != pinListEnd; ++it) {
        if ((*it)->port() == port) {
            if ((*it)->type() == PinSettings::pt_input)
                num += pinPower;
            pinPower *= 2;
        }
    }

    return num;
}

void MicroSettings::setPortState(const QString &port, int state)
{
    PortList::iterator plit = m_ports.find(port);
    if (plit == m_ports.end())
        return;

    const PinSettingsList::iterator plitEnd = plit.value().end();
    for (PinSettingsList::iterator it = plit.value().begin(); it != plitEnd; ++it) {
        // 		cout << "state="<<state<<endl;
        (*it)->setState((state % 2 == 1) ? PinSettings::ps_on : PinSettings::ps_off);
        // 		cout << "(*it)->state()="<<(*it)->state()<<endl;
        state /= 2;
    }
}

void MicroSettings::setPortType(const QString &port, int type)
{
    PortList::iterator plit = m_ports.find(port);
    if (plit == m_ports.end())
        return;

    const PinSettingsList::iterator plitEnd = plit.value().end();
    for (PinSettingsList::iterator it = plit.value().begin(); it != plitEnd; ++it) {
        (*it)->setType((type % 2 == 1) ? PinSettings::pt_input : PinSettings::pt_output);
        type /= 2;
    }
}

MicroData MicroSettings::microData() const
{
    MicroData data;
    data.id = microInfo()->id();
    data.pinMappings = pinMappings();

    const PinSettingsList::const_iterator pinListEnd = m_pinSettingsList.end();
    for (PinSettingsList::const_iterator it = m_pinSettingsList.begin(); it != pinListEnd; ++it) {
        data.pinMap[(*it)->id()].type = (*it)->type();
        data.pinMap[(*it)->id()].state = (*it)->state();
    }

    const VariableMap::const_iterator variableMapEnd = m_variableMap.end();
    for (VariableMap::const_iterator it = m_variableMap.begin(); it != variableMapEnd; ++it) {
        if (it.value().permanent)
            data.variableMap[it.key()] = it.value().valueAsString();
    }

    return data;
}

void MicroSettings::restoreFromMicroData(const MicroData &microData)
{
    setPinMappings(microData.pinMappings);

    const PinDataMap::const_iterator pinMapEnd = microData.pinMap.end();
    for (PinDataMap::const_iterator it = microData.pinMap.begin(); it != pinMapEnd; ++it) {
        PinSettings *pin = pinWithID(it.key());
        if (pin) {
            pin->setState(it.value().state);
            pin->setType(it.value().type);
        }
    }

    const QStringMap::const_iterator variableMapEnd = microData.variableMap.end();
    for (QStringMap::const_iterator it = microData.variableMap.begin(); it != variableMapEnd; ++it) {
        setVariable(it.key(), it.value(), true);
    }
}

void MicroSettings::setVariable(const QString &name, QVariant value, bool permanent)
{
    if (name.isNull())
        return;
    VariableMap::iterator it = m_variableMap.find(name);
    if (it != m_variableMap.end()) {
        it.value().setValue(value);
        it.value().permanent = permanent;
        it.value().initAtStart = permanent;
    } else {
        VariableInfo info;
        info.setValue(value);
        info.permanent = permanent;
        info.initAtStart = permanent;
        m_variableMap[name] = info;
    }
}

QStringList MicroSettings::variableNames()
{
    QStringList list;
    const VariableMap::iterator variableMapEnd = m_variableMap.end();
    for (VariableMap::iterator it = m_variableMap.begin(); it != variableMapEnd; ++it) {
        list += it.key();
    }
    return list;
}

VariableInfo *MicroSettings::variableInfo(const QString &name)
{
    if (name.isNull())
        return nullptr;
    VariableMap::iterator it = m_variableMap.find(name);
    if (it != m_variableMap.end()) {
        return &(it.value());
    } else {
        return nullptr;
    }
}

bool MicroSettings::deleteVariable(const QString &name)
{
    if (name.isNull())
        return false;
    VariableMap::iterator it = m_variableMap.find(name);
    if (it != m_variableMap.end()) {
        m_variableMap.erase(it);
        return true;
    } else {
        return false;
    }
}

void MicroSettings::removeAllVariables()
{
    m_variableMap.clear();
}

PinMapping MicroSettings::pinMapping(const QString &id) const
{
    return m_pinMappings[id];
}

void MicroSettings::setPinMappings(const PinMappingMap &pinMappings)
{
    m_pinMappings = pinMappings;
    emit pinMappingsChanged();
}

PinMappingMap MicroSettings::pinMappings() const
{
    return m_pinMappings;
}
// END class MicroSettings

#include "moc_microsettings.cpp"
