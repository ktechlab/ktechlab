/***************************************************************************
 *   Copyright (C) 2004-2005 by Daniel Clarke <daniel.jc@gmail.com>        *
 *                      2005 by David Saxton <david@bluehaze.org>          *
 *									   *
 *   24-04-2007                                                            *
 *   Modified to add pic 16f877,16f627 and 16f628 			   *
 *   by george john george@space-kerala.org 				   *
 *   supported by SPACE www.space-kerala.org		 		   *	
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
#include "instruction.h"
#include "optimizer.h"
#include "pic14.h"
#include <kdebug.h>
#include <Qt/qstringlist.h>
#include <cassert>
#include <iostream>
using namespace std;
//modified new varable pic_type is added
extern QString pic_type;
//BEGIN class Register
Register::Register( Type type )
{
	m_type = type;
//***********modified almost all the register names are included**************//	
	switch ( m_type )
	{
//----------------------------------------------Bank0---------------------------//
		case TMR0:
			m_name = "TMR0";
			break;
		case PCL:
			m_name = "PCL";
			break;
		case STATUS:
			m_name = "STATUS";
			break;
		case FSR:
			m_name = "FSR";
			break;
		case PORTA:
			m_name = "PORTA";
			break;
		case PORTB:
			m_name = "PORTB";
			break;
		case PORTC:
			m_name = "PORTC";
			break;
		case PORTD:
			m_name = "PORTD";
			break;
		case PORTE:
			m_name = "PORTE";
			break;
		case PCLATH:
			m_name = "PCLATH";
			break;
		case INTCON:
			m_name = "INTCON";
			break;
		case PIR1:
			m_name = "PIR1";
			break;
		case PIR2:
			m_name = "PIR2";
			break;
		case TMR1L:
			m_name = "TMR1L";
			break;
		case TMR1H:
			m_name = "TMR1H";
			break;
		case T1CON:
			m_name = "T1CON";
			break;
		case TMR2:
			m_name = "TMR2";
			break;
		case T2CON:
			m_name = "T2CON";
			break;
		case SSPBUF:
			m_name = "SSPBUF";
			break;
		case SSPCON:
			m_name = "SSPCON";
			break;
		case CCPR1L:
			m_name = "CCPR1L";
			break;
		case CCPR1H:
			m_name = "CCPR1H";
			break;
		case CCP1CON:
			m_name = "CCP1CON";
			break;
		case RCSTA:
			m_name = "RCSTA";
			break;
		case TXREG:
			m_name = "TXREG";
			break;
		case RCREG:
			m_name = "RCREG";
			break;
		case CCPR2L:
			m_name = "CCPR2L";
			break;
		case CCPR2H:
			m_name = "CCPR2H";
			break;
		case CCP2CON:
			m_name = "CCP2CON";
			break;
		case ADRESH:
			m_name = "ADRESH";
			break;
		case ADCON0:
                        m_name = "ADCON0";
			break;
		case CMCON:
                        m_name = "CMCON";
			break;
//----------------------------------------------Bank1---------------------------//

		case OPTION_REG:
			m_name = "OPTION_REG";
			break;
		case TRISA:
			m_name = "TRISA";
			break;
		case TRISB:
			m_name = "TRISB";
			break;
		case TRISC:
			m_name = "TRISC";
			break;
		case TRISD:
			m_name = "TRISD";
			break;
		case TRISE:
			m_name = "TRISE";
			break;
		case PIE1:
			m_name = "PIE1";
			break;
		case PIE2:
			m_name = "PIE2";
			break;
		case PCON:
			m_name = "PCON";
			break;
		case SSPCON2:
			m_name = "SSPCON2";
			break;
		case PR2:
			m_name = "PR2";
			break;
		case SSPADD:
			m_name = "SSPADD";
			break;
		case SSPSTAT:
			m_name = "SSPSTAT";
			break;
		case TXSTA:
			m_name = "TXSTA";
			break;
		case SPBRG:
			m_name = "SPBRG";
			break;
		case ADRESL:
			m_name = "ADRESL";
			break;
		case ADCON1:
                        m_name = "ADCON1";
			break;
		case VRCON:
                        m_name = "VRCON";
			break;
//----------------------------------------------Bank2---------------------------//
		case EEDATA:
			m_name = "EEDATA";
			break;
		case EEADR:
			m_name = "EEADR";
			break;
		case EEDATH:
			m_name = "EEDATH";
			break;
		case EEADRH:
			m_name = "EEADRH";
			break;
//----------------------------------------------Bank3---------------------------//
		case EECON1:
			m_name = "EECON1";
			break;

		case EECON2:
			m_name = "EECON2";
			break;
//---------------------------------------------NoBank---------------------------//       
		case WORKING:
			m_name = "<working>";
			break;
		case GPR:
		case none:
			break;
	}
}


Register::Register( const QString & name )//--to find a name varable or register(ex  trise)
{
	m_name = name.trimmed();
	QString upper = m_name.toUpper();
//--------------------------------------------Bank0-------------------//
	if ( upper == "TMR0" )
		m_type = TMR0;
	else if ( upper == "PCL" )
		m_type = PCL;
	else if ( upper == "STATUS")
		m_type = STATUS;
	else if ( upper == "FSR")
		m_type = FSR;
	else if ( upper == "PORTA")
		m_type = PORTA;
	else if ( upper == "PORTB")
		m_type = PORTB;
	else if ( upper == "PORTC")
		m_type = PORTC;
	else if ( upper == "PORTD")
		m_type = PORTD;
	else if ( upper == "PORTE")
		m_type = PORTE;
	else if ( upper == "PCLATH")
		m_type = PCLATH;
	else if ( upper == "INTCON")
	 	m_type = INTCON;
	else if ( upper == "PIR1")
		m_type = PIR1;
	else if ( upper == "PIR2")
		m_type = PIR2;
	else if ( upper == "TMR1L")
		m_type = TMR1L;
	else if ( upper == "TMR1H")
		m_type = TMR1H;
	else if ( upper == "T1CON")
		m_type = T1CON;
	else if ( upper == "TMR2")
		m_type = TMR2;
	else if ( upper == "T2CON")
		m_type = T2CON;
	else if ( upper == "SSPBUF")
		m_type = SSPBUF;
	else if ( upper == "SSPCON")
		m_type = SSPCON;
	else if ( upper == "CCPR1L")
		m_type = CCPR1L;
	else if ( upper == "CCPR1H")
		m_type = CCPR1H;
	else if ( upper == "CCP1CON")
		m_type = CCP1CON;
	else if ( upper == "RCSTA")
		m_type = RCSTA;
	else if ( upper == "TXREG")
		m_type = TXREG;
	else if ( upper == "RCREG")
		m_type = RCREG;
	else if ( upper == "CCPR2L")
		m_type = CCPR2L;
	else if ( upper == "CCPR2H")
		m_type = CCPR2H;
	else if ( upper == "CCP2CON")
		m_type = CCP2CON;
	else if ( upper == "ADRESH")
		m_type = ADRESH;
	else if ( upper == "ADCON0")
		m_type = ADCON0;
	else if ( upper == "CMCON")
		m_type = CMCON;
//--------------------------------------------Bank1-------------------//
	else if ( upper == "OPTION_REG" )
		m_type = OPTION_REG;
	else if ( upper == "TRISA")
		m_type = TRISA;
	else if ( upper == "TRISB")
		m_type = TRISB;
	else if ( upper == "TRISC")
		m_type = TRISC;
	else if ( upper == "TRISD")
		m_type = TRISD;
	else if ( upper == "TRISE")
		m_type = TRISE;
	else if ( upper == "PIE1")
		m_type = PIE1;
	else if ( upper == "PIE2")
		m_type = PIE2;
	else if ( upper == "PCON")
		m_type = PCON;
	else if ( upper == "SSPCON2")
		m_type = SSPCON2;
	else if ( upper == "PR2")
		m_type = PR2;
	else if ( upper == "SSPADD")
		m_type = SSPADD;
	else if ( upper == "SSPSTAT")
		m_type = SSPSTAT;
	else if ( upper == "TXSTA")
		m_type = TXSTA;
	else if ( upper == "SPBRG")
		m_type = SPBRG;
	else if ( upper == "ADRESL")
		m_type = ADRESL;
	else if ( upper == "ADCON1")
		m_type = ADCON1;
	else if ( upper == "VRCON")
		m_type = VRCON;
//--------------------------------------------Bank2-------------------//
	else if ( upper == "EEDATA")
		m_type = EEDATA;
	else if ( upper == "EEADR")
		m_type = EEADR;
	else if ( upper == "EEDATH")
		m_type = EEDATH;
	else if ( upper == "EEADRH")
	 	m_type = EEADRH;
//--------------------------------------------Bank3-------------------//
	else if ( upper == "EECON1")
		m_type = EECON1;
	else if ( upper == "EECON2")
		m_type = EECON2;
//---------------------------------------------NoBank----------------//
	else
		m_type = GPR;
}


Register::Register( const char * name )
{
	*this = Register( QString(name) );
}


bool Register::operator < ( const Register & reg ) const
{
	if ( (type() != GPR) || (reg.type() != GPR) )
		return type() < reg.type();
	
	return name() < reg.name();
}


bool Register::operator == ( const Register & reg ) const
{
	if ( type() != reg.type() )
		return false;
	
	return name() == reg.name();
}


uchar Register::banks() const
{
	switch ( m_type )
	{
//---------------------bank 0 registers return zero---------------------------// 		

		case TMR0: return Bank0& Bank1;//Bank0
		case PCL: return Bank0 & Bank1;//Bank0 | Bank1
		case STATUS: return Bank0 & Bank1;//Bank0 | Bank1
		case FSR: return Bank0 & Bank1;//Bank0 | Bank1
		case PORTA: return Bank0 & Bank1;//Bank0
		case PORTB: return Bank0 & Bank1;//Bank0
		case PORTC: return Bank0 & Bank1;//Bank0
		case PORTD: return Bank0 & Bank1;//Bank0
		case PORTE: return Bank0 & Bank1;//Bank0
		case PCLATH: return Bank0 & Bank1;//Bank0 | Bank1
		case INTCON: return Bank0 & Bank1;//Bank0 | Bank1
		case PIR1: return Bank0 & Bank1;
		case PIR2: return Bank0 & Bank1;
		case TMR1L: return Bank0 & Bank1;
		case TMR1H: return Bank0 & Bank1;
		case T1CON: return Bank0 & Bank1;
		case TMR2: return Bank0 & Bank1;
		case T2CON: return Bank0 & Bank1;
		case SSPBUF: return Bank0 & Bank1;
		case SSPCON: return Bank0 & Bank1;
		case CCPR1L: return Bank0 & Bank1;
		case CCPR1H: return Bank0 & Bank1;
		case CCP1CON: return Bank0 & Bank1;
		case RCSTA: return Bank0 & Bank1;
		case TXREG: return Bank0 & Bank1;
		case RCREG: return Bank0 & Bank1;
		case CCPR2L: return Bank0 & Bank1;
		case CCPR2H: return Bank0 & Bank1;
		case CCP2CON: return Bank0 & Bank1;
		case ADRESH: return Bank0 & Bank1;//Bank0	
		case ADCON0: return Bank0 & Bank1;//Bank0
		case CMCON: return Bank0 & Bank1;//Bank0
//-----------------------------NO Bank-------------------------------------//
		case GPR: return Bank0 & Bank1;//Bank0 | Bank1
		case WORKING: return Bank0 & Bank1;//Bank0 | Bank1
		case none: return Bank0 & Bank1;//Bank0 | Bank1

//-------------------bank 1 registers return one---------------------------// 		

		case OPTION_REG: return Bank0;//Bank1
///------tris registers-------//
		case TRISA: return Bank0;//Bank1
		case TRISB: return Bank0;//Bank1
		case TRISC: return Bank0;//Bank1
		case TRISD: return Bank0;//Bank1
		case TRISE: return Bank0;//Bank1
//--------------------------------------------------------------------------//
		case PIE1: return Bank0;
		case PIE2: return Bank0;
		case PCON: return Bank0;
		case SSPCON2: return Bank0;
		case PR2: return Bank0;
		case SSPADD: return Bank0;
		case SSPSTAT: return Bank0;
		case TXSTA: return Bank0;
		case SPBRG: return Bank0;	
//--------adc register-------//
		case ADRESL: return Bank0;//Bank1 
		case ADCON1: return Bank0;//Bank1
		case VRCON: return Bank0;//Bank1		

//------------------bank 2 registers return two----------completed------------//

		case EEDATA: return Bank1;//Bank1
		case EEADR: return Bank1;//Bank0
		case EEDATH: return Bank1;//Bank0
		case EEADRH: return Bank1;//Bank0	

//------------------bank 3 registers return three--------completed----------------//

		case EECON1: return Bank0|Bank1;//Bank1
		case EECON2: return Bank0|Bank1;//Bank1

	}
	
	return Bank0 & Bank1; // Vacously true (and useful too) - a non-existent bank can be accessed anywhere
}


bool Register::bankDependent() const
{
	return ( banks() != (Bank0 | Bank1) );
}


bool Register::affectsExternal() const
{
	switch ( m_type )
	{
		case PORTA:
		case TRISA:
		case PORTB:
		case TRISB:
		case PORTC:
		case TRISC:
		case PORTD:
		case TRISD:
		case PORTE:
		case TRISE:
		case INTCON:
		case ADCON0:
		case ADCON1:
//************************modification***************************		
		case TMR0:
		case OPTION_REG:
		case PCL:
		case STATUS:
		case FSR:
		case EEDATA:
		case EECON1:
		case EEADR:
		case EECON2:
		case PCLATH:
		case GPR:
//--------------------------------FINAL LAST-------------//
		case PIR1:
		case PIR2:
		case TMR1L:
		case TMR2:
		case TMR1H:
		case T1CON:
		case T2CON:
		case SSPBUF:
		case SSPCON:
		case CCPR1L:
		case CCPR1H:
		case CCP1CON:
		case CCPR2L:
		case CCPR2H:
		case CCP2CON:
		case ADRESH:
		case PIE1:
		case PIE2:
		case PCON:
		case SSPCON2:
		case PR2:
		case SSPADD:
		case SSPSTAT:
		case TXSTA:
		case SPBRG:
		case ADRESL:
		case EEDATH:
		case EEADRH:
		case RCSTA:
		case TXREG:
		case RCREG:
		case CMCON:
		case VRCON:
			return true;
		case WORKING:
		case none:
			return false;
	}
	return false;
}



//BEGIN class RegisterBit
RegisterBit::RegisterBit( uchar bitPos, Register::Type reg )
{
	m_bitPos = bitPos;
	m_registerType = reg;
	
	switch ( m_registerType )
	{	
		case Register::TMR0:
		case Register::PCL:
			break;
		case Register::STATUS:
		{
			switch ( m_bitPos )
			{
				case 0: m_name = "C"; break;
				case 1: m_name = "DC"; break;
				case 2: m_name = "Z"; break;
				case 3: m_name = "NOT_PD"; break;
				case 4: m_name = "NOT_TO"; break;
				case 5: m_name = "RP0"; break;
				case 6: m_name = "RP1"; break;
				case 7: m_name = "IRP"; break;
			}
			break;
		}
		case Register::FSR:
		case Register::PORTA:
		case Register::PORTB:
		case Register::PORTC:
		case Register::PORTD:
		case Register::PORTE:
		case Register::PCLATH:
			break;
		case Register::INTCON:
		{
			switch ( m_bitPos )
			{
				case 0: m_name = "RBIF"; break;
				case 1: m_name = "INTF"; break;
				case 2: m_name = "T0IF"; break;
				case 3: m_name = "RBIE"; break;
				case 4: m_name = "INTE"; break;
				case 5: m_name = "T0IE"; break;
				case 6: 
				{
				  if(pic_type=="P16F84"||pic_type=="P16C84")
					m_name = "EEIE"; break;
				  if(pic_type=="P16F877"||pic_type=="P16F627" ||pic_type =="P16F628")
					m_name = "PEIE"; break;
	 			  break;


				}
				case 7: m_name = "GIE"; break;
			}
			break;
		}
		case Register::PIR1:
		{
			switch ( m_bitPos )
			{
				case 0: m_name = "TMR1F"; break;
				case 1: m_name = "TMR2F"; break;
				case 2: m_name = "CCP1IF"; break;
				case 3: m_name = "SSPIF"; break;
				case 4: m_name = "TXIF"; break;
				case 5: m_name = "RCIF"; break;
				case 6:
				  if(pic_type=="P16F877")
					 m_name = "ADIF"; break;
				  if(pic_type=="P16F627"||pic_type=="P16F628")
					 m_name = "CMIF";break;
				  break;					
				case 7:
				  if(pic_type=="P16F877") 
					m_name = "PSPIF"; break;
				  if(pic_type=="P16F627"||pic_type=="P16F628")
					 m_name = "EEIF";break;
				  break;
			}
			break;
		}
		case Register::PIR2:
		{
			switch ( m_bitPos )
			{
				case 0: m_name = "CCP2IF"; break;
				case 3: m_name = "BCLIF"; break;
				case 4: m_name = "EEIF"; break;
	
			}
			break;
		}
		case Register::TMR1L:
		case Register::TMR1H:
			break;
		case Register::T1CON:
		{
			switch ( m_bitPos )
			{
				case 0: m_name = "TMR1ON"; break;
				case 1: m_name = "TMRCS"; break;
				case 2:
				  if(pic_type=="P16F877")
					 m_name = "T1SYNC"; break;
				  if(pic_type=="P16F627"||pic_type=="P16F628")
					 m_name = "NOT_T1SYNC"; break;
				  break;
				case 3: m_name = "T1OSCEN"; break;
				case 4: m_name = "T1CKPS0"; break;
				case 5: m_name = "T1CKPS1"; break;
			}
			break;
		}
		case Register::TMR2:
			break;
		case Register::T2CON:
		{
			switch ( m_bitPos )
			{
				case 0: m_name = "T2CKPS0"; break;
				case 1: m_name = "T2CKPS1"; break;
				case 2: m_name = "TMR2ON"; break;
				case 3: m_name = "TOUTPS0"; break;
				case 4: m_name = "TOUTPS1"; break;
				case 5: m_name = "TOUTPS2"; break;
				case 6: m_name = "TOUTPS3"; break;
			}
			break;
		}
		case Register::SSPBUF:
			break;
		case Register::SSPCON:
			switch ( m_bitPos )
			{
				case 0: m_name = "SSPM0"; break;
				case 1: m_name = "SSPM1"; break;
				case 2: m_name = "SSPM2"; break;
				case 3: m_name = "SSPM3"; break;
				case 4: m_name = "CKP"; break;
				case 5: m_name = "SSPEN"; break;
				case 6: m_name = "SSPOV"; break;//!!!!!!START&STOPEEIE!!!
				case 7: m_name = "WCOL"; break;
			}
			break;
		case Register::CCPR1L:
		case Register::CCPR1H:
			break;
		case Register::CCP1CON:
			switch ( m_bitPos )
			{
				case 0: m_name = "CCP1M0"; break;
				case 1: m_name = "CCP1M1"; break;
				case 2: m_name = "CCP1M2"; break;
				case 3: m_name = "CCP1M3"; break;
				case 4: m_name = "CCP1Y"; break;
				case 5: m_name = "CCP1X"; break;
			}
			break;
		case Register::RCSTA:
			switch ( m_bitPos )
			{
				case 0: m_name = "RX9D"; break;
				case 1: m_name = "OERR"; break;
				case 2: m_name = "FERR"; break;
				case 3: 
				  if(pic_type=="P16F877")
					m_name = "ADDEN"; break;
				  if(pic_type=="P16F627"||pic_type=="P16F628")
					m_name = "ADEN"; break;
				case 4: m_name = "CREN"; break;
				case 5: m_name = "SREN"; break;
				case 6: m_name = "RX9"; break;
				case 7: m_name = "SPEN"; break;
			}
			break;
		case Register::TXREG:
		case Register::RCREG:
		case Register::CCPR2L:
		case Register::CCPR2H:
			break;
		case Register::CCP2CON:
			switch ( m_bitPos )
			{
				case 0: m_name = "CCP2M0"; break;
				case 1: m_name = "CCP2M1"; break;
				case 2: m_name = "CCP2M2"; break;
				case 3: m_name = "CCP2M3"; break;
				case 4: m_name = "CCP2Y"; break;
				case 5: m_name = "CCP2X"; break;
			}
			break;

		case Register::ADRESH:
			break;
		case Register::ADCON0:
		{
			switch ( m_bitPos )
			{
				case 0: m_name = "ADON"; break;
				case 2: m_name = "GO"; break;
				case 3: m_name = "CHS0"; break;
				case 4: m_name = "CHS1"; break;
				case 5: m_name = "CHS2"; break;
				case 6: m_name = "ADCS0"; break;
				case 7: m_name = "ADCS1"; break;
			}
			break;
		}
		case Register::CMCON:
		{
			switch ( m_bitPos )
			{
				case 0: m_name = "CM0"; break;
				case 1: m_name = "CM1"; break;
				case 2: m_name = "CM2"; break;
				case 3: m_name = "CIS"; break;
				case 4: m_name = "C1INV"; break;
				case 5: m_name = "C2INV"; break;
				case 6: m_name = "C1OUT"; break;
				case 7: m_name = "C2OUT"; break;
			}
			break;
		}
//-----------------------------------------------------Bank1----------------//
		case Register::OPTION_REG:
		{
			switch ( m_bitPos )
			{
				case 0: m_name = "PS0"; break;
				case 1: m_name = "PS1"; break;
				case 2: m_name = "PS2"; break;
				case 3: m_name = "PSA"; break;
				case 4: m_name = "T0SE"; break;
				case 5: m_name = "T0CS"; break;
				case 6: m_name = "INTEDG"; break;
				case 7: 
				{
					if(pic_type=="P16F84")
						m_name = "RBPU";
					if(pic_type=="P16F877"||pic_type=="P16C84"||pic_type=="P16F627"||pic_type=="P16F628")
						m_name = "NOT_RBPU";
	 				break;


				}
			}
			break;
		}
		case Register::TRISA:
		case Register::TRISB:
		case Register::TRISC:
		case Register::TRISD:
		case Register::TRISE:
			break;
		case Register::PIE1:
			switch ( m_bitPos )
			{
				case 0: m_name = "TMR1IE"; break;
				case 1: m_name = "TMR2IE"; break;
				case 2: m_name = "CCP1IE"; break;
				case 3: m_name = "SSPIE"; break;
				case 4: m_name = "TXIE"; break;
				case 5: m_name = "RCIE"; break;
				case 6:
				{
				   if (pic_type=="P16F877")
 					m_name = "ADIE"; break;
				   if (pic_type=="P16F627"||pic_type=="P16F628")
 					m_name = "CMIE"; break;
				   break;
 				}
				case 7:
				{
				   if (pic_type=="P16F877")
 					m_name = "PSPIE"; break;
				   if (pic_type=="P16F627"||pic_type=="P16F628")
 					m_name = "EEIE"; break;
				   break;
 				} 
			}
			break;
		case Register::PIE2:
			switch ( m_bitPos )
			{
				case 0: m_name = "CCP2IE"; break;
				case 3: m_name = "BCLIE"; break;
				case 4: m_name = "EEIE"; break;
			}
			break;
		case Register::PCON:
			switch ( m_bitPos )
			{
				case 0: m_name = "NOT_BOR"; break;
				case 1: m_name = "NOT_POR"; break;
				case 3: m_name = "OSCF"; break;

			}
			break;
		case Register::SSPCON2:
			switch ( m_bitPos )
			{
				case 0: m_name = "SEN"; break;
				case 1: m_name = "RSEN"; break;
				case 2: m_name = "PEN"; break;
				case 3: m_name = "RCEN"; break;
				case 4: m_name = "ACKEN"; break;
				case 5: m_name = "ACKDT"; break;
				case 6: m_name = "ACKSTAT"; break;
				case 7: m_name = "GCEN"; break;
			}
			break;
		case Register::PR2:
		case Register::SSPADD:
			break;
		case Register::SSPSTAT:
			switch ( m_bitPos )
			{
				case 0: m_name = "BF"; break;
				case 1: m_name = "UA"; break;
				case 2: m_name = "R"; break;
				case 3: m_name = "S"; break;
				case 4: m_name = "P"; break;
				case 5: m_name = "D"; break;
				case 6: m_name = "CKE"; break;
				case 7: m_name = "SMP"; break;
			}
			break;	
		case Register::TXSTA:
			switch ( m_bitPos )
			{
				case 0: m_name = "TX9D"; break;
				case 1: m_name = "TRMT"; break;
				case 2: m_name = "BRGH"; break;
				case 4: m_name = "SYNC"; break;
				case 5: m_name = "TXEN"; break;
				case 6: m_name = "TX9"; break;
				case 7: m_name = "CSRC"; break;
			}
			break;
		case Register::SPBRG:
		case Register::ADRESL:
			break;
		case Register::ADCON1:
		{
			switch ( m_bitPos )
			{
				case 0: m_name = "PCFG0"; break;
				case 1: m_name = "PCFG1"; break;
				case 2: m_name = "PCFG2"; break;
				case 3: m_name = "PCFG3"; break;
				case 7: m_name = "ADFM"; break;
			}
			break;
		}

//-----------------------------------------------------Bank2----------------//
		case Register::EEDATA:
		case Register::EEADR:
		case Register::EEDATH:
		case Register::EEADRH:
			break;
//-----------------------------------------------------Bank3----------------//
		case Register::EECON1:
		{
			switch ( m_bitPos )
			{
				case 0: m_name = "RD"; break;
				case 1: m_name = "WR"; break;
				case 2: m_name = "WREN"; break;
				case 3: m_name = "WRERR"; break;
				case 4: m_name = "EEIF"; break;
				case 7: m_name = "EEPGD"; break;//imp *****
			}
			break;
		}
			
		case Register::EECON2:
			break;
		case Register::VRCON:
		{
			switch ( m_bitPos )
			{
				case 0: m_name = "VR0"; break;
				case 1: m_name = "VR1"; break;
				case 2: m_name = "VR2"; break;
				case 3: m_name = "VR3"; break;
				case 5: m_name = "VRR"; break;
				case 6: m_name = "VROE"; break;
				case 7: m_name = "VREN"; break;
			}
			break;
		}
		case Register::GPR:
		case Register::WORKING:
		case Register::none:
		{
// 			kdError() << k_funcinfo << "Bad register: " << reg << endl;
		}
	}
}


RegisterBit::RegisterBit( const QString & name )
{
	m_name = name.toUpper().trimmed();
	initFromName();
}


RegisterBit::RegisterBit( const char * name )
{
	m_name = QString(name).toUpper().trimmed();
	initFromName();
}


void RegisterBit::initFromName()
{
	bool ok;
	m_bitPos = m_name.toInt( & ok, 0 );
	if ( ok )
		m_registerType = Register::none; // hmm it should be unknown - not none.
//----------------------------------------Bank0----------------------------//

//--------STATUS REGISTER--------//
	
	else if ( m_name == "C" )
	{
		m_registerType = Register::STATUS;
		m_bitPos = 0;
	}
	else if ( m_name == "DC" )
	{
		m_registerType = Register::STATUS;
		m_bitPos = 1;
	}
	else if ( m_name == "Z" )
	{
		m_registerType = Register::STATUS;
		m_bitPos = 2;
	}
	else if ( m_name == "NOT_PD" )
	{
		m_registerType = Register::STATUS;
		m_bitPos = 3;
	}
	else if ( m_name == "NOT_TO" )
	{
		m_registerType = Register::STATUS;
		m_bitPos = 4;
	}
	else if ( m_name == "RP0" )
	{
		m_registerType = Register::STATUS;
		m_bitPos = 5;
	}
	else if ( m_name == "RP1" )
	{
		m_registerType = Register::STATUS;
		m_bitPos = 6;
	}
	else if ( m_name == "IRP" )
	{
		m_registerType = Register::STATUS;
		m_bitPos = 7;
	}

//-----------INTCON REGISTER---------//

	else if ( m_name == "RBIF" )
	{
		m_registerType = Register::INTCON;
		m_bitPos = 0;
	}
	else if ( m_name == "INTF" )
	{
		m_registerType = Register::INTCON;
		m_bitPos = 1;
	}
	else if ( m_name == "T0IF" )
	{
		m_registerType = Register::INTCON;
		m_bitPos = 2;
	}
	else if ( m_name == "RBIE" )
	{
		m_registerType = Register::INTCON;
		m_bitPos = 3;
	}
	else if ( m_name == "INTE" )
	{
		m_registerType = Register::INTCON;
		m_bitPos = 4;
	}
	else if ( m_name == "T0IE" )
	{
		m_registerType = Register::INTCON;
		m_bitPos = 5;
	}
	else if ( m_name =="PEIE"&&(pic_type=="P16F877"||pic_type=="P16F627"))
	{
		m_registerType = Register::INTCON;
		m_bitPos = 6;
	}
	else if (m_name == "EEIE"&& (pic_type=="P16F84"||pic_type=="P16C84"))
	{
		m_registerType = Register::INTCON;
		m_bitPos = 6;
	}
	else if ( m_name == "GIE" )
	{
		m_registerType = Register::INTCON;
		m_bitPos = 7;
	}
//-------PIR1---------//

	else if ( m_name == "TMR1F" )
	{
		m_registerType = Register::PIR1;
		m_bitPos = 0;
	}
	else if ( m_name == "TMR2F" )
	{
		m_registerType = Register::PIR1;
		m_bitPos = 1;
	}
	else if ( m_name == "CCP1IF" )
	{
		m_registerType = Register::PIR1;
		m_bitPos = 2;
	}
	else if ( m_name == "SSPIF"&& pic_type=="P16F877" )
	{
		m_registerType = Register::PIR1;
		m_bitPos = 3;
	}
	else if ( m_name == "TXIF" )
	{
		m_registerType = Register::PIR1;
		m_bitPos = 4;
	}
	else if ( m_name == "RCIF" )
	{
		m_registerType = Register::PIR1;
		m_bitPos = 5;
	}
	else if ( m_name == "ADIF" && pic_type=="P16F877")
	{
		m_registerType = Register::PIR1;
		m_bitPos = 6;
	}
	else if ( m_name == "CMIF" && pic_type=="P16F627")
	{
		m_registerType = Register::PIR1;
		m_bitPos = 6;
	}
	else if ( m_name == "PSPIF"&& pic_type=="P16F877")
	{
		m_registerType = Register::PIR1;
		m_bitPos = 7;
	}
	else if ( m_name == "EEIF"&& pic_type=="P16F627")
	{
		m_registerType = Register::PIR1;
		m_bitPos = 7;
	}
//-------PIR2---------//
	else if ( m_name == "CCP2IF" )
	{
		m_registerType = Register::PIR2;
		m_bitPos = 0;
	}
	else if ( m_name == "BCLIF" )
	{
		m_registerType = Register::PIR2;
		m_bitPos = 3;
	}
	else if ( m_name == "EEIF" && pic_type=="P16F877" )
	{
		m_registerType = Register::PIR2;
		m_bitPos = 4;
	}
//-------T1CON--------//
	else if ( m_name == "TMR1ON" )
	{
		m_registerType = Register::T1CON;
		m_bitPos = 0;
	}
	else if ( m_name == "TMR1CS" )
	{
		m_registerType = Register::T1CON;
		m_bitPos = 1;
	}
	else if ( m_name == "T1SYNC"&& pic_type=="P16F877" )
	{
		m_registerType = Register::T1CON;
		m_bitPos = 2;
	}
	else if ( m_name == "NOT_T1SYNC"&& pic_type=="P16F627" )
	{
		m_registerType = Register::T1CON;
		m_bitPos = 2;
	}
	else if ( m_name == "T1OSCEN" )
	{
		m_registerType = Register::T1CON;
		m_bitPos = 3;
	}
	else if ( m_name == "T1CKPS0" )
	{
		m_registerType = Register::T1CON;
		m_bitPos = 4;
	}
	else if ( m_name == "T1CKPS1" )
	{
		m_registerType = Register::T1CON;
		m_bitPos = 5;
	}
//-------T2CON--------//

	else if ( m_name == "T2CKPS0" )
	{
		m_registerType = Register::T2CON;
		m_bitPos = 0;
	}
	else if ( m_name == "T2CKPS1" )
	{
		m_registerType = Register::T2CON;
		m_bitPos = 1;
	}
	else if ( m_name == "TMR2ON" )
	{
		m_registerType = Register::T2CON;
		m_bitPos = 2;
	}
	else if ( m_name == "TOUTPS0" )
	{
		m_registerType = Register::T2CON;
		m_bitPos = 3;
	}
	else if ( m_name == "TOUTPS1" )
	{
		m_registerType = Register::T2CON;
		m_bitPos = 4;
	}
	else if ( m_name == "TOUTPS2" )
	{
		m_registerType = Register::T2CON;
		m_bitPos = 5;
	}
	else if ( m_name == "TOUTPS3" )
	{
		m_registerType = Register::T2CON;
		m_bitPos = 6;
	}
//---SSPCON------//

	else if ( m_name == "SSPM0" )
	{
		m_registerType = Register::SSPCON;
		m_bitPos = 0;
	}
	else if ( m_name == "SSPM1" )
	{
		m_registerType = Register::SSPCON;
		m_bitPos = 1;
	}
	else if ( m_name == "SSPM2" )
	{
		m_registerType = Register::SSPCON;
		m_bitPos = 2;
	}
	else if ( m_name == "SSPM3" )
	{
		m_registerType = Register::SSPCON;
		m_bitPos = 3;
	}
	else if ( m_name == "CKP" )
	{
		m_registerType = Register::SSPCON;
		m_bitPos = 4;
	}
	else if ( m_name == "SSPEN" )
	{
		m_registerType = Register::SSPCON;
		m_bitPos = 5;
	}
	else if ( m_name == "SSPOV" )
	{
		m_registerType = Register::SSPCON;
		m_bitPos = 6;
	}
	else if ( m_name == "WCOL" )
	{
		m_registerType = Register::SSPCON;
		m_bitPos = 7;
	}
//-------CCP1CON----//
	else if ( m_name == "CCP1M0" )
	{
		m_registerType = Register::CCP1CON;
		m_bitPos = 0;
	}
	else if ( m_name == "CCP1M1" )
	{
		m_registerType = Register::CCP1CON;
		m_bitPos = 1;
	}
	else if ( m_name == "CCP1M2" )
	{
		m_registerType = Register::CCP1CON;
		m_bitPos = 2;
	}
	else if ( m_name == "CCP1M3" )
	{
		m_registerType = Register::CCP1CON;
		m_bitPos = 3;
	}
	else if ( m_name == "CCP1Y" )
	{
		m_registerType = Register::CCP1CON;
		m_bitPos = 4;
	}
	else if ( m_name == "CCP1X" )
	{
		m_registerType = Register::CCP1CON;
		m_bitPos = 5;
	}
//-------RCSTA----//
	else if ( m_name == "RX9D" )
	{
		m_registerType = Register::RCSTA;
		m_bitPos = 0;
	}
	else if ( m_name == "OERR" )
	{
		m_registerType = Register::RCSTA;
		m_bitPos = 1;
	}
	else if ( m_name == "FERR" )
	{
		m_registerType = Register::RCSTA;
		m_bitPos = 2;
	}
	else if ( m_name == "ADDEN"&& pic_type=="P16F877" )
	{
		m_registerType = Register::RCSTA;
		m_bitPos = 3;
	}
	else if ( m_name == "ADEN"&& pic_type=="P16F627" )
	{
		m_registerType = Register::RCSTA;
		m_bitPos = 3;
	}
	else if ( m_name == "CREN" )
	{
		m_registerType = Register::RCSTA;
		m_bitPos = 4;
	}
	else if ( m_name == "SREN" )
	{
		m_registerType = Register::RCSTA;
		m_bitPos = 5;
	}
	else if ( m_name == "RX9" )
	{
		m_registerType = Register::RCSTA;
		m_bitPos = 6;
	}
	else if ( m_name == "SPEN" )
	{
		m_registerType = Register::RCSTA;
		m_bitPos = 7;
	}
//-----CCP2CON-------//
	else if ( m_name == "CCP2M0" )
	{
		m_registerType = Register::CCP2CON;
		m_bitPos = 0;
	}
	else if ( m_name == "CCP2M1" )
	{
		m_registerType = Register::CCP2CON;
		m_bitPos = 1;
	}
	else if ( m_name == "CCP2M2" )
	{
		m_registerType = Register::CCP2CON;
		m_bitPos = 2;
	}
	else if ( m_name == "CCP2M3" )
	{
		m_registerType = Register::CCP2CON;
		m_bitPos = 3;
	}
	else if ( m_name == "CCP2Y" )
	{
		m_registerType = Register::CCP2CON;
		m_bitPos = 4;
	}
	else if ( m_name == "CCP2X" )
	{
		m_registerType = Register::CCP2CON;
		m_bitPos = 5;
	}
//--------ADCON0------//
	else if ( m_name == "ADON" )
	{
		m_registerType = Register::ADCON0;
		m_bitPos = 0;
	}
	else if ( m_name == "GO" )
	{
		m_registerType = Register::ADCON0;
		m_bitPos = 2;
	}
	else if ( m_name == "CHS0" )
	{
		m_registerType = Register::ADCON0;
		m_bitPos = 3;
	}
	else if ( m_name == "CHS1" )
	{
		m_registerType = Register::ADCON0;
		m_bitPos = 4;
	}
	else if ( m_name == "CHS2" )
	{
		m_registerType = Register::ADCON0;
		m_bitPos = 5;
	}
	else if ( m_name == "ADCS0" )
	{
		m_registerType = Register::ADCON0;
		m_bitPos = 6;
	}
	else if ( m_name == "ADCS1" )
	{
		m_registerType = Register::ADCON0;
		m_bitPos = 7;
	}
//-------CMCON---------------//pic16f627
	else if ( m_name == "CM0"&& pic_type=="P16F627")
	{
		m_registerType = Register::CMCON;
		m_bitPos = 0;
	}
	else if ( m_name == "CM1"&& pic_type=="P16F627")
	{
		m_registerType = Register::CMCON;
		m_bitPos = 1;
	}
	else if ( m_name == "CM2"&& pic_type=="P16F627")
	{
		m_registerType = Register::CMCON;
		m_bitPos = 2;
	}
	else if ( m_name == "CM3"&& pic_type=="P16F627")
	{
		m_registerType = Register::CMCON;
		m_bitPos = 3;
	}
	else if ( m_name == "CIS"&& pic_type=="P16F627")
	{
		m_registerType = Register::CMCON;
		m_bitPos = 4;
	}
	else if ( m_name == "C2INV"&& pic_type=="P16F627")
	{
		m_registerType = Register::CMCON;
		m_bitPos = 5;
	}
	else if ( m_name == "C1OUT"&& pic_type=="P16F627")
	{
		m_registerType = Register::CMCON;
		m_bitPos = 6;
	}
	else if ( m_name == "C2OUT"&& pic_type=="P16F627")
	{
		m_registerType = Register::CMCON;
		m_bitPos = 7;
	}
//---------------------------------------------Bank1-------------------------------//
//-------OPTION_REGSITER---------------//
	else if ( m_name == "PS0" )
	{
		m_registerType = Register::OPTION_REG;
		m_bitPos = 0;
	}
	else if ( m_name == "PS1" )
	{
		m_registerType = Register::OPTION_REG;
		m_bitPos = 1;
	}
	else if ( m_name == "PS2" )
	{
		m_registerType = Register::OPTION_REG;
		m_bitPos = 2;
	}
	else if ( m_name == "PSA" )
	{
		m_registerType = Register::OPTION_REG;
		m_bitPos = 3;
	}
	else if ( m_name == "T0SE" )
	{
		m_registerType = Register::OPTION_REG;
		m_bitPos = 4;
	}
	else if ( m_name == "T0CS" )
	{
		m_registerType = Register::OPTION_REG;
		m_bitPos = 5;
	}
	else if ( m_name == "INTEDG" )
	{
		m_registerType = Register::OPTION_REG;
		m_bitPos = 6;
	}
	else if(m_name =="NOT_RBPU"&&(pic_type=="P16C84"||pic_type=="P16F84"||pic_type=="P16F627"))
	{
		m_registerType = Register::OPTION_REG;
		m_bitPos = 7;
	}
	else if (m_name == "RBPU" && pic_type=="P16C84")
	{
		m_registerType = Register::OPTION_REG;
		m_bitPos = 7;
	}
//--------PIE1---------//
	else if ( m_name == "TMR1IE" )
	{
		m_registerType = Register::PIE1;
		m_bitPos = 0;
	}
	else if ( m_name == "TMR2IE" )
	{
		m_registerType = Register::PIE1;
		m_bitPos = 1;
	}
	else if ( m_name == "CCP1IE" )
	{
		m_registerType = Register::PIE1;
		m_bitPos = 2;
	}
	else if ( m_name == "SSPIE" && pic_type=="P16F877")
	{
		m_registerType = Register::PIE1;
		m_bitPos = 3;
	}
	else if ( m_name == "TXIE" )
	{
		m_registerType = Register::PIE1;
		m_bitPos = 4;
	}
	else if ( m_name == "RCIE" )
	{
		m_registerType = Register::PIE1;
		m_bitPos = 5;
	}
	else if ( m_name == "ADIE" && pic_type=="P16F877" )
	{
		m_registerType = Register::PIE1;
		m_bitPos = 6;
	}
	else if ( m_name == "CMIE" && pic_type=="P16F627" )
	{
		m_registerType = Register::PIE1;
		m_bitPos = 6;
	}
	else if ( m_name == "PSPIE" && pic_type=="P16F877" )
	{
		m_registerType = Register::PIE1;
		m_bitPos = 7;
	}
	else if ( m_name == "EEIE" && pic_type=="P16F627" )
	{
		m_registerType = Register::PIE1;
		m_bitPos = 7;
	}
 //--------PIE2---------//
	else if ( m_name == "CCP2IE" )
	{
		m_registerType = Register::PIE2;
		m_bitPos = 0;
	}
	else if ( m_name == "BCLIE" )
	{
		m_registerType = Register::PIE2;
		m_bitPos = 3;
	}
	else if ( m_name == "EEIE"&& pic_type=="P16F877" )
	{
		m_registerType = Register::PIE2;
		m_bitPos = 4;
	}
//--------PCON---------//
	else if ( m_name == "NOT_BOR" )
	{
		m_registerType = Register::PCON;
		m_bitPos = 0;
	}
	else if ( m_name == "NOT_POR" )
	{
		m_registerType = Register::PCON;
		m_bitPos = 1;
	}
	else if ( m_name == "OSCF"&& pic_type=="P16F627" )
	{
		m_registerType = Register::PCON;
		m_bitPos = 3;
	}
//--------SSPCON2------//
	else if ( m_name =="SEN")
	{
		m_registerType = Register::SSPCON2;
		m_bitPos = 0;
	}
	else if ( m_name =="RSEN")
	{
		m_registerType = Register::SSPCON2;
		m_bitPos = 1;
	}
	else if ( m_name =="PEN")
	{
		m_registerType = Register::SSPCON2;
		m_bitPos = 2;
	}
	else if ( m_name =="RCEN")
	{
		m_registerType = Register::SSPCON2;
		m_bitPos = 3;
	}
	else if ( m_name =="ACKEN")
	{
		m_registerType = Register::SSPCON2;
		m_bitPos = 4;
	}
	else if ( m_name =="ACKDT")
	{
		m_registerType = Register::SSPCON2;
		m_bitPos = 5;
	}
	else if ( m_name =="ACKSTAT" )
	{
		m_registerType = Register::SSPCON2;
		m_bitPos = 6;
	}
	else if ( m_name == "GCEN" )
	{
		m_registerType = Register::SSPCON2;
		m_bitPos = 7;
	}
//--------SSPSTAT------//
	else if ( m_name =="BF")
	{
		m_registerType = Register::SSPSTAT;
		m_bitPos = 0;
	}
	else if ( m_name == "UA")
	{
		m_registerType = Register::SSPSTAT;
		m_bitPos = 1;
	}
	else if ( m_name =="R")
	{
		m_registerType = Register::SSPSTAT;
		m_bitPos = 2;
	}
	else if ( m_name =="S")
	{
		m_registerType = Register::SSPSTAT;
		m_bitPos = 3;
	}
	else if ( m_name == "P")
	{
		m_registerType = Register::SSPSTAT;
		m_bitPos = 4;
	}
	else if ( m_name == "D")
	{
		m_registerType = Register::SSPSTAT;
		m_bitPos = 5;
	}
	else if ( m_name == "CKE" )
	{
		m_registerType = Register::SSPSTAT;
		m_bitPos = 6;
	}
	else if ( m_name == "SMP" )
	{
		m_registerType = Register::SSPSTAT;
		m_bitPos = 7;
	}
//--------TXSTA--------//
	else if ( m_name == "TX9D" )
	{
		m_registerType = Register::TXSTA;
		m_bitPos = 0;
	}
	else if ( m_name == "TRMT" )
	{
		m_registerType = Register::TXSTA;
		m_bitPos = 1;
	}
	else if ( m_name == "BRGH" )
	{
		m_registerType = Register::TXSTA;
		m_bitPos = 2;
	}
	else if ( m_name == "SYNC" )
	{
		m_registerType = Register::TXSTA;
		m_bitPos = 4;
	}
	else if ( m_name == "TXEN" )
	{
		m_registerType = Register::TXSTA;
		m_bitPos = 5;
	}
	else if ( m_name == "TX9" )
	{
		m_registerType = Register::TXSTA;
		m_bitPos = 6;
	}
	else if ( m_name == "CSRC" )
	{
		m_registerType = Register::TXSTA;
		m_bitPos = 7;
	}
 //---------ADCON1-----//
	else if ( m_name == "PCFG0" )
	{
		m_registerType = Register::ADCON1;
		m_bitPos = 0;
	}
	else if ( m_name == "PCFG1" )
	{
		m_registerType = Register::ADCON1;
		m_bitPos = 1;
	}
	else if ( m_name == "PCFG2" )
	{
		m_registerType = Register::ADCON1;
		m_bitPos = 2;
	}
	else if ( m_name == "PCFG3" )
	{
		m_registerType = Register::ADCON1;
		m_bitPos = 3;
	}
	else if ( m_name == "ADFM" )
	{
		m_registerType = Register::ADCON1;
		m_bitPos = 7;
	}
 //--------------------------------------------Bank2------------------//
//-----NOTHING TODO---//
//
//--------------------------------------------Bank3------------------//
	else if ( m_name == "RD" )
	{
		m_registerType = Register::EECON1;
		m_bitPos = 0;
	}
	else if ( m_name == "WR" )
	{
		m_registerType = Register::EECON1;
		m_bitPos = 1;
	}
	else if ( m_name == "WREN" )
	{
		m_registerType = Register::EECON1;
		m_bitPos = 2;
	}
	else if ( m_name == "WRERR" )
	{
		m_registerType = Register::EECON1;
		m_bitPos = 3;
	}
	else if ( m_name == "EEIF"&&(pic_type=="P16F84"||pic_type=="P16C84"))//imp ****
	{
		m_registerType = Register::EECON1;
		m_bitPos = 4;
	}
	else if ( m_name == "EEPGD" && pic_type=="P16F877" )
	{
		m_registerType = Register::EECON1;
		m_bitPos = 7;
	}
//---------VRCON------//
	else if ( m_name == "VR0" && pic_type=="P16F627" )
	{
		m_registerType = Register::VRCON;
		m_bitPos = 0;
	}
	else if ( m_name == "VR1" && pic_type=="P16F627" )
	{
		m_registerType = Register::VRCON;
		m_bitPos = 1;
	}
	else if ( m_name == "VR2" && pic_type=="P16F627" )
	{
		m_registerType = Register::VRCON;
		m_bitPos = 2;
	}
	else if ( m_name == "VR3" && pic_type=="P16F627" )
	{
		m_registerType = Register::VRCON;
		m_bitPos = 3;
	}
	else if ( m_name == "VRR" && pic_type=="P16F627" )
	{
		m_registerType = Register::VRCON;
		m_bitPos = 5;
	}
	else if ( m_name == "VROE" && pic_type=="P16F627" )
	{
		m_registerType = Register::VRCON;
		m_bitPos = 6;
	}
	else if ( m_name == "VREN" && pic_type=="P16F627" )
	{
		m_registerType = Register::VRCON;
		m_bitPos = 7;
	}
	else
	{
		m_registerType = Register::none;
		m_bitPos = 0;
		kdError() << k_funcinfo << "Unknown bit: " << m_name << endl;
	}
}
//END class RegisterBit




//BEGIN class RegisterState
RegisterState::RegisterState()
{
	reset();
}


void RegisterState::reset()
{
	known = 0x0;
	value = 0x0;
}


void RegisterState::merge( const RegisterState & state )
{
	known &= state.known;
	known &= ~( value ^ state.value );
}


bool RegisterState::operator == ( const RegisterState & state ) const
{
	return (known == state.known) && (value == state.value);
}


void RegisterState::print()
{
	cout << "   known="<< binary(known).toStdString() <<endl;
	cout << "   value="<< binary(value).toStdString() <<endl;
}
//END class RegisterState



//BEGIN class RegisterBehaviour
RegisterBehaviour::RegisterBehaviour()
{
	reset();
}


void RegisterBehaviour::reset()
{
	depends = 0x0;
	indep = 0x0;
}
//END class RegisterBehaviour



//BEGIN class ProcessorState
ProcessorState::ProcessorState()
{
}


void ProcessorState::reset()
{
	working.reset();
	status.reset();
	
	RegisterMap::iterator end = m_registers.end();
	for ( RegisterMap::iterator it = m_registers.begin(); it != end; ++it )
		(*it).reset();
}


void ProcessorState::merge( const ProcessorState & state )
{
	working.merge( state.working );
	status.merge( state.status );
	
	RegisterMap::iterator this_it = m_registers.begin();
	RegisterMap::const_iterator other_it = state.m_registers.begin();
	
	RegisterMap::iterator this_end = m_registers.end();
	RegisterMap::const_iterator other_end = state.m_registers.end();
	
	while ( true )
	{
		if ( this_it == this_end )
		{
			// So remaining registers of this are default
			while ( other_it != other_end )
			{
				m_registers[ other_it.key() ].merge( *other_it );
				++other_it;
			}
			return;
		}
		
		if ( other_it == other_end )
		{
			// So remaining registers of other are default
			while ( this_it != this_end )
			{
				(*this_it).merge( RegisterState() );
				++this_it;
			}
			return;
		}
		
		RegisterState thisReg = *this_it;
		RegisterState otherReg = *other_it;
		
		if ( this_it.key() == other_it.key() )	
		{
			(*this_it).merge( *other_it );
			++this_it;
			++other_it;
		}
		else if ( this_it.key() < other_it.key() )
		{
			(*this_it).merge( RegisterState() );
			++this_it;
		}
		else // other_it.key() < this_it.key()
		{
			m_registers[ other_it.key() ].merge( *other_it );
			++other_it;
		}
	}
}


RegisterState & ProcessorState::reg( const Register & reg )
{
	if ( reg.type() == Register::WORKING )
		return working;
	
	if ( reg.type() == Register::STATUS )
		return status;
	
	return m_registers[ reg ];
}


RegisterState ProcessorState::reg( const Register & reg ) const
{
	if ( reg.type() == Register::WORKING )
		return working;
	
	if ( reg.type() == Register::STATUS )
		return status;
	
	return m_registers[ reg ];
}


bool ProcessorState::operator == ( const ProcessorState & state ) const
{
	if ( working != state.working )
		return false;
	
	if ( status != state.status )
		return false;
	
	RegisterMap::const_iterator this_it = m_registers.begin();
	RegisterMap::const_iterator other_it = state.m_registers.begin();
	
	RegisterMap::const_iterator this_end = m_registers.end();
	RegisterMap::const_iterator other_end = state.m_registers.end();
	
	while ( true )
	{
		if ( this_it == this_end )
		{
			// So remaining registers of this are default
			while ( other_it != other_end )
			{
				if ( *other_it != RegisterState() )
					return false;
				++other_it;
			}
			return true;
		}
		
		if ( other_it == other_end )
		{
			// So remaining registers of other are default
			while ( this_it != this_end )
			{
				if ( *this_it != RegisterState() )
					return false;
				++this_it;
			}
			return true;
		}
		
		RegisterState thisReg = *this_it;
		RegisterState otherReg = *other_it;
		
		if ( this_it.key() == other_it.key() )	
		{
			if ( *this_it != *other_it )
				return false;
			++this_it;
			++other_it;
		}
		else if ( this_it.key() < other_it.key() )
		{
			if ( *this_it != RegisterState() )
				return false;
			++this_it;
		}
		else // other_it.key() < this_it.key()
		{
			if ( *other_it != RegisterState() )
				return false;
			++other_it;
		}
	}
}


void ProcessorState::print()
{
	cout << " WORKING:\n";
	working.print();
	cout << " STATUS:\n";
	working.print();
	RegisterMap::iterator end = m_registers.end();
	for ( RegisterMap::iterator it = m_registers.begin(); it != end; ++it )
	{
		cout << " " << it.key().name().toStdString() << ":\n";
        it.value().print();
// 		it.data().print();
	}
}
//END class ProcessorState



//BEGIN class ProcessorBehaviour
ProcessorBehaviour::ProcessorBehaviour()
{
}


void ProcessorBehaviour::reset()
{
	working.reset();
	status.reset();
	
	RegisterMap::iterator end = m_registers.end();
	for ( RegisterMap::iterator it = m_registers.begin(); it != end; ++it )
		(*it).reset();
}


RegisterBehaviour & ProcessorBehaviour::reg( const Register & reg )
{
	if ( reg.type() == Register::WORKING )
		return working;
	
	if ( reg.type() == Register::STATUS )
		return status;
	
	return m_registers[ reg ];
}
//END class ProcessorBehaviour



//BEGIN class RegisterDepends
RegisterDepends::RegisterDepends()
{
	reset();
}


void RegisterDepends::reset()
{
	working = 0x0;
	status = 0x0;
	
	RegisterMap::iterator end = m_registers.end();
	for ( RegisterMap::iterator it = m_registers.begin(); it != end; ++it )
		(*it) = 0x0;
}


uchar & RegisterDepends::reg( const Register & reg )
{
	if ( reg.type() == Register::WORKING )
		return working;
	
	if ( reg.type() == Register::STATUS )
		return status;
	
	// If we don't already have the register, we need to reset it first
	if ( !m_registers.contains( reg.name() ) )
		m_registers[ reg ] = 0xff;
	
	return m_registers[ reg ];
}
//END class RegisterDepends



//BEGIN clas Code
Code::Code()
{
}


void Code::merge( Code * code, InstructionPosition middleInsertionPosition )
{
	if ( code == this )
	{
		cout << k_funcinfo << "identical\n";
		return;
	}
	
	if ( !code )
		return;
	
	// Reparent instructions
	for ( unsigned i = 0; i < PositionCount; ++i )
	{
		InstructionList * list = code->instructionList( (InstructionPosition)i );
		InstructionList::const_iterator end = list->end();
		for ( InstructionList::const_iterator it = list->begin(); it != end; ++it )
			append( *it, ( (i == Middle) ? middleInsertionPosition : (InstructionPosition)i ) );
		
		// Queue any labels that the other code has queued
		m_queuedLabels[i] += code->queuedLabels( (InstructionPosition)i );
	}
}


void Code::queueLabel( const QString & label, InstructionPosition position )
{
// 	cout << k_funcinfo << "label="<<label<<" position="<<position<<'\n';
	m_queuedLabels[ position ] << label;
}


void Code::removeInstruction( Instruction * instruction )
{
	if ( !instruction )
		return;
	
	// If the instruction could potentially be jumped over by a BTFSS or a
	// BTFSC intsruction, then we must also remove the bit test instruction,
	// else the next instruction will be jumped over instead after removal.
	// Removing the bit test instruction is perfectly safe as it only does
	// branching (not setting of any bits, etc).
	
	// Any labels that the instruction has must be given to the next
	// instruction.
	
	iterator e = end();
	iterator previous = e; // Refers to the previous instruction if it was a bit test instruction
	for ( iterator i = begin(); i != e; ++i )
	{
		if ( *i != instruction )
		{
			if ( dynamic_cast<Instr_btfss*>(*i) || dynamic_cast<Instr_btfsc*>(*i) )
				previous = i;
			else
				previous = e;
			continue;
		}
		
		iterator next = ++iterator(i);
		
		QStringList labels = instruction->labels();
		i.list->erase( i.it );
		
		if ( previous != e )
		{
			labels += (*previous)->labels();
			previous.list->erase( previous.it );
		}
		
		if ( next != e )
			(*next)->addLabels( labels );
		
		break;
	}
	
// 	instruction->removeOutputs();
}


void Code::append( Instruction * instruction, InstructionPosition position )
{
	if ( !instruction )
		return;
	
// 	cout << k_funcinfo << instruction->code() << '\n';
	
	removeInstruction( instruction );
	m_instructionLists[position].append( instruction );
	
	instruction->setCode( this );
	
	if ( instruction->type() == Instruction::Assembly /*||
			instruction->type() == Instruction::Raw*/ )
	{
// 		if ( (position == Middle) && !m_queuedLabels[position].isEmpty() )
// 			cout << "adding queued labels for 1: " << m_queuedLabels[position].join(",") << '\n';
		instruction->addLabels( m_queuedLabels[position] );
		m_queuedLabels[position].clear();
	}
}


Instruction * Code::instruction( const QString & label ) const
{
	for ( unsigned i = 0; i < PositionCount; ++i )
	{
		InstructionList::const_iterator end = m_instructionLists[i].end();
		for ( InstructionList::const_iterator it = m_instructionLists[i].begin(); it != end; ++it )
		{
			if ( (*it)->labels().contains( label ) )
				return *it;
		}
	}
	return 0l;
}


Code::iterator Code::find( Instruction * instruction )
{
	iterator e = end();
	iterator i = begin();
	for ( ; i != e; ++i )
	{
		if ( *i == instruction )
			break;
	}
	return i;
}


void Code::postCompileConstruct()
{
	// Give any queued labels to the instructions in the subsequent code block
	for ( unsigned i = 0; i < PositionCount; ++i )
	{
		if ( m_queuedLabels[i].isEmpty() )
			continue;
		
		QStringList labels = m_queuedLabels[i];
		m_queuedLabels[i].clear();
		
		// Find an instruction to dump them onto
		for ( unsigned block = i+1; block < PositionCount; ++block )
		{
			bool added = false;
			
			InstructionList::iterator end = m_instructionLists[block].end();
			for ( InstructionList::iterator it = m_instructionLists[block].begin(); it != end; ++it )
			{
				if ( (*it)->type() == Instruction::Assembly )
				{
					(*it)->addLabels( labels );
					added = true;
					break;
				}
			}
			
			if ( added )
				break;
		}
	}
}


QString Code::generateCode( PIC14 * pic ) const
{
	QString code;
	
	const QStringList variables = findVariables();
	if ( !variables.isEmpty() )
	{
		code += "; Variables\n";
		uchar reg = pic->gprStart();
		QStringList::const_iterator end = variables.end();
		for ( QStringList::const_iterator it = variables.begin(); it != end; ++it )
			code += QString("%1\tequ\t0x%2\n").arg( *it ).arg( QString::number( reg++, 16 ) );
		
		code += "\n";
	}
	
	QString picString = pic->minimalTypeString();
	code += QString("list p=%1\n").arg( picString );
	code += QString("include \"p%2.inc\"\n\n").arg( picString.toLower() );
	
	code += "; Config options\n";
	code += "  __config _WDT_OFF\n\n";
	
	code += "START\n\n";
	
	for ( unsigned i = 0; i < PositionCount; ++i )
	{
		InstructionList::const_iterator end = m_instructionLists[i].end();
		for ( InstructionList::const_iterator it = m_instructionLists[i].begin(); it != end; ++it )
		{
			const QStringList labels = (*it)->labels();
			if ( !labels.isEmpty() )
			{
				code += '\n';
				QStringList::const_iterator labelsEnd = labels.end();
				for ( QStringList::const_iterator labelsIt = labels.begin(); labelsIt != labelsEnd; ++labelsIt )
					code += *labelsIt + '\n';
			}
			
			if ( (*it)->type() == Instruction::Assembly )
				code += '\t';
			code += (*it)->code() + '\n';
		}
	}
	
	return code;
}


QStringList Code::findVariables() const
{
	QStringList variables;
	
	const_iterator e = end();
	for ( const_iterator i = begin(); i != e; ++i )
	{
		if ( (*i)->file().type() != Register::GPR )
			continue;
		
		QString alias = (*i)->file().name();
		if ( !variables.contains( alias ) )
			variables << alias;
	}
	
	return variables;
}


void Code::generateLinksAndStates()
{
	CodeIterator e = end();
	
	for ( CodeIterator it = begin(); it != e; ++it )
			(*it)->clearLinks();
	
	for ( CodeIterator it = begin(); it != e; ++it )
		(*it)->generateLinksAndStates( it );
	
	// Generate return links for call instructions
	// This cannot be done from the call instructions as we need to have
	// generated the links first.
	for ( CodeIterator it = begin(); it != e; ++it )
	{
		Instr_call * ins = dynamic_cast<Instr_call*>(*it);
		if ( !ins )
			continue;
		
		Instruction * next = *(++Code::iterator(it));
		ins->makeReturnLinks( next );
	}
}


void Code::setAllUnused()
{
	CodeIterator e = end();
	for ( CodeIterator it = begin(); it != e; ++it )
	{
		(*it)->setUsed( false );
		(*it)->resetRegisterDepends();
	}
}


CodeIterator Code::begin()
{
	// Following code is very similar to the  version of this function.
	// Make sure any changes are applied to both (when applicable).
	
	for ( unsigned i = 0; i < PositionCount; ++i )
	{
		if ( m_instructionLists[i].isEmpty() )
			continue;
		
		CodeIterator codeIterator;
		codeIterator.code = this;
		codeIterator.it = m_instructionLists[i].begin();
		codeIterator.pos = (Code::InstructionPosition)i;
		codeIterator.list = & m_instructionLists[i];
		codeIterator.listEnd = m_instructionLists[i].end();
		
		return codeIterator;
	}
	
	return end();
}


CodeIterator Code::end()
{
	// Following code is very similar to the  version of this function.
	// Make sure any changes are applied to both (when applicable).
	
	CodeIterator codeIterator;
	codeIterator.code = this;
	codeIterator.it = m_instructionLists[ PositionCount - 1 ].end();
	codeIterator.pos = (Code::InstructionPosition)(Code::PositionCount - 1);
	codeIterator.list = & m_instructionLists[ PositionCount - 1 ];
	codeIterator.listEnd = m_instructionLists[ PositionCount - 1 ].end();
	return codeIterator;
}


CodeConstIterator Code::begin() const
{
	// Following code is very similar to the non-const version of this function.
	// Make sure any changes are applied to both (when applicable).
	
	for ( unsigned i = 0; i < PositionCount; ++i )
	{
		if ( m_instructionLists[i].isEmpty() )
			continue;
		
		CodeConstIterator codeIterator;
		codeIterator.code = this;
		codeIterator.it = m_instructionLists[i].begin();
		codeIterator.pos = (Code::InstructionPosition)i;
		codeIterator.list = & m_instructionLists[i];
		codeIterator.listEnd = m_instructionLists[i].end();
		
		return codeIterator;
	}
	
	return end();
}


CodeConstIterator Code::end() const
{
	// Following code is very similar to the non-const version of this function.
	// Make sure any changes are applied to both (when applicable).
	
	CodeConstIterator codeIterator;
	codeIterator.code = this;
	codeIterator.it = m_instructionLists[ PositionCount - 1 ].end();
	codeIterator.pos = (Code::InstructionPosition)(Code::PositionCount - 1);
	codeIterator.list = & m_instructionLists[ PositionCount - 1 ];
	codeIterator.listEnd = m_instructionLists[ PositionCount - 1 ].end();
	return codeIterator;
}
//END class Code



//BEGIN class CodeIterator
CodeIterator & CodeIterator::operator ++ ()
{
	// NOTE: This code is very similar to the const version.
	// Any changes to thsi code should be applied there as well (when applicable).
	
	do
	{
		if ( ++it == listEnd && pos < (Code::PositionCount - 1) )
		{
			bool found = false;
			for ( pos = (Code::InstructionPosition)(pos+1); pos < Code::PositionCount; pos = (Code::InstructionPosition)(pos+1) )
			{
				list = code->instructionList( pos );
				listEnd = list->end();
				if ( list->isEmpty() )
					continue;
		
				it = list->begin();
				found = true;
				break;
			}
	
			if ( !found )
				it = listEnd;
		}
	}
	while ( (it != listEnd) && ((*it)->type() != Instruction::Assembly) );
	
	return *this;
}


CodeIterator & CodeIterator::removeAndIncrement()
{
	Instruction * i = *it;
	++(*this);
	code->removeInstruction( i );
	return *this;
}


void CodeIterator::insertBefore( Instruction * ins )
{
	list->insert( it, ins );
}
//END class CodeIterator



//BEGIN class CodeConstIterator
CodeConstIterator & CodeConstIterator::operator ++ ()
{
	// NOTE: This code is very similar to the non-const version.
	// Any changes to thsi code should be applied there as well (when applicable).
	
	do
	{
		if ( ++it == listEnd && pos < (Code::PositionCount - 1) )
		{
			bool found = false;
			for ( pos = (Code::InstructionPosition)(pos+1); pos < Code::PositionCount; pos = (Code::InstructionPosition)(pos+1) )
			{
				list = code->instructionList( pos );
				listEnd = list->end();
				if ( list->isEmpty() )
					continue;
		
				it = list->begin();
				found = true;
				break;
			}
	
			if ( !found )
				it = listEnd;
		}
	}
	while ( (it != listEnd) && ((*it)->type() != Instruction::Assembly) );
	
	return *this;
}
//END class CodeConstIterator




//BEGIN class Instruction
Instruction::Instruction()
{
	m_bInputStateChanged = true;
	m_bPositionAffectsBranching = false;
	m_bUsed = false;
	m_literal = 0;
	m_dest = 0;
}


Instruction::~ Instruction()
{
}


void Instruction::addLabels( const QStringList & labels )
{
	m_labels += labels;
}


void Instruction::setLabels( const QStringList & labels )
{
	m_labels = labels;
}


void Instruction::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	m_outputState.reset();
}


ProcessorBehaviour Instruction::behaviour() const
{
	return ProcessorBehaviour();
}


void Instruction::makeOutputLinks( Code::iterator current, bool firstOutput, bool secondOutput )
{
	if ( !firstOutput && !secondOutput )
		return;
	
	++current;
	if ( !*current )
	{
		kdWarning() << k_funcinfo << "current+1 is null"<<endl;
		return;
	}
	if ( firstOutput )
		(*current)->addInputLink( this );
	
	if ( !secondOutput )
		return;
	
	++current;
	(*current)->addInputLink( this );
}


void Instruction::makeLabelOutputLink( const QString & label )
{
	Instruction * output = m_pCode->instruction( label );
	if ( output )
		output->addInputLink( this );
}


void Instruction::addInputLink( Instruction * instruction )
{
	// Don't forget that a link to ourself is valid!
	if ( !instruction || m_inputLinks.contains( instruction ) )
		return;
	
	m_inputLinks << instruction;
	instruction->addOutputLink( this );
}


void Instruction::addOutputLink( Instruction * instruction )
{
	// Don't forget that a link to ourself is valid!
	if ( !instruction || m_outputLinks.contains( instruction ) )
		return;
	
	m_outputLinks << instruction;
	instruction->addInputLink( this );
}


void Instruction::removeInputLink( Instruction * instruction )
{
	m_inputLinks.removeAll( instruction );
}


void Instruction::removeOutputLink( Instruction * instruction )
{
	m_outputLinks.removeAll( instruction );
}


void Instruction::clearLinks()
{
	m_inputLinks.clear();
	m_outputLinks.clear();
}
//END class Instruction



//BEGIN Byte-Oriented File Register Operations
QString Instr_addwf::code() const
{
	return QString("addwf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_addwf::generateLinksAndStates( Code::iterator current )
{
	m_outputState = m_inputState;
	
	m_outputState.reg( outputReg() ).value = (m_inputState.working.value + m_inputState.reg( m_file ).value) & 0xff;
	m_outputState.reg( outputReg() ).known = ((m_inputState.working.known == 0xff) && (m_inputState.reg( m_file ).known == 0xff)) ? 0xff : 0x0;
	
	m_outputState.status.known &= ~( (1 << RegisterBit::C) | (1 << RegisterBit::DC) | (1 << RegisterBit::Z) );
	
	if ( m_file.type() != Register::PCL || m_dest == 0 )
	{
		makeOutputLinks( current );
		return;
	}
	
	++current; // Don't have a link to ourself
	
	// maxInc is the greatest possibly value that we might have incremented the program counter by.
	// It is generated by ORing the known bits of the working register with the greatest value
	// of the unknown bits;
	uchar maxInc = m_inputState.working.maxValue();
	if ( maxInc < 0xff )
		maxInc++;
// 	cout << "m_inputState.working.known="<<int(m_inputState.working.known)<<" maxInc="<<int(maxInc)<<'\n';
	Code::iterator end = m_pCode->end();
	for ( int i = 0; current != end && i < maxInc; ++i, ++current )
	{
		(*current)->addInputLink( this );
// 		if ( i != maxInc-1 )
// 			(*current)->setPositionAffectsBranching( true );
	}
}

ProcessorBehaviour Instr_addwf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	// Depend on W and f
	behaviour.working.depends = 0xff;
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.status.indep = (1 << RegisterBit::C) | (1 << RegisterBit::DC) | (1 << RegisterBit::Z);
	return behaviour;
}



QString Instr_andwf::code() const
{
	return QString("andwf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_andwf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	m_outputState = m_inputState;
	
	uchar definiteOnes = m_inputState.reg( m_file ).definiteOnes() & m_outputState.working.definiteOnes();
	m_outputState.reg( outputReg() ).value = definiteOnes;
	m_outputState.reg( outputReg() ).known = m_inputState.reg( m_file ).definiteZeros() | m_inputState.working.definiteZeros() | definiteOnes;
	
	m_outputState.status.known &= ~(1 << RegisterBit::Z);
}

ProcessorBehaviour Instr_andwf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	// Depend on W and f
	behaviour.working.depends = 0xff;
	behaviour.reg( m_file ).depends = 0xff;
	
	if ( m_dest == 0 )
		behaviour.working.indep = m_inputState.reg( m_file ).known & ~( m_inputState.reg( m_file ).value);
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.status.indep = (1 << RegisterBit::Z);
	return behaviour;
}


QString Instr_clrf::code() const
{
	return QString("clrf\t%1").arg( m_file.name() );
}

void Instr_clrf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.reg( m_file ).known = 0xff;
	m_outputState.reg( m_file ).value = 0x0;
	
	m_outputState.status.known |= (1 << RegisterBit::Z);
	m_outputState.status.value |= (1 << RegisterBit::Z);
}

ProcessorBehaviour Instr_clrf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	behaviour.reg( m_file ).indep = 0xff;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.status.indep = (1 << RegisterBit::Z);
	
	return behaviour;
}


//TODO CLRW
//TODO COMF


QString Instr_decf::code() const
{
	return QString("decf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_decf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.status.known &= ~(1 << RegisterBit::Z);
	
	m_outputState.reg( outputReg() ).known = 0x0;
}

ProcessorBehaviour Instr_decf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.status.indep = (1 << RegisterBit::Z);
	
	return behaviour;
}


QString Instr_decfsz::code() const
{
	return QString("decfsz\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_decfsz::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current, true, true );
	
	m_outputState = m_inputState;
	m_outputState.reg( outputReg() ).known = 0x0;
}

ProcessorBehaviour Instr_decfsz::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	return behaviour;
}


QString Instr_incf::code() const
{
	return QString("incf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_incf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.status.known &= ~(1 << RegisterBit::Z);
	
	m_outputState.reg( outputReg() ).known = 0x0;
}

ProcessorBehaviour Instr_incf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.status.indep = (1 << RegisterBit::Z);
	return behaviour;
}


//TODO INCFSZ


QString Instr_iorwf::code() const
{
	return QString("iorwf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_iorwf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.status.known &= ~(1 << RegisterBit::Z);
	
	m_outputState.reg( outputReg() ).known = 0x0;
}

ProcessorBehaviour Instr_iorwf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	// Depend on W and f
	behaviour.working.depends = 0xff;
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.status.indep = (1 << RegisterBit::Z);
	return behaviour;
}


QString Instr_movf::code() const
{
	return QString("movf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_movf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	
	if ( m_inputState.reg( m_file ).known == 0xff )
	{
		m_outputState.status.known |= (1 << RegisterBit::Z);
		bool isZero = (m_inputState.reg( m_file ).value == 0x0);
		if ( isZero )
			m_outputState.status.value |= (1 << RegisterBit::Z);
		else
			m_outputState.status.value &= ~(1 << RegisterBit::Z);
	}
	else
		m_outputState.status.known &= ~(1 << RegisterBit::Z);
	
	if ( m_dest == 0 )
	{
		// Writing to the working register
		m_outputState.working.known = m_inputState.reg( m_file ).known;
		m_outputState.working.value = m_inputState.reg( m_file ).value;
	}
}

ProcessorBehaviour Instr_movf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	if ( m_dest == 0 )
		behaviour.working.indep = 0xff;
	
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.status.indep = (1 << RegisterBit::Z);
	return behaviour;
}


QString Instr_movwf::code() const
{
	return QString("movwf\t%1").arg( m_file.name() );
}

void Instr_movwf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.reg( m_file ).known = m_inputState.working.known;
	m_outputState.reg( m_file ).value = m_inputState.working.value;
}

ProcessorBehaviour Instr_movwf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	behaviour.reg( m_file ).indep = 0xff;
	behaviour.working.depends = 0xff;
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	
	return behaviour;
}


//TODO NOP



QString Instr_rlf::code() const
{
	return QString("rlf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_rlf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.status.known &= ~(1 << RegisterBit::C);
	
	m_outputState.reg( outputReg() ).known = 0x0;
}

ProcessorBehaviour Instr_rlf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	// Is the value written to W or f?
	if ( m_dest == 0 )
		behaviour.working.indep = 0xff;
	
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = (1 << RegisterBit::C) | (m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0);
	behaviour.status.indep = (1 << RegisterBit::C);
	return behaviour;
}


QString Instr_rrf::code() const
{
	return QString("rrf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_rrf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.status.known &= ~(1 << RegisterBit::C);
	
	m_outputState.reg( outputReg() ).known = 0x0;
}

ProcessorBehaviour Instr_rrf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	if ( m_dest == 0 )
		behaviour.working.indep = 0xff;
	
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = (1 << RegisterBit::C) | (m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0);
	behaviour.status.indep = (1 << RegisterBit::C);
	return behaviour;
}


QString Instr_subwf::code() const
{
	return QString("subwf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_subwf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	
	if ( (m_inputState.working.known == 0xff) && (m_inputState.reg( m_file ).known == 0xff) )
	{
		m_outputState.reg( outputReg() ).known = 0xff;
		m_outputState.reg( outputReg() ).value = (m_inputState.reg( m_file ).value - m_inputState.working.value) & 0xff;
	}
	else
		m_outputState.reg( outputReg() ).known = 0x0;
	
	
	m_outputState.status.known &= ~( (1 << RegisterBit::C) | (1 << RegisterBit::DC) | (1 << RegisterBit::Z) );
	
	if ( m_inputState.working.minValue() > m_inputState.reg( m_file ).maxValue() )
	{
		m_outputState.status.value &= ~(1 << RegisterBit::C);
		m_outputState.status.known |= (1 << RegisterBit::C);
	}
	else if ( m_inputState.working.maxValue() <= m_inputState.reg( m_file ).minValue() )
	{
		m_outputState.status.value |= (1 << RegisterBit::C);
		m_outputState.status.known |= (1 << RegisterBit::C);
	}
	
	if ( (m_inputState.working.known == 0xff) && (m_inputState.reg( m_file ).known == 0xff) )
	{
		bool isZero = (m_inputState.working.value == m_inputState.reg( m_file ).value);
		if ( isZero )
			m_outputState.status.value |= (1 << RegisterBit::Z);
		else
			m_outputState.status.value &= ~(1 << RegisterBit::Z);
		m_outputState.status.known |= (1 << RegisterBit::Z);
	}
}

ProcessorBehaviour Instr_subwf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	// Depend on W and f
	behaviour.working.depends = 0xff;
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.status.indep = (1 << RegisterBit::C) | (1 << RegisterBit::DC) | (1 << RegisterBit::Z);
	return behaviour;
}


QString Instr_swapf::code() const
{
	return QString("swapf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_swapf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	if ( m_dest == 0 )
	{
		// Writing to the working register
		m_outputState.working.known = 0x0;
	}
}

ProcessorBehaviour Instr_swapf::behaviour() const
{
	ProcessorBehaviour behaviour;
	behaviour.reg( m_file ).depends = 0xff;
	behaviour.working.indep = ( m_dest == 0 ) ? 0xff : 0x0;
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	return behaviour;
}


QString Instr_xorwf::code() const
{
	return QString("xorwf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_xorwf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.status.known &= ~(1 << RegisterBit::Z);
	
	m_outputState.reg( outputReg() ).known = 0x0;
}

ProcessorBehaviour Instr_xorwf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	// Depend on W and f
	behaviour.working.depends = 0xff;
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.status.indep = (1 << RegisterBit::Z);
	return behaviour;
}
//END Byte-Oriented File Register Operations



//BEGIN Bit-Oriented File Register Operations
QString Instr_bcf::code() const
{
	return QString("bcf\t\t%1,%2").arg( m_file.name() ).arg( m_bit.name() );
}

void Instr_bcf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.reg( m_file ).value &= ~uchar(1 << m_bit.bitPos());
	m_outputState.reg( m_file ).known |= uchar(1 << m_bit.bitPos());
}

ProcessorBehaviour Instr_bcf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.reg( m_file ).indep = 1 << m_bit.bitPos();
	return behaviour;
}


QString Instr_bsf::code() const
{
	return QString("bsf\t\t%1,%2").arg( m_file.name() ).arg( m_bit.name() );
}

void Instr_bsf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.reg( m_file ).value |= uchar(1 << m_bit.bitPos());
	m_outputState.reg( m_file ).known |= uchar(1 << m_bit.bitPos());
}

ProcessorBehaviour Instr_bsf::behaviour() const
{
	ProcessorBehaviour behaviour;
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.reg( m_file ).indep = 1 << m_bit.bitPos();
	return behaviour;
}


QString Instr_btfsc::code() const
{
	return QString("btfsc\t%1,%2").arg( m_file.name() ).arg( m_bit.name() );
}

void Instr_btfsc::generateLinksAndStates( Code::iterator current )
{
	m_outputState = m_inputState;
	
	if ( m_inputState.reg( m_file ).known & (1 << m_bit.bitPos()) )
	{
		bool bit = m_inputState.reg( m_file ).value & (1 << m_bit.bitPos());
		makeOutputLinks( current, bit, !bit );
	}
	else
		makeOutputLinks( current, true, true );
}

ProcessorBehaviour Instr_btfsc::behaviour() const
{
	ProcessorBehaviour behaviour;
	behaviour.reg( m_file ).depends = 1 << m_bit.bitPos();
	behaviour.status.depends = (m_file.type() == Register::STATUS) ? m_bit.bit() : 0x0;
	return behaviour;
}


QString Instr_btfss::code() const
{
	return QString("btfss\t%1,%2").arg( m_file.name() ).arg( m_bit.name() );
}

void Instr_btfss::generateLinksAndStates( Code::iterator current )
{
	m_outputState = m_inputState;
	
	if ( m_inputState.reg( m_file ).known & (1 << m_bit.bitPos()) )
	{
		bool bit = m_inputState.reg( m_file ).value & (1 << m_bit.bitPos());
		makeOutputLinks( current, !bit, bit );
	}
	else
		makeOutputLinks( current, true, true );
}

ProcessorBehaviour Instr_btfss::behaviour() const
{
	ProcessorBehaviour behaviour;
	behaviour.reg( m_file ).depends = 1 << m_bit.bitPos();
	behaviour.status.depends = (m_file.type() == Register::STATUS) ? m_bit.bit() : 0x0;
	return behaviour;
}
//END Bit-Oriented File Register Operations



//BEGIN Literal and Control Operations
QString Instr_addlw::code() const
{
	return QString("addlw\t%1").arg( m_literal );
}

void Instr_addlw::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.working.value = (m_inputState.working.value + m_literal) & 0xff;
	m_outputState.working.known = (m_inputState.working.known == 0xff) ? 0xff : 0x0;
	m_outputState.status.known &= ~( (1 << RegisterBit::C) | (1 << RegisterBit::DC) | (1 << RegisterBit::Z) );
}

ProcessorBehaviour Instr_addlw::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	behaviour.working.depends = 0xff;
	
	behaviour.status.indep = (1 << RegisterBit::C) | (1 << RegisterBit::DC) | (1 << RegisterBit::Z);
	
	return behaviour;
}


QString Instr_andlw::code() const
{
	return QString("andlw\t%1").arg( m_literal );
}

void Instr_andlw::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.working.value = (m_inputState.working.value & m_literal) & 0xff;
	m_outputState.working.known |= ~m_literal; // Now know any bits that are zero in value
	m_outputState.status.known &= ~(1 << RegisterBit::Z);
}

ProcessorBehaviour Instr_andlw::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	behaviour.working.indep = ~m_literal;
	behaviour.working.depends = m_literal;
	
	behaviour.status.indep = (1 << RegisterBit::Z);
	return behaviour;
}


QString Instr_call::code() const
{
	return QString("call\t%1").arg( m_label );
}

void Instr_call::generateLinksAndStates( Code::iterator current )
{
	(void)current;
	makeLabelOutputLink( m_label );
	
	m_outputState = m_inputState;
}

ProcessorBehaviour Instr_call::behaviour() const
{
	ProcessorBehaviour behaviour;
	return behaviour;
}

void Instr_call::makeReturnLinks( Instruction * next )
{
	m_pCode->setAllUnused();
	linkReturns( m_pCode->instruction( m_label ), next );
}


void Instr_call::linkReturns( Instruction * current, Instruction * returnPoint )
{
	while (true)
	{
		if ( !current || current->isUsed() )
			return;
		
		current->setUsed( true );
		if ( dynamic_cast<Instr_return*>(current) || dynamic_cast<Instr_retlw*>(current) )
		{
// 			cout << "Added return link" << endl;
// 			cout << "   FROM: " << current->code() << endl;
// 			cout << "   TO:   " << returnPoint->code() << endl;
			returnPoint->addInputLink( current );
			return;
		}
		if ( dynamic_cast<Instr_call*>(current) )
		{
			// Jump over the call instruction to its return point,
			// which will be the instruction after current.
			current = *(++m_pCode->find( current ));
			continue;
		}
		
		const InstructionList outputs = current->outputLinks();
		
		if ( outputs.isEmpty() )
			return;
		
		if ( outputs.size() == 1 )
			current = outputs.first();
		
		else
		{
			// Can't avoid function recursion now.
			InstructionList::const_iterator end = outputs.end();
			for ( InstructionList::const_iterator it = outputs.begin(); it != end; ++it )
				linkReturns( *it, returnPoint );
			return;
		}
	};
}


//TODO CLRWDT


QString Instr_goto::code() const
{
	return QString("goto\t%1").arg( m_label );
}

void Instr_goto::generateLinksAndStates( Code::iterator current )
{
	(void)current;
	
	makeLabelOutputLink( m_label );
	
	m_outputState = m_inputState;
}

ProcessorBehaviour Instr_goto::behaviour() const
{
	ProcessorBehaviour behaviour;
	return behaviour;
}


QString Instr_iorlw::code() const
{
	return QString("iorlw\t%1").arg( m_literal );
}

void Instr_iorlw::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.working.value = (m_inputState.working.value | m_literal) & 0xff;
	m_outputState.working.known |= m_literal; // Now know any bits that are one in value
	m_outputState.status.known &= ~(1 << RegisterBit::Z);
}

ProcessorBehaviour Instr_iorlw::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	behaviour.working.indep = m_literal;
	behaviour.working.depends = ~m_literal;
	
	behaviour.status.indep = (1 << RegisterBit::Z);;
	return behaviour;
}


QString Instr_movlw::code() const
{	
	return QString("movlw\t%1").arg( m_literal );
}

void Instr_movlw::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	m_outputState = m_inputState;
	m_outputState.working.known = 0xff;
	m_outputState.working.value = m_literal;
}

ProcessorBehaviour Instr_movlw::behaviour() const
{
	ProcessorBehaviour behaviour;
	behaviour.working.indep = 0xff;
	return behaviour;
}


QString Instr_retfie::code() const
{
	return "retfie";
}

void Instr_retfie::generateLinksAndStates( Code::iterator current )
{
	// Don't generate any output links
	(void)current;
	
	m_inputState = m_outputState;
}

ProcessorBehaviour Instr_retfie::behaviour() const
{
	ProcessorBehaviour behaviour;
	return behaviour;
}


QString Instr_retlw::code() const
{
	return QString("retlw\t%1").arg( m_literal );
}

void Instr_retlw::generateLinksAndStates( Code::iterator current )
{
	(void)current;
	
	m_outputState = m_inputState;
	m_outputState.working.known = 0xff;
	m_outputState.working.value = m_literal;
}

ProcessorBehaviour Instr_retlw::behaviour() const
{
	ProcessorBehaviour behaviour;
	behaviour.working.indep = 0xff;
	return behaviour;
}



QString Instr_return::code() const
{
	return "return";
}

void Instr_return::generateLinksAndStates( Code::iterator current )
{
	(void)current;
	
	m_outputState = m_inputState;
}

ProcessorBehaviour Instr_return::behaviour() const
{
	ProcessorBehaviour behaviour;
	return behaviour;
}


QString Instr_sleep::code() const
{
	return "sleep";
}

void Instr_sleep::generateLinksAndStates( Code::iterator current )
{
	// Don't generate any output links
	(void)current;
	
	m_outputState = m_inputState;
	m_outputState.status.value &= ~(1 << RegisterBit::NOT_PD);
	m_outputState.status.value |= (1 << RegisterBit::NOT_TO);
	m_outputState.status.known |= (1 << RegisterBit::NOT_TO) | (1 << RegisterBit::NOT_PD);
}

ProcessorBehaviour Instr_sleep::behaviour() const
{
	ProcessorBehaviour behaviour;
	behaviour.status.indep = (1 << RegisterBit::NOT_TO) | (1 << RegisterBit::NOT_PD);
	return behaviour;
}


QString Instr_sublw::code() const
{
	return QString("sublw\t%1").arg( m_literal );
}

void Instr_sublw::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.working.value = (m_literal - m_inputState.working.value) & 0xff;
	m_outputState.working.known = (m_inputState.working.known == 0xff) ? 0xff : 0x00;
	m_outputState.status.known &= ~( (1 << RegisterBit::C) | (1 << RegisterBit::DC) | (1 << RegisterBit::Z) );
	
	if ( m_inputState.working.minValue() > m_literal )
	{
		m_outputState.status.value &= ~(1 << RegisterBit::C);
		m_outputState.status.known |= (1 << RegisterBit::C);
	}
	else if ( m_inputState.working.maxValue() <= m_literal )
	{
		m_outputState.status.value |= (1 << RegisterBit::C);
		m_outputState.status.known |= (1 << RegisterBit::C);
	}
	
	if ( m_inputState.working.known == 0xff )
	{
		bool isZero = (m_inputState.working.value == m_literal);
		if ( isZero )
			m_outputState.status.value |= (1 << RegisterBit::Z);
		else
			m_outputState.status.value &= ~(1 << RegisterBit::Z);
		m_outputState.status.known |= (1 << RegisterBit::Z);
	}
}

ProcessorBehaviour Instr_sublw::behaviour() const
{
	ProcessorBehaviour behaviour;
	behaviour.working.depends = 0xff;
	behaviour.status.indep = (1 << RegisterBit::C) | (1 << RegisterBit::DC) | (1 << RegisterBit::Z);
	return behaviour;
}


QString Instr_xorlw::code() const
{
	return QString("xorlw\t%1").arg( m_literal );
}

void Instr_xorlw::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	m_outputState = m_inputState;
	m_outputState.working.value = (m_inputState.working.value ^ m_literal) & 0xff;
	m_outputState.working.known = m_inputState.working.known;
	m_outputState.status.known &= ~(1 << RegisterBit::Z);
}

ProcessorBehaviour Instr_xorlw::behaviour() const
{
	ProcessorBehaviour behaviour;
	behaviour.working.depends = 0xff;
	behaviour.status.indep = (1 << RegisterBit::Z);
	return behaviour;
}
//END Literal and Control Operations



//BEGIN Microbe (non-assembly) Operations
QString Instr_sourceCode::code() const
{
	QStringList sourceLines = m_raw.split("\n"); // QString::split("\n",m_raw);
	return ";" + sourceLines.join("\n;");
}


QString Instr_asm::code() const
{
	return "; asm {\n" + m_raw + "\n; }";
}


QString Instr_raw::code() const
{
	return m_raw;
}
//END Microbe (non-assembly) Operations

