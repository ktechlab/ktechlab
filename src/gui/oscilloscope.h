/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#ifndef PROBE_H
#ifndef KTECHLAB_H
#ifndef OSCILLOSCOPEDATA_H
#include "ui_oscilloscopewidget.h"
#endif
#endif
#endif

#include "simulator.h"

#include <Qt/qmap.h>
#include <Qt/qwidget.h>

#include <stdint.h>

class FloatingProbeData;
class LogicProbe;
class LogicProbeData;
class KTechlab;
class Oscilloscope;
class Probe;
class ProbeData;
class VoltageProbe;
class QTimer;
namespace KateMDI { class ToolView; }

typedef QMap< int, ProbeData * > ProbeDataMap;
typedef QMap< int, LogicProbeData * > LogicProbeDataMap;
typedef QMap< int, FloatingProbeData * > FloatingProbeDataMap;

#if 0
const double MAX_BITS_PER_S = 100000;

// NOTE: The 10 has to agree with the 2^10 = 1024.0
const int MIN_MAX_LOG_2_DIFF = 10;
const double MIN_BITS_PER_S = MAX_BITS_PER_S / 1024.0;
#else
const double MAX_BITS_PER_S = LOGIC_UPDATE_RATE * 4;

// NOTE: The 18 has to agree with the 2^18 = 262144.0
const int MIN_MAX_LOG_2_DIFF = 18;
const double MIN_BITS_PER_S = MAX_BITS_PER_S / 262144.0;
#endif


/*
Due to strangeness with generation of .[cpp/h] files from .ui files (that is,
my inability to sort it out neatly), files other than those in /src/gui can't
see header files such as "oscilloscopewidget.h", so we have to provide some
interface functions for accessing the functionality in this class
*/
ProbeData * registerProbe( Probe * probe);
void unregisterProbe( int id);
void addOscilloscopeAsToolView( KTechlab *ktechlab);


#ifndef PROBE_H
#ifndef KTECHLAB_H
#ifndef OSCILLOSCOPEDATA_H
/**
@author David Saxton
*/
class Oscilloscope : public QWidget, public Ui::OscilloscopeWidget
{
	Q_OBJECT
	public:
		static Oscilloscope * self( KateMDI::ToolView * parent = 0);
		static QString toolViewIdentifier() { return "Oscilloscope"; }
		virtual ~Oscilloscope();
		
        static bool isInstantiated() ;

		/**
	 	* Register a probe (that outputs boolean data) with the oscilloscope.
	 	* Returns a unique id that the probe can use to add data points
		 */
		ProbeData * registerProbe( Probe * probe);
		void unregisterProbe( int id);
		/**
		 * Returns the Simulator time since recording started.
		 */
		uint64_t time() const;
		/**
		 * Returns how much of an increment in value of the oscilloscope slider
		 * is equivalent to one second.
		 */
		int sliderTicksPerSecond() const;
		/**
		 * Returns the number of pixels per second the user has requested to be
		 * displayed.
		 */
		double pixelsPerSecond() const;
		/**
		 * Zoom level; a value between 0 and 1. 0 is maximum zoom out, and 1 is
		 * maximum zoom in.
		 */
		double zoomLevel() const { return m_zoomLevel; }
		/**
		 * Sets the zoom level (and in the process, checks that it is within the
		 * bounds allowed).
		 */
		void setZoomLevel( double zoomLevel);
		/**
		 * Returns the Simulator time as given by the current scrollbar
		 * position.
		 */
		int64_t scrollTime() const;
		/**
		 * @returns pointer to probe with given id, or NULL if no such probe exists
		 */
		ProbeData * probeData( int id) const;
		/**
		 * @returns the total number of probes
		 */
		int numberOfProbes() const;
		/**
		 * @returns number of the probe with the given id, starting from 0, or -1 if no such probe
		 */
		int probeNumber( int id) const;
		
	signals:
		/**
		 * Emitted when a probe is registered
		 */
		void probeRegistered( int id, ProbeData * probe);
		/**
		 * Emitted when a probe is unregistered
		 */
		void probeUnregistered( int id);
		
	public slots:
		/**
		 * Resets all recorded data
		 */
		void reset();
		/**
		 * Called when the zoom slider value was changed.
		 */
		void slotZoomSliderChanged( int value);
		/**
		 * Called when the horizontal scrollbar was scrolled by the user
		 */
		void slotSliderValueChanged( int value);
		/**
		 * Pause the data capture (e.g. user clicked on pause button)
		 */
		void slotTogglePause();
	
	protected:
		void getOldestProbe();
		
		int m_nextId;
		ProbeData * m_oldestProbe;
		int m_oldestId;
		int m_nextColor; // For giving the probes colours
		
		ProbeDataMap m_probeDataMap;
		LogicProbeDataMap m_logicProbeDataMap;
		FloatingProbeDataMap m_floatingProbeDataMap;
		
		Simulator * m_pSimulator;
		
	protected slots:
		void updateScrollbars();
		
	private:
		Oscilloscope( KateMDI::ToolView * parent);
		
		static Oscilloscope * m_pSelf;
		double m_zoomLevel;
		
		friend class OscilloscopeView;
		friend class ProbePositioner;
};

#endif // OSCILLOSCOPEDATA_H
#endif // KTECHLAB_H
#endif // PROBE_H

#endif // OSCILLOSCOPE_H
