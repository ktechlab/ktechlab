/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DOCMANAGERIFACE_H
#define DOCMANAGERIFACE_H

//#include <dcopobject.h>
//#include <dcopref.h>
#include "dcop_stub.h"

class DocManager;
class Document;

/**
@author David Saxton
*/
class DocManagerIface : public DCOPObject
{
	K_DCOP

	public:
		DocManagerIface( DocManager * docManager );
		~DocManagerIface();
		
	k_dcop:
		/**
		 * Attempt to close all documents, returning true if successful.
		 */
		bool closeAll();
		/**
		 * Attempt to open the given URL.
		 */
		DCOPRef openURL( const QString & url );
		/**
		 * Attempt to open the text file at the given url (if it isn't already
		 * open), and then go to that line in the text file.
		 */
		void gotoTextLine( const QString & url, int line );
		/**
		 * Creates a new Text document.
		 */
		DCOPRef createTextDocument();
		/**
		 * Creates a new Circuit document.
		 */
		DCOPRef createCircuitDocument();
		/**
		 * Creates a new FlowCode document.
		 */
		DCOPRef createFlowCodeDocument();
		/**
		 * Creates a new Mechanics document.
		 */
		DCOPRef createMechanicsDocument();
		
	protected:
		DCOPRef docToRef( Document * document );
		
		DocManager * m_pDocManager;
};

#endif
