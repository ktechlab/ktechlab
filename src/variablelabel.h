/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "config.h"
#ifndef NO_GPSIM

#ifndef VARIABLELABEL_H
#define VARIABLELABEL_H

#include <Qt/qpointer.h>
#include <Qt/qlabel.h>

class TextView;


/**
Used for displaying the value of a variable when the user hovers his mouse over
a variable while debugging.

@author David Saxton
 */
class VariableLabel : public QLabel
{
	Q_OBJECT
	public:
		VariableLabel( TextView * parent );
		/**
		 * Sets the register that this label is displaying the value of.
		 */
		void setRegister( RegisterInfo * info, const QString & name );
		/**
		 * Sets the value that this label is displaying. This is an alternative
		 * to setRegister.
		 */
		void setValue( unsigned value );
		
	protected slots:
		/**
		 * Updates what is displayed from m_pRegisterInfo.
		 */
		void updateText();
		
	protected:
		void disconnectRegisterInfo();
		
		QPointer<RegisterInfo> m_pRegisterInfo;
		QString m_registerName;
		int m_value;
};

#endif

#endif // !NO_GPSIM
