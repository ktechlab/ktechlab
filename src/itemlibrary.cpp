/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "config.h"

#include "cnitem.h"
#include "canvasitemparts.h"
#include "circuitdocument.h"
#include "component.h"
#include "ecsubcircuit.h"
#include "ecnode.h"
#include "itemlibrary.h"
#include "node.h"
#include "subcircuits.h"

#ifdef MECHANICS
#include "chassiscircular2.h"
#endif

#include "dptext.h"
#include "dpline.h"
#include "solidshape.h"

#include "callsub.h"
#include "count.h"
#include "delay.h"
#include "embed.h"
#include "end.h"
#include "inputbutton.h"
#include "interrupt.h"
#include "forloop.h"
#include "keypad.h"
#include "pulse.h"
#include "readport.h"
#include "repeat.h"
#include "setpin.h"
#include "sevenseg.h"
#include "start.h"
#include "sub.h"
#include "testpin.h"
#include "unary.h"
#include "varassignment.h"
#include "varcomparison.h"
#include "while.h"
#include "writeport.h"

#include "addac.h"
#include "bidirled.h"
#include "binarycounter.h"
#include "bussplitter.h"
#include "demultiplexer.h"
#include "dependentsource.h"
#include "discretelogic.h"
#include "externalconnection.h"
#include "flipflop.h"
#include "fulladder.h"
#include "inductor.h"
#include "magnitudecomparator.h"
#include "matrixdisplay.h"
#include "matrixdisplaydriver.h"
#include "meter.h"
#include "multiinputgate.h"
#include "multiplexer.h"
#include "parallelportcomponent.h"
#include "piccomponent.h"
#include "pushswitch.h"
#include "probe.h"
#include "ram.h"
#include "resistordip.h"
#include "rotoswitch.h"
#include "serialportcomponent.h"
#include "toggleswitch.h"

#include "ec555.h"
#include "ecbcdto7segment.h"
#include "eccapacitor.h"
#include "ecclockinput.h"
#include "eccurrentsignal.h"
#include "eccurrentsource.h"
#include "ecdiode.h"
#include "ecfixedvoltage.h"
#include "ecground.h"
#include "eckeypad.h"
#include "ecled.h"
#include "ecbjt.h"
#include "ecopamp.h"
#include "ecpotentiometer.h"
#include "ecresistor.h"
#include "ecsevensegment.h"
#include "ecsignallamp.h"
#include "ecvoltagesignal.h"
#include "ecvoltagesource.h"

#include "pinmapping.h"

#include "libraryitem.h"

#include "kdebug.h"
#include <kiconloader.h>
#include <qbitmap.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qregexp.h>

#include <cassert>

ItemLibrary::ItemLibrary()
{
	addFlowParts();
	addComponents();
	addMechanics();
	addDrawParts();
}


ItemLibrary::~ItemLibrary()
{
	const LibraryItemList::iterator end = m_items.end();
	for(LibraryItemList::iterator it = m_items.begin(); it != end; ++it) {
		delete *it;
	}
	m_items.clear();
}


void ItemLibrary::addFlowParts()
{
	// Container loops
	addLibraryItem( Repeat::libraryItem());
	addLibraryItem( While::libraryItem());
	addLibraryItem( ForLoop::libraryItem());
	
	// Variable operations
	addLibraryItem( Unary::libraryItem());
	addLibraryItem( VarAssignment::libraryItem());
	addLibraryItem( VarComparison::libraryItem());
	
	// I/O
	addLibraryItem( SetPin::libraryItem());
	addLibraryItem( TestPin::libraryItem());
	addLibraryItem( WritePort::libraryItem());
	addLibraryItem( ReadPort::libraryItem());
	
	// Functions
	addLibraryItem( SevenSeg::libraryItem());
// 	addLibraryItem( Pulse::libraryItem());
	addLibraryItem( Keypad::libraryItem());
// 	addLibraryItem( Count::libraryItem());
// 	addLibraryItem( InputButton::libraryItem());
	addLibraryItem( Delay::libraryItem());
	
	// Common
	addLibraryItem( Embed::libraryItem());
	addLibraryItem( CallSub::libraryItem());
//  	addLibraryItem( Interrupt::libraryItem());
	addLibraryItem( Sub::libraryItem());
	addLibraryItem( End::libraryItem());
	addLibraryItem( Start::libraryItem());
}


void ItemLibrary::addComponents()
{
	// Integrated Circuits
	addLibraryItem( ECBCDTo7Segment::libraryItem());
	addLibraryItem( MatrixDisplayDriver::libraryItem());
	addLibraryItem( BinaryCounter::libraryItem());
	addLibraryItem( DAC::libraryItem());
	addLibraryItem( ADC::libraryItem());
	addLibraryItem( ECOpAmp::libraryItem());
	addLibraryItem( MagnitudeComparator::libraryItem());
	addLibraryItem( Demultiplexer::libraryItem());
	addLibraryItem( Multiplexer::libraryItem());
	addLibraryItem( FullAdder::libraryItem());
	addLibraryItem( RAM::libraryItem());
	addLibraryItem( EC555::libraryItem());
	addLibraryItem( ECDFlipFlop::libraryItem());
	addLibraryItem( ECSRFlipFlop::libraryItem());
	addLibraryItem( ECJKFlipFlop::libraryItem());
#ifndef NO_GPSIM
	addLibraryItem( PICComponent::libraryItem());
#endif
	
	// Connections
	addLibraryItem( ParallelPortComponent::libraryItem());
	addLibraryItem( SerialPortComponent::libraryItem());
	addLibraryItem( ExternalConnection::libraryItem());
	addLibraryItem( BusSplitter::libraryItem());
	
	// Logic
	addLibraryItem( ECXnor::libraryItem());
	addLibraryItem( ECXor::libraryItem());
	addLibraryItem( ECNor::libraryItem());
	addLibraryItem( ECOr::libraryItem());
	addLibraryItem( ECNand::libraryItem());
	addLibraryItem( ECAnd::libraryItem());
	addLibraryItem( Inverter::libraryItem());
	addLibraryItem( Buffer::libraryItem());
	addLibraryItem( ECClockInput::libraryItem());
	addLibraryItem( ECLogicOutput::libraryItem());
	addLibraryItem( ECLogicInput::libraryItem());
	
	
	// Outputs
// 	addLibraryItem( FrequencyMeter::libraryItem());
	addLibraryItem( CurrentProbe::libraryItem());
	addLibraryItem( VoltageProbe::libraryItem());
	addLibraryItem( LogicProbe::libraryItem());
	addLibraryItem( ECAmmeter::libraryItem());
	addLibraryItem( ECVoltMeter::libraryItem());
	addLibraryItem( MatrixDisplay::libraryItem());
	addLibraryItem( ECSevenSegment::libraryItem());
	addLibraryItem( BiDirLED::libraryItem());
	addLibraryItem( ECSignalLamp::libraryItem());
	addLibraryItem( ECLed::libraryItem());
	
	// Switches
	addLibraryItem( ECRotoSwitch::libraryItem());
	addLibraryItem( ECDPDT::libraryItem());
	addLibraryItem( ECSPDT::libraryItem());
	addLibraryItem( ECDPST::libraryItem());
	addLibraryItem( ECSPST::libraryItem());
	addLibraryItem( ECKeyPad::libraryItem());
	addLibraryItem( ECPTBSwitch::libraryItem());
	addLibraryItem( ECPTMSwitch::libraryItem());
	
	
	// Discrete
	addLibraryItem( ECPotentiometer::libraryItem());
	addLibraryItem( ResistorDIP::libraryItem());
	addLibraryItem( ECBJT::libraryItemPNP());
	addLibraryItem( ECBJT::libraryItemNPN());
	addLibraryItem( Inductor::libraryItem());
	addLibraryItem( ECDiode::libraryItem());
	addLibraryItem( ECCapacitor::libraryItem());
	addLibraryItem( ECResistor::libraryItem());
	
	// Dependent Sources
	addLibraryItem( ECVCVS::libraryItem());
	addLibraryItem( ECVCCS::libraryItem());
	addLibraryItem( ECCCVS::libraryItem());
	addLibraryItem( ECCCCS::libraryItem());
	
	// Independent Sources
	addLibraryItem( ECCurrentSignal::libraryItem());
	addLibraryItem( ECVoltageSignal::libraryItem());
	addLibraryItem( ECCurrentSource::libraryItem());
	addLibraryItem( ECGround::libraryItem());
	addLibraryItem( ECFixedVoltage::libraryItem());
	addLibraryItem( ECCell::libraryItem());
	
	// Other
	addLibraryItem( ECSubcircuit::libraryItem());
	addLibraryItem( PIC_IC::libraryItem());
}


void ItemLibrary::addDrawParts()
{
	addLibraryItem( DPText::libraryItem());
	addLibraryItem( DPLine::libraryItem());
	addLibraryItem( DPArrow::libraryItem());
	addLibraryItem( DPRectangle::libraryItem());
	addLibraryItem( DPEllipse::libraryItem());
}


void ItemLibrary::addMechanics()
{
#ifdef MECHANICS
	addLibraryItem( ChassisCircular2::libraryItem());
#endif
}


void ItemLibrary::addLibraryItem( LibraryItem *item)
{
	m_items.append(item);
}


Item *ItemLibrary::createItem( const QString &id, ItemDocument *itemDocument, bool newItem, const char *newId, bool finishCreation)
{
	Item * item = 0;
	if( id.startsWith("sc/"))
	{
		// Is a subcircuit...
		
		CircuitDocument * circuitDocument = dynamic_cast<CircuitDocument*>(itemDocument);
		if(!circuitDocument)
		{
			kdWarning() << "Cannot create subcircuit without a circuit document" << endl;
			return 0;
		}
		
		QString temp = id;
		int numId = temp.remove("sc/").toInt();
		
		item = subcircuits()->createSubcircuit( numId, /*id,*/ circuitDocument, newItem, newId);
	} else {
		const LibraryItemList::iterator end = m_items.end();
		LibraryItemList::iterator it = m_items.begin();
		for( ; it != end; ++it)
		{
			if( (*it)->allIDs().contains(id))
			{
				item = (*it)->createItemFnPtr()( itemDocument, newItem, newId);
				item->m_type = (*it)->activeID();
				break;
			}
		}
		
		if( it == end)
			kdWarning() << "Could not find the item constructor for id " << id << endl;
	}
	
	if( finishCreation && item)
		item->finishedCreation();
	
	return item;
}


QImage ItemLibrary::itemImage( Item *item, const uint maxSize)
{
	Component *component = dynamic_cast<Component*>(item);
	
    QRect bound = item->boundingRect().normalize();
	bound.setLeft( bound.left()-8);
	bound.setRight( bound.right()+8);
	bound.setTop( bound.top()-8);
	bound.setBottom( bound.bottom()+8);

	// We want a nice square bounding rect
	const int dy = bound.width() - bound.height();
	if( dy > 0)
	{
		bound.setTop( bound.top()-(dy/2));
		bound.setBottom( bound.bottom()+(dy/2));
	}
	else if( dy < 0)
	{
		bound.setLeft( bound.left()+(dy/2));
		bound.setRight( bound.right()-(dy/2));
	}
	
	const bool cache = ((bound.width()*bound.height()) > (int)maxSize);
	QString type;
	if( cache && m_imageMap.contains(item->type()))
		return m_imageMap[item->type()];
	
	// Create pixmap big enough to contain CNItem and surrounding nodes
	// and copy the button grab to it
	
	QPixmap pm( bound.size());
	
	QBitmap mask( bound.size());
	mask.fill( Qt::color0);
	
	QPainter maskPainter(&mask);
	maskPainter.translate( -bound.x(), -bound.y());
	maskPainter.setPen( Qt::color1);
	maskPainter.setBrush( Qt::color1);
	
	
	QPainter p(&pm);
	p.translate( -bound.x(), -bound.y());
	p.setPen( item->pen());
	p.setBrush( item->brush());
	
	// Now draw the shape :-)
	const bool sel = item->isSelected();
	if(sel)
	{
		// We block the signals as we end up in an infinite loop with cnitem emitting a selected signal
		item->blockSignals(true);
		item->setSelected(false);
		item->blockSignals(false);
	}
	item->drawShape(p);
	item->drawShape(maskPainter);
	if(sel)
	{
		item->blockSignals(true);
		item->setSelected(sel);
		item->blockSignals(false);
	}
	
	maskPainter.setPen( Qt::color1);
	maskPainter.setBrush( Qt::color1);
	
	QWMatrix transMatrix; // Matrix to apply to the image
	
	CNItem *cnItem = dynamic_cast<CNItem*>(item);
	if(cnItem)
	{
		NodeMap nodes = cnItem->nodeMap();
		const NodeMap::iterator nodesEnd = nodes.end();
		for( NodeMap::iterator it = nodes.begin(); it != nodesEnd; ++it)
		{
			Node *node = it.data().node;
			const bool sel = node->isSelected();
			if(sel)
				node->setSelected(false);
			if( ECNode *ecnode = dynamic_cast<ECNode*>(node))
			{
				const bool showVB = ecnode->showVoltageBars();
				ecnode->setShowVoltageBars(false);
				ecnode->drawShape(p);
				ecnode->drawShape(maskPainter);
				ecnode->setShowVoltageBars(showVB);
			}
			else
			{
				node->drawShape(p);
				node->drawShape(maskPainter);
			}
			if(sel)
				node->setSelected(sel);
		}
		
		p.setPen(Qt::black);
		TextMap text = cnItem->textMap();
		const TextMap::iterator textEnd = text.end();
		for( TextMap::iterator it = text.begin(); it != textEnd; ++it)
		{
			it.data()->drawShape(p);
			it.data()->drawShape(maskPainter);
		}
		
// 		maskPainter.setPen( Qt::color1);
// 		maskPainter.setBrush( Qt::color1);
		cnItem->drawWidgets(p);
// 		cnItem->drawWidgets(maskPainter);
		
		transMatrix = Component::transMatrix( component->angleDegrees(), component->flipped(), bound.width()/2, bound.height()/2, true);
	}
	
	pm.setMask(mask);
		
	// Now, rotate the image so that it's the right way up, and scale it to size
	QImage im = pm.convertToImage();
	im = im.xForm(transMatrix);
	im = im.smoothScale( 50, 50, QImage::ScaleMin);
	
	if(cache)
		m_imageMap[item->type()] = im;
	
	return im;
}

QPixmap ItemLibrary::itemIconFull( const QString &id)
{
	LibraryItemList::iterator end = m_items.end();
	for( LibraryItemList::iterator it = m_items.begin(); it != end; ++it)
	{
		if( *it && (*it)->allIDs().contains(id))
		{
			return (*it)->iconFull();
		}
	}
	return QPixmap();
}
