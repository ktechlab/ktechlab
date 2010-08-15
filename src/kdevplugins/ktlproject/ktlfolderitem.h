/*
    Folders are sub-projects (can be programmes or libraries) in KTechLab
    Copyright (C) 2009-2010 Julian Bäume <julian@svg4all.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef KTLFOLDERITEM_H
#define KTLFOLDERITEM_H

#include <project/projectmodel.h>

namespace KDevelop
{
/**
* Folders in KTechLab act as sub-projects. They can be of type program or library.
* This will affect the build-chain.
*/
class KTLFolderItem : public ProjectFolderItem
{
  public:
    KTLFolderItem ( IProject* , const KUrl& dir, QStandardItem* parent = 0 );
};

}

#endif // KTLFOLDERITEM_H
