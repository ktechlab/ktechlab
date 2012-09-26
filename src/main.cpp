/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton <david@bluehaze.org>          *
 *   Copyright (C) 2008-2009 by Julian Bäume <julian@svg4all.de>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "simulationmanager.h"
#include "ktechlabideextension.h"
#include "config.h"

#include <sublime/controller.h>
#include <shell/core.h>
#include <shell/projectcontroller.h>
#include <shell/documentcontroller.h>
#include <interfaces/iuicontroller.h>

#include <KAboutData>
#include <KCmdLineArgs>
#include <KUniqueApplication>
#include <KUrl>

#include <QFileInfo>

static const char description[] =
    "An IDE for microcontrollers and electronics";

int main(int argc, char **argv)
{
    KAboutData about( QString("ktechlab").toUtf8(),
                      QString("ktechlab").toUtf8(),
                      ki18n("KTechLab"),
                      QString(VERSION).toUtf8(),
                      ki18n(description),
                      KAboutData::License_GPL,
                      ki18n("(C) 2003-2009, The KTechLab developers"),
                      ki18n(""),
                      QString("http://ktechlab.org").toUtf8(),
                      QString("ktechlab-devel@lists.sourceforge.net").toUtf8()
                    );
    about.addAuthor( ki18n("Alan Grimes"),
                     ki18n("Developer, Simulation"),
                     ki18n("").toString().toUtf8()
                   );
    about.addAuthor( ki18n("Zoltan Padrah"),
                     ki18n("Developer"),
                     ki18n("zoltan_padrah@users.sourceforge.net").toString().toUtf8()
                   );
    about.addAuthor( ki18n("Juan De Vincenzo"),
                     ki18n(""),
                     ki18n("").toString().toUtf8()
                   );
    about.addAuthor( ki18n("Julian Bäume"),
                     ki18n("Developer, KDE4 Port, GUI"),
                     ki18n("julian@svg4all.de").toString().toUtf8()
                   );
    about.addCredit( ki18n("Lawrence Shafer"),
                     ki18n("Website, wiki and forum"),
                     ki18n("").toString().toUtf8()
                   );
    about.addCredit( ki18n("Jason Lucas"),
                     ki18n("keeping up the project during lack of developers"),
                     ki18n("").toString().toUtf8()
                   );
    about.addCredit( ki18n("David Saxton"),
                     ki18n("Initial KDE3 Version, former Maintainer"),
                     ki18n("david@bluehaze.org").toString().toUtf8()
                   );
    about.addCredit( ki18n("Daniel Clarke"),
                     ki18n("former developer"),
                     ki18n("daniel.jc@gmail.com").toString().toUtf8()
                   );
    about.addCredit( ki18n("Couriousous"),
                     ki18n("JK flip-flop, asyncronous preset/reset in the D flip-flop.")
                   );
    about.addCredit( ki18n("John Myers"),
                     ki18n("Rotary Switch")
                   );
    about.addCredit( ki18n("Ali Akcaagac"),
                     ki18n("Glib friendliness in KDE3 version.")
                   );
    about.addCredit( ki18n("David Leggett"),
                     ki18n("former Website hosting and feedback during early development.")
                   );

    KCmdLineOptions options;
    options.add( QString("+[URL]").toUtf8(), ki18n("Document to open."));
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

    KTechlabIDEExtension::init();

    KTechLab::SimulationManager::initialize();

    KDevelop::Core::initialize();
    KDevelop::Core *core = KDevelop::Core::self();

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    bool openProject = false;
    if ( args->count() > 0) {
        KUrl url = args->url( 0 );
        QString ext = QFileInfo( url.fileName() ).suffix();
        if( ext == "kdev4" || ext == "ktechlab" ) {
            core->projectController()->openProject( url );
            openProject = true;
        }
    }

    if ( !openProject ) {
        for ( int i=0; i < args->count(); ++i ) {
            core->documentController()->openDocument( args->url( i ) );
        }
    }

    args->clear(); // Free up some memory
    return app.exec();
}
