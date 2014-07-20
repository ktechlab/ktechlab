/*
    Make components aware of themes
    Copyright (C) 2009  Julian Bäume <julian@svg4all.de>

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

#ifndef THEME_H
#define THEME_H
#include <QObject>

#include <QStringList>

#include "circuitexport.h"

class QString;

namespace KTechLab
{

/**
 * \short Help dealing with different themes.
 * This class helps to locate the correct
 * file for a theme.
 * \p Themes are organised in one directory for each theme.
 * All themes are located in a sub-directory "themes" within
 * the ktechlab data directory. Themes installed in the user's
 * home directory are prefered in favour of those installed in
 * the systems global directory.
 *
 * \author Julian Bäume
 */
class CIRCUIT_EXPORT Theme : public QObject
{
    Q_OBJECT
public:
    /**
     * \short Default constructor
     */
    Theme ( QObject* parent = 0 );

    /**
     * \short Find a file for a given item type
     * Searches default locations to find the file containing
     * the graphical representation of the \param item for this
     * theme.
     * \p It will search the following locations in this order:
     * \li ktechlab/themes/NAME/components/fileName
     * \li ktechlab/themes/DEFAULT_THEME/components/fileName
     * \li ktechlab/themes/NAME/unknown.svgz
     * \li ktechlab/themes/unknown.svgz
     *
     * and return the first match.
     * \param fileName - the type of the searched item
     * \return the path to the first file containing the graphic
     */
    QString findFirstFile( const QString &fileName );
    /**
     * \short Set a name for the theme
     * \param name - new name for the theme
     */
    void setThemeName ( const QString &name );
    /**
     * \short Return the default theme name
     */
    QString defaultTheme();
private:
    QString m_name;
    QStringList m_dataDirs;
};

}
#endif // THEME_H
