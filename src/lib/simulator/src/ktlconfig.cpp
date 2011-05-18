/*
    KTechLab, an IDE for electronics
    Copyright (C) 2010 Zoltan Padrah
      zoltan_padrah@users.sourceforge.net

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "ktlconfig.h"

KtlConfig *KtlConfig::m_self = 0;

KtlConfig* KtlConfig::self()
{
    if(!m_self){
        m_self = new KtlConfig;
    }
    return m_self;
}

void KtlConfig::destroy()
{
    if(m_self){
        delete m_self;
        m_self = 0;
    }
}

KtlConfig::~KtlConfig()
{

}


KtlConfig::KtlConfig(QObject* parent): QObject(parent)
{
    m_autoFormatMbOutput = false;
    m_indentAsmName = 4;
    m_indentAsmData = 15;
    m_indentComment = 40;
    m_indentEqu = 14;
    m_indentEquValue = 20;
    m_logicRisingTrigger = 2.5;
    m_logicFallingTrigger = 2;
    m_LogicOutputHigh = 5;
    m_LogicOutputHighImpedance = 15;
    m_LogicOutputLowImpedance = 0;
    m_hexFormat = inhx32;
    m_radix = Decimal;
    m_gpasmWarningLevel = All;
    m_ignoreCase = true;
    m_dosFormat = false;
    m_miscGpasmOptions = "";
    m_picProgrammerProgram = "picp";
    m_gplink_link_shared = true;
}

#include "ktlconfig.moc"