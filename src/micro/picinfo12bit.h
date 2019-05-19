/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PICINFO12BIT_H
#define PICINFO12BIT_H

#include "picinfo.h"
#include "asminfo.h"

/**
@short 12 Bit PIC Instructions
@author David Saxton
 */
class PicAsm12bit : public AsmInfo
{
	public:
		static PicAsm12bit *self();
		virtual Set set() const override { return AsmInfo::PIC12; }

	//protected:
	//	static PicAsm12bit *m_self;

        PicAsm12bit();
};

/**
@author David Saxton
 */
class PicInfo12bit : public PicInfo
{
	public:
		PicInfo12bit();
		~PicInfo12bit();

		virtual AsmInfo * instructionSet() override { return PicAsm12bit::self(); }
};

/**
@author David Saxton
 */
class PicInfo16C54 : public PicInfo12bit
{
	public:
		PicInfo16C54();
		~PicInfo16C54();
		virtual Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16C55 : public PicInfo12bit
{
	public:
		PicInfo16C55();
		~PicInfo16C55();
		virtual Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo12C508 : public PicInfo12bit
{
	public:
		PicInfo12C508();
		~PicInfo12C508();
		virtual Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo12C509 : public PicInfo12C508
{
	public:
		PicInfo12C509();
		~PicInfo12C509();
		virtual Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo12C671 : public PicInfo12C508
{
	public:
		PicInfo12C671();
		~PicInfo12C671();
		virtual Support gpsimSupport() const override { return NoSupport; }
};

/**
@author David Saxton
 */
class PicInfo12C672 : public PicInfo12C508
{
	public:
		PicInfo12C672();
		~PicInfo12C672();
		virtual Support gpsimSupport() const override { return NoSupport; }
};

#endif
