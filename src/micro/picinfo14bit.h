/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PICINFO14BIT_H
#define PICINFO14BIT_H

#include "picinfo.h"
#include "asminfo.h"

/**
@short 14 Bit PIC Instructions
@author David Saxton
 */
class PicAsm14bit : public AsmInfo
{
	public:
		static PicAsm14bit *self();
		Set set() const override { return AsmInfo::PIC14; }
	
	//protected:
	//	static PicAsm14bit *m_self;

	//private:
		PicAsm14bit();
};

/**
@author David Saxton
 */
class PicInfo14bit : public PicInfo
{
	public:
		PicInfo14bit();
		~PicInfo14bit() override;
	
		AsmInfo* instructionSet() override { return PicAsm14bit::self(); }
};

/**
@author David Saxton
 */
class PicInfo16C8x : public PicInfo14bit
{
	public:
		PicInfo16C8x();
		~PicInfo16C8x() override;
};

/**
Note: this PIC is superseeded by 16F84. SDCC does not recognize it.
@author David Saxton
 */
class PicInfo16C84 : public PicInfo16C8x
{
	public:
		PicInfo16C84();
		~PicInfo16C84() override;
		Support gpsimSupport() const override { return FullSupport; }
		Support microbeSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16F84 : public PicInfo16C8x
{
	public:
		PicInfo16F84();
		~PicInfo16F84() override;
		Support gpsimSupport() const override { return FullSupport; }
		Support flowcodeSupport() const override { return FullSupport; }
		Support microbeSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16CR84 : public PicInfo16F84
{
	public:
		PicInfo16CR84();
		~PicInfo16CR84() override;
		Support gpsimSupport() const override { return FullSupport; }
		Support flowcodeSupport() const override { return NoSupport; }
		Support microbeSupport() const override { return NoSupport; }
};

/**
@author David Saxton
 */
class PicInfo16F83 : public PicInfo16C8x
{
	public:
		PicInfo16F83();
		~PicInfo16F83() override;
		Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16CR83 : public PicInfo16F83
{
	public:
		PicInfo16CR83();
		~PicInfo16CR83() override;
		Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16C61 : public PicInfo16C8x
{
	public:
		PicInfo16C61();
		~PicInfo16C61() override;
		Support gpsimSupport() const override { return FullSupport; }
};


/**
@author David Saxton
 */
class PicInfo16X6X : public PicInfo14bit
{
	public:
		PicInfo16X6X();
		~PicInfo16X6X() override;
		Support gpsimSupport() const override { return NoSupport; }
};

/**
@author David Saxton
 */
class PicInfo16C62 : public PicInfo16X6X
{
	public:
		PicInfo16C62();
		~PicInfo16C62() override;
		Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16C63 : public PicInfo16C62
{
	public:
		PicInfo16C63();
		~PicInfo16C63() override;
		Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16C64 : public PicInfo16X6X
{
	public:
		PicInfo16C64();
		~PicInfo16C64() override;
		Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16C65 : public PicInfo16C64
{
	public:
		PicInfo16C65();
		~PicInfo16C65() override;
		Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16F62x : public PicInfo16X6X
{
	public:
		PicInfo16F62x();
		~PicInfo16F62x() override;
		Support gpsimSupport() const override { return NoSupport; }
};

/**
@author David Saxton
 */
class PicInfo16F627 : public PicInfo16F62x
{
	public:
		PicInfo16F627();
		~PicInfo16F627() override;
		Support gpsimSupport() const override { return FullSupport; }
		Support flowcodeSupport() const override { return PartialSupport; }
		Support microbeSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16F628 : public PicInfo16F627
{
	public:
		PicInfo16F628();
		~PicInfo16F628() override;
		Support gpsimSupport() const override { return FullSupport; }
		Support flowcodeSupport() const override { return PartialSupport; }
		Support microbeSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16F648 : public PicInfo16F628
{
	public:
		PicInfo16F648();
		~PicInfo16F648() override;
		Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16C71 : public PicInfo16C61
{
	public:
		PicInfo16C71();
		~PicInfo16C71() override;
		Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16C712 : public PicInfo16C62
{
	public:
		PicInfo16C712();
		~PicInfo16C712() override;
		Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16C716 : public PicInfo16C712
{
	public:
		PicInfo16C716();
		~PicInfo16C716() override;
		Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16C72 : public PicInfo16C62
{
	public:
		PicInfo16C72();
		~PicInfo16C72() override;
		Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16C73 : public PicInfo16C63
{
	public:
		PicInfo16C73();
		~PicInfo16C73() override;
		Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16C74 : public PicInfo16C65
{
	public:
		PicInfo16C74();
		~PicInfo16C74() override;
		Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16F873 : public PicInfo16C73
{
	public:
		PicInfo16F873();
		~PicInfo16F873() override;
		Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16F874 : public PicInfo16C74
{
	public:
		PicInfo16F874();
		~PicInfo16F874() override;
		Support gpsimSupport() const override { return FullSupport; }
};

/**
@author David Saxton
 */
class PicInfo16F877 : public PicInfo16F874
{
	public:
		PicInfo16F877();
		~PicInfo16F877() override;
		Support gpsimSupport() const override { return FullSupport; }
};

#endif
