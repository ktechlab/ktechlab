/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PORT_H
#define PORT_H

#include <QStringList>

class QSerialPort;

/**
@author David Saxton
 */
class Port
{
public:
    enum ProbeResult { ExistsAndRW = 1 << 0, ExistsButNotRW = 1 << 1, DoesntExist = 1 << 2 };

    Port();
    virtual ~Port();

    /**
     * Returns a list of available ports.
     * This function just returns the combination of the lists for
     * SerialPort::ports and ParallelPort::ports.
     */
    static QStringList ports();
};

/**
Abstraction for a serial port, allowing control over individual pins.

@author David Saxton
 */
class SerialPort : public Port
{
public:
    SerialPort();
    ~SerialPort() override;

    void setBreakEnabled(bool state);
    void setDataTerminalReady(bool state);
    void setDataSetReady(bool state);
    void setRequestToSend(bool state);

    bool getDataCarrierDetectSignal();
    bool getSecondaryReceivedDataSignal();
    bool getClearToSendSignal();
    bool getRingIndicatorSignal();

    /**
     * @see Port::ports
     */
    static QStringList ports();
    /**
     * Opens the given port.
     * @return if the port could be opened.
     * @param baudRate The baud rate
     */
    bool openPort(const QString &port, qint32 baudRate);
    /**
     * Closes any currently open port.
     */
    void closePort();

    static bool isAvailable();

protected:
    QSerialPort *m_port;
};

/**
Abstraction for a parallel port, allowing control over individual pins.
Based loosely on code in the parapin project; see http://parapin.sf.net

@author David Saxton
*/
class ParallelPort : public Port
{
public:
    enum Pin {
        // Data Register
        //   Offset: Base + 0
        //   Readable / writable
        PIN02 = 1 << 0, // Data 0
        PIN03 = 1 << 1, // Data 1
        PIN04 = 1 << 2, // Data 2
        PIN05 = 1 << 3, // Data 3
        PIN06 = 1 << 4, // Data 4
        PIN07 = 1 << 5, // Data 5
        PIN08 = 1 << 6, // Data 6
        PIN09 = 1 << 7, // Data 7
        DATA_PINS = PIN02 | PIN03 | PIN04 | PIN05 | PIN06 | PIN07 | PIN08 | PIN09,

        // Status Register
        //   Offset: Base + 1
        //   Read only
        PIN15 = 1 << 11, // Error
        PIN13 = 1 << 12, // Online
        PIN12 = 1 << 13, // Paper End
        PIN10 = 1 << 14, // Ack
        PIN11 = 1 << 15, // Busy
        STATUS_PINS = PIN15 | PIN13 | PIN12 | PIN10 | PIN11,

        // Control Register
        //   Offset: Base + 2
        //   Readable / writable
        PIN01 = 1 << 16, // Strobe
        PIN14 = 1 << 17, // Auto Feed
        PIN16 = 1 << 18, // Init
        PIN17 = 1 << 19, // Select
        CONTROL_PINS = PIN01 | PIN14 | PIN16 | PIN17

        // Pins 18 to 25 are ground
    };

    enum Register { Data = 0, Status = 1, Control = 2 };

    /**
     * For setting the direction of the Data register or the Control pins.
     */
    enum Direction { Input = 0, Output = 1 };

    ParallelPort();
    ~ParallelPort() override;

    /**
     * Opens the given port.
     * @return if the port could be opened.
     */
    bool openPort(const QString &port);
    /**
     * Closes any currently open port.
     */
    void closePort();

    // BEGIN Pin-oriented operations
    /**
     * @param pins A list of ParallelPort::Pin OR'd together.
     */
    void setPinState(int pins, bool state);
    /**
     * @return the pin states for the given list of pins.
     */
    int pinState(int pins);
    /**
     * Sets the given pins to the given state in the Data register.
     */
    void setDataState(uchar pins, bool state);
    /**
     * Sets the given pins to the given state in the Control register.
     */
    void setControlState(uchar pins, bool state);
    // END Pin-oriented operations

    // BEGIN Register-oriented operations
    /**
     * Reads and stores the value in the given registers, and returns it.
     */
    uchar readFromRegister(Register reg);
    /**
     * Write the given value to the Data register.
     */
    void writeToData(uchar value);
    /**
     * Write the given value to the Control register (any input pins will be
     * set to one).
     */
    void writeToControl(uchar value);
    // END Register-oriented operations

    // BEGIN Changing pin directions
    /**
     * Sets the (input / output) direction of the data pins.
     */
    void setDataDirection(Direction dir);
    /**
     * Sets the given pins to the given direction.
     */
    void setControlDirection(int pins, Direction dir);
    // END Changing pin directions

    static ProbeResult probe(const QString &port);
    /**
     * @see Port::ports
     */
    static QStringList ports();

    static bool isAvailable();

protected:
    /**
     * Writes the value to the given register.
     */
    void writeToRegister(Register reg, uchar value);
    void reset();

#ifdef Q_OS_LINUX
    uchar m_reg[3];

    /// Mask of the pins that are currently set as input
    int m_inputPins;

    /// Mask of the pins that are currently set as output
    int m_outputPins;

    /// File descriptor for the port.
    int m_file;
#endif
};

#endif
