/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktechlab.h"
#include "diagnosticstyle.h"
#include "logtofilemsghandler.h"

//#include <dcopclient.h>
#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kconfig.h>
#include <config.h>
#include <klocale.h>

static const char description[] =
    I18N_NOOP("An IDE for microcontrollers and electronics");

// static KCmdLineOptions options[] =
// {
//     { "+[URL]", I18N_NOOP( "Document to open." ), 0 },
//     KCmdLineLastOption
// };


int main(int argc, char **argv)
{
    LogToFileMsgHandler logFileHandler;

    KAboutData about(QByteArray("ktechlab"), QByteArray("KTechLab"), ki18n("KTechLab"), VERSION, ki18n(description),
                    KAboutData::License_GPL, ki18n("(C) 2003-2009, The KTechLab developers"),
                    KLocalizedString(), "http://ktechlab.org", "ktechlab-devel@lists.sourceforge.net" );
	about.addAuthor( ki18n("Alan Grimes"), ki18n("developer"), "" );
	about.addAuthor( ki18n("Zoltan Padrah"), ki18n("developer") , QByteArray("zoltan_padrah@users.sourceforge.net"));
	about.addCredit( ki18n("Jason Lucas"), ki18n("keeping the project up as a maintainer during the time David left"), "" );
	about.addCredit( ki18n("Lawrence Shafer"), ki18n("Website, wiki and forum"), "" );
	about.addCredit( ki18n("Julian Bäume"), ki18n("some bug-fixes"), QByteArray("julian@svg4all.de") );
	about.addCredit( ki18n("David Saxton"), ki18n("former developer, project founder, former maintainer"), QByteArray("david@bluehaze.org") );
	about.addCredit( ki18n("Daniel Clarke"), ki18n("former developer"), QByteArray("daniel.jc@gmail.com") );
	about.addCredit( ki18n("Couriousous"), ki18n("JK flip-flop, asynchronous preset/reset in the D flip-flop."), "" );
	about.addCredit( ki18n("John Myers"), ki18n("Rotary Switch"), "" );
	about.addCredit( ki18n("Ali Akcaagac"), ki18n("Glib friendliness."), "" );
	about.addCredit( ki18n("David Leggett"), ki18n("former Website hosting and feedback during early development."), "" );

    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineOptions options;
    options.add( QByteArray("+[URL]"), ki18n( "Document to open." ), 0);
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

    if (true) {  // TODO add a command line option for debugging the program's visual look
        //app.setStyle(new DiagnosticStyle());
    }
	
    // register ourselves as a dcop client
	//app.dcopClient()->registerAs(app.name(), false);
	
	KTechlab *ktechlab = new KTechlab();
	
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	for ( int i=0; i < args->count(); ++i )
		ktechlab->load( args->url(i) );
	
	ktechlab->show();
	args->clear(); // Free up some memory
	return app.exec();
}
