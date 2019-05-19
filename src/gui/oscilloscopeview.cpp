/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "oscilloscope.h"
#include "oscilloscopedata.h"
#include "oscilloscopeview.h"
#include "probepositioner.h"
#include "simulator.h"

#include <kconfig.h>
#include <kconfiggroup.h>
#include <klocalizedstring.h>
#include <kglobal.h>
// #include <k3popupmenu.h>

#include <qcheckbox.h>
#include <qcursor.h>
#include <qdebug.h>
#include <qevent.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qscrollbar.h>
#include <qmenu.h>
#include <qtimer.h>

#include <algorithm>
#include <cmath>


using namespace std;

inline uint64_t min( uint64_t a, uint64_t b)
{
	return a < b ? a : b;
}


OscilloscopeView::OscilloscopeView( QWidget *parent, const char *name)
	: QFrame( parent /*, name */ /*, Qt::WNoAutoErase */ ),
	b_needRedraw(true),
	m_pixmap(0),
	m_fps(10),
	m_sliderValueAtClick(-1),
	m_clickOffsetPos(-1),
	m_pSimulator( Simulator::self()),
	m_halfOutputHeight(0.0)
{
    setObjectName( name );
	//KGlobal::config()->setGroup("Oscilloscope");
    KConfigGroup grOscill(KSharedConfig::openConfig(), "Oscilloscope");
	m_fps = grOscill.readEntry( "FPS", 25);

	//setBackgroundMode(Qt::NoBackground); // 2018.12.07
    setBackgroundRole( QPalette::NoRole );
	setMouseTracking(true);

	m_updateViewTmr = new QTimer(this);
	connect( m_updateViewTmr, SIGNAL(timeout()), this, SLOT(updateViewTimeout()));
}

OscilloscopeView::~OscilloscopeView()
{
	delete m_pixmap;
}

void OscilloscopeView::updateView()
{
	if(m_updateViewTmr->isActive()) return;

    m_updateViewTmr->setSingleShot( true );
	m_updateViewTmr->start( 1000/m_fps /*, true */ );
}

void OscilloscopeView::updateViewTimeout()
{
	b_needRedraw = true;
	repaint( /* false  - 2018.12.07 */);
	updateTimeLabel();
}


void OscilloscopeView::updateTimeLabel()
{
	if( testAttribute(Qt::WA_UnderMouse) ) {
		int x = mapFromGlobal( QCursor::pos()).x();
		double time = (double(Oscilloscope::self()->scrollTime()) / LOGIC_UPDATE_RATE) + (x / Oscilloscope::self()->pixelsPerSecond());
		Oscilloscope::self()->timeLabel->setText( QString::number( time, 'f', 6));
	} else Oscilloscope::self()->timeLabel->setText( QString::null);
}


void OscilloscopeView::resizeEvent( QResizeEvent *e)
{
	delete m_pixmap;
	m_pixmap = new QPixmap( e->size());
	b_needRedraw = true;
	QFrame::resizeEvent(e);
}


void OscilloscopeView::mousePressEvent( QMouseEvent *event)
{
	switch ( event->button())
	{
		case Qt::LeftButton:
		{
			event->accept();
			m_clickOffsetPos = event->pos().x();
			m_sliderValueAtClick = Oscilloscope::self()->horizontalScroll->value();
			setCursor( Qt::SizeAllCursor);
			return;
		}

		case Qt::RightButton:
		{
			event->accept();

			QMenu fpsMenu;
			//fpsMenu.insertTitle( i18n("Framerate")); // 2017.12.27 - use setTitle
            //fpsMenu.insertItem( i18n("Framerate"), 1 );   // 2018.12.07 - use actions
            //fpsMenu.setItemEnabled(1, false);
            {
                QAction *a = fpsMenu.addAction( i18n("Framerate") );
                a->setData( 1 );
                a->setEnabled( false );
            }
            //fpsMenu.insertSeparator(); // 2018.12.07
            fpsMenu.addSeparator();

			const int fps[] = { 10, 25, 50, 75, 100 };

			for( uint i=0; i<5; ++i)
			{
				const int num = fps[i];
				//fpsMenu.insertItem( i18n("%1 fps", QString::number(num)), num);   // 2018.12.07
				//fpsMenu.setItemChecked( num, num == m_fps);
                QAction *a = fpsMenu.addAction( i18n("%1 fps", QString::number(num)) );
                a->setData( num );
			}

			connect( &fpsMenu, SIGNAL(triggered(QAction*)), this, SLOT(slotSetFrameRate(QAction*)));
			fpsMenu.exec( event->globalPos());
			return;
		}

		default:
		{
			QFrame::mousePressEvent(event);
			return;
		}
	}
}


void OscilloscopeView::mouseMoveEvent( QMouseEvent *event)
{
	event->accept();
	updateTimeLabel();

	if( m_sliderValueAtClick != -1)
	{
		int dx = event->pos().x() - m_clickOffsetPos;
		int dTick = int( dx * Oscilloscope::self()->sliderTicksPerSecond() / Oscilloscope::self()->pixelsPerSecond());
		Oscilloscope::self()->horizontalScroll->setValue( m_sliderValueAtClick - dTick);
	}
}


void OscilloscopeView::mouseReleaseEvent( QMouseEvent *event)
{
	if( m_sliderValueAtClick == -1)
		return QFrame::mouseReleaseEvent(event);

	event->accept();
	m_sliderValueAtClick = -1;
	setCursor( Qt::ArrowCursor);
}


void OscilloscopeView::slotSetFrameRate( QAction *action)
{
    int fps = action->data().toInt();
	m_fps = fps;
	//KGlobal::config()->setGroup("Oscilloscope");
    KConfigGroup grOscill(KSharedConfig::openConfig(), "Oscilloscope");
	grOscill.writeEntry( "FPS", m_fps);
}


// returns a % b
static double lld_modulus( int64_t a, double b)
{
	return double(a) - int64_t(a/b)*b;
}


void OscilloscopeView::paintEvent( QPaintEvent *e)
{
	QRect r = e->rect();

	if(b_needRedraw)
	{
		updateOutputHeight();
		const double pixelsPerSecond = Oscilloscope::self()->pixelsPerSecond();

        if (!m_pixmap) {
            qWarning() << Q_FUNC_INFO << " unexpected null m_pixmap in " << this;
            return;
        }

		QPainter p;
		//m_pixmap->fill( paletteBackgroundColor());
        m_pixmap->fill( palette().color( backgroundRole() ));
		const bool startSuccess = p.begin(m_pixmap);
        if ((!startSuccess) || (!p.isActive())) {
            qWarning() << Q_FUNC_INFO << " painter is not active";
        }

		p.setClipRegion(e->region());

		//BEGIN Draw vertical marker lines
		const double divisions = 5.0;
		const double min_sep = 10.0;

		double spacing = pixelsPerSecond/(std::pow( divisions, std::floor(std::log(pixelsPerSecond/min_sep)/std::log(divisions))));

		// Pixels offset is the number of pixels that the view is scrolled along
		const int64_t pixelsOffset = int64_t(Oscilloscope::self()->scrollTime()*pixelsPerSecond/LOGIC_UPDATE_RATE);
		double linesOffset = - lld_modulus( pixelsOffset, spacing);

		int blackness = 256 - int(184.0 * spacing / (min_sep*divisions*divisions));
		p.setPen( QColor( blackness, blackness, blackness));

		for( double i = linesOffset; i <= frameRect().width(); i += spacing)
			p.drawLine( int(i), 1, int(i), frameRect().height()-2);



		spacing *= divisions;
		linesOffset = - lld_modulus( pixelsOffset, spacing);

		blackness = 256 - int(184.0 * spacing / (min_sep*divisions*divisions));
		p.setPen( QColor( blackness, blackness, blackness));

		for( double i = linesOffset; i <= frameRect().width(); i += spacing)
			p.drawLine( int(i), 1, int(i), frameRect().height()-2);



		spacing *= divisions;
		linesOffset = - lld_modulus( pixelsOffset, spacing);

		blackness = 256 - int(184.0);
		p.setPen( QColor( blackness, blackness, blackness));

		for( double i = linesOffset; i <= frameRect().width(); i += spacing)
			p.drawLine( int(i), 1, int(i), frameRect().height()-2);
		//END Draw vertical marker lines

		drawLogicData(p);
		drawFloatingData(p);

		p.setPen(Qt::black);
		p.drawRect( frameRect());

		b_needRedraw = false;
	}

	//bitBlt( this, r.x(), r.y(), m_pixmap, r.x(), r.y(), r.width(), r.height()); // 2018.12.07
	QPainter p;
    const bool paintStarted = p.begin(this);
    if (!paintStarted) {
        qWarning() << Q_FUNC_INFO << " failed to start painting ";
    }
    p.drawImage(r, m_pixmap->toImage(), r);

}


void OscilloscopeView::updateOutputHeight()
{
	m_halfOutputHeight = int((Oscilloscope::self()->probePositioner->probeOutputHeight() - (probeArrowWidth/Oscilloscope::self()->numberOfProbes()))/2)-1;
}


void OscilloscopeView::drawLogicData( QPainter & p)
{
	const double pixelsPerSecond = Oscilloscope::self()->pixelsPerSecond();

	const LogicProbeDataMap::iterator end = Oscilloscope::self()->m_logicProbeDataMap.end();
	for( LogicProbeDataMap::iterator it = Oscilloscope::self()->m_logicProbeDataMap.begin(); it != end; ++it)
	{
		// When searching for the next logic value to display, we look along
		// until there is a recorded point which is at least one pixel along
		// If we are zoomed out far, there might be thousands of data points
		// between each pixel. It is time consuming searching for the next point
		// to display one at a time, so we record the average number of data points
		// between pixels ( = deltaAt / totalDeltaAt)
		int64_t deltaAt = 1;
		int totalDeltaAt = 1;

		LogicProbeData * probe = it.value();

		vector<LogicDataPoint> *data = probe->m_data;
		if(!data->size()) continue;

		const int midHeight = Oscilloscope::self()->probePositioner->probePosition(probe);
		const int64_t timeOffset = Oscilloscope::self()->scrollTime();

		// Draw the horizontal line indicating the midpoint of our output
		p.setPen( QColor( 228, 228, 228));
		p.drawLine( 0, midHeight, width(), midHeight);

		// Set the pen colour according to the colour the user has selected for the probe
		p.setPen( probe->color());

		// The smallest time step that will display in our oscilloscope
		const int minTimeStep = int(LOGIC_UPDATE_RATE/pixelsPerSecond);

		int64_t at = probe->findPos(timeOffset);
		const int64_t maxAt = probe->m_data->size();
		int64_t prevTime = (*data)[at].time;
		int prevX = (at > 0) ? 0 : int((prevTime - timeOffset)*(pixelsPerSecond/LOGIC_UPDATE_RATE));
		bool prevHigh = (*data)[at].value;
		int prevY = midHeight + int(prevHigh ? -m_halfOutputHeight : +m_halfOutputHeight);

		while ( at < maxAt) {
			// Search for the next pos which will show up at our zoom level
			int64_t previousAt = at;
			int64_t dAt = deltaAt / totalDeltaAt;

			while ( (dAt > 1) && (at < maxAt) && ( (int64_t((*data)[at].time) - prevTime) != minTimeStep))
			{
				// Search forwards until we overshoot
				while ( at < maxAt && ( int64_t((*data)[at].time) - prevTime) < minTimeStep)
					at += dAt;
				dAt /= 2;

				// Search backwards until we undershoot
				while ( (at < maxAt) && ( int64_t((*data)[at].time) - prevTime) > minTimeStep)
				{
					at -= dAt;
					if( at < 0)
						at = 0;
				}
				dAt /= 2;
			}

			// Possibly increment the value of at found by one (or more if this is the first go)
			while ( (previousAt == at) || ((at < maxAt) && ( int64_t((*data)[at].time) - prevTime) < minTimeStep))
				at++;

			if( at >= maxAt) break;

			// Update the average values
			deltaAt += at - previousAt;
			totalDeltaAt++;

			bool nextHigh = (*data)[at].value;
			if( nextHigh == prevHigh) continue;

			int64_t nextTime = (*data)[at].time;
			int nextX = int((nextTime - timeOffset)*(pixelsPerSecond/LOGIC_UPDATE_RATE));
			int nextY = midHeight + int(nextHigh ? -m_halfOutputHeight : +m_halfOutputHeight);

			p.drawLine( prevX, prevY, nextX, prevY);
			p.drawLine( nextX, prevY, nextX, nextY);

			prevHigh = nextHigh;
			prevTime = nextTime;
			prevX = nextX;
			prevY = nextY;

			if( nextX > width()) break;
		};

		// If we could not draw right to the end; it is because we exceeded
		// maxAt
		if( prevX < width())
			p.drawLine( prevX, prevY, width(), prevY);
	}
}

#define v_to_y int(midHeight - (logarithmic ? ( (v>0) ? log(v/lowerAbsValue) : -log(-v/lowerAbsValue)) : v) * sf)

void OscilloscopeView::drawFloatingData(QPainter &p)
{
	const double pixelsPerSecond = Oscilloscope::self()->pixelsPerSecond();

	const FloatingProbeDataMap::iterator end = Oscilloscope::self()->m_floatingProbeDataMap.end();
	for(FloatingProbeDataMap::iterator it = Oscilloscope::self()->m_floatingProbeDataMap.begin(); it != end; ++it) {
		FloatingProbeData * probe = it.value();
		vector<float> *data = probe->m_data;

		if(!data->size()) continue;

		bool logarithmic = probe->scaling() == FloatingProbeData::Logarithmic;
		double lowerAbsValue = probe->lowerAbsValue();
		double sf = m_halfOutputHeight / (logarithmic ? log(probe->upperAbsValue()/lowerAbsValue) : probe->upperAbsValue());

		const int midHeight = Oscilloscope::self()->probePositioner->probePosition(probe);
		const int64_t timeOffset = Oscilloscope::self()->scrollTime();

		// Draw the horizontal line indicating the midpoint of our output
		p.setPen( QColor( 228, 228, 228));
		p.drawLine( 0, midHeight, width(), midHeight);

		// Set the pen colour according to the colour the user has selected for the probe
		p.setPen( probe->color());

		int64_t at = probe->findPos(timeOffset);
		const int64_t maxAt = probe->m_data->size();
		if(at > maxAt) at = maxAt;
		int64_t prevTime = probe->toTime(at);

		double v = (*data)[(at>0)?at:0];
		int prevY = v_to_y;
		int prevX = int((prevTime - timeOffset)*(pixelsPerSecond/LOGIC_UPDATE_RATE));

		while ( at < maxAt) {
			at++;

			uint64_t nextTime = prevTime + uint64_t(LOGIC_UPDATE_RATE * LINEAR_UPDATE_PERIOD);

			double v = (*data)[(at>0)?at:0];
			int nextY = v_to_y;
			int nextX = int((nextTime - timeOffset)*(pixelsPerSecond/LOGIC_UPDATE_RATE));

			p.drawLine( prevX, prevY, nextX, nextY);

			prevTime = nextTime;
			prevX = nextX;
			prevY = nextY;

			if( nextX > width()) break;
		};

		// If we could not draw right to the end; it is because we exceeded
		// maxAt
		if( prevX < width())
			p.drawLine( prevX, prevY, width(), prevY);
	}
}

#include "oscilloscopeview.moc"
