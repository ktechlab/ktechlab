/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DOCUMENTPLUGIN_H
#define DOCUMENTPLUGIN_H

namespace Plasma
{
class DataSource;
} // namespace Plasma

namespace KDevelop
{
class IDocument;
} // namespace KDevelop

namespace KTechLab
{

class DocumentPlugin
{
public:
    virtual ~DocumentPlugin() {};

    /**
     * create a Plasma::DataSource for a given document
     */
    virtual Plasma::DataSource * createDataSource( KDevelop::IDocument *document )=0;
};

} // namespace KTechLab{

#endif

