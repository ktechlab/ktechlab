/*
 *  Copyright (C) 2010 Julian Bäume <julian@svg4all.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef IROUTINGINFORMATION_H
#define IROUTINGINFORMATION_H
#include <QObject>

namespace KTechLab{

/**
 * \short Store basic information about routing in a scene
 *
 * This interface can be implemented to store routing information
 * in the \class IDocumentScene and use it in the routing plugin,
 * when routing is in progress.
 */
class IRoutingInformation : public QObject
{
};

}

#endif //IROUTINGINFORMATION_H