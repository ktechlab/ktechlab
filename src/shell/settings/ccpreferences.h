/* KDevelop Project Settings
 *
 * Copyright 2006  Matt Rogers <mattr@kde.org>
 * Copyright 2007  Hamish Rodda <rodda@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef KDEVCCPREFERENCES_H
#define KDEVCCPREFERENCES_H

#include <kcmodule.h>

#include <kurl.h>
#include <kstandarddirs.h>

namespace Ui
{
class CCSettings;
}

namespace KDevelop
{

class CCPreferences : public KCModule
{
    Q_OBJECT
public:
    CCPreferences( QWidget *parent, const QVariantList &args );
    virtual ~CCPreferences();

    virtual void save();

    virtual void load();
    
    virtual KUrl localNonShareableFile() const
    {
        return KUrl::fromPath(
                   KStandardDirs::locate( "data", "kdevelop/data.kdev4" ) );
    }
    
    void writeToSettings();

private:
    Ui::CCSettings *preferencesDialog;
};

}
#endif
