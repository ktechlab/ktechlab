/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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

#include "theme.h"

#include <QString>
#include <QStringList>
#include <KDebug>
#include <KStandardDirs>

using namespace KTechLab;

Theme::Theme ( QObject* parent ) : QObject ( parent )
{
    m_dataDirs = KGlobal::dirs()->findDirs("data","ktechlab/themes/");
    kDebug() << "Found theme directories:" << m_dataDirs;
    m_name = defaultTheme();
}

QString Theme::defaultTheme()
{
    //TODO: read default from config file
    //and/or use KLocale to make a better guess
    return "din";
}

QString Theme::findFile ( const QString& item )
{
    QString file;
    QStringList fileList;
    const QString filterBase("ktechlab/themes/%1/components/%2.svgz");
    // find at default location
    QString filter = QString(filterBase)
            .arg(m_name)
            .arg(QString(item).replace("/","_")
        );
    fileList << KGlobal::dirs()->findAllResources( "data", filter );

    //find in default theme at default location
    filter = QString(filterBase)
            .arg(defaultTheme())
            .arg(QString(item).replace("/","_");
    fileList << KGlobal::dirs()->findAllResources( "data", filter );

    //find unkown for theme
    filter = QString("ktechlab/themes/%1/components/%2.svgz")
                .arg(m_name)
                .arg("unknown");
    fileList << KGlobal::dirs()->findAllResources( "data", filter );

    //find global unknown
    filter = QString("ktechlab/themes/components/%1.svgz").arg("unknown");
    fileList << KGlobal::dirs()->findAllResources( "data", filter );

    if (!fileList.isEmpty()) {
        file = fileList.first();
    }
    kDebug() << "Found file for" << item << ":" << file;
    return file;
}

void KTechLab::Theme::setThemeName ( const QString& name )
{
    m_name = name;
}

#include "theme.moc"