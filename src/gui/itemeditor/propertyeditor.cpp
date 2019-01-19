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

#include "drawparts/drawpart.h"

#include <klocalizedstring.h>
#include <kdebug.h>
#include <kpushbutton.h>
#include <kicon.h>

#include <qevent.h>
#include <qfontmetrics.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qeventloop.h>
#include <qstyleditemdelegate.h>

// #include <q3header.h>
#include <qheaderview.h>

struct PropertyEditorStyledItemColProperty : public QStyledItemDelegate {
    PropertyEditor *m_propEditor;

    PropertyEditorStyledItemColProperty(PropertyEditor *propEditor) : m_propEditor(propEditor) { }

    virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
//         if ( depth() == 0 )
//             return;
        QTableWidgetItem *itemPtr = m_propEditor->item( index.row(), index.column());
        if (!itemPtr) {
            qWarning() << Q_FUNC_INFO << " null item";
            return;
        }
        PropertyEditorItem *itemProp = dynamic_cast<PropertyEditorItem*>( itemPtr );
        if (!itemProp) {
            qWarning() << Q_FUNC_INFO << " cannot cast item";
            return;
        }

        m_propEditor->contentsMargins();
        int margin = 3; // listView()->itemMargin(); // TODO set decent value

        const int width = option.rect.width();
        const int height = option.rect.height();
        const int top = option.rect.top();
        const int left = option.rect.left();

        const bool isHighlighted = m_propEditor->currentRow() == index.row();

        painter->save();

        //qWarning() << " draw col " << index.column() << " row " << index.row()
        //    << " isHighlighted=" << isHighlighted << " state_selected=" << option.state.testFlag(QStyle::State_Selected);

        if (isHighlighted || option.state.testFlag(QStyle::State_Selected))
        {
            painter->fillRect(left,top, width, height, option.palette.highlight());
            painter->setPen(option.palette.color(QPalette::BrightText) /* highlightedText() */ );
        } else {
            QColor bgColor = option.palette.color(QPalette::Base); // 2018.12.07
            painter->fillRect(left,top, width, height, QBrush(bgColor));
        }

        QFont f = option.font;

        if ( itemProp->property()->changed() || (!itemProp->property()->isAdvanced())) {
            f.setBold(true);
        }

        painter->setFont(f);
        painter->drawText( QRect(left + margin, top, width-1, height-1), Qt::AlignVCenter, itemProp->text() );

        //qWarning() << Q_FUNC_INFO << " draw " << itemProp->text() << " at " << option.rect;

        painter->setPen( QColor(200,200,200) ); //like in table view
        painter->drawLine(left + width-1, top, left + width-1, top + height-1);

        painter->setPen( QColor(200,200,200) ); //like in t.v.
        painter->drawLine(left-50, top + height-1, left + width-1, top + height-1 );

        painter->restore();
    }
    /*
    virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const {
        // TODO
    }
    */
};

struct PropertyEditorStyledItemColValue : public QStyledItemDelegate {
    PropertyEditor *m_propEditor;

    PropertyEditorStyledItemColValue(PropertyEditor *propEditor) : m_propEditor(propEditor) { }

    virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
//         if ( depth() == 0 )
//             return;

        QTableWidgetItem *itemPtr = m_propEditor->item( index.row(), index.column());
        if (!itemPtr) {
            qWarning() << Q_FUNC_INFO << " null item";
            return;
        }
        PropertyEditorItem *itemProp = dynamic_cast<PropertyEditorItem*>( itemPtr );
        if (!itemProp) {
            qWarning() << Q_FUNC_INFO << " cannot cast item";
            return;
        }

        int margin = 3; // listView()->itemMargin();  // TODO set decent value

        const int width = option.rect.width();
        const int height = option.rect.height();
        const int top = option.rect.top();
        const int left = option.rect.left();

        //const bool isHighlighted = m_propEditor->currentRow() == index.row(); // TODO

        QColor bgColor = option.palette.color(QPalette::Window); // backgroundColor(0); // 2018.06.02 - is this better?

        painter->save();

        Property *property = itemProp->property();
        switch(property->type())
        {
//          case QVariant::Pixmap:
//          {
//              p->fillRect(0,0,width,height(),QBrush(backgroundColor()));
//              p->drawPixmap(margin, margin, m_property->value().toPixmap());
//              break;
//          }

            case Variant::Type::Color:
            {
                painter->fillRect(left,top, width,height, QBrush(bgColor));
                //QColor ncolor = m_property->value().toColor();
                QColor ncolor = property->value().value<QColor>();
                painter->setBrush(ncolor);
                painter->drawRect(left + margin, top + margin, width - 2*margin, height - 2*margin);
//                 QColorGroup nGroup(cg);
                break;
            }

            case Variant::Type::Bool:
            {
                painter->fillRect(left, top , width,height, QBrush(bgColor));
                if(property->value().toBool())
                {
                    painter->drawPixmap(left + margin, top + height/2 -8, SmallIcon("dialog-ok"));
                    painter->drawText(QRect(left + margin+20, top, width,height-1), Qt::AlignVCenter, i18n("Yes"));
                }
                else
                {
                    painter->drawPixmap(left + margin, top + height/2 -8, SmallIcon("dialog-cancel"));
                    painter->drawText(QRect(left + margin+20, top, width,height-1), Qt::AlignVCenter, i18n("No"));
                }
                break;
            }

            case Variant::Type::PenStyle:
            {
                painter->fillRect(left, top, width,height, QBrush(bgColor));

                Qt::PenStyle style = DrawPart::nameToPenStyle( property->value().toString() );
                int penWidth = 3;
                QPen pen( Qt::black, penWidth, style );
                painter->setPen( pen );
                painter->drawLine( left + height/2, top + height/2-1, left + width-height/2, top + height/2-1 );
                break;
            }

#if 0
            case Variant::Type::PenCapStyle:
            {
                p->fillRect(0,0,width,height, QBrush(bgColor));

                PenCapStyle style = DrawPart::nameToPenCapStyle( property->value().toString() );
                int penWidth = 6;
                QPen pen( black, penWidth, SolidLine, style, MiterJoin );
                p->setPen( pen );
                p->drawLine( width/2-10, height/2-2, width/2+10, height/2-2 );
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
                QStyledItemDelegate::paint(painter, option, index);
                break;
            }
        }

        painter->setPen( QColor(200,200,200) ); //like in t.v.
        painter->drawLine( left-50, top + height-1, left + width, top + height-1 );

        painter->restore();
    }
    /*
    virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const {
        // TODO
    }
    */
};

PropertyEditor::PropertyEditor( QWidget * parent, const char * name )
	: QTableWidget( parent )
 //, m_items(101, false) // 2018.08.13 - unused
 , justClickedItem(false)
 , m_lastCellWidgetRow(-1)
 , m_lastCellWidgetCol(-1)
 , m_colPropertyDelegate(NULL)
 , m_colValueDelegate(NULL)
{
    setObjectName( name );
	//m_items.setAutoDelete(false); // 2018.08.13 - unused

    setColumnCount(2);
    QStringList headerLabels;
    headerLabels.append( i18n("Property") );
    headerLabels.append( i18n("Value") );
    setHorizontalHeaderLabels(headerLabels);
	//addColumn( i18n("Property") );    // 2018.08.13 - ported to QTableWidget
	//addColumn( i18n("Value") );

    m_colPropertyDelegate = new PropertyEditorStyledItemColProperty(this);
    setItemDelegateForColumn(0, m_colPropertyDelegate);

    m_colValueDelegate = new PropertyEditorStyledItemColValue(this);
    setItemDelegateForColumn(1, m_colValueDelegate);

	m_topItem = 0;
	m_editItem = 0;

	connect(this, SIGNAL(clicked(const QModelIndex&)), this, SLOT(slotClicked(const QModelIndex&)));
	connect(this, SIGNAL(itemActivated(QTableWidgetItem*)), this, SLOT(slotCurrentChanged(QTableWidgetItem *)));
// 	connect(this, SIGNAL(expanded(Q3ListViewItem *)), this, SLOT(slotExpanded(Q3ListViewItem *)));  // TODO
// 	connect(this, SIGNAL(collapsed(Q3ListViewItem *)), this, SLOT(slotCollapsed(Q3ListViewItem *)));

    connect(this, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(slotCurrentCellChanged(int,int,int,int)));

// 	connect(header(), SIGNAL(sizeChange( int, int, int )), this, SLOT(slotColumnSizeChanged( int, int, int ))); // TODO
// 	connect(header(), SIGNAL(clicked( int )), this, SLOT(moveEditor()));
// 	connect(header(), SIGNAL(sectionHandleDoubleClicked ( int )), this, SLOT(slotColumnSizeChanged( int )));

	m_defaults = new KPushButton(viewport());
	m_defaults->setFocusPolicy(Qt::NoFocus);
	setFocusPolicy(Qt::ClickFocus);
	m_defaults->setIcon(KIcon("edit-undo"));
	m_defaults->setToolTip(i18n("Undo changes"));
	m_defaults->hide();
	connect(m_defaults, SIGNAL(clicked()), this, SLOT(resetItem()));

    // TODO
    const int itemMargin=2;
	//setRootIsDecorated( false );
	//setShowSortIndicator( false );
	// setTooltipColumn(0); // TODO equivalent?
	setSortingEnabled(false /*true*/); // note: enabling it causes crashes, apperently
    horizontalHeader()->setSortIndicatorShown(false);
    horizontalHeader()->setContentsMargins(itemMargin, itemMargin, itemMargin, itemMargin);
	//setItemMargin(2); // needed?
	horizontalHeader()->setResizeMode(QHeaderView::QHeaderView::Stretch);
    horizontalHeader()->setMovable(false);
	//header()->setMovingEnabled( false );
    verticalHeader()->setVisible(false);
	//setTreeStepSize(0);
    setSelectionMode(QAbstractItemView::SingleSelection);

	m_baseRowHeight = QFontMetrics(font()).height() + itemMargin*2;
}


PropertyEditor::~PropertyEditor()
{
    // note: delete m_colPropertyDelegate and m_colValueDelegate
}


void PropertyEditor::slotClicked(const QModelIndex& index)
{
	if (!index.isValid())
		return;
	
// 2019.01.19 - moved to slotCurrentCellChanged()
//     if (index.column() == 1) {
//         // PropertyEditorItem *i = static_cast<PropertyEditorItem *>(item);// 2018.08.13 - not needed
//         createEditor(index);
//     }

	justClickedItem = true;
}


void PropertyEditor::slotCurrentChanged(QTableWidgetItem* /*itemParam*/)
{
// TODO
// 	if (itemParam == firstChild())
// 	{
// 		Q3ListViewItem *oldItem = item;
// 		while (item && (!item->isSelectable() || !item->isVisible()))
// 			item = item->itemBelow();
//
// 		if (item && item!=oldItem)
// 		{
// 			setSelected(item,true);
// 			return;
// 		}
// 	}
}

void PropertyEditor::slotCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    viewport()->repaint(); // force a repaint to clear the "selected" background on items

    if (currentColumn == 0) {
        setCurrentCell(currentRow, 1); // move focus to the value column
    }
    if (currentColumn == 1) {
        createEditor(currentIndex());
    }
}

void PropertyEditor::slotExpanded(QTableWidgetItem* item)
{
	if (!item)
		return;
	moveEditor();
}


void PropertyEditor::slotCollapsed(QTableWidgetItem* item)
{
	if (!item)
		return;
	moveEditor();
}


void PropertyEditor::createEditor( const QModelIndex& index )
{
    PropertyEditorItem *i = dynamic_cast<PropertyEditorItem *>(item(index.row(), index.column()));
    if (!i) {
        qWarning() << Q_FUNC_INFO << "no item";
        return;
    }

// 	int y = viewportToContents(QPoint(0, itemRect(i).y())).y();
// 	QRect geometry(columnWidth(0), y, columnWidth(1), i->height());

	//delete m_currentEditor;
    //m_currentEditor->deleteLater();
    if (m_lastCellWidgetRow >= 0 && m_lastCellWidgetCol >= 0) {
        removeCellWidget(m_lastCellWidgetRow, m_lastCellWidgetCol);
        m_lastCellWidgetRow = -1;
        m_lastCellWidgetCol = -1;
    }
    m_currentEditor = 0;

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
            qDebug() << Q_FUNC_INFO << "creating PropertyEditorFile";
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
		//addChild(editor);
		//moveChild(editor, geometry.x(), geometry.y());
        m_lastCellWidgetRow = index.row();
        m_lastCellWidgetCol = index.column();
        setCellWidget(index.row(), index.column(), editor);
		editor->show();

		editor->setFocus();
	}
	
	m_currentEditor = editor;
	showDefaultsButton( i->property()->changed() );
}


void PropertyEditor::showDefaultsButton( bool show )
{
    QRect editItemRect = visualItemRect(m_editItem);
	int y = editItemRect.y(); // viewportToContents(QPoint(0, itemRect(m_editItem).y())).y(); // TODO
	QRect geometry( columnWidth(0), y, columnWidth(1), editItemRect.height() /* m_editItem->height() TOOD */ );
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

	QPoint p = geometry.topLeft() ; // = contentsToViewport(QPoint(0, geometry.y())); // TODO
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
	repaint(); //m_editItem->repaint();
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
	slotColumnSizeChanged(section, 0, horizontalHeader()->sectionSize(section));
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
	//if ( m_currentEditor )
	//	m_currentEditor->deleteLater();
    if (m_lastCellWidgetRow >= 0 && m_lastCellWidgetCol >= 0) {
        removeCellWidget(m_lastCellWidgetRow, m_lastCellWidgetCol);
        m_lastCellWidgetRow = -1;
        m_lastCellWidgetCol = -1;
    }
    m_currentEditor = 0;
	
	if ( m_defaults->isVisible() )
		m_defaults->hide();

	//clear();
    QTableWidget::reset();
	m_editItem = 0;
	m_topItem = 0;
}


QSize PropertyEditor::sizeHint() const
{
	return QSize( QFontMetrics(font()).width(
        horizontalHeaderItem(0)->text() +
        horizontalHeaderItem(1)->text() + "   "),
		QTableWidget::sizeHint().height());
}


void PropertyEditor::create( ItemGroup * b )
{
    qDebug() << Q_FUNC_INFO << "b=" << b;
	m_pItemGroup = b;
	
	//QCString selectedPropertyName1, selectedPropertyName2;
//     QByteArray selectedPropertyName1, selectedPropertyName2;     // 2018.08. 13 - dead code
	
	fill();
/* 2018.08. 13 - dead code
	//select prev. selecteed item
	PropertyEditorItem * item = 0;
	if (!selectedPropertyName2.isEmpty()) //try other one for old buffer
		item = m_items[selectedPropertyName2];
	if (!item && !selectedPropertyName1.isEmpty()) //try old one for current buffer
		item = m_items[selectedPropertyName1];
	if (item)
	{
		setItemSelected(item, true);
        scrollToItem(item);
	} else {
        qWarning() << Q_FUNC_INFO << "no item to select ";
    }
*/
    qDebug() << Q_FUNC_INFO << "column count= " << columnCount() << "rowCount=" << rowCount();
}


void PropertyEditor::fill()
{
	reset();
	
	if ( !m_pItemGroup || !m_pItemGroup->activeItem() ) {
        qWarning() << Q_FUNC_INFO << " no active item " << m_pItemGroup;
		return;
    }

	if(!m_topItem)
	{
		m_topItem = new PropertyEditorItem(this,"Top Item");
	}

	//m_items.clear();  // 2018.08.13 - unused
	setRowCount(0); // remove all items from the table

	VariantDataMap *vmap = m_pItemGroup->activeItem()->variantMap();
	// Build the list
	for( VariantDataMap::iterator vait = vmap->begin(); vait != vmap->end(); ++vait )
	{
		Variant * v = *vait;
		if ( v->isHidden() ) {
			continue;
        }
		qDebug() << Q_FUNC_INFO << "add variant id=" << v->id() << " v=" << v;

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
		
        const int nextRow = rowCount();
        setRowCount(nextRow + 1);
        {
            QTableWidgetItem *itemPropName = new PropertyEditorItem( m_topItem, v );
            itemPropName->setText(v->editorCaption());
            itemPropName->setFlags(Qt::ItemIsEnabled);
            setItem(nextRow, 0, itemPropName);
        }
        {
            PropertyEditorItem  *itemPropValue = new PropertyEditorItem( m_topItem, v );
            itemPropValue->setText( v->displayString() );
            connect( v, SIGNAL(valueChanged( QVariant, QVariant )), itemPropValue, SLOT(propertyValueChanged()) );
            itemPropValue->updateValue();
            setItem(nextRow, 1, itemPropValue);
        }
		//m_items.insert( v->id().latin1(), item ); // 2018.08.13 - unused
		
	}
}


void PropertyEditor::setFocus()
{
    selectedItems();
	PropertyEditorItem *item = static_cast<PropertyEditorItem *>(selectedItem());
	if (item) {
		if (!justClickedItem) {
            scrollToItem(item);
        }
		justClickedItem = false;
	}
	else
	{
		//select an item before focusing
		item = static_cast<PropertyEditorItem *>(itemAt(QPoint(10,1)));
		if (item)
		{
			scrollToItem(item); // ensureItemVisible(item);
            item->setSelected(true);    //setSelected(item, true);
		}
	}
	if (m_currentEditor) {
		m_currentEditor->setFocus();
    }
	else {
		QTableWidget::setFocus();
    }
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
	
	QPoint p = QPoint(0, visualItemRect(m_editItem).y()); //  = contentsToViewport(QPoint(0, itemPos(m_editItem))); // TODO
	m_currentEditor->move(m_currentEditor->x(), p.y());
	if( m_defaults->isVisible() ) {
		m_defaults->move(m_defaults->x(), p.y());
    }
}


void PropertyEditor::resizeEvent(QResizeEvent *ev)
{
	QTableWidget::resizeEvent(ev);
    updateDefaultsButton();
// 	if(m_defaults->isVisible())
// 	{
//         rect();
// 		QRect r = visualItemRect(m_editItem) ; // = itemRect(m_editItem); // TODO
// 		if(r.y()) { // r.y() == 0 if the item is not visible on the screen
// 			m_defaults->move(r.x() + r.width() - m_defaults->width(), r.y());
//         }
// 	}
//
// 	if ( m_currentEditor )
// 	{
// 		m_currentEditor->resize(
// 			columnWidth(1)-((m_currentEditor->leavesTheSpaceForRevertButton()||m_defaults->isVisible()) ? m_defaults->width() : 0),
// 			m_currentEditor->height());
// 	}
}


bool PropertyEditor::handleKeyPress( QKeyEvent* /*ev*/ )
{
#if 0 // TODO
    const int k = ev->key();
	const Qt::ButtonState s = ev->state();

	//selection moving
	QTableWidgetItem *item = 0;

	if ((s==Qt::NoButton && k==Qt::Key_Up) || k==Qt::Key_Backtab) {
		//find prev visible
		item = selectedItem() ? selectedItem()->itemAbove() : 0;
		while (item && (!item->flags().testFlag(Qt::ItemIsSelectable) || !item->isVisible()))
			item = item->itemAbove();
		if (!item)
			return true;
	}
	else if (s==Qt::NoButton && (k==Qt::Key_Down || k==Qt::Key_Tab)) {
		//find next visible
		item = selectedItem() ? selectedItem()->itemBelow() : 0;
		while (item && (!item->flags().testFlag(Qt::ItemIsSelectable) || !item->isVisible()))
			item = item->itemBelow();
		if (!item)
			return true;
	}
	else if(s==Qt::NoButton && k==Qt::Key_Home) {
		if (m_currentEditor && m_currentEditor->hasFocus())
			return false;
		//find 1st visible
		item = firstChild();
		while (item && (!item->flags().testFlag(Qt::ItemIsSelectable) || !item->isVisible()))
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
			if (item && item->flags().testFlag(Qt::ItemIsSelectable) && item->isVisible())
				lastVisible = item;
		}
		item = lastVisible;
	}
	if(item) {
		ev->accept();
        scrollToItem(item);
		item->setSelected(true); // setSelected(item, true);
		return true;
	}
#endif
	return false;
}

PropertyEditorItem *PropertyEditor::selectedItem() {
    QModelIndexList selList = selectedIndexes();
    if (selList.empty()) {
        return NULL;
    }
    if (selList.size() > 1) {
        qWarning() << Q_FUNC_INFO << " unexpected selection size of " << selList.size();
    }
    QModelIndex selIndex = selList.first();
    PropertyEditorItem *itemProp = dynamic_cast<PropertyEditorItem*>(item(selIndex.row(), selIndex.column()));
    if (!itemProp) {
        qWarning() << Q_FUNC_INFO << " failed to cast " << selIndex;
    }
    return itemProp;
}

#include "propertyeditor.moc"
