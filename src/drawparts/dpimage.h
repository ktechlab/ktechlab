/***************************************************************************
 *   Copyright (C) 2006 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DPIMAGE_H
#define DPIMAGE_H

#include "drawpart.h"

#include <Qt/qimage.h>
#include <Qt/qpixmap.h>
#include <Qt/qthread.h>

/**
@short Thread to perform quick and then good image scaling.
@author David Saxton
*/
class ImageScaleThread : public QThread
{
	public:
		enum BestScaling { Unscaled, NormalScaled, SmoothScaled };
		
		ImageScaleThread();
		/**
		 * Use the given settings.
		 * @return if any of the settings changed.
		 */
		bool updateSettings( const QString & imageURL, int width, int height );
		/**
		 * @param scaling is set to the type of scaling that this image has had.
		 * @return the best image done so far.
		 */
		QImage bestScaling( BestScaling * scaling = 0 ) const;
		
	protected:
		/**
		 * Start scaling.
		 */
		virtual void run();
		
		QImage m_image;
		QImage m_normalScaled;
		QImage m_smoothScaled;
		
		bool m_bDoneNormalScale;
		bool m_bDoneSmoothScale;
		
		int m_width;
		int m_height;
		QString m_imageURL;
		bool m_bSettingsChanged;
};


/**
@short Represents editable text on the canvas
@author David Saxton
 */
class DPImage : public DrawPart
{
	Q_OBJECT
	public:
		DPImage( ItemDocument *itemDocument, bool newItem, const char *id = 0L );
		~DPImage();

		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();

		virtual void setSelected( bool yes );

	protected:
		virtual void postResize();
		
	protected slots:
		/**
		 * Called from a timeout event after resizing to see if the image
		 * resizing thread has done anything useful yet.
		 */
		void checkImageScaling();
	
	private:
		virtual void drawShape( QPainter &p );
		void dataChanged();
		
		ImageScaleThread::BestScaling m_imageScaling;
		QPixmap m_image;
		ImageScaleThread m_imageScaleThread;
		RectangularOverlay * m_pRectangularOverlay;
		QTimer * m_pCheckImageScalingTimer;
		QString m_imageURL;
		bool m_bSettingsChanged;
		
		/**
		 * If we have been loaded from a file, etc, then we want to keep the
		 * previous size instead of resizing ourselves to the new image size
		 * like we would do normally if the user loads an image.
		 */
		bool m_bResizeToImage;
};

#endif
