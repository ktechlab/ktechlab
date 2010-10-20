/**************************************************************************
*   Copyright (C) 2009 by Julian Bäume <julian@svg4all.de>                *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KTECHLABIDEEXTENSION_H
#define KTECHLABIDEEXTENSION_H

#include <shell/shellextension.h>

class KTechlabIDEExtension : public KDevelop::ShellExtension
{
public:
    static void init();

    virtual QString xmlFile();
    virtual KDevelop::AreaParams defaultArea();
    virtual QString projectFileExtension();
    virtual QString projectFileDescription();
    virtual QStringList defaultPlugins();
    virtual QString binaryPath();

protected:
    KTechlabIDEExtension();
};

#endif // KTECHLABIDEEXTENSION_H
