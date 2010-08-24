/***************************************************************************
 *   Copyright (C) 2004-2005 by Daniel Clarke                              *
 *   Copyright (C) 2005 by David Saxton                                    *
 *   daniel.jc@gmail.com                                                   *
 *                                                                         *
 *   24-04-2007                                                            *
 *   Modified to add pic 16f877,16f627 and 16f628                          *
 *   by george john george@space-kerala.org,az.j.george@gmail.com          *
 *   supported by SPACE www.space-kerala.org                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "microbe.h"
#include "pic14.h"

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <qfile.h>
#include <KDebug>

#include <iostream>
#include <fstream>
using namespace std;

static const char description[] =
    "The Microbe Compiler";

static const char version[] = "0.3";

int main(int argc, char **argv)
{
    KAboutData about(QByteArray("microbe"), QByteArray("languages"), ki18n("Microbe"),
                     QByteArray(version), ki18n(description),
                     KAboutData::License_GPL, ki18n("(C) 2004-2005, The KTechLab developers"),
                     KLocalizedString(), QByteArray("http://ktechlab.org"),
                     QByteArray("ktechlab-devel@lists.sourceforge.net") );
    about.addAuthor( ki18n("Daniel Clarke"), KLocalizedString(),
                     QByteArray("daniel.jc@gmail.com") );
    about.addAuthor( ki18n("David Saxton"), KLocalizedString(),
                     QByteArray("david@bluehaze.org") );
    about.addAuthor( ki18n("Modified to add pic 16f877,16f627 and 16f628 by George John"),
                     KLocalizedString(), QByteArray("az.j.george@gmail.com") );
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineOptions o;
    o.add( QByteArray("show-source"),
           ki18n( "Show source code lines in assembly output") );
    o.add( QByteArray("nooptimize"),
           ki18n( "Do not attempt optimization of generated instructions.") );
    o.add( QByteArray("+[Input URL]"),
           ki18n( "Input filename" ) );
    o.add( QByteArray("+[Output URL]"),
           ki18n( "Output filename" ) );

    KCmdLineArgs::addCmdLineOptions(o);

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if (args->count() == 2 )
    {
        Microbe mb;
//      QString s = mb.compile( args->arg(0), args->isSet("show-source"), args->isSet("optimize"));

        QString s = mb.compile( args->arg(0), args->isSet("optimize"));

        QString errorReport = mb.errorReport();

        if ( !errorReport.isEmpty() )
        {
            kError() << errorReport;
            return 1; // If there was an error, don't write the output to file.
        }

        else
        {
            QFile outFile(args->arg(1));
            outFile.open(QIODevice::WriteOnly);
            QDataStream out(&outFile);
            out << s;
            return 0;
        }
    }
    else args->usage();
}
