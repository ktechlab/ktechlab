/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cnitem.h"
#include "cnitemgroup.h"
#include "contexthelp.h"
#include "itemlibrary.h"
#include "katemdi.h"

#include <klocale.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qregexp.h>
#include <qtextbrowser.h>
#include <qwhatsthis.h>

#include <cassert>

ContextHelp * ContextHelp::m_pSelf = 0;

ContextHelp * ContextHelp::self( KateMDI::ToolView * parent)
{
	if(!m_pSelf)
	{
		assert(parent);
		m_pSelf = new ContextHelp(parent);
	}
	return m_pSelf;
}


ContextHelp::ContextHelp( KateMDI::ToolView * parent)
	: QWidget( parent, "Context Help")
{
	QWhatsThis::add( this, i18n("Provides context-sensitive help relevant to the current editing being performed."));
	
	QVBoxLayout *vlayout = new QVBoxLayout( this, 0, 6);

	m_nameLbl = new QLabel( this, "");
	vlayout->addWidget(m_nameLbl);
	vlayout->addSpacing(8);
	
	m_info = new QTextBrowser( this, "");
	vlayout->addWidget(m_info);
	m_info->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	
	QSpacerItem *spacer3 = new QSpacerItem( 1, 1, QSizePolicy::Preferred, QSizePolicy::Preferred);
	vlayout->addItem(spacer3);
	
	slotClear();
} 


ContextHelp::~ContextHelp()
{
}


void ContextHelp::slotUpdate( Item *item)
{
	if(!item)
	{
		slotClear();
		return;
	}
	m_nameLbl->setText("<h2>"+item->name()+"</h2>");
	m_info->setText( "<b></b>"+item->description());
}


void ContextHelp::slotClear()
{
	m_nameLbl->setText(i18n("<h2>No Item Selected</h2>"));
	m_info->setText("");
}


void ContextHelp::slotMultipleSelected()
{
	m_nameLbl->setText(i18n("<h2>Multiple Items</h2>"));
	m_info->setText("");
}


void ContextHelp::setContextHelp(const QString& name, const QString& help)
{
	m_nameLbl->setText("<h2>"+name+"</h2>");
	QString parsed = help;
	parseInfo(parsed);
	m_info->setText( "<b></b>"+parsed);
}

void ContextHelp::parseInfo( QString &info)
{
	info.replace("<example>","<br><br><b>Example:</b><blockquote>");
	info.replace("</example>","</blockquote>");
}

#include "contexthelp.moc"
