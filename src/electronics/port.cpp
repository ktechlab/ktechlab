/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

// This section should be kept at the top to handle detection of os
#include <Qt/qglobal.h>
#if defined(Q_OS_DARWIN) || defined(Q_OS_MACX)
	#define DARWIN
#endif

#include "port.h"
#include <kdebug.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#ifndef DARWIN
#include <linux/ppdev.h>
#endif

//BEGIN class Port
Port::Port()
{
}


Port::~Port()
{
}


QStringList Port::ports( unsigned probeResult )
{
#ifndef DARWIN
	return SerialPort::ports(probeResult) + ParallelPort::ports(probeResult);
#else
	return SerialPort::ports(probeResult);
#endif
}
//END class Port



//BEGIN class SerialPort
SerialPort::SerialPort()
{
	m_file = -1;
}


SerialPort::~SerialPort()
{
	closePort();
}


void SerialPort::setPinState( Pin pin, bool state )
{
	if ( m_file == -1 )
		return;
	
	int flags = -1;
	
	switch ( pin )
	{
		case TD:
			ioctl( m_file, state ? TIOCSBRK : TIOCCBRK, 0 );
			return;
			
		case DTR:
			flags = TIOCM_DTR;
			break;
			
		case DSR:
			flags = TIOCM_DSR;
			break;
			
		case RTS:
			flags = TIOCM_RTS;
			break;
			
		case CD:
		case RD:
		case GND:
		case CTS:
		case RI:
			break;
	};
	
	if ( flags == -1 )
	{
		kError() << k_funcinfo << "Bad pin " << pin << endl;
		return;
	}
	
	if ( ioctl( m_file, state ? TIOCMBIS : TIOCMBIC, & flags ) == -1 )
		kError() << k_funcinfo << "Could not set pin " << pin << " errno = " << errno << endl;
}


bool SerialPort::pinState( Pin pin )
{
	if ( m_file == -1 )
		return false;
	
	int mask = 0;
	
	switch ( pin )
	{
		case CD:
			mask = TIOCM_CD;
			break;
			
		case RD:
			mask = TIOCM_SR;
			break;
			
		case CTS:
			mask = TIOCM_CTS;
			break;
			
		case RI:
			mask = TIOCM_RI;
			break;
			
		case TD:
		case DTR:
		case GND:
		case DSR:
		case RTS:
			break;
	}
	
	if ( mask == 0 )
	{
		kError() << k_funcinfo << "Bad pin " << pin << endl;
		return false;
	}
	
	int bits = 0;
	if ( ioctl( m_file, TIOCMGET, & bits ) == -1 )
	{
		kError() << k_funcinfo << "Could not read pin" << pin << " errno = " << errno << endl;
		return false;
	}
	
	return bits & mask;
}

		
Port::ProbeResult SerialPort::probe( const QString & port )
{
	int file = open( port.ascii(), O_NOCTTY | O_NONBLOCK | O_RDONLY );
	if ( file == -1 )
		return Port::DoesntExist;
	
	close(file);
	
	file = open( port.ascii(), O_NOCTTY | O_NONBLOCK | O_RDWR );
	if ( file == -1 )
		return Port::ExistsButNotRW;
	close(file);
	
	return Port::ExistsAndRW;
}


bool SerialPort::openPort( const QString & port, speed_t baudRate )
{
	closePort();
	
	m_file = open( port.ascii(), O_NOCTTY | O_NONBLOCK | O_RDWR );
	if ( m_file == -1 )
	{
		kError() << k_funcinfo << "Could not open port " << port << endl;
		return false;
	}
	
	termios state;
	tcgetattr( m_file, & state );
	
	// Save the previous state for restoration in close.
	m_previousState = state;
	
	state.c_iflag = IGNBRK | IGNPAR;
	state.c_oflag = 0;
	state.c_cflag = baudRate | CS8 | CREAD | CLOCAL;
	state.c_lflag = 0;
	tcsetattr( m_file, TCSANOW, & state );
	
	return true;
}


void SerialPort::closePort()
{
	if ( m_file == -1 )
		return;
	
	ioctl( m_file, TIOCCBRK, 0 );
	usleep(1);
	tcsetattr( m_file, TCSANOW, & m_previousState );
	close( m_file );
	m_file = -1;
}


QStringList SerialPort::ports( unsigned probeResult )
{
	QStringList list;
	
	for ( int i = 0; i < 8; ++i )
	{
		QString dev = QString("/dev/ttyS%1").arg(i);
		if ( probe(dev) & probeResult )
			list << dev;
	}
	
	for ( unsigned i = 0; i < 8; ++i )
	{
		QString dev = QString("/dev/tts/%1").arg(i);
		if ( probe(dev) & probeResult )
			list << dev;
	}
	
	for ( unsigned i = 0; i < 8; ++i )
	{
		QString dev = QString("/dev/ttyUSB%1").arg(i);
		if ( probe(dev) & probeResult )
			list << dev;
	}
	
	for ( unsigned i = 0; i < 8; ++i )
	{
		QString dev = QString("/dev/usb/tts/%1").arg(i);
		if ( probe(dev) & probeResult )
			list << dev;
	}
	
	return list;
}
//END class SerialPort



//BEGIN class ParallelPort
// I wasn't able to find any documentation on programming the parallel port
// in Darwin, so I've just functionally neutered this section.  Apparently
// parallel output is handled on a case by case basis (???) by the
// manufacturer of whatever USB dongle is, unless they build it as a
// Comms class device, in which case it is treated as a serial device.
// ( Info from Garth Cummings, Apple Developer Technical Support )

const int IRQ_MODE_BIT = 1 << 20; // Controls if pin 10 (Ack) causes interrupts
const int INPUT_MODE_BIT = 1 << 21; // Controls if the data pins are input or output

// No code using these values will be reached on Darwin, this is just to
// keep the preprocessor happy.
#ifdef DARWIN
	#define PPRDATA		0xFACADE
	#define PPRCONTROL	0xC001D00D
	#define PPWDATA		0xC0EDBABE
	#define PPWCONTROL	0xFEEDFACE
	#define PPRSTATUS	0xBAADF00D
	#define PPCLAIM 	0xDEADBEEF
	#define PPRELEASE 	0xCAFE
#endif

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

const int INVERT_MASK[3] = {
	0x0,
	0x80, // 10000000
	0x0b, // 00001011
};

ParallelPort::ParallelPort()
{
	reset();
}


ParallelPort::~ParallelPort()
{
}


void ParallelPort::reset()
{
	m_file = -1;
	m_reg[Data] = 0;
	m_reg[Status] = 0;
	m_reg[Control] = 0;
	m_outputPins = INPUT_MODE_BIT | IRQ_MODE_BIT;
	m_inputPins = STATUS_PINS | INPUT_MODE_BIT | IRQ_MODE_BIT;
}


//BEGIN Pin-oriented operations
void ParallelPort::setPinState( int pins, bool state )
{
	// only allow writing to output pins
	pins &= m_outputPins;
	
	if ( pins & DATA_PINS )
		setDataState( (pins & DATA_PINS) >> 0, state );
	
	if ( pins & CONTROL_PINS )
		setControlState( (pins & CONTROL_PINS) >> 16, state );
}


int ParallelPort::pinState( int pins )
{
	int value = 0;
	
	// only allow reading from input pins
	pins &= m_inputPins;
	
	if ( pins & DATA_PINS )
		value |= ((readFromRegister( Data ) & ((pins & DATA_PINS) >> 0)) << 0);
	
	if ( pins & STATUS_PINS )
		value |= ((readFromRegister( Status ) & ((pins & STATUS_PINS) >> 8)) << 8);
	
	if ( pins & CONTROL_PINS )
		value |= ((readFromRegister( Control ) & ((pins & CONTROL_PINS) >> 16)) << 16);
	
	return value;
}


void ParallelPort::setDataState( uchar pins, bool state )
{
	uchar value = readFromRegister( Data );
	
	if ( state )
		value |= pins;
	else
		value &= ~pins;
	
	writeToData( value );
}


void ParallelPort::setControlState( uchar pins, bool state )
{
	uchar value = readFromRegister( Control );
	
	if ( state )
		value |= pins;
	else
		value &= ~pins;
	
	writeToControl( value );
}
//END Pin-oriented operations



//BEGIN Register-oriented operations
uchar ParallelPort::readFromRegister( Register reg )
{
#ifdef DARWIN
	return 0;
#endif
	
	if ( m_file == -1 )
		return 0;
	
// 	uchar value = inb( m_lpBase + reg ) ^ INVERT_MASK[reg];
	uchar value = 0;
	if ( ioctl( m_file, IOCTL_REG_READ[reg], &value ) )
		kError() << k_funcinfo << "errno=" << errno << endl;
	else
		m_reg[reg] = value;
	return value;
}


void ParallelPort::writeToRegister( Register reg, uchar value )
{
#ifdef DARWIN
	return;
#endif
	
	if ( m_file == -1 )
		return;
	
// 	outb( value ^ INVERT_MASK[reg], m_lpBase + reg );
	if ( ioctl( m_file, IOCTL_REG_WRITE[reg], & value ) )
		kError() << k_funcinfo << "errno=" << errno << endl;
	else
		m_reg[reg] = value;
}


void ParallelPort::writeToData( uchar value )
{
	writeToRegister( Data, value );
}


void ParallelPort::writeToControl( uchar value )
{
	// Set all inputs to ones
	value |= ((m_inputPins & CONTROL_PINS) >> 16);
	
	writeToRegister( Control, value );
}
//END Register-oriented operations


//BEGIN Changing pin directions
void ParallelPort::setDataDirection( Direction dir )
{
	if ( dir == Input )
	{
		m_inputPins |= DATA_PINS;
		m_outputPins &= ~DATA_PINS;
	}
	else
	{
		m_inputPins &= DATA_PINS;
		m_outputPins |= ~DATA_PINS;
	}
	
	setPinState( INPUT_MODE_BIT, dir == Input );
}


void ParallelPort::setControlDirection( int pins, Direction dir )
{
	pins &= CONTROL_PINS;
	
	if ( dir == Input )
	{
		m_inputPins |= pins;
		m_outputPins &= ~pins;
	}
	else
	{
		m_inputPins &= pins;
		m_outputPins |= ~pins;
	}
	
	setControlState( 0, true );
}
//END Changing pin directions


Port::ProbeResult ParallelPort::probe( const QString & port )
{
#ifdef DARWIN
	return Port::DoesntExist;
#endif
	
	int file = open( port.ascii(), O_RDWR );
	if ( file == -1 )
		return Port::DoesntExist;
	
	if ( ioctl( file, PPCLAIM ) != 0 )
	{
		close(file);
		return Port::ExistsButNotRW;
	}
	
	ioctl( file, PPRELEASE );
	close(file);
	return Port::ExistsAndRW;
}


QStringList ParallelPort::ports( unsigned probeResult )
{
	QStringList list;
	
#ifdef DARWIN
	return list;
#endif
	
	for ( unsigned i = 0; i < 8; ++i )
	{
		QString dev = QString("/dev/parport%1").arg(i);
		if ( probe(dev) & probeResult )
			list << dev;
	}
	
	for ( unsigned i = 0; i < 8; ++i )
	{
		QString dev = QString("/dev/parports/%1").arg(i);
		if ( probe(dev) & probeResult )
			list << dev;
	}
	
	return list;
}


bool ParallelPort::openPort( const QString & port )
{
#ifdef DARWIN
	kWarning() << k_funcinfo << "Parallel ports disabled on Darwin" << endl;
	return false;
#endif
	
	if ( m_file != -1 )
	{
		kWarning() << k_funcinfo << "Port already open" << endl;
		return false;
	}
	
	m_file = open( port.ascii(), O_RDWR );
	
	if ( m_file == -1 )
	{
		kError() << k_funcinfo << "Could not open port \"" << port << "\": errno="<<errno<<endl;
		return false;
	}
	
	if ( ioctl( m_file, PPCLAIM ) )
	{
		kError() << k_funcinfo << "Port " << port << " must be RW" << endl;
		close( m_file );
		m_file = -1;
		return false;
	}
	
	return true;
}


void ParallelPort::closePort()
{
#ifdef DARWIN
	return;
#endif
	
	if ( m_file == -1 )
		return;
	
	int res = ioctl( m_file, PPRELEASE );
	close( m_file );
	
	if ( res )
		kError() << k_funcinfo << "res="<<res<<endl;
	
	m_file = -1;
}
//END class ParallelPort

