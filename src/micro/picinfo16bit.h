/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PICINFO16BIT_H
#define PICINFO16BIT_H

#include "picinfo.h"
#include "asminfo.h"

/**
@short 16 Bit PIC Instructions
@author David Saxton
 */
class PicAsm16bit : public AsmInfo
{
	public:
		static PicAsm16bit *self();
		virtual Set set() const { return AsmInfo::PIC16; }
	
	protected:
		static PicAsm16bit *m_self;
	
	private:
		PicAsm16bit();
};

/**
@author David Saxton
 */
class PicInfo16bit : public PicInfo
{
	public:
		PicInfo16bit();
		~PicInfo16bit();
	
		virtual AsmInfo * instructionSet() { return PicAsm16bit::self(); }
};

/**
@author David Saxton
 */
class PicInfo17C7xx : public PicInfo16bit
{
	public:
		PicInfo17C7xx();
		~PicInfo17C7xx();
		virtual Support gpsimSupport() const { return NoSupport; }
};

/**
@author David Saxton
 */
class PicInfo17C75x : public PicInfo17C7xx
{
	public:
		PicInfo17C75x();
		~PicInfo17C75x();
		virtual Support gpsimSupport() const { return NoSupport; }
};

/**
@author David Saxton
 */
class PicInfo17C752 : public PicInfo17C75x
{
	public:
		PicInfo17C752();
		~PicInfo17C752();
		virtual Support gpsimSupport() const { return PartialSupport; }
};

/**
@author David Saxton
 */
class PicInfo17C756 : public PicInfo17C75x
{
	public:
		PicInfo17C756();
		~PicInfo17C756();
		virtual Support gpsimSupport() const { return PartialSupport; }
};

/**
@author David Saxton
 */
class PicInfo17C756A : public PicInfo17C75x
{
	public:
		PicInfo17C756A();
		~PicInfo17C756A();
		virtual Support gpsimSupport() const { return NoSupport; }
};

/**
@author David Saxton
 */
class PicInfo17C762 : public PicInfo17C75x
{
	public:
		PicInfo17C762();
		~PicInfo17C762();
		virtual Support gpsimSupport() const { return PartialSupport; }
};

/**
@author David Saxton
 */
class PicInfo17C766 : public PicInfo17C75x
{
	public:
		PicInfo17C766();
		~PicInfo17C766();
		virtual Support gpsimSupport() const { return PartialSupport; }
};

/**
@author David Saxton
 */
class PicInfo18Cxx2 : public PicInfo16bit
{
	public:
		PicInfo18Cxx2();
		~PicInfo18Cxx2();
		virtual Support gpsimSupport() const { return NoSupport; }
};

/**
@author David Saxton
 */
class PicInfo18C2x2 : public PicInfo16bit
{
	public:
		PicInfo18C2x2();
		~PicInfo18C2x2();
		virtual Support gpsimSupport() const { return NoSupport; }
};

/**
@author David Saxton
 */
class PicInfo18C242 : public PicInfo18C2x2
{
	public:
		PicInfo18C242();
		~PicInfo18C242();
		virtual Support gpsimSupport() const { return PartialSupport; }
};

/**
@author David Saxton
 */
class PicInfo18C252 : public PicInfo18C242
{
	public:
		PicInfo18C252();
		~PicInfo18C252();
		virtual Support gpsimSupport() const { return PartialSupport; }
};

/**
@author David Saxton
 */
class PicInfo18C4x2 : public PicInfo16bit
{
	public:
		PicInfo18C4x2();
		~PicInfo18C4x2();
		virtual Support gpsimSupport() const { return NoSupport; }
};

/**
@author David Saxton
 */
class PicInfo18C442 : public PicInfo18C4x2
{
	public:
		PicInfo18C442();
		~PicInfo18C442();
		virtual Support gpsimSupport() const { return PartialSupport; }
};

/**
@author David Saxton
 */
class PicInfo18C452 : public PicInfo18C442
{
	public:
		PicInfo18C452();
		~PicInfo18C452();
		virtual Support gpsimSupport() const { return PartialSupport; }
};

/**
@author David Saxton
 */
class PicInfo18F442 : public PicInfo18C442
{
	public:
		PicInfo18F442();
		~PicInfo18F442();
		virtual Support gpsimSupport() const { return PartialSupport; }
};

/**
@author David Saxton
 */
class PicInfo18F248 : public PicInfo18F442
{
	public:
		PicInfo18F248();
		~PicInfo18F248();
		virtual Support gpsimSupport() const { return NoSupport; }
};

/**
@author David Saxton
 */
class PicInfo18F452 : public PicInfo18F442
{
	public:
		PicInfo18F452();
		~PicInfo18F452();
		virtual Support gpsimSupport() const { return PartialSupport; }
};

/**
@author David Saxton
 */
class PicInfo18Fxx20 : public PicInfo16bit
{
	public:
		PicInfo18Fxx20();
		~PicInfo18Fxx20();
		virtual Support gpsimSupport() const { return NoSupport; }
};

/**
@author David Saxton
 */
class PicInfo18F1220 : public PicInfo18Fxx20
{
	public:
		PicInfo18F1220();
		~PicInfo18F1220();
		virtual Support gpsimSupport() const { return NoSupport; }
};

/**
@author David Saxton
 */
class PicInfo18F1320 : public PicInfo18F1220
{
	public:
		PicInfo18F1320();
		~PicInfo18F1320();
		virtual Support gpsimSupport() const { return NoSupport; }
};

#endif
