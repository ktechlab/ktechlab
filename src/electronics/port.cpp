/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "port.h"

#include <QScopedPointer>
#include <QSerialPort>
#include <QSerialPortInfo>

#ifdef Q_OS_UNIX
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#ifdef Q_OS_LINUX
#include <linux/ppdev.h>
#endif

#include <ktechlab_debug.h>

// BEGIN class Port
Port::Port()
{
}

Port::~Port()
{
}

QStringList Port::ports()
{
    return SerialPort::ports() + ParallelPort::ports();
}
// END class Port

// BEGIN class SerialPort
SerialPort::SerialPort()
{
    m_port = nullptr;
}

SerialPort::~SerialPort()
{
    closePort();
}

#ifdef Q_OS_UNIX
static void unixPinEnable(QSerialPort *port, SerialPort::Pin pin, bool state, int flags)
{
    if (ioctl(port->handle(), state ? TIOCMBIS : TIOCMBIC, &flags) == -1)
        qCCritical(KTL_LOG) << "Could not set pin" << pin << "errno = " << errno;
}
#endif

void SerialPort::setPinState(Pin pin, bool state)
{
    if (!m_port)
        return;

    switch (pin) {
    case TD:
        m_port->setBreakEnabled(state);
        return;

    case DTR:
        m_port->setDataTerminalReady(state);
        return;

    case DSR:
#ifdef Q_OS_UNIX
        unixPinEnable(m_port, pin, state, TIOCM_DSR);
#else
        qCWarning(KTL_LOG) << "Cannot set pin" << pin << "on non-Unix OS";
#endif
        return;

    case RTS:
        m_port->setRequestToSend(state);
        return;

    default:
        qCCritical(KTL_LOG) << "Bad pin" << pin;
    };
}

bool SerialPort::pinState(Pin pin)
{
    if (!m_port)
        return false;

    int mask = 0;

    switch (pin) {
    case CD:
        mask = QSerialPort::DataCarrierDetectSignal;
        break;

    case RD:
        mask = QSerialPort::SecondaryReceivedDataSignal;
        break;

    case CTS:
        mask = QSerialPort::ClearToSendSignal;
        break;

    case RI:
        mask = QSerialPort::RingIndicatorSignal;
        break;

    case TD:
    case DTR:
    case GND:
    case DSR:
    case RTS:
        break;
    }

    if (mask == 0) {
        qCCritical(KTL_LOG) << "Bad pin " << pin << endl;
        return false;
    }

    const QSerialPort::PinoutSignals bits = m_port->pinoutSignals();

    return bits & mask;
}

bool SerialPort::openPort(const QString &port, qint32 baudRate)
{
    closePort();

    QScopedPointer<QSerialPort> newPort(new QSerialPort(port));
    newPort->setDataBits(QSerialPort::Data8);
    newPort->setBaudRate(baudRate);
    newPort->setStopBits(QSerialPort::OneStop);
    newPort->setFlowControl(QSerialPort::NoFlowControl);
    if (!newPort->open(QIODevice::ReadWrite)) {
        qCCritical(KTL_LOG) << "Could not open port " << port << endl;
        return false;
    }

    m_port = newPort.take();

    return true;
}

void SerialPort::closePort()
{
    if (!m_port)
        return;

    m_port->close();
    delete m_port;
    m_port = nullptr;
}

QStringList SerialPort::ports()
{
    QStringList list;

    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
        list << serialPortInfo.systemLocation();
    }

    return list;
}

bool SerialPort::isAvailable()
{
    return true;
}
// END class SerialPort

// BEGIN class ParallelPort
// I wasn't able to find any documentation on programming the parallel port
// in Darwin, so I've just functionally neutered this section.  Apparently
// parallel output is handled on a case by case basis (???) by the
// manufacturer of whatever USB dongle is, unless they build it as a
// Comms class device, in which case it is treated as a serial device.
// ( Info from Garth Cummings, Apple Developer Technical Support )

#ifdef Q_OS_LINUX

const int IRQ_MODE_BIT = 1 << 20;   // Controls if pin 10 (Ack) causes interrupts
const int INPUT_MODE_BIT = 1 << 21; // Controls if the data pins are input or output

const unsigned long IOCTL_REG_READ[3] = {
    PPRDATA,
    PPRSTATUS,
    PPRCONTROL,
};

const unsigned long IOCTL_REG_WRITE[3] = {
    PPWDATA,
    0,
    PPWCONTROL,
};

// const int INVERT_MASK[3] = { // 2017.10.01 - comment unused code
// 	0x0,
// 	0x80, // 10000000
// 	0x0b, // 00001011
// };

#endif

ParallelPort::ParallelPort()
{
    reset();
}

ParallelPort::~ParallelPort()
{
}

void ParallelPort::reset()
{
#ifdef Q_OS_LINUX
    m_file = -1;
    m_reg[Data] = 0;
    m_reg[Status] = 0;
    m_reg[Control] = 0;
    m_outputPins = INPUT_MODE_BIT | IRQ_MODE_BIT;
    m_inputPins = STATUS_PINS | INPUT_MODE_BIT | IRQ_MODE_BIT;
#endif
}

// BEGIN Pin-oriented operations
void ParallelPort::setPinState(int pins, bool state)
{
#ifdef Q_OS_LINUX
    // only allow writing to output pins
    pins &= m_outputPins;

    if (pins & DATA_PINS)
        setDataState((pins & DATA_PINS) >> 0, state);

    if (pins & CONTROL_PINS)
        setControlState((pins & CONTROL_PINS) >> 16, state);
#else
    Q_UNUSED(pins);
    Q_UNUSED(state);
#endif
}

int ParallelPort::pinState(int pins)
{
    int value = 0;

#ifdef Q_OS_LINUX
    // only allow reading from input pins
    pins &= m_inputPins;

    if (pins & DATA_PINS)
        value |= ((readFromRegister(Data) & ((pins & DATA_PINS) >> 0)) << 0);

    if (pins & STATUS_PINS)
        value |= ((readFromRegister(Status) & ((pins & STATUS_PINS) >> 8)) << 8);

    if (pins & CONTROL_PINS)
        value |= ((readFromRegister(Control) & ((pins & CONTROL_PINS) >> 16)) << 16);
#else
    Q_UNUSED(pins);
#endif

    return value;
}

void ParallelPort::setDataState(uchar pins, bool state)
{
    uchar value = readFromRegister(Data);

    if (state)
        value |= pins;
    else
        value &= ~pins;

    writeToData(value);
}

void ParallelPort::setControlState(uchar pins, bool state)
{
    uchar value = readFromRegister(Control);

    if (state)
        value |= pins;
    else
        value &= ~pins;

    writeToControl(value);
}
// END Pin-oriented operations

// BEGIN Register-oriented operations
uchar ParallelPort::readFromRegister(Register reg)
{
#ifdef Q_OS_LINUX
    if (m_file == -1)
        return 0;

    // 	uchar value = inb( m_lpBase + reg ) ^ INVERT_MASK[reg];
    uchar value = 0;
    if (ioctl(m_file, IOCTL_REG_READ[reg], &value))
        qCCritical(KTL_LOG) << "errno=" << errno << endl;
    else
        m_reg[reg] = value;
    return value;
#else
    Q_UNUSED(reg);
    return 0;
#endif
}

void ParallelPort::writeToRegister(Register reg, uchar value)
{
#ifdef Q_OS_LINUX
    if (m_file == -1)
        return;

    // 	outb( value ^ INVERT_MASK[reg], m_lpBase + reg );
    if (ioctl(m_file, IOCTL_REG_WRITE[reg], &value))
        qCCritical(KTL_LOG) << "errno=" << errno << endl;
    else
        m_reg[reg] = value;
#else
    Q_UNUSED(reg);
    Q_UNUSED(value);
#endif
}

void ParallelPort::writeToData(uchar value)
{
    writeToRegister(Data, value);
}

void ParallelPort::writeToControl(uchar value)
{
#ifdef Q_OS_LINUX
    // Set all inputs to ones
    value |= ((m_inputPins & CONTROL_PINS) >> 16);
#endif

    writeToRegister(Control, value);
}
// END Register-oriented operations

// BEGIN Changing pin directions
void ParallelPort::setDataDirection(Direction dir)
{
#ifdef Q_OS_LINUX
    if (dir == Input) {
        m_inputPins |= DATA_PINS;
        m_outputPins &= ~DATA_PINS;
    } else {
        m_inputPins &= DATA_PINS;
        m_outputPins |= ~DATA_PINS;
    }

    setPinState(INPUT_MODE_BIT, dir == Input);
#else
    Q_UNUSED(dir);
#endif
}

void ParallelPort::setControlDirection(int pins, Direction dir)
{
#ifdef Q_OS_LINUX
    pins &= CONTROL_PINS;

    if (dir == Input) {
        m_inputPins |= pins;
        m_outputPins &= ~pins;
    } else {
        m_inputPins &= pins;
        m_outputPins |= ~pins;
    }

    setControlState(0, true);
#else
    Q_UNUSED(pins);
    Q_UNUSED(dir);
#endif
}
// END Changing pin directions

Port::ProbeResult ParallelPort::probe(const QString &port)
{
#ifdef Q_OS_LINUX
    int file = open(port.toAscii(), O_RDWR);
    if (file == -1)
        return Port::DoesntExist;

    if (ioctl(file, PPCLAIM) != 0) {
        close(file);
        return Port::ExistsButNotRW;
    }

    ioctl(file, PPRELEASE);
    close(file);
    return Port::ExistsAndRW;
#else
    Q_UNUSED(port);
    return Port::DoesntExist;
#endif
}

QStringList ParallelPort::ports()
{
    QStringList list;

#ifdef Q_OS_LINUX
    for (unsigned i = 0; i < 8; ++i) {
        QString dev = QString("/dev/parport%1").arg(i);
        if (probe(dev) & ExistsAndRW)
            list << dev;
    }

    for (unsigned i = 0; i < 8; ++i) {
        QString dev = QString("/dev/parports/%1").arg(i);
        if (probe(dev) & ExistsAndRW)
            list << dev;
    }
#endif

    return list;
}

bool ParallelPort::openPort(const QString &port)
{
#ifdef Q_OS_LINUX
    if (m_file != -1) {
        qCWarning(KTL_LOG) << "Port already open" << endl;
        return false;
    }

    m_file = open(port.toAscii(), O_RDWR);

    if (m_file == -1) {
        qCCritical(KTL_LOG) << "Could not open port \"" << port << "\": errno=" << errno << endl;
        return false;
    }

    if (ioctl(m_file, PPCLAIM)) {
        qCCritical(KTL_LOG) << "Port " << port << " must be RW" << endl;
        close(m_file);
        m_file = -1;
        return false;
    }

    return true;
#else
    Q_UNUSED(port);
    return false;
#endif
}

void ParallelPort::closePort()
{
#ifdef Q_OS_LINUX
    if (m_file == -1)
        return;

    int res = ioctl(m_file, PPRELEASE);
    close(m_file);

    if (res)
        qCCritical(KTL_LOG) << "res=" << res << endl;

    m_file = -1;
#endif
}

bool ParallelPort::isAvailable()
{
#ifdef Q_OS_LINUX
    return true;
#else
    return false;
#endif
}
// END class ParallelPort
