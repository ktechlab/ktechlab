/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton <david@bluehaze.org>          *
 *   Copyright (C) 2008 by Julian Bäume <julian@svg4all.de>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktechlab.h"
#include "config.h"

#include <KAboutData>
#include <KCmdLineArgs>
#include <KApplication>
#include <KUrl>

static const char description[] =
    "An IDE for microcontrollers and electronics";

int main(int argc, char **argv)
{
    KAboutData about( QString("ktechlab").toUtf8(),
                      QString("ktechlab").toUtf8(),
                      ki18n("KTechlab"),
                      QString(VERSION).toUtf8(),
                      ki18n(description),
                      KAboutData::License_GPL,
                      ki18n("(C) 2003-2008, The KTechlab developers"),
                      ki18n(""),
                      QString("http://ktechlab.org").toUtf8(),
                      QString("ktechlab-devel@lists.sourceforge.net").toUtf8()
                    );
    about.addAuthor( ki18n("David Saxton"),
                     ki18n("Initial KDE3 Version, former Maintainer"),
                     ki18n("david@bluehaze.org").toString().toUtf8()
                   );
    about.addAuthor( ki18n("Daniel Clarke"),
                     ki18n(""),
                     ki18n("daniel.jc@gmail.com").toString().toUtf8()
                   );
    about.addAuthor( ki18n("Julian Bäume"),
                     ki18n("KDE4 Port"),
                     ki18n("julian@svg4all.de").toString().toUtf8()
                   );
    about.addCredit( ki18n("Couriousous"),
                     ki18n("JK flip-flop, asyncronous preset/reset in the D flip-flop.")
                   );
    about.addCredit( ki18n("John Myers"),
                     ki18n("Rotary Switch")
                   );
    about.addCredit( ki18n("Ali Akcaagac"),
                     ki18n("Glib friendliness.")
                   );
    about.addCredit( ki18n("David Leggett"),
                     ki18n("Website hosting and feedback during early development.")
                   );

    KCmdLineOptions options;
    options.add( QString("+[URL]").toUtf8(), ki18n("Document to open."));
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

    KTechlab *ktechlab = new KTechlab();

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    for ( int i=0; i < args->count(); ++i )
        ktechlab->load( args->url(i) );

    ktechlab->show();

    args->clear(); // Free up some memory
    return app.exec();
}
