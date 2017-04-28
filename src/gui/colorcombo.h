/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef COLORCOMBO_H
#define COLORCOMBO_H

#include <qcolor.h>
#include <qcombobox.h>

/**
Based on KColorCombo, Copyright (C) 1997 Martin Jones (mjones@kde.org). Allows
which colours are displayed to be changed.

@author David Saxton
*/
class ColorCombo : public QComboBox
{
	Q_OBJECT
	Q_PROPERTY( QColor color READ color WRITE setColor )

	public:
		enum ColorScheme
		{
			QtStandard = 0,
			LED = 1,
			NumberOfSchemes = 2 ///< for internal usage; this should be one less than the number of items in the enum
		};
		
  	 	/**
		 * Constructs a color combo box.
		 */
		ColorCombo( ColorScheme colorScheme, QWidget *parent, const char *name = 0L );
		~ColorCombo();

		/**
		 * Returns the currently selected color.
		**/
		QColor color() const { return internalColor; }
		
	public slots:
		/**
		 * Selects the color @p col.
		 */
		void setColor( const QColor & col );

	signals:
    	/**
		 * Emitted when a new color box has been selected.
		 */
		void activated( const QColor &col );
	    /**
		 * Emitted when a new item has been highlighted.
		 */
		void highlighted( const QColor &col );
		
	protected slots:
		void slotActivated( int index );
		void slotHighlighted( int index );

	protected:
		virtual void resizeEvent( QResizeEvent *re );
		void addColors();
		void createPalettes();
		
		QColor customColor;
		QColor internalColor;
		ColorScheme m_colorScheme;
		
		static bool createdPalettes;
		static QColor * palette[ NumberOfSchemes ];
		static int paletteSize[ NumberOfSchemes ];
};

#endif
