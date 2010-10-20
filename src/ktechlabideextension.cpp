/**************************************************************************
*   Copyright (C) 2009 by Julian Bäume <julian@svg4all.de>                *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ktechlabideextension.h"

#include <QString>

#include <KLocale>
#include <QApplication>

KTechlabIDEExtension::KTechlabIDEExtension()
    : KDevelop::ShellExtension()
{
}

void KTechlabIDEExtension::init() {
    s_instance = new KTechlabIDEExtension();
}

QString KTechlabIDEExtension::xmlFile() {
    return QString("ktechlabui.rc");
}

KDevelop::AreaParams KTechlabIDEExtension::defaultArea() {
    KDevelop::AreaParams params = {"ktlcircuit",i18n("Circuit")};
    return params;
}

QString KTechlabIDEExtension::projectFileExtension() {
    return i18n("ktl4");
}

QString KTechlabIDEExtension::projectFileDescription() {
    return i18n("KTechLab Project Files");
}

QStringList KTechlabIDEExtension::defaultPlugins() {
    QStringList plugins = QStringList();
    plugins << QString( "ktlcircuit" );
    plugins << QString( "flowcode" );
    plugins << QString( "code_asm" );
    plugins << QString( "code_c" );
    return plugins;
}

QString KTechlabIDEExtension::binaryPath()
{
    return QApplication::applicationDirPath() + "/ktechlab";
}
