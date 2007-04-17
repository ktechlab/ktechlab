/***************************************************************************
 *   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>                     *
 *   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>            *
 *   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>                     *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "drawpart.h"
#include "propertyeditor.h"
#include "propertyeditoritem.h"

#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>

#include <qcolor.h>
#include <qcursor.h>
#include <qfont.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qsize.h>


//BEGIN Class PropertyEditorItem
PropertyEditorItem::PropertyEditorItem( PropertyEditorItem * par, Property * property )
	: KListViewItem( par, property->editorCaption(), property->displayString() )
{
	setExpandable( false );
	m_property=property;
	connect( m_property, SIGNAL(valueChanged( QVariant, QVariant )), this, SLOT(propertyValueChanged()) );

	updateValue();

	//3 rows per item is enough?
	setMultiLinesEnabled( true );
	setHeight(static_cast<PropertyEditor*>(listView())->baseRowHeight()*3);
}


PropertyEditorItem::PropertyEditorItem(KListView *par, const QString &text)
	: KListViewItem(par, text, "")
{
	m_property = 0;
	setSelectable(false);
	setOpen(true);

	//3 rows per item is enough?
	setMultiLinesEnabled( true );
	setHeight(static_cast<PropertyEditor*>(par)->baseRowHeight()*3);
}


void PropertyEditorItem::propertyValueChanged()
{
	setText( 1, m_property->displayString() );
}


void PropertyEditorItem::paintCell(QPainter *p, const QColorGroup & cg, int column, int width, int align)
{
	if ( depth() == 0 )
		return;
	
	int margin = listView()->itemMargin();
	
	QColor bgColor = backgroundColor();
	
	if(column == 1)
	{
		switch(m_property->type())
		{
// 			case QVariant::Pixmap:
// 			{
// 				p->fillRect(0,0,width,height(),QBrush(backgroundColor()));
// 				p->drawPixmap(margin, margin, m_property->value().toPixmap());
// 				break;
// 			}
			
			case Variant::Type::Color:
			{
				p->fillRect(0,0,width,height(), QBrush(bgColor));
				QColor ncolor = m_property->value().toColor();
				p->setBrush(ncolor);
				p->drawRect(margin, margin, width - 2*margin, height() - 2*margin);
				QColorGroup nGroup(cg);
				break;
			}
			
			case Variant::Type::Bool:
			{
				p->fillRect(0,0,width,height(), QBrush(bgColor));
				if(m_property->value().toBool())
				{
					p->drawPixmap(margin, height()/2 -8, SmallIcon("button_ok"));
					p->drawText(QRect(margin+20,0,width,height()-1), Qt::AlignVCenter, i18n("Yes"));
				}
				else
				{
					p->drawPixmap(margin, height()/2 -8, SmallIcon("button_cancel"));
					p->drawText(QRect(margin+20,0,width,height()-1), Qt::AlignVCenter, i18n("No"));
				}
				break;
			}
			
			case Variant::Type::PenStyle:
			{
				p->fillRect(0,0,width,height(), QBrush(bgColor));
				
				PenStyle style = DrawPart::nameToPenStyle( m_property->value().toString() );
				int penWidth = 3;
				QPen pen( black, penWidth, style );
				p->setPen( pen );
				p->drawLine( height()/2, height()/2-1, width-height()/2, height()/2-1 );
				break;
			}
			
#if 0
			case Variant::Type::PenCapStyle:
			{
				p->fillRect(0,0,width,height(), QBrush(bgColor));
				
				PenCapStyle style = DrawPart::nameToPenCapStyle( m_property->value().toString() );
				int penWidth = 6;
				QPen pen( black, penWidth, SolidLine, style, MiterJoin );
				p->setPen( pen );
				p->drawLine( width/2-10, height()/2-2, width/2+10, height()/2-2 );
				break;
			}
#endif
			
			case Variant::Type::None:
			case Variant::Type::Int:
			case Variant::Type::Raw:
			case Variant::Type::Double:
			case Variant::Type::String:
			case Variant::Type::Multiline:
			case Variant::Type::RichText:
			case Variant::Type::Select:
			case Variant::Type::Combo:
			case Variant::Type::FileName:
			case Variant::Type::VarName:
			case Variant::Type::PenCapStyle:
			case Variant::Type::Port:
			case Variant::Type::Pin:
			case Variant::Type::SevenSegment:
			case Variant::Type::KeyPad:
			{
				KListViewItem::paintCell(p, cg, column, width, align);
				break;
			}
		}
	}
	else
	{
		if(isSelected())
		{
			p->fillRect(0,0,width, height(), QBrush(cg.highlight()));
			p->setPen(cg.highlightedText());
		}
		else
			p->fillRect(0,0,width, height(), QBrush(bgColor));

		QFont f = listView()->font();
		p->save();
		
		if ( m_property->changed() )
			f.setBold(true);
		
		p->setFont(f);
		p->drawText(QRect(margin,0,width, height()-1), Qt::AlignVCenter, text(0));
		p->restore();

		p->setPen( QColor(200,200,200) ); //like in table view
		p->drawLine(width-1, 0, width-1, height()-1);
	}

	p->setPen( QColor(200,200,200) ); //like in t.v.
	p->drawLine(-50, height()-1, width, height()-1 );
}


void PropertyEditorItem::setup()
{
	KListViewItem::setup();
	if ( depth() == 0 )
		setHeight(0);
}


PropertyEditorItem::~PropertyEditorItem()
{
}


void PropertyEditorItem::updateValue(bool alsoParent)
{
	QString text;
	if ( m_property )
		text = m_property->displayString();
	
	setText( 1, text );
	if ( alsoParent && QListViewItem::parent() )
		static_cast<PropertyEditorItem*>(QListViewItem::parent())->updateValue();
}


void PropertyEditorItem::paintFocus ( QPainter * , const QColorGroup & , const QRect &  )
{
}
//END class PropertyEditorItem

#include "propertyeditoritem.moc"
