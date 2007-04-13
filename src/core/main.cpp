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
#include "config.h"

#include <dcopclient.h>
#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kconfig.h>
#include <klocale.h>

static const char description[] =
    I18N_NOOP("An IDE for microcontrollers and electronics");

static KCmdLineOptions options[] =
{
    { "+[URL]", I18N_NOOP( "Document to open." ), 0 },
    KCmdLineLastOption
};


int main(int argc, char **argv)
{
/* FIXME: should use package names found in config.h provided by autoconf */

    KAboutData about("ktechlab", I18N_NOOP("KTechlab"), VERSION, description,
					 KAboutData::License_GPL, "(C) 2003-2005, The KTechlab developers", "", "http://ktechlab.org", "ktechlab-devel@lists.sourceforge.net" );
    about.addAuthor( "David Saxton", 0, "david@bluehaze.org" );
	about.addAuthor( "Daniel Clarke", 0, "daniel.jc@gmail.com" );
	about.addCredit( "Couriousous", "JK flip-flop, asyncronous preset/reset in the D flip-flop." );
	about.addCredit( "John Myers", "Rotary Switch" );
	about.addCredit( "Ali Akcaagac", "Glib friendliness." );
	about.addCredit( "David Leggett", "Website hosting and feedback during early development." );
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;
	
    // register ourselves as a dcop client
	app.dcopClient()->registerAs(app.name(), false);
	
	KTechlab *ktechlab = new KTechlab();
	
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	for ( int i=0; i < args->count(); ++i )
		ktechlab->load( args->url(i) );
	
	ktechlab->show();
	args->clear(); // Free up some memory
	return app.exec();
}
