/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "flowcodedocument.h"
#include "flowcode.h"
#include "flowcontainer.h"
#include "flowpart.h"
#include "microsettings.h"
#include "microinfo.h"
#include "micropackage.h"
#include "node.h"
#include "outputflownode.h"
#include "pinmapping.h"

#include <KLocalizedString>
// #include <KMessageBox>
#include <QFile>
#include <QTextStream>

FlowCode::FlowCode( ProcessChain *processChain )
	: Language( processChain, i18n("FlowCode") )
{
	m_successfulMessage = i18n("*** Microbe generation successful ***");
	m_failedMessage = i18n("*** Microbe generation failed ***");
	p_startPart = nullptr;
}

FlowCode::~FlowCode()
{
}


void FlowCode::processInput( ProcessOptions options )
{
	m_processOptions = options;
	
	if ( !options.p_flowCodeDocument )
	{
		options.p_flowCodeDocument = new FlowCodeDocument( QString::null, nullptr );
		options.p_flowCodeDocument->openURL(QUrl::fromLocalFile(options.inputFiles().first()));
		
		connect( this, SIGNAL(processSucceeded( Language *)), options.p_flowCodeDocument, SLOT(deleteLater()) );
		connect( this, SIGNAL(processFailed( Language *)), options.p_flowCodeDocument, SLOT(deleteLater()) );
	}

	if ( !options.p_flowCodeDocument->microSettings() )
	{
		finish(false);
		return;
	}

	QFile file(options.intermediaryOutput());
	if ( file.open(QIODevice::WriteOnly | QIODevice::ReadOnly) == false )
	{
		finish(false);
		return;
	}
	file.close();
	
	if ( file.open(QIODevice::WriteOnly) == false )
	{
		finish(false);
		return;
	}

	const QString code = generateMicrobe( options.p_flowCodeDocument->itemList(), options.p_flowCodeDocument->microSettings() );
	if (code.isEmpty())
	{
		finish(false);
		return;
	}

	QTextStream stream(&file);
	stream << code;
	file.close();
	finish(true);
}


void FlowCode::setStartPart( FlowPart *startPart )
{
	p_startPart = startPart;
}


void FlowCode::addCode( const QString& code )
{
	m_code += code;
	if ( !m_code.endsWith("\n") ) m_code += '\n';
}

bool FlowCode::isValidBranch( FlowPart *flowPart )
{
	return flowPart && (flowPart->level() >= m_curLevel) && !m_stopParts.contains(flowPart);
}

void FlowCode::addCodeBranch( FlowPart * flowPart )
{
	if (!flowPart)
		return;
	
	if ( !isValidBranch(flowPart) )
		return;
	
	if ( m_addedParts.contains(flowPart) )
	{
		const QString labelName = genLabel(flowPart->id());
		addCode( "goto "+labelName );
		m_gotos.append(labelName);
		return;
	}
	else
	{
		m_addedParts.append(flowPart);
		int prevLevel = m_curLevel;
		m_curLevel = flowPart->level();
		
		const QString labelName = genLabel(flowPart->id());
		addCode(labelName+':');
		m_labels.append(labelName);
		
		flowPart->generateMicrobe(this);
		m_curLevel = prevLevel;
	}
}

QString FlowCode::genLabel( const QString &id )
{
	return "__label_"+id;
}

void FlowCode::addStopPart( FlowPart *part )
{
	if (part) m_stopParts.append(part);
}

void FlowCode::removeStopPart( FlowPart *part )
{
	if (!part) return;
	
	// We only want to remove one instance of the FlowPart, in case it has been
	// used as a StopPart for more than one FlowPart
	//FlowPartList::iterator it = m_stopParts.find(part);  // 2018.12.01
	//if ( it != m_stopParts.end() ) m_stopParts.remove(it);
	int foundIndex = m_stopParts.indexOf(part);
	if ( -1 == foundIndex ) {
        m_stopParts.removeAll(part);
    }
}

QString FlowCode::generateMicrobe( const ItemList &itemList, MicroSettings *settings )
{
	bool foundStart = false;
	const ItemList::const_iterator end = itemList.end();
	for ( ItemList::const_iterator it = itemList.begin(); it != end; ++it )
	{
		if (!*it)
			continue;
		
		FlowPart * startPart = dynamic_cast<FlowPart*>((Item*)*it);
		
		if (!startPart)
			continue;
		
		// Check to see if we have any floating connections
		const NodeInfoMap nodeMap = startPart->nodeMap();
		NodeInfoMap::const_iterator nodeMapEnd = nodeMap.end();
		for ( NodeInfoMap::const_iterator nodeMapIt = nodeMap.begin(); nodeMapIt != nodeMapEnd; ++nodeMapIt )
		{
			Node * node = nodeMapIt.value().node;
				// FIXME dynamic_cast used
			if(  !node || ( dynamic_cast<OutputFlowNode*>(node) == nullptr) )
				continue;
			
			if ( !startPart->outputPart( nodeMapIt.key() ) )
				outputWarning( i18n("Warning: Floating connection for %1", startPart->id() ) );
		}
		
		FlowContainer * fc = dynamic_cast<FlowContainer*>((Item*)*it);
		
		if ( (*it)->id().startsWith("START") && startPart )
		{
			foundStart = true;
			setStartPart(startPart);
		}
		else if ( ((*it)->id().startsWith("interrupt") || (*it)->id().startsWith("sub")) && fc )
		{
			addSubroutine(fc);
		}
	}
	
	if (!foundStart)
	{
		outputError( i18n("KTechlab was unable to find the \"Start\" part.\nThis must be included as the starting point for your program.") );
		return nullptr;
	}
	
	m_addedParts.clear();
	m_stopParts.clear();
	m_gotos.clear();
	m_labels.clear();
	m_code = QString::null;
	
	// PIC type
	{
		const QString codeString = settings->microInfo()->id() + "\n";
		addCode(codeString);
	}
	
	// Initial variables
	{
		QStringList vars = settings->variableNames();
		
		// If "inited" is true at the end, we comment at the insertion point
		bool inited = false;
		const QString codeString = "// Initial variable values:\n";
		addCode(codeString);
		
		const QStringList::iterator end = vars.end();
		for ( QStringList::iterator it = vars.begin(); it != end; ++it )
		{
			VariableInfo *info = settings->variableInfo(*it);
			if ( info /*&& info->initAtStart*/ )
			{
				inited = true;
				addCode(*it+" = "+info->valueAsString());
			}
		}
		if (!inited) {
			m_code.remove(codeString);
		} else {
			addCode("\n");
		}
	}
	
	// Initial pin maps
	{
		const PinMappingMap pinMappings = settings->pinMappings();
		PinMappingMap::const_iterator end = pinMappings.end();
		for ( PinMappingMap::const_iterator it = pinMappings.begin(); it != end; ++it )
		{
			QString type;
			
			switch ( it.value().type() )
			{
				case PinMapping::Keypad_4x3:
				case PinMapping::Keypad_4x4:
					type = "keypad";
					break;
					
				case PinMapping::SevenSegment:
					type = "sevenseg";
					break;
					
				case PinMapping::Invalid:
					break;
			}
			
			if ( type.isEmpty() )
				continue;
			
			addCode( QString("%1 %2 %3").arg( type ).arg( it.key() ).arg( it.value().pins().join(" ") ) );
		}
	}
	
	// Initial port settings
	{
		QStringList portNames = settings->microInfo()->package()->portNames();
		const QStringList::iterator end = portNames.end();
		
		// TRIS registers (remember that this is set to ..11111 on all resets)
		for ( QStringList::iterator it = portNames.begin(); it != end; ++it )
		{
			const int portType = settings->portType(*it);
			const int pinCount = settings->microInfo()->package()->pinCount( 0, *it );
			
			// We don't need to reset it if portType == 2^(pinCount-1)
			if ( portType != (1<<pinCount)-1 )
			{
				QString name = *it;
				name.replace("PORT","TRIS");
				addCode( name+" = "+QString::number(portType) );
			}
		}
		
		// PORT registers
		for ( QStringList::iterator it = portNames.begin(); it != end; ++it )
		{
			const int portState = settings->portState(*it);
			addCode( (*it)+" = "+QString::number(portState) );
		}
	}
	
	
	m_curLevel = p_startPart->level();
	addCodeBranch(p_startPart);
	addCode("end");
	
	{
		const FlowPartList::iterator end = m_subroutines.end();
		for ( FlowPartList::iterator it = m_subroutines.begin(); it != end; ++it )
		{
			m_curLevel = 0;
			if (*it)
			{
				addCode("\n");
				addCodeBranch(*it);
			}
		}
	}
	
	tidyCode();
	return m_code;
}

void FlowCode::tidyCode()
{
	// First, get rid of the unused labels
	const QStringList::iterator end = m_labels.end();
	for ( QStringList::iterator it = m_labels.begin(); it != end; ++it )
	{
		if ( !m_gotos.contains(*it) ) m_code.remove(*it+':');
	}
	
	
	// And now on to handling indentation :-)

	if ( !m_code.endsWith("\n") ) m_code.append("\n");
	QString newCode;
	bool multiLineComment = false; // For "/*"..."*/"
	bool comment = false; // For "//"
	bool asmEmbed = false;
	bool asmEmbedAllowed = true;
	bool asmKeyword = false;
	int asmEmbedLevel = -1;
	int level = 0;
	
	int pos=-1;
	const int length = m_code.length();
	while ( ++pos<length )
	{
		switch ( m_code[pos].toLatin1() )
		{
			case '\n':
			{
				if (comment && !multiLineComment) comment = false;
				newCode += '\n';
				if ( !comment && !asmEmbed )
				{
					while ( pos+1<length && m_code[pos+1].isSpace() ) pos++;
					bool closeBrace = false;
					if ( pos+1<length && m_code[pos+1] == '}' )
					{
						level--;
						pos++;
						closeBrace = true;
					}
					for ( int i=0; i<level; i++ ) newCode += '\t';
					if (closeBrace) newCode += '}';
					asmEmbedAllowed = true;
				}
				break;
			} 
			case '/':
			{
				newCode += '/';
				if ( pos+1<length )
				{
					if		( m_code[pos+1] == '/' ) comment = true;
					else if ( m_code[pos+1] == '*' ) multiLineComment = comment = true;
					newCode += m_code[++pos];
				}
				asmEmbedAllowed = false;
				asmKeyword = false;
				break;
			}
			case '*':
			{
				newCode += '*';
				if ( pos+1<length )
				{
					if ( m_code[pos++] == '/' && multiLineComment ) comment = multiLineComment = false;
					newCode += m_code[pos];
				}
				asmEmbedAllowed = false;
				asmKeyword = false;
				break;
			}
			case '{':
			{
				if (asmKeyword) {
					asmEmbed = true;
					asmEmbedLevel = level;
				}
				
				if ( !comment ) level++;
				newCode += '{';
				
				asmEmbedAllowed = false;
				asmKeyword = false;
				break;
			}
			case '}':
			{
				if ( !comment ) level--;
				
				if (asmEmbed && asmEmbedLevel == level) 
				{
					asmEmbed = false;
					newCode += "\n";
					for ( int i=0; i<level; i++ ) newCode += '\t';
				}
				newCode += '}';
				
				asmEmbedAllowed = true;
				asmKeyword = false;
				break;
			}
			case 'a':
			{
				newCode += m_code[pos];
				if ( asmEmbedAllowed && !comment && pos+2<length )
				{
					if ( m_code[pos+1] == 's' && m_code[pos+2] == 'm' )
					{
						asmKeyword = true;
						newCode += "sm";
						pos += 2;
					}
				}
				break;
			}
			default:
			{
				asmEmbedAllowed = false;
				asmKeyword = false;
				newCode += m_code[pos];
				break;
			}
		}
	}
	m_code = newCode;
}

void FlowCode::addSubroutine( FlowPart *part )
{
	if ( !part || m_subroutines.contains(part) || part->parentItem() || !dynamic_cast<FlowContainer*>(part) ) return;
	m_subroutines.append(part);
}


ProcessOptions::ProcessPath::Path FlowCode::outputPath( ProcessOptions::ProcessPath::Path inputPath ) const
{
	switch (inputPath)
	{
		case ProcessOptions::ProcessPath::FlowCode_AssemblyAbsolute:
			return ProcessOptions::ProcessPath::Microbe_AssemblyAbsolute;
			
		case ProcessOptions::ProcessPath::FlowCode_Microbe:
			return ProcessOptions::ProcessPath::None;
			
		case ProcessOptions::ProcessPath::FlowCode_PIC:
			return ProcessOptions::ProcessPath::Microbe_PIC;
			
		case ProcessOptions::ProcessPath::FlowCode_Program:
			return ProcessOptions::ProcessPath::Microbe_Program;
			
		case ProcessOptions::ProcessPath::AssemblyAbsolute_PIC:
		case ProcessOptions::ProcessPath::AssemblyAbsolute_Program:
		case ProcessOptions::ProcessPath::AssemblyRelocatable_Library:
		case ProcessOptions::ProcessPath::AssemblyRelocatable_Object:
		case ProcessOptions::ProcessPath::AssemblyRelocatable_PIC:
		case ProcessOptions::ProcessPath::AssemblyRelocatable_Program:
		case ProcessOptions::ProcessPath::C_AssemblyRelocatable:
		case ProcessOptions::ProcessPath::C_Library:
		case ProcessOptions::ProcessPath::C_Object:
		case ProcessOptions::ProcessPath::C_PIC:
		case ProcessOptions::ProcessPath::C_Program:
		case ProcessOptions::ProcessPath::Microbe_AssemblyAbsolute:
		case ProcessOptions::ProcessPath::Microbe_PIC:
		case ProcessOptions::ProcessPath::Microbe_Program:
		case ProcessOptions::ProcessPath::Object_Disassembly:
		case ProcessOptions::ProcessPath::Object_Library:
		case ProcessOptions::ProcessPath::Object_PIC:
		case ProcessOptions::ProcessPath::Object_Program:
		case ProcessOptions::ProcessPath::PIC_AssemblyAbsolute:
		case ProcessOptions::ProcessPath::Program_Disassembly:
		case ProcessOptions::ProcessPath::Program_PIC:
		case ProcessOptions::ProcessPath::Invalid:
		case ProcessOptions::ProcessPath::None:
			return ProcessOptions::ProcessPath::Invalid;
	}
	
	return ProcessOptions::ProcessPath::Invalid;
}

