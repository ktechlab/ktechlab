/***************************************************************************
 *   Copyright (C) 2003-2006 David Saxton <david@bluehaze.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "config.h"

#include "cnitem.h"
#include "canvasitemparts.h"
#include "electronics/circuitdocument.h"
#include "component.h"
#include "ecsubcircuit.h"
#include "ecnode.h"
#include "itemlibrary.h"
#include "libraryitem.h"
#include "node.h"
#include "pinmapping.h"
#include "subcircuits.h"

#include <qapplication.h>
#include <kconfig.h>
#include <KConfigGroup>
#include <KSharedConfig>
#include <qlocale.h>
#include <klocalizedstring.h>
#include <kmessagebox.h>

#include <qbitmap.h>
#include "qdebug.h"
#include <qdir.h>
#include <qfile.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qtimer.h>
#include <QStandardPaths>

#include <cassert>


//BEGIN Item includes
#ifdef MECHANICS
#include "chassiscircular2.h"
#endif

#include "dpimage.h"
#include "dpline.h"
#include "dptext.h"
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
#include "ecbjt.h"
#include "capacitor.h"
#include "ecclockinput.h"
#include "eccurrentsignal.h"
#include "eccurrentsource.h"
#include "ecdiode.h"
#include "ecfixedvoltage.h"
#include "ecground.h"
#include "ecjfet.h"
#include "eckeypad.h"
#include "led.h"
#include "ledbargraphdisplay.h"
#include "ecmosfet.h"
#include "ecopamp.h"
#include "ecpotentiometer.h"
#include "resistor.h"
#include "ecsevensegment.h"
#include "ecsignallamp.h"
#include "variablecapacitor.h"
#include "variableresistor.h"
#include "ecvoltagesignal.h"
#include "ecvoltagesource.h"


//END Item includes


KLocalizedString ItemLibrary::m_emptyItemDescription = ki18n("This help item does not yet exist for the %1 language. Help out with KTechlab by creating one via the \"Edit\" button.");


ItemLibrary::ItemLibrary()
{
	addFlowParts();
	addComponents();
	addMechanics();
	addDrawParts();

	loadItemDescriptions();
}


ItemLibrary::~ItemLibrary()
{
// 	qDebug() << "m_itemDescriptions[\"en_US\"].size()="<<m_itemDescriptions["en_US"].size()<<endl;

	const LibraryItemList::iterator end = m_items.end();
	for ( LibraryItemList::iterator it = m_items.begin(); it != end; ++it )
	{
		delete *it;
	}
	m_items.clear();
}


void ItemLibrary::addFlowParts()
{
	// Container loops
	addLibraryItem( Repeat::libraryItem() );
	addLibraryItem( While::libraryItem() );
	addLibraryItem( ForLoop::libraryItem() );

	// Variable operations
	addLibraryItem( Unary::libraryItem() );
	addLibraryItem( VarAssignment::libraryItem() );
	addLibraryItem( VarComparison::libraryItem() );

	// I/O
	addLibraryItem( SetPin::libraryItem() );
	addLibraryItem( TestPin::libraryItem() );
	addLibraryItem( WritePort::libraryItem() );
	addLibraryItem( ReadPort::libraryItem() );

	// Functions
	addLibraryItem( SevenSeg::libraryItem() );
// 	addLibraryItem( Pulse::libraryItem() );
	addLibraryItem( Keypad::libraryItem() );
// 	addLibraryItem( Count::libraryItem() );
// 	addLibraryItem( InputButton::libraryItem() );
	addLibraryItem( Delay::libraryItem() );

	// Common
	addLibraryItem( Embed::libraryItem() );
	addLibraryItem( CallSub::libraryItem() );
//  	addLibraryItem( Interrupt::libraryItem() );
	addLibraryItem( Sub::libraryItem() );
	addLibraryItem( End::libraryItem() );
	addLibraryItem( Start::libraryItem() );
}


void ItemLibrary::addComponents()
{
	// Integrated Circuits
	addLibraryItem( ECBCDTo7Segment::libraryItem() );
	addLibraryItem( MatrixDisplayDriver::libraryItem() );
	addLibraryItem( BinaryCounter::libraryItem() );
	addLibraryItem( DAC::libraryItem() );
	addLibraryItem( ADC::libraryItem() );
	addLibraryItem( ECOpAmp::libraryItem() );
	addLibraryItem( MagnitudeComparator::libraryItem() );
	addLibraryItem( Demultiplexer::libraryItem() );
	addLibraryItem( Multiplexer::libraryItem() );
	addLibraryItem( FullAdder::libraryItem() );
	addLibraryItem( RAM::libraryItem() );
	addLibraryItem( EC555::libraryItem() );
	addLibraryItem( ECDFlipFlop::libraryItem() );
	addLibraryItem( ECSRFlipFlop::libraryItem() );
	addLibraryItem( ECJKFlipFlop::libraryItem() );
#ifndef NO_GPSIM
	addLibraryItem( PICComponent::libraryItem() );
#endif

	// Connections
	addLibraryItem( ParallelPortComponent::libraryItem() );
	addLibraryItem( SerialPortComponent::libraryItem() );
	addLibraryItem( ExternalConnection::libraryItem() );
	addLibraryItem( BusSplitter::libraryItem() );

	// Logic
	addLibraryItem( ECXnor::libraryItem() );
	addLibraryItem( ECXor::libraryItem() );
	addLibraryItem( ECNor::libraryItem() );
	addLibraryItem( ECOr::libraryItem() );
	addLibraryItem( ECNand::libraryItem() );
	addLibraryItem( ECAnd::libraryItem() );
	addLibraryItem( Inverter::libraryItem() );
	addLibraryItem( Buffer::libraryItem() );
	addLibraryItem( ECClockInput::libraryItem() );
	addLibraryItem( ECLogicOutput::libraryItem() );
	addLibraryItem( ECLogicInput::libraryItem() );

	// Outputs
// 	addLibraryItem( FrequencyMeter::libraryItem() );
	addLibraryItem( CurrentProbe::libraryItem() );
	addLibraryItem( VoltageProbe::libraryItem() );
	addLibraryItem( LogicProbe::libraryItem() );
	addLibraryItem( ECAmmeter::libraryItem() );
	addLibraryItem( ECVoltMeter::libraryItem() );
	addLibraryItem( LEDBarGraphDisplay::libraryItem() );
	addLibraryItem( MatrixDisplay::libraryItem() );
	addLibraryItem( ECSevenSegment::libraryItem() );
	addLibraryItem( BiDirLED::libraryItem() );
	addLibraryItem( ECSignalLamp::libraryItem() );
	addLibraryItem( LED::libraryItem() );

	// Switches
	addLibraryItem( ECRotoSwitch::libraryItem() );
	addLibraryItem( ECDPDT::libraryItem() );
	addLibraryItem( ECSPDT::libraryItem() );
	addLibraryItem( ECDPST::libraryItem() );
	addLibraryItem( ECSPST::libraryItem() );
	addLibraryItem( ECKeyPad::libraryItem() );
	addLibraryItem( ECPTBSwitch::libraryItem() );
	addLibraryItem( ECPTMSwitch::libraryItem() );

	// Nonlinear
// 	addLibraryItem( ECMOSFET::libraryItemPDM() );
// 	addLibraryItem( ECMOSFET::libraryItemNDM() );
	addLibraryItem( ECMOSFET::libraryItemPEM() );
	addLibraryItem( ECMOSFET::libraryItemNEM() );
	addLibraryItem( ECJFET::libraryItemPJFET() );
	addLibraryItem( ECJFET::libraryItemNJFET() );
	addLibraryItem( ECBJT::libraryItemPNP() );
	addLibraryItem( ECBJT::libraryItemNPN() );
	addLibraryItem( ECDiode::libraryItem() );

	// Discrete
//	addLibraryItem( VoltageRegulator::libraryItem() );
	addLibraryItem( VariableResistor::libraryItem() );
	addLibraryItem( VariableCapacitor::libraryItem() );
	addLibraryItem( ECPotentiometer::libraryItem() );
	addLibraryItem( ResistorDIP::libraryItem() );
	addLibraryItem( Inductor::libraryItem() );
	addLibraryItem( Capacitor::libraryItem() );
	addLibraryItem( Resistor::libraryItem() );

	// Dependent Sources
	addLibraryItem( ECVCVS::libraryItem() );
	addLibraryItem( ECVCCS::libraryItem() );
	addLibraryItem( ECCCVS::libraryItem() );
	addLibraryItem( ECCCCS::libraryItem() );

	// Independent Sources
	addLibraryItem( ECCurrentSignal::libraryItem() );
	addLibraryItem( ECVoltageSignal::libraryItem() );
	addLibraryItem( ECCurrentSource::libraryItem() );
	addLibraryItem( ECGround::libraryItem() );
	addLibraryItem( ECFixedVoltage::libraryItem() );
	addLibraryItem( ECCell::libraryItem() );

	// Other
	addLibraryItem( ECSubcircuit::libraryItem() );
	addLibraryItem( PIC_IC::libraryItem() );
}


void ItemLibrary::addDrawParts()
{
	addLibraryItem( DPImage::libraryItem() );
	addLibraryItem( DPText::libraryItem() );
	addLibraryItem( DPLine::libraryItem() );
	addLibraryItem( DPArrow::libraryItem() );
	addLibraryItem( DPRectangle::libraryItem() );
	addLibraryItem( DPEllipse::libraryItem() );
}


void ItemLibrary::addMechanics()
{
#ifdef MECHANICS
	addLibraryItem( ChassisCircular2::libraryItem() );
#endif
}


void ItemLibrary::addLibraryItem( LibraryItem *item )
{
	m_items.prepend(item);
}


LibraryItem * ItemLibrary::libraryItem( QString type ) const
{
	if ( type.startsWith(QString::fromLatin1("/")) )
	{
		// Possibly change e.g. "/ec/capacitor" to "ec/capacitor"
		type.remove( 0, 1 );
	}

	LibraryItemList::const_iterator end = m_items.end();
	LibraryItemList::const_iterator it = m_items.begin();
	for ( ; it != end; ++it )
	{
		if ( (*it)->allIDs().contains( type ) )
			return *it;
	}
	return nullptr;
}


Item * ItemLibrary::createItem( const QString &id, ItemDocument *itemDocument, bool newItem, const char *newId, bool finishCreation  )
{
	Item *item = nullptr;
	if ( id.startsWith(QString::fromLatin1("sc/")) )
	{
		// Is a subcircuit...

		CircuitDocument *circuitDocument = dynamic_cast<CircuitDocument*>(itemDocument);
		if (!circuitDocument)
		{
			qWarning() << "Cannot create subcircuit without a circuit document" << endl;
			return nullptr;
		}

		QString temp = id;
		int numId = temp.remove(QString::fromLatin1("sc/")).toInt();

		item = subcircuits()->createSubcircuit( numId,  circuitDocument, newItem, newId );
	} else {
		LibraryItem * li = libraryItem( id );

		if ( !li )
			qWarning() << "Could not find the item constructor for id " << id << endl;
		else {
			item = li->createItemFnPtr()( itemDocument, newItem, newId );
			item->m_type = li->activeID();
		}
	}

	if ( finishCreation && item )
		item->finishedCreation();

	return item;
}


QImage ItemLibrary::componentImage( Component * component, const uint maxSize )
{
	// Default orientation for painting
	const int angleDegrees = component->angleDegrees();
	const bool flipped = component->flipped();
	component->setAngleDegrees( 0 );
	component->setFlipped( false );

	QRect bound = component->boundingRect().normalized();
	bound.setLeft( bound.left()-8 );
	bound.setRight( bound.right()+8 );
	bound.setTop( bound.top()-8 );
	bound.setBottom( bound.bottom()+8 );

	// We want a nice square bounding rect
	const int dy = bound.width() - bound.height();
	if ( dy > 0 ) {
		bound.setTop( bound.top()-(dy/2) );
		bound.setBottom( bound.bottom()+(dy/2) );
	} else if ( dy < 0 ) {
		bound.setLeft( bound.left()+(dy/2) );
		bound.setRight( bound.right()-(dy/2) );
	}

	const bool cache = ((bound.width()*bound.height()) > (int)maxSize);
	QString type;
	if ( cache && m_imageMap.contains(component->type()) )
		return m_imageMap[component->type()];

	// Create pixmap big enough to contain CNItem and surrounding nodes
	// and copy the button grab to it

	QPixmap pm( bound.size() );

	QBitmap mask( bound.size() );
	mask.fill( Qt::color0 );

	//QPainter maskPainter(&mask); // 2016.05.03 - initialize painter explicitly
    QPainter maskPainter;
    {
        const bool isSuccess = maskPainter.begin(&mask);
        if (!isSuccess) {
            qWarning() << Q_FUNC_INFO << " painter not active at line " << __LINE__;
        }
    }
	maskPainter.translate( -bound.x(), -bound.y() );
	maskPainter.setPen( Qt::color1 );
	maskPainter.setBrush( Qt::color1 );

    //BEGIN painting on the pixmap
    {
	//QPainter p(&pm); // 2016.05.03 - initialize painter explicitly
    QPainter p;
    const bool isBeginSuccess = p.begin(&pm);
    if (!isBeginSuccess) {
        qWarning() << Q_FUNC_INFO << " painter not active at line " << __LINE__;
    }
	p.translate( -bound.x(), -bound.y() );
	p.setPen( component->pen() );
	p.setBrush( component->brush() );

	//BEGIN Draw the component
	const bool sel = component->isSelected();

	if (sel) {
		// We block the signals as we end up in an infinite loop with component emitting a selected signal
		component->blockSignals(true);
		component->setSelected(false);
		component->blockSignals(false);
	}

	component->drawShape(p);
	component->drawShape(maskPainter);

	if (sel) {
		component->blockSignals(true);
		component->setSelected(sel);
		component->blockSignals(false);
	}
	//END Draw the component

	maskPainter.setPen( Qt::color1 );
	maskPainter.setBrush( Qt::color1 );

	QMatrix transMatrix; // Matrix to apply to the image


	NodeInfoMap nodes = component->nodeMap();
	const NodeInfoMap::iterator nodesEnd = nodes.end();
	for ( NodeInfoMap::iterator it = nodes.begin(); it != nodesEnd; ++it )
	{
		Node *node = it.value().node;
		const bool sel = node->isSelected();
		if (sel)
			node->setSelected(false);
		if ( ECNode *ecnode = dynamic_cast<ECNode*>(node)  )
		{
			bool showVB = ecnode->showVoltageBars();
			bool showVC = ecnode->showVoltageColor();

			ecnode->setShowVoltageBars( false );
			ecnode->setShowVoltageColor( false );

			ecnode->drawShape(p);
			ecnode->drawShape( maskPainter );

			ecnode->setShowVoltageBars( showVB );
			ecnode->setShowVoltageColor( showVC );
		} else {
			node->drawShape(p);
			node->drawShape(maskPainter);
		}

		if (sel) node->setSelected(sel);
	}

	p.setPen(Qt::black);
	TextMap text = component->textMap();
	const TextMap::iterator textEnd = text.end();
	for ( TextMap::iterator it = text.begin(); it != textEnd; ++it )
	{
		it.value()->drawShape(p);
		it.value()->drawShape(maskPainter);
	}

// 	maskPainter.setPen( Qt::color1 );
// 	maskPainter.setBrush( Qt::color1 );
	component->drawWidgets(p);
// 	component->drawWidgets(maskPainter);
    }
    //END painting on the pixmap

	pm.setMask(mask); // pm needs not to have active painters on it

	// Now, rotate the image so that it's the right way up, and scale it to size
	QImage im = pm.toImage();
	//im = im.smoothScale( 50, 50, Qt::ScaleMin ); //2018.12.01
    im = im.scaled( QSize( 50, 50 ), Qt::KeepAspectRatio, Qt::SmoothTransformation );

	if (cache)
		m_imageMap[component->type()] = im;

	// Restore original orientation
	component->setAngleDegrees( angleDegrees );
	component->setFlipped( flipped );

	return im;
}

QPixmap ItemLibrary::itemIconFull( const QString &id )
{
	LibraryItemList::iterator end = m_items.end();
	for ( LibraryItemList::iterator it = m_items.begin(); it != end; ++it )
	{
		if ( *it && (*it)->allIDs().contains(id) )
		{
			return (*it)->iconFull();
		}
	}
	return QPixmap();
}


bool ItemLibrary::saveDescriptions( const QString & languageCode )
{
	QString url = itemDescriptionsFile( languageCode );

	QFile file( url );
	if ( !file.open( QIODevice::WriteOnly ) )
	{
		KMessageBox::sorry( nullptr, i18n("Could not open item descriptions file \"%1\" for writing.", url) );
		return false;
	}

	QTextStream stream( & file );

	const QStringMap itemDescriptions = m_itemDescriptions[ languageCode ];
	for (auto descIt = itemDescriptions.begin(), end = itemDescriptions.end(); descIt != end; ++descIt) {
		stream << QString::fromLatin1("<!-- item: %1 -->\n").arg( descIt.key() );
		stream << descIt.value() << endl;
	}

	file.close();

	return true;
}


bool ItemLibrary::haveDescription( QString type, const QString & languageCode ) const
{
	if ( type.startsWith(QString::fromLatin1("/")) )
	{
		// Possibly change e.g. "/ec/capacitor" to "ec/capacitor"
		type.remove( 0, 1 );
	}

	if ( !m_itemDescriptions[ languageCode ].contains( type ) )
	{
		return libraryItem( type );
	}

	return ! m_itemDescriptions[ languageCode ][ type ].isEmpty();
}


QString ItemLibrary::description( QString type, const QString & languageCode ) const
{
	if ( type.startsWith(QString::fromLatin1("/")) )
	{
		// Possibly change e.g. "/ec/capacitor" to "ec/capacitor"
		type.remove( 0, 1 );
	}

	QString current = m_itemDescriptions[ languageCode ][ type ];

	if ( current.isEmpty() )
	{
		// Try english-language description
		current = m_itemDescriptions[ QString::fromLatin1("en_US") ][ type ];
		if ( current.isEmpty() )
			return emptyItemDescription( languageCode );
	}

	return current;
}


QString ItemLibrary::emptyItemDescription(const QString &languageCode) const
{
	//return m_emptyItemDescription.arg( KGlobal::locale()->twoAlphaToLanguageName( language ) );
    return m_emptyItemDescription.subs(QLocale(languageCode).nativeLanguageName()).toString();
}


bool ItemLibrary::setDescription(QString type, const QString & description, const QString & languageCode)
{
	if ( type.startsWith(QString::fromLatin1("/")) )
	{
		// Possibly change e.g. "/ec/capacitor" to "ec/capacitor"
		type.remove( 0, 1 );
	}

	m_itemDescriptions[ languageCode ][ type ] = description;
	return saveDescriptions( languageCode );
}


void ItemLibrary::setItemDescriptionsDirectory( QString dir )
{
	if ( !dir.isEmpty() && !dir.endsWith(QString::fromLatin1("/")) )
		dir += QString::fromLatin1("/");

	KSharedConfigPtr conf = KSharedConfig::openConfig();
	//QString prevGroup = conf->group();

	KConfigGroup grGen = conf->group("General");
	grGen.writePathEntry( "ItemDescriptionsDirectory", dir );
	//conf->setGroup( prevGroup );
}


QString ItemLibrary::itemDescriptionsDirectory() const
{
	KSharedConfigPtr conf = KSharedConfig::openConfig();
	//QString prevGroup = conf->group();

	KConfigGroup grGen = conf->group("General");
	QString dir = grGen.readPathEntry( "ItemDescriptionsDirectory", QStandardPaths::locate( QStandardPaths::AppDataLocation, "contexthelp/", QStandardPaths::LocateDirectory) );
	//conf->setGroup( prevGroup );

	if ( !dir.isEmpty() && !dir.endsWith(QString::fromLatin1("/")) )
		dir += QString::fromLatin1("/");

	return dir;
}


QString ItemLibrary::itemDescriptionsFile( const QString & languageCode) const
{
	QString dir( itemDescriptionsDirectory() );
	if ( dir.isEmpty() )
		return QString::null;

	const QString url = dir + QLatin1String("help-") + languageCode;

	return url;
}


void ItemLibrary::loadItemDescriptions()
{
	// Create an entry for the default language (American English)
	// and the current language
//     KLocale * locale = KLocale::global();
// 	m_itemDescriptions[ locale->defaultLanguage() ];
// 	m_itemDescriptions[ locale->language() ];
	
	m_itemDescriptions[QLocale().name()];

	const QStringList languages = descriptionLanguages();
	QStringList::const_iterator end = languages.end();
	for ( QStringList::const_iterator it = languages.begin(); it != end; ++it )
	{
		QString url = itemDescriptionsFile( *it );

		QFile file( url );
		if ( !file.open( QIODevice::ReadOnly ) )
		{
			qWarning() << Q_FUNC_INFO << "Could not open file \"" << url << "\"" << endl;
			continue;
		}

		QTextStream stream( & file );

		QString type;
		QString description;
		while ( !stream.atEnd() )
		{
			QString line = stream.readLine();
			if ( line.startsWith( QString::fromLatin1("<!-- item: ") ) )
			{
				// Save the previous description
				if ( !type.isEmpty() )
					m_itemDescriptions[ *it ][ type ] = description.trimmed();

				line.remove( QString::fromLatin1("<!-- item: ") );
				line.remove( QString::fromLatin1(" -->") );

				type = line.trimmed();
				if ( type.startsWith(QString::fromLatin1("/")) )
				{
					// Possibly change e.g. "/ec/capacitor" to "ec/capacitor"
					type.remove( 0, 1 );
				}

				description = QString::null;
			} else description += line + '\n';
		}

		// Save the previous description
		if ( !type.isEmpty() )
			m_itemDescriptions[ *it ][ type ] = description.trimmed();

		file.close();
	}
}
