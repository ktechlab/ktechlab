/***************************************************************************
*   Copyright (C) 2012 Zoltan Padrah <zoltan_padrah@users.sourceforge.net>*
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef __KTL_IGUIPLUGIN_H__
#define __KTL_IGUIPLUGIN_H__

#include "ktlinterfacesexport.h"

#include <QtCore/QObject>

class QListWidgetItem;

namespace KTechLab {

	/**
	 * A GUI plugin interface. It allows to the users of this plugin to
	 * hook in various locations in the GUI, and receive events.
	 */
class KTLINTERFACES_EXPORT IGuiPlugin {

public:
	IGuiPlugin();
	virtual ~IGuiPlugin();

	/**
	 * Add a new file type to the file dialog.
	 * @param item The item that will appear in the new file dialog
	 * @param receiver the QObject that will receive an event when
	 * 		a new file instanceshould be created
	 * @param slot the slot of the receiver object that will handle the
	 * 		creation of a new file.
	 *
	 * Note that the slot has to take no parameters.
	 */
	virtual void addFiletypeToNewFileDialog(const QListWidgetItem &item,
									const QObject *receiver,
									const char *slot
   								) = 0;

	/**
	 * Remove a registered file type from the new file dialog.
	 * @param receiver the object that has been registered to receive
	 * 		events.
	 * @param slot the slot of the receiver object that is registered
	 * 		to receive events.
	 * @return 0 if at least one receiver and slot pair has been found
	 * 	or return -1 if the receiver has not been found.
	 */
	virtual int removeFiletypeFromNewFileDialog(const QObject *receiver,
										const char *slot
									   ) = 0;

};

}

Q_DECLARE_INTERFACE( KTechLab::IGuiPlugin, "org.ktechlab.IGuiPlugin" )

#endif // __KTL_IGUIPLUGIN_H__
