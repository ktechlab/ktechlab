/***************************************************************************
 *   Copyright (C) 2004-2005 by Daniel Clarke                              *
 *   Copyright (C) 2005 by David Saxton                                    *
 *   daniel.jc@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either VERSION 2 of the License, or     *
 *   (at your option) any later VERSION.                                   *
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
#include "config.h"

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <qfile.h>

#include <iostream>
#include <fstream>
using namespace std;

static const char description[] =
    I18N_NOOP("The Microbe Compiler");

static KCmdLineOptions options[] =
{
	{ "show-source", I18N_NOOP( "Show source code lines in assembly output"),0},
	{ "nooptimize", I18N_NOOP( "Do not attempt optimization of generated instructions."),0},
	{ "+[Input URL]", I18N_NOOP( "Input filename" ),0},
	{ "+[Output URL]", I18N_NOOP( "Output filename" ),0},
	KCmdLineLastOption
};

int main(int argc, char **argv)
{
	KAboutData about("microbe", I18N_NOOP("Microbe"), VERSION, description, KAboutData::License_GPL, "(C) 2004-2005, The KTechlab developers", 0, "http://ktechlab.org", "ktechlab-devel@lists.sourceforge.net" );
	about.addAuthor( "Daniel Clarke", 0, "daniel.jc@gmail.com" );
	about.addAuthor( "David Saxton", 0, "david@bluehaze.org" );
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );
	
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	
	if(args->count() == 2 ) {
		Microbe mb;	
		QString s = mb.compile( args->arg(0), args->isSet("show-source"), args->isSet("optimize"));
		QString errorReport = mb.errorReport();
		
		if ( !errorReport.isEmpty() ) {
			cerr << mb.errorReport();
			return 1; // If there was an error, don't write the output to file.
		} else {
			ofstream out(args->arg(1));
			out << s;
			return 0;
		}
	} else args->usage();
}

