/***************************************************************************
 *   Copyright (C) 2004-2005 by Daniel Clarke                              *
 *   Copyright (C) 2005 by David Saxton                                    *
 *   daniel.jc@gmail.com                                                   *
 *									   *
 *   24-04-2007                                                            *
 *   Modified to add pic 16f877,16f627 and 16f628 			   *
 *   by george john george@space-kerala.org,az.j.george@gmail.com	   *
 *   supported by SPACE www.space-kerala.org	 			   *
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

#include <KAboutData>
#include <KLocalizedString>

#include <QCoreApplication>
#include <QCommandLineParser>

#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("ktechlab"); // sharing strings with ktechlab

    KAboutData aboutData("microbe",
        i18n("Microbe"),
        QStringLiteral("0.3"),
        i18n("The Microbe Compiler"),
        KAboutLicense::GPL_V2,
        i18n("(C) 2004-2005, The KTechlab developers"),
        QString(),
        QStringLiteral("https://userbase.kde.org/KTechlab"));
    aboutData.addAuthor(i18n("Daniel Clarke"), QString(), QStringLiteral("daniel.jc@gmail.com"));
    aboutData.addAuthor(i18n("David Saxton"), QString(), QStringLiteral("david@bluehaze.org"));
    aboutData.addAuthor(i18n("George John"), i18n("Added pic 16f877,16f627 and 16f628"), QStringLiteral("az.j.george@gmail.com"));

    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);

    QCommandLineOption showSourceOption(QStringLiteral("show-source"), i18n( "Show source code lines in assembly output"));
    parser.addOption(showSourceOption);
    QCommandLineOption noOptimizeOption(QStringLiteral("no-optimize"), i18n( "Do not attempt optimization of generated instructions."));
    parser.addOption(noOptimizeOption);
    parser.addPositionalArgument( QStringLiteral("Input URL"), i18n( "Input filename" ));
    parser.addPositionalArgument( QStringLiteral("Output URL"), i18n( "Output filename" ));

    parser.process(app);
    aboutData.processCommandLine(&parser);

    const QStringList positionArguments = parser.positionalArguments();
    if (positionArguments.count() == 2 )
	{
		MicrobeApp mb;
//		QString s = mb.compile( positionArguments[0], parser.isSet(showSourceOption), !parser.isSet(noOptimizeOption));

		QString s = mb.compile( positionArguments[0], !parser.isSet(noOptimizeOption));

		QString errorReport = mb.errorReport();
		
		if ( !errorReport.isEmpty() )
		{
			cerr << mb.errorReport().toStdString();
			return 1; // If there was an error, don't write the output to file.
		}
		
		else
		{
			ofstream out(positionArguments[1].toStdString().c_str());
			out << s.toStdString();
			return 0;
		}
	}
	else parser.showHelp();
}

