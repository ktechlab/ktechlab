/***************************************************************************
 *   Copyright (C) 2004-2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "itemdocument.h"
#include "itemdocumentdata.h"
#include "ktechlab.h"
#include "richtexteditor.h"

#include <cmath>
#include <kdebug.h>
#include <kdialog.h>
#include <ktextedit.h>
#include <kstandardguiitem.h>

#include <qbitarray.h>
#include <qlayout.h>
#include <qtimer.h>

#include <ktlconfig.h>

const int minPrefixExp = -24;
const int maxPrefixExp = 24;
const int numPrefix = int((maxPrefixExp-minPrefixExp)/3)+1;
const QString SIprefix[] = {"y","z","a","f","p","n",QChar(0xB5),"m","","k","M","G","T","P","E","Z","Y"};


Item::Item( ItemDocument *itemDocument, bool newItem, const QString &id )
	: //QObject(),
        KtlQCanvasPolygon( itemDocument ? itemDocument->canvas() : 0 )
{
    QString name(QString("Item-%1").arg(id));
    setObjectName(name.toLatin1().data());
    qDebug() << Q_FUNC_INFO << " this=" << this;

	m_bDynamicContent = false;
	m_bIsRaised = false;
	m_bDoneCreation = false;
	p_parentItem = 0l;
	b_deleted = false;
	p_itemDocument = itemDocument;
	m_baseZ = -1;
	
	if ( p_itemDocument )
	{
		if (newItem)
			m_id = p_itemDocument->generateUID(id);
		else {
			m_id = id;
			p_itemDocument->registerUID(id);
		}
	}
	
	m_pPropertyChangedTimer = new QTimer( this );
	connect( m_pPropertyChangedTimer, SIGNAL(timeout()), this, SLOT(dataChanged()) );
}


Item::~Item()
{
	if ( p_itemDocument )
	{
		p_itemDocument->requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems );
		p_itemDocument->unregisterUID( id() );
	}
	
	KtlQCanvasPolygon::hide();
	
	const VariantDataMap::iterator variantDataEnd = m_variantData.end();
	for ( VariantDataMap::iterator it = m_variantData.begin(); it != variantDataEnd; ++it )
		delete it.data();
	m_variantData.clear();
}


void Item::removeItem()
{
	if (b_deleted)
		return;
	b_deleted = true;
	
	hide();
	setCanvas(0l);
	emit removed(this);
	p_itemDocument->appendDeleteList(this);
}


QFont Item::font() const
{
	if ( KTechlab::self() )
		return KTechlab::self()->itemFont();
	else
		return QFont();
}


void Item::moveBy( double dx, double dy )
{
	KtlQCanvasPolygon::moveBy(dx,dy);
	emit movedBy( dx, dy );
}


void Item::setChanged()
{
	if (b_deleted)
		return;
	
	if (canvas())
		canvas()->setChanged(boundingRect());
}


void Item::setItemPoints( const QPolygon & pa, bool setSizeFromPoints )
{
	m_itemPoints = pa;
	if (setSizeFromPoints)
		setSize( m_itemPoints.boundingRect() );
	itemPointsChanged();
}


void Item::itemPointsChanged()
{
	setPoints(m_itemPoints);
}


void Item::setSize( QRect sizeRect, bool forceItemPoints )
{
	if ( !canvas() )
		return;
	
	if ( m_sizeRect == sizeRect && !forceItemPoints )
		return;
	
	if ( !preResize(sizeRect) )
		return;
	
	canvas()->setChanged(areaPoints().boundingRect());
	m_sizeRect = sizeRect;
	if ( m_itemPoints.isEmpty() || forceItemPoints )
	{
		setItemPoints( QPolygon( m_sizeRect ), false );
	}
	canvas()->setChanged(areaPoints().boundingRect());
	postResize();
	emit resized();
}


ItemData Item::itemData() const
{
	ItemData itemData;
	
	itemData.type = m_type;
	itemData.x = x();
	itemData.y = y();
	
	if ( !parentItem() )
		itemData.z = m_baseZ;
	
	itemData.size = m_sizeRect;
	itemData.setSize = canResize();
	
	if (p_parentItem)
		itemData.parentId = p_parentItem->id();
	
	const VariantDataMap::const_iterator end = m_variantData.end();
	for ( VariantDataMap::const_iterator it = m_variantData.begin(); it != end; ++it )
	{
		switch( it.data()->type() )
		{
			case Variant::Type::String:
			case Variant::Type::FileName:
			case Variant::Type::Port:
			case Variant::Type::Pin:
			case Variant::Type::VarName:
			case Variant::Type::Combo:
			case Variant::Type::Select:
			case Variant::Type::Multiline:
			case Variant::Type::RichText:
			case Variant::Type::SevenSegment:
			case Variant::Type::KeyPad:
			{
				itemData.dataString[it.key()] = it.data()->value().toString();
				break;
			}
			case Variant::Type::Int:
			case Variant::Type::Double:
			{
				itemData.dataNumber[it.key()] = it.data()->value().toDouble();
				break;
			}
			case Variant::Type::Color:
			{
				itemData.dataColor[it.key()] = it.data()->value().value<QColor>();
				break;
			}
			case Variant::Type::Bool:
			{
				itemData.dataBool[it.key()] = it.data()->value().toBool();
				break;
			}
			case Variant::Type::Raw:
			{
				itemData.dataRaw[it.key()] = it.data()->value().toBitArray();
				break;
			}
			case Variant::Type::PenStyle:
			case Variant::Type::PenCapStyle:
			{
				// These types are only created from DrawPart, and that class
				// deals with these, so we can ignore them
				break;
			}
			case Variant::Type::None:
			{
				// ? Maybe obsoleted data...
				break;
			}
		}
	}
	
	return itemData;
}


void Item::restoreFromItemData( const ItemData &itemData )
{
	move( itemData.x, itemData.y );
	if ( canResize() )
		setSize( itemData.size );
	
	Item *parentItem = p_itemDocument->itemWithID( itemData.parentId );
	if (parentItem)
		setParentItem(parentItem);
	else
		m_baseZ = itemData.z;
	
	//BEGIN Restore data
	const QStringMap::const_iterator stringEnd = itemData.dataString.end();
	for ( QStringMap::const_iterator it = itemData.dataString.begin(); it != stringEnd; ++it )
	{
		if ( hasProperty(it.key()) )
			property( it.key() )->setValue( it.data() );
	}
	
	const DoubleMap::const_iterator numberEnd = itemData.dataNumber.end();
	for ( DoubleMap::const_iterator it = itemData.dataNumber.begin(); it != numberEnd; ++it )
	{
		if ( hasProperty(it.key()) )
			property( it.key() )->setValue( it.data() );
	}
	
	const QColorMap::const_iterator colorEnd = itemData.dataColor.end();
	for ( QColorMap::const_iterator it = itemData.dataColor.begin(); it != colorEnd; ++it )
	{
		if ( hasProperty(it.key()) )
			property( it.key() )->setValue( it.data() );
	}
	
	const BoolMap::const_iterator boolEnd = itemData.dataBool.end();
	for ( BoolMap::const_iterator it = itemData.dataBool.begin(); it != boolEnd; ++it )
	{
		if ( hasProperty(it.key()) )
			property( it.key() )->setValue( QVariant( it.data(), 0 ) );
	}
	
	const QBitArrayMap::const_iterator rawEnd = itemData.dataRaw.end();
	for ( QBitArrayMap::const_iterator it = itemData.dataRaw.begin(); it != rawEnd; ++it )
	{
		if ( hasProperty(it.key()) )
			property( it.key() )->setValue( it.data() );
	}
	//END Restore Data
}


bool Item::mousePressEvent( const EventInfo &eventInfo )
{
	Q_UNUSED(eventInfo);
	return false;
}
bool Item::mouseReleaseEvent( const EventInfo &eventInfo )
{
	Q_UNUSED(eventInfo);
	return false;
}
bool Item::mouseMoveEvent( const EventInfo &eventInfo )
{
	Q_UNUSED(eventInfo);
	return false;
}
bool Item::wheelEvent( const EventInfo &eventInfo )
{
	Q_UNUSED(eventInfo);
	return false;
}
void Item::enterEvent(QEvent *)
{
}
void Item::leaveEvent(QEvent *)
{
}

bool Item::mouseDoubleClickEvent( const EventInfo & eventInfo )
{
	Q_UNUSED(eventInfo);
	
	Property * property = 0l;
	Variant::Type::Value type = Variant::Type::None;
	
	const VariantDataMap::iterator variantDataEnd = m_variantData.end();
	for ( VariantDataMap::iterator it = m_variantData.begin(); it != variantDataEnd; ++it )
	{
		Property * current = *it;
		
		if ( current->type() == Variant::Type::Multiline ||
				   current->type() == Variant::Type::RichText )
		{
			property = current;
			type = current->type();
			break;
		}
	}
	if ( !property )
		return false;
	
	if ( type == Variant::Type::Multiline )
	{
		//KDialog * dlg = new KDialog( 0l, "", true, property->editorCaption(), KDialog::Ok|KDialog::Cancel|KDialog::User1, KDialog::Ok,
        //                             false, KStandardGuiItem::clear() );
        KDialog * dlg = new KDialog( 0 );
        dlg->setModal(true);
        dlg->setCaption( property->editorCaption() );
        dlg->setButtons(KDialog::Ok|KDialog::Cancel|KDialog::User1);
        dlg->setDefaultButton(KDialog::Ok);
        dlg->showButtonSeparator(false);
        dlg->setButtonText(KDialog::User1, KStandardGuiItem::clear().text());
        //QFrame *frame = dlg->makeMainWidget();
		QFrame *frame = new QFrame(dlg);
        dlg->setMainWidget(frame);
		QVBoxLayout *layout = new QVBoxLayout( frame, 0, dlg->spacingHint() );
		KTextEdit *textEdit = new KTextEdit( frame );
		textEdit->setTextFormat( Qt::PlainText );
		textEdit->setText( property->value().toString() );
		layout->addWidget( textEdit, 10 );
		textEdit->setFocus();
		connect( dlg, SIGNAL( user1Clicked() ), textEdit, SLOT( clear() ) );
		dlg->setMinimumWidth( 600 );
		
		if ( dlg->exec() == KDialog::Accepted )
		{
			property->setValue( textEdit->text() );
			dataChanged();
			p_itemDocument->setModified(true);
		}
		delete dlg;
	}
	else
	{
		// Is rich text
		RichTextEditorDlg * dlg = new RichTextEditorDlg( 0l, property->editorCaption() );
		dlg->setText( property->value().toString() );
		
		if ( dlg->exec() == KDialog::Accepted )
		{
			property->setValue( dlg->text() );
			dataChanged();
			p_itemDocument->setModified(true);
		}
		delete dlg;
	}
	
	return true;
}


void Item::setSelected( bool yes )
{
	if ( isSelected() == yes )
		return;
	KtlQCanvasPolygon::setSelected(yes);
	emit selectionChanged();
}


void Item::setParentItem( Item *newParentItem )
{
// 	kDebug() << k_funcinfo << "this = "<<this<<" newParentItem = "<<newParentItem<<endl;
	if ( newParentItem == p_parentItem )
		return;
	
	Item *oldParentItem = p_parentItem;
	
	if (oldParentItem)
	{
		disconnect( oldParentItem, SIGNAL(removed(Item*)), this, SLOT(removeItem()) );
		oldParentItem->removeChild(this);
	}
	
	if (newParentItem)
	{
		if ( newParentItem->contains(this) );
// 			kError() << k_funcinfo << "Already a child of " << newParentItem << endl;
		else
		{
			connect( newParentItem, SIGNAL(removed(Item*)), this, SLOT(removeItem()) );
			newParentItem->addChild(this);
		}
	}
	
	p_parentItem = newParentItem;
	(void)level();
	reparented( oldParentItem, newParentItem );
	p_itemDocument->slotUpdateZOrdering();
}


int Item::level() const
{
	return p_parentItem ? p_parentItem->level()+1 : 0;
}


ItemList Item::children( bool includeGrandChildren ) const
{
	if (!includeGrandChildren)
		return m_children;
	
	ItemList children = m_children;
	ItemList::const_iterator end = m_children.end();
	for ( ItemList::const_iterator it = m_children.begin(); it != end; ++it )
	{
		if (!*it)
			continue;
		
		children += (*it)->children(true);
	}
	
	return children;
}


void Item::addChild( Item *child )
{
	if ( !child )
		return;
	
	if ( child->contains(this) )
	{
// 		kError() << k_funcinfo << "Attempting to add a child to this item that is already a parent of this item. Incest results in stack overflow." << endl;
		return;
	}
	
	if ( contains( child, true ) )
	{
// 		kError() << k_funcinfo << "Already have child " << child << endl;
		return;
	}
	
	m_children.append(child);
	connect( child, SIGNAL(removed(Item* )), this, SLOT(removeChild(Item* )) );
	
	child->setParentItem(this);
	childAdded(child);
	p_itemDocument->slotUpdateZOrdering();
}


void Item::removeChild( Item *child )
{
	if ( !child || !m_children.contains(child) )
		return;
	
	m_children.remove(child);
	disconnect( child, SIGNAL(removed(Item* )), this, SLOT(removeChild(Item* )) );
	
	childRemoved(child);
	p_itemDocument->slotUpdateZOrdering();
}


bool Item::contains( Item *item, bool direct ) const
{
	const ItemList::const_iterator end = m_children.end();
	for ( ItemList::const_iterator it = m_children.begin(); it != end; ++it )
	{
		if ( (Item*)*it == item || ( !direct && (*it)->contains( item, false ) ) )
			return true;
	}
	return false;
}


void Item::setRaised( bool isRaised )
{
	m_bIsRaised = isRaised;
	// We'll get called later to update our Z
}


void Item::updateZ( int baseZ )
{
	m_baseZ = baseZ;
	double z = ItemDocument::Z::Item + (ItemDocument::Z::DeltaItem)*baseZ;
	
	if ( isRaised() )
		z += ItemDocument::Z::RaisedItem - ItemDocument::Z::Item;
	
	setZ(z);
	
	const ItemList::const_iterator end = m_children.end();
	for ( ItemList::const_iterator it = m_children.begin(); it != end; ++it )
	{
		if (*it)
			(*it)->updateZ(baseZ+1);
	}
}


int Item::getNumberPre( double num )
{
	return (int)(num/getMultiplier(num));
}

QString Item::getNumberMag( double num )
{
	if ( num == 0. ) return "";
	const double exp_n = std::log10(std::abs(num));
	if ( exp_n < minPrefixExp+3 ) return SIprefix[0];
	else if ( exp_n >= maxPrefixExp ) return SIprefix[numPrefix-1];
	else return SIprefix[(int)std::floor((double)(exp_n/3))-(int)floor(double(minPrefixExp/3))];
}

double Item::getMultiplier( double num )
{
	if ( num == 0. ) return 1.;
	else return std::pow( 10, 3*std::floor(std::log10(std::abs(num))/3) );
}

double Item::getMultiplier( const QString &_mag )
{
	QString mag;
	// Allow the user to enter in "u" instead of mu, as unfortunately many keyboards don't have the mu key
	if ( _mag == "u" )
		mag = QChar(0xB5);
	else
		mag = _mag;
	
	for ( int i=0; i<numPrefix; ++i )
	{
		if ( mag == SIprefix[i] )
		{
			return std::pow( 10., (i*3)+minPrefixExp );
		}
	}
	
	// I think it is safer to return '1' if the unit is unknown
	return 1.;
// 	return pow( 10., maxPrefixExp+3. );
}



//BEGIN Data stuff
double Item::dataDouble( const QString & id ) const
{
	Variant * variant = property(id);
	return variant ? variant->value().toDouble() : 0.0;
}


int Item::dataInt( const QString & id ) const
{
	Variant * variant = property(id);
	return variant ? variant->value().toInt() : 0;
}


bool Item::dataBool( const QString & id ) const
{
	Variant * variant = property(id);
	return variant ? variant->value().toBool() : false;
}


QString Item::dataString( const QString & id ) const
{
	Variant * variant = property(id);
	return variant ? variant->value().toString() : QString::null;
}


QColor Item::dataColor( const QString & id ) const
{
	Variant * variant = property(id);
	return variant ? variant->value().value<QColor>() : Qt::black;
}


Variant * Item::createProperty( const QString & id, Variant::Type::Value type )
{
	if ( !m_variantData.contains(id) )
	{
		m_variantData[id] = new Variant( id, type );
		connect( m_variantData[id], SIGNAL(valueChanged(QVariant,QVariant)), this, SLOT(propertyChangedInitial()) );
	}
	
	return m_variantData[id];
}


Variant * Item::property( const QString & id ) const
{
	if ( m_variantData.contains(id) )
		return m_variantData[id];
	
	kError() << k_funcinfo << " No such property with id " << id << endl;
	return 0l;
}


bool Item::hasProperty( const QString & id ) const
{
	return m_variantData.contains(id);
}


void Item::finishedCreation( )
{
	m_bDoneCreation = true;
	dataChanged();
}


void Item::propertyChangedInitial()
{
	if ( !m_bDoneCreation )
		return;
	
	m_pPropertyChangedTimer->start( 0, true );
}
//END Data stuff

#include "item.moc"
