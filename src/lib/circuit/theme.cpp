/*
    Copyright (C) 2009  Julian Bäume

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

#if KDE_ENABLED
#include <KDebug>
#include <KStandardDirs>
#else
#include <QDebug>
#include <QDesktopServices>
// for Qt5: QStandardPaths
#endif

using namespace KTechLab;

Theme::Theme ( QObject* parent ) : QObject ( parent )
{
#if KDE_ENABLED
    m_dataDirs = KGlobal::dirs()->findDirs("data","ktechlab/themes/");
    kDebug() << "Found theme directories:" << m_dataDirs;
#else
    m_dataDirs.append(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
    qDebug() << "Found theme directories:" << m_dataDirs;
#endif
    m_name = defaultTheme();
}

QString Theme::defaultTheme()
{
    /* TODO: read default from config file
    and/or use KLocale to make a better guess
    since this is a string, translators should be
    able to just translate this

    when still on plasma, I used something like:
        KConfigGroup cg = config( "circuit" );
        m_componentTheme = cg.readEntry( "componentTheme", "din" );
    */
    return "din";
}

#if KDE_ENABLED
QString Theme::findFirstFile ( const QString& fileName )
{
    if (fileName.isEmpty())
        return findFirstFile("unknown.svgz");

    QString file;
    QStringList fileList;
    const QString filterBase("ktechlab/themes/%1/components/%2");
    // find at default location
    QString filter = QString(filterBase)
            .arg(m_name)
            .arg(QString(fileName));
    fileList << KGlobal::dirs()->findAllResources( "data", filter );

    //find in default theme at default location
    filter = QString(filterBase)
            .arg(defaultTheme())
            .arg(QString(fileName));
    fileList << KGlobal::dirs()->findAllResources( "data", filter );

    //find unkown for theme
    filter = QString("ktechlab/themes/%1/components/%2")
                .arg(m_name)
                .arg("unknown.svgz");
    fileList << KGlobal::dirs()->findAllResources( "data", filter );

    //find global unknown
    filter = QString("ktechlab/themes/components/%1").arg("unknown.svg");
    fileList << KGlobal::dirs()->findAllResources( "data", filter );

    if (!fileList.isEmpty()) {
        file = fileList.first();
    }
    kDebug() << "Found file for" << fileName << ":" << file;
    return file;
}
#else
QString Theme::findFirstFile ( const QString& fileName )
{
    if (fileName.isEmpty())
        return findFirstFile("unknown.svgz");

    qWarning() << "Theme::findFirstFile is not implemented";
    // TODO search for the resources on the path; paths starting with ":" should point to the
    // application's data
    // also can use QDesktopServices methods

    QString file;

#if 0
    QStringList fileList;
    const QString filterBase("ktechlab/themes/%1/components/%2");
    // find at default location
    QString filter = QString(filterBase)
            .arg(m_name)
            .arg(QString(fileName));
    fileList << KGlobal::dirs()->findAllResources( "data", filter );

    //find in default theme at default location
    filter = QString(filterBase)
            .arg(defaultTheme())
            .arg(QString(fileName));
    fileList << KGlobal::dirs()->findAllResources( "data", filter );

    //find unkown for theme
    filter = QString("ktechlab/themes/%1/components/%2")
                .arg(m_name)
                .arg("unknown.svgz");
    fileList << KGlobal::dirs()->findAllResources( "data", filter );

    //find global unknown
    filter = QString("ktechlab/themes/components/%1").arg("unknown.svg");
     fileList << KGlobal::dirs()->findAllResources( "data", filter );

    if (!fileList.isEmpty()) {
        file = fileList.first();
    }
#endif
    file = "";
    qDebug() << "Found file for" << fileName << ":" << file;
    return file;
}

#endif

void KTechLab::Theme::setThemeName ( const QString& name )
{
    m_name = name;
}

// #include "theme.moc"
