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

#include "item.h"
#include "iteminterface.h"
#include "itemgroup.h"
#include "ktechlab.h"
#include "propertyeditor.h"
#include "propertyeditorcolor.h"
#include "propertyeditorfile.h"
#include "propertyeditorlist.h"
#include "propertyeditorinput.h"

#include <klocale.h>
#include <kdebug.h>
#include <kpushbutton.h>
#include <kiconloader.h>

#include <Qt/q3header.h>
#include <Qt/qevent.h>
#include <Qt/qfontmetrics.h>
#include <Qt/qtimer.h>
#include <Qt/qapplication.h>
#include <Qt/qeventloop.h>

PropertyEditor::PropertyEditor( QWidget * parent, const char * name )
	: K3ListView( parent /*, name */ )
 , m_items(101, false)
 , justClickedItem(false)
{
	m_items.setAutoDelete(false);

	addColumn( i18n("Property") );
	addColumn( i18n("Value") );

	m_topItem = 0;
	m_editItem = 0;

	connect(this, SIGNAL(selectionChanged(Q3ListViewItem *)), this, SLOT(slotClicked(Q3ListViewItem *)));
	connect(this, SIGNAL(currentChanged(Q3ListViewItem *)), this, SLOT(slotCurrentChanged(Q3ListViewItem *)));
	connect(this, SIGNAL(expanded(Q3ListViewItem *)), this, SLOT(slotExpanded(Q3ListViewItem *)));
	connect(this, SIGNAL(collapsed(Q3ListViewItem *)), this, SLOT(slotCollapsed(Q3ListViewItem *)));
	connect(header(), SIGNAL(sizeChange( int, int, int )), this, SLOT(slotColumnSizeChanged( int, int, int )));
	connect(header(), SIGNAL(clicked( int )), this, SLOT(moveEditor()));
	connect(header(), SIGNAL(sectionHandleDoubleClicked ( int )), this, SLOT(slotColumnSizeChanged( int )));

	m_defaults = new KPushButton(viewport());
	m_defaults->setFocusPolicy(Qt::NoFocus);
	setFocusPolicy(Qt::ClickFocus);
	m_defaults->setPixmap(SmallIcon("undo"));
	m_defaults->setToolTip(i18n("Undo changes"));
	m_defaults->hide();
	connect(m_defaults, SIGNAL(clicked()), this, SLOT(resetItem()));

	setRootIsDecorated( false );
	setShowSortIndicator( false );
	// setTooltipColumn(0); // TODO equivalent?
	setSorting(0);
	setItemMargin(2);
	setResizeMode(K3ListView::LastColumn);
	header()->setMovingEnabled( false );
	setTreeStepSize(0);

	m_baseRowHeight = QFontMetrics(font()).height() + itemMargin()*2;
}


PropertyEditor::~PropertyEditor()
{
}


void PropertyEditor::slotClicked(Q3ListViewItem *item)
{
	if (!item)
		return;
	
	PropertyEditorItem *i = static_cast<PropertyEditorItem *>(item);
	createEditor(i);

	justClickedItem = true;
}


void PropertyEditor::slotCurrentChanged(Q3ListViewItem *item)
{
	if (item==firstChild())
	{
		Q3ListViewItem *oldItem = item;
		while (item && (!item->isSelectable() || !item->isVisible()))
			item = item->itemBelow();
		
		if (item && item!=oldItem)
		{
			setSelected(item,true);
			return;
		}
	}
}


void PropertyEditor::slotExpanded(Q3ListViewItem *item)
{
	if (!item)
		return;
	moveEditor();
}


void PropertyEditor::slotCollapsed(Q3ListViewItem *item)
{
	if (!item)
		return;
	moveEditor();
}


void PropertyEditor::createEditor( PropertyEditorItem * i )
{
	int y = viewportToContents(QPoint(0, itemRect(i).y())).y();
	QRect geometry(columnWidth(0), y, columnWidth(1), i->height());

	delete m_currentEditor;

	m_editItem = i;

	PropertySubEditor *editor=0;
	switch ( i->type() )
	{
		case Variant::Type::String:
			editor = new PropertyEditorInput( viewport(), i->property() );
			break;

		case Variant::Type::Port:
		case Variant::Type::Pin:
		case Variant::Type::Combo:
		case Variant::Type::VarName:
		case Variant::Type::Select:
		case Variant::Type::PenStyle:
		case Variant::Type::PenCapStyle:
		case Variant::Type::SevenSegment:
		case Variant::Type::KeyPad:
			editor = new PropertyEditorList( viewport(), i->property() );
			break;
			
		case Variant::Type::FileName:
			editor = new PropertyEditorFile( viewport(), i->property() );
			break;
			
		case Variant::Type::Int:
			editor = new PropertyEditorSpin( viewport(), i->property() );
			break;
			
		case Variant::Type::Double:
			editor = new PropertyEditorDblSpin( viewport(), i->property() );
			break;
			
		case Variant::Type::Color:
			editor = new PropertyEditorColor( viewport(), i->property() );
			break;
			
		case Variant::Type::Bool:
			editor = new PropertyEditorBool( viewport(), i->property() );
			break;

		case Variant::Type::Raw:
		case Variant::Type::Multiline:
		case Variant::Type::RichText:
		case Variant::Type::None:
			break;
	}

	if (editor)
	{
		addChild(editor);
		moveChild(editor, geometry.x(), geometry.y());
		editor->show();

		editor->setFocus();
	}
	
	m_currentEditor = editor;
	showDefaultsButton( i->property()->changed() );
}


void PropertyEditor::showDefaultsButton( bool show )
{
	int y = viewportToContents(QPoint(0, itemRect(m_editItem).y())).y();
	QRect geometry(columnWidth(0), y, columnWidth(1), m_editItem->height());
	m_defaults->resize(m_baseRowHeight, m_baseRowHeight);

	if (!show) {
		if (m_currentEditor) {
			if (m_currentEditor->leavesTheSpaceForRevertButton()) {
				geometry.setWidth(geometry.width()-m_defaults->width());
			}
			m_currentEditor->resize(geometry.width(), geometry.height());
		}
		m_defaults->hide();
		return;
	}

	QPoint p = contentsToViewport(QPoint(0, geometry.y()));
	m_defaults->move(geometry.x() + geometry.width() - m_defaults->width(), p.y());
	if (m_currentEditor) {
		m_currentEditor->move(m_currentEditor->x(), p.y());
		m_currentEditor->resize(geometry.width()-m_defaults->width(), geometry.height());
	}
	m_defaults->show();
}


void PropertyEditor::updateDefaultsButton()
{
	if (!m_editItem)
		return;
	showDefaultsButton( m_editItem->property()->changed() );
	m_editItem->repaint();
}


void PropertyEditor::slotColumnSizeChanged( int section, int, int newS)
{
	if ( m_currentEditor )
	{
		if(section == 0)
		{
			m_currentEditor->move(newS, m_currentEditor->y());
		}
		else
		{
			if(m_defaults->isVisible())
				m_currentEditor->resize(newS - m_defaults->width(), m_currentEditor->height());
			else
				m_currentEditor->resize(
					newS-(m_currentEditor->leavesTheSpaceForRevertButton()?m_defaults->width():0),
					m_currentEditor->height());
		}
	}
}


void PropertyEditor::slotColumnSizeChanged( int section)
{
	setColumnWidth(1, viewport()->width() - columnWidth(0));
	slotColumnSizeChanged(section, 0, header()->sectionSize(section));
	if(m_currentEditor)
	{
		if(m_defaults->isVisible())
			m_currentEditor->resize(columnWidth(1) - m_defaults->width(), m_currentEditor->height());
		else
			m_currentEditor->resize(
				columnWidth(1)-(m_currentEditor->leavesTheSpaceForRevertButton()?m_defaults->width():0),
				m_currentEditor->height());
	}
}


void PropertyEditor::reset()
{
	if ( m_currentEditor )
		m_currentEditor->deleteLater();
	m_currentEditor = 0;
	
	if ( m_defaults->isVisible() )
		m_defaults->hide();

	clear();
	m_editItem = 0;
	m_topItem = 0;
}


QSize PropertyEditor::sizeHint() const
{
	return QSize( QFontMetrics(font()).width(columnText(0)+columnText(1)+"   "),
		K3ListView::sizeHint().height());
}


void PropertyEditor::create( ItemGroup * b )
{
	m_pItemGroup = b;
	
	//QCString selectedPropertyName1, selectedPropertyName2;
    QByteArray selectedPropertyName1, selectedPropertyName2;
	
	fill();
	
	//select prev. selecteed item
	PropertyEditorItem * item = 0;
	if (!selectedPropertyName2.isEmpty()) //try other one for old buffer
		item = m_items[selectedPropertyName2];
	if (!item && !selectedPropertyName1.isEmpty()) //try old one for current buffer
		item = m_items[selectedPropertyName1];
	if (item)
	{
		setSelected(item, true);
		ensureItemVisible(item);
	}
}


void PropertyEditor::fill()
{
	reset();
	
	if ( !m_pItemGroup || !m_pItemGroup->activeItem() )
		return;

	if(!m_topItem)
	{
		m_topItem = new PropertyEditorItem(this,"Top Item");
	}

	m_items.clear();

	VariantDataMap *vmap = m_pItemGroup->activeItem()->variantMap();
	// Build the list
	for( VariantDataMap::iterator vait = vmap->begin(); vait != vmap->end(); ++vait )
	{
		Variant * v = *vait;
		if ( v->isHidden() )
			continue;
		
		switch ( v->type() )
		{
			case Variant::Type::String:
			case Variant::Type::Port:
			case Variant::Type::Pin:
			case Variant::Type::Combo:
			case Variant::Type::VarName:
			case Variant::Type::Select:
			case Variant::Type::PenStyle:
			case Variant::Type::PenCapStyle:
			case Variant::Type::SevenSegment:
			case Variant::Type::KeyPad:
			case Variant::Type::FileName:
			case Variant::Type::Int:
			case Variant::Type::Double:
			case Variant::Type::Color:
			case Variant::Type::Bool:
				// These are all handled by the ItemEditor
				break;

			case Variant::Type::Raw:
			case Variant::Type::Multiline:
			case Variant::Type::None:
			case Variant::Type::RichText:
				// These are not handled by the ItemEditor
				continue;
		}
		
		PropertyEditorItem  *item = new PropertyEditorItem( m_topItem, v );
		m_items.insert( v->id().latin1(), item );
		
	}
}


void PropertyEditor::setFocus()
{
	PropertyEditorItem *item = static_cast<PropertyEditorItem *>(selectedItem());
	if (item) {
		if (!justClickedItem)
			ensureItemVisible(item);
		justClickedItem = false;
	}
	else
	{
		//select an item before focusing
		item = static_cast<PropertyEditorItem *>(itemAt(QPoint(10,1)));
		if (item)
		{
			ensureItemVisible(item);
			setSelected(item, true);
		}
	}
	if (m_currentEditor)
		m_currentEditor->setFocus();
	
	else
		K3ListView::setFocus();
}


void PropertyEditor::resetItem()
{
	if ( m_editItem  )
	{
		ItemInterface::self()->slotSetData( m_editItem->property()->id(), m_editItem->property()->defaultValue() );
	}
}


void PropertyEditor::moveEditor()
{
	if ( !m_currentEditor )
		return;
	
	QPoint p = contentsToViewport(QPoint(0, itemPos(m_editItem)));
	m_currentEditor->move(m_currentEditor->x(), p.y());
	if( m_defaults->isVisible() )
		m_defaults->move(m_defaults->x(), p.y());
}


void PropertyEditor::resizeEvent(QResizeEvent *ev)
{
	K3ListView::resizeEvent(ev);
	if(m_defaults->isVisible())
	{
		QRect r = itemRect(m_editItem);
		if(r.y()) // r.y() == 0 if the item is not visible on the screen
			m_defaults->move(r.x() + r.width() - m_defaults->width(), r.y());
	}

	if ( m_currentEditor )
	{
		m_currentEditor->resize(
			columnWidth(1)-((m_currentEditor->leavesTheSpaceForRevertButton()||m_defaults->isVisible()) ? m_defaults->width() : 0),
			m_currentEditor->height());
	}
}


bool PropertyEditor::handleKeyPress( QKeyEvent* ev )
{
	const int k = ev->key();
	const Qt::ButtonState s = ev->state();

	//selection moving
	Q3ListViewItem *item = 0;

	if ((s==Qt::NoButton && k==Qt::Key_Up) || k==Qt::Key_Backtab) {
		//find prev visible
		item = selectedItem() ? selectedItem()->itemAbove() : 0;
		while (item && (!item->isSelectable() || !item->isVisible()))
			item = item->itemAbove();
		if (!item)
			return true;
	}
	else if (s==Qt::NoButton && (k==Qt::Key_Down || k==Qt::Key_Tab)) {
		//find next visible
		item = selectedItem() ? selectedItem()->itemBelow() : 0;
		while (item && (!item->isSelectable() || !item->isVisible()))
			item = item->itemBelow();
		if (!item)
			return true;
	}
	else if(s==Qt::NoButton && k==Qt::Key_Home) {
		if (m_currentEditor && m_currentEditor->hasFocus())
			return false;
		//find 1st visible
		item = firstChild();
		while (item && (!item->isSelectable() || !item->isVisible()))
			item = item->itemBelow();
	}
	else if(s==Qt::NoButton && k==Qt::Key_End) {
		if (m_currentEditor && m_currentEditor->hasFocus())
			return false;
		//find last visible
		item = selectedItem();
		Q3ListViewItem *lastVisible = item;
		while (item) { // && (!item->isSelectable() || !item->isVisible()))
			item = item->itemBelow();
			if (item && item->isSelectable() && item->isVisible())
				lastVisible = item;
		}
		item = lastVisible;
	}
	if(item) {
		ev->accept();
		ensureItemVisible(item);
		setSelected(item, true);
		return true;
	}
	return false;
}

#include "propertyeditor.moc"
