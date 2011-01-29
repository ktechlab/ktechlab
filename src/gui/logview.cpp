/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "logview.h"


//BEGIN class LogView
LogView::LogView()
{

}


LogView::~LogView()
{
}


void LogView::clear()
{
    m_messageInfoMap.clear();
}


void LogView::addOutput( QString text, OutputType outputType, MessageInfo messageInfo )
{

    m_messageInfoMap[ m_messageInfoMap.size()-1 ] = messageInfo;
}


//END class LogView



//BEGIN class MessageInfo
MessageInfo::MessageInfo()
{
    m_fileLine = -1;
}


MessageInfo::MessageInfo( QString fileURL, int fileLine )
{
    m_fileURL = fileURL;
    m_fileLine = fileLine;
}
//END class MessageInfo


#include "logview.moc"
