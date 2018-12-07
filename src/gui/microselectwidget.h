/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MICROSELECTWIDGET_H
#define MICROSELECTWIDGET_H

// #include <q3groupbox.h>
#include <qgroupbox.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QLabel;
class KComboBox;

/**
@author David Saxton
*/
class MicroSelectWidget : public QGroupBox
{
	Q_OBJECT

	public:
		MicroSelectWidget( QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
		~MicroSelectWidget();

		void setMicro( const QString & id );
		QString micro() const;
		
		/**
		 * @see MicroLibrary::microIDs
		 */
		void setAllowedAsmSet( unsigned allowed );
		/**
		 * @see MicroLibrary::microIDs
		 */
		void setAllowedGpsimSupport( unsigned allowed );
		/**
		 * @see MicroLibrary::microIDs
		 */
		void setAllowedFlowCodeSupport( unsigned allowed );
		/**
		 * @see MicroLibrary::microIDs
		 */
		void setAllowedMicrobeSupport( unsigned allowed );
		
	protected slots:
		void microFamilyChanged( const QString & family );

	protected:
		void updateFromAllowed();
		
		unsigned int m_allowedAsmSet;
		unsigned int m_allowedGpsimSupport;
		unsigned int m_allowedFlowCodeSupport;
		unsigned int m_allowedMicrobeSupport;
		
		QHBoxLayout * m_pWidgetLayout;
		QLabel * m_pMicroFamilyLabel;
		KComboBox * m_pMicroFamily;
		QLabel * m_pMicroLabel;
		KComboBox * m_pMicro;
};

#endif
