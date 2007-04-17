/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef OSCILLOSCOPEDATA_H
#define OSCILLOSCOPEDATA_H

#include <qcolor.h>
#include <qobject.h>

typedef long long llong;
typedef unsigned long long ullong;
typedef unsigned int uint;

#define DATA_CHUNK_SIZE (8192/sizeof(T))
#define DATA_CHUNK_ARRAY_SIZE ((8192-sizeof(uint))/sizeof(DataChunk<T>*))

// Allow a minimum of 64 megabytes of stored data (67108864 bytes)
/// \todo The maximum allowed amount of stored data should be configurable or
/// more intelligent (e.g. taking into account the number of probes or the
/// amount of physical memory on the system).
#define DCARRAY_ARRAY_SIZE ((67108864/(8192*DATA_CHUNK_ARRAY_SIZE))+1)


/**
For use in LogicProbe: Every time the input changes state, the new input state
is recorded in value, along with the simulator time that it occurs at.
 */
class LogicDataPoint
{
	public:
		LogicDataPoint()
		{
			value = 0;
			time = 0;
		}
		LogicDataPoint( bool v, ullong t )
		{
			value = v;
			time = t;
		}
		bool	value	:  1;
		ullong	time	: 63;
};


template <typename T>
class DataChunk
{
	public:
		DataChunk() { memset( data, 0, DATA_CHUNK_SIZE*sizeof(T) ); }
		
		T data[ DATA_CHUNK_SIZE ];
		
	private:
		// We don't want to accidently copy a shedload of data
		DataChunk( const DataChunk & );
};

typedef DataChunk<LogicDataPoint> LogicChunk;
typedef DataChunk<float> FloatingChunk;


template <typename T>
class DCArray
{
	public:
		DCArray()
		{
			memset( m_data, 0, DATA_CHUNK_ARRAY_SIZE*sizeof(DataChunk<T> *) );
			m_allocatedUpTo = 0;
		}
		~DCArray()
		{
			for ( uint i=0; i<m_allocatedUpTo; ++i)
				delete m_data[i];
		}
		
		inline DataChunk<T> * chunk( uint i )
		{
			if ( i >= m_allocatedUpTo )
				allocateUpTo(i+1024);
			
			if ( i >= DATA_CHUNK_ARRAY_SIZE )
				return 0l;
			
			return m_data[i];
		}
		uint allocatedUpTo() const { return m_allocatedUpTo; }
		
		
	protected:
		void allocateUpTo( uint upTo )
		{
			if ( upTo > DATA_CHUNK_ARRAY_SIZE )
				upTo = DATA_CHUNK_ARRAY_SIZE;
	
			for ( uint i=m_allocatedUpTo; i<upTo; ++i )
				m_data[i] = new DataChunk<T>;
			m_allocatedUpTo = upTo;
		}
		
		DataChunk<T> * m_data[DATA_CHUNK_ARRAY_SIZE];
		uint m_allocatedUpTo;
		
	private:
		// We don't want to accidently copy a shedload of data
		DCArray( const DCArray & );
};


template <typename T>
class StoredData
{
	public:
		StoredData()
		{
			memset( m_data, 0, DCARRAY_ARRAY_SIZE*sizeof(DCArray<T> *) );
			m_allocatedUpTo = 0;
		}
		~StoredData()
		{
			reset();
		}
		
		inline T & operator[]( ullong i )
		{
			return dataAt(i);
		}
		inline T & dataAt( ullong i, ullong * insertPos = 0 )
		{
			ullong c = i % DATA_CHUNK_SIZE;
			ullong b = ullong((i-c)/DATA_CHUNK_SIZE) % DATA_CHUNK_ARRAY_SIZE;
			ullong a = ullong((ullong((i-c)/DATA_CHUNK_SIZE)-b)/DATA_CHUNK_ARRAY_SIZE);
			
			if ( a >= m_allocatedUpTo )
				allocateUpTo(a+1);
			
			if ( a >= DCARRAY_ARRAY_SIZE )
			{
				a = DCARRAY_ARRAY_SIZE - 1;
				if ( insertPos )
					*insertPos = toPos( a, b, c );
			}
			
			return m_data[a]->chunk(b)->data[c];
		}
		
		ullong toPos( ullong a, ullong b, ullong c ) const
		{
			return (((a*DATA_CHUNK_ARRAY_SIZE)+b)*DATA_CHUNK_SIZE)+c;
		}
		
		uint allocatedUpTo() const { return m_allocatedUpTo; }
		
		DCArray<T> * dcArray( unsigned pos ) const
		{
			return (pos < m_allocatedUpTo) ? m_data[pos] : 0l;
		}
		
		/**
		 * Initialises all data to 0
		 */
		void reset()
		{
			for ( uint i=0; i<m_allocatedUpTo; ++i)
				delete m_data[i];
			m_allocatedUpTo = 0;
		}
		
	protected:
		void allocateUpTo( uint upTo )
		{
			if ( upTo >= DCARRAY_ARRAY_SIZE )
			{
				// Shuffle all data (getting rid of the oldest data)
				delete m_data[0];
				for ( unsigned i = 1; i < m_allocatedUpTo; ++i )
					m_data[i-1] = m_data[i];
				
				upTo = DCARRAY_ARRAY_SIZE;
				m_allocatedUpTo--;
			}
	
			for ( unsigned i = m_allocatedUpTo; i < upTo; ++i )
				m_data[i] = new DCArray<T>;
			
			m_allocatedUpTo = upTo;
		}
		DCArray<T> * m_data[DCARRAY_ARRAY_SIZE];
		
		uint m_allocatedUpTo;
		
	private:
		// We don't want to accidently copy a shedload of data
		StoredData( const StoredData<T> & );
};


/**
@author David Saxton
 */
class ProbeData : public QObject
{
	Q_OBJECT;
	public:
		ProbeData( int id );
		~ProbeData();
		
		/**
		 * @returns unique id for oscilloscope, set on construction
		 */
		int id() const { return m_id; }
		/**
		 * Set the proportion (0 = top, 1 = bottom) of the way down the
		 * oscilloscope view that the probe output is drawn. If the proportion
		 * is out of range ( <0, or >1), then the drawPosition is set to 0/1
		 */
		void setDrawPosition( float drawPosition ) { m_drawPosition = drawPosition; }
		/**
		 * Returns the draw position. Default is 0.5.
		 * @see setDrawPosition
		 */
		float drawPosition() const { return m_drawPosition; }
		/**
		 * Set the colour that is used to display the probe in the oscilloscope.
		 * Default is black.
		 */
		void setColor( QColor color );
		/**
		 * @returns the colour that is used to display the probe in the oscilloscope
		 */
		QColor color() const { return m_color; }
// 		/**
// 		 * Will not record any data when paused
// 		 */
// 		void setPaused( bool isPaused ) { b_isPaused = isPaused; }
		/**
		 * Returns the time (in Simulator time) that this probe was created at,
		 * or last reset.
		 */
		ullong resetTime() const { return m_resetTime; }
		/**
		 * Erases all recorded data, and sets m_resetTime to the current
		 * simulator time.
		 */
		virtual void eraseData() = 0;
		/**
		 * Searches for and returns the position of the last DataPoint that was
		 * added before or at the given Simulator time. If no DataPoints were
		 * were recorded before the given time, then will return the one closest
		 * to the given time. Will return 0 if no DataPoints have been recorded
		 * yet.
		 */
		virtual ullong findPos( llong time ) const = 0;
		
		ullong insertPos() const { return m_insertPos; }
		
	signals:
		/**
		 * Emitted when an attribute that affects how the probe is drawn in the
		 * oscilloscope is changed.
		 */
		void displayAttributeChanged();
		
	protected:
		const int m_id;
		float m_drawPosition;
		ullong m_insertPos;
// 		bool b_isPaused;
		ullong m_resetTime;
		QColor m_color;
};


/**
@author David Saxton
*/
class LogicProbeData : public ProbeData
{
	public:
		LogicProbeData( int id );
		
		/**
		 * Appends the data point to the set of data.
		 */
		void addDataPoint( LogicDataPoint data )
		{
			ullong next = m_insertPos++;
			m_data.dataAt( next, & m_insertPos ) = data;
		}
		
		virtual void eraseData();
		virtual ullong findPos( llong time ) const;
		
	protected:
		StoredData<LogicDataPoint> m_data;
		friend class ScopeViewBase;
		friend class OscilloscopeView;
		friend class ScopeScreenView;
};


/**
@author David Saxton
*/
class FloatingProbeData : public ProbeData
{
	public:
		enum Scaling { Linear, Logarithmic };
		
		FloatingProbeData( int id );
		
		/**
		 * Appends the data point to the set of data.
		 */
		void addDataPoint( float data ) { m_data[m_insertPos++] = data; }
		/**
		 * Converts the insert position to a Simulator time.
		 */
		ullong toTime( ullong at ) const;
		/**
		 * Sets the scaling to use in the oscilloscope display.
		 */
		void setScaling( Scaling scaling );
		/**
		 * @return the scaling used for the oscilloscope display.
		 */
		Scaling scaling() const { return m_scaling; }
		/**
		 * Sets the value to use as the upper absolute value in the display.
		 */
		void setUpperAbsValue( double upperAbsValue );
		/**
		 * @return the upper absolute value to use in the display.
		 */
		double upperAbsValue() const { return m_upperAbsValue; }
		/**
		 * Sets the value to use as the lower absolute value in the display
		 * (this is only used with logarithmic scaling).
		 */
		void setLowerAbsValue( double lowerAbsValue );
		/**
		 * @return the lower absolute value to use in the display (this is
		 * only used with logarithmic scaling).
		 */
		double lowerAbsValue() const { return m_lowerAbsValue; }
		
		virtual void eraseData();
		virtual ullong findPos( llong time ) const;
		
	protected:
		Scaling m_scaling;
		double m_upperAbsValue;
		double m_lowerAbsValue;
		StoredData<float> m_data;
		friend class OscilloscopeView;
		friend class ScopeScreenView;
		friend class ScopeViewBase;
};
	

#endif
