/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "diagnosticstyle.h"
#include "ktechlab.h"
#include "logtofilemsghandler.h"

//#include <dcopclient.h>
#include <config.h>
#include <ktechlab_version.h>

#include <KAboutData>
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>

int main(int argc, char **argv)
{
    // enable high dpi support
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);

    LogToFileMsgHandler logFileHandler;
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("ktechlab");

    KAboutData about(
        "ktechlab", i18n("KTechLab"), KTECHLAB_VERSION_STRING, i18n("An IDE for microcontrollers and electronics"), KAboutLicense::GPL_V2, i18n("(C) 2003-2017, The KTechLab developers"), "", "https://userbase.kde.org/KTechlab", "ktechlab-devel@kde.org");
    about.addAuthor(i18n("Alan Grimes"), i18n("Developer, Simulation"), "");
    about.addAuthor(i18n("Zoltan Padrah"), i18n("Developer"), "zoltan_padrah@users.sourceforge.net");
    about.addAuthor(i18n("Julian Bäume"), i18n("Developer, KDE4 Port, GUI"), "julian@svg4all.de");
    about.addAuthor(i18n("Juan De Vincenzo"), i18n("KDE4 Port"), "");
    about.addCredit(i18n("Lawrence Shafer"), i18n("Website, wiki and forum"), "");
    about.addCredit(i18n("Jason Lucas"), i18n("Keeping up the project during lack of developers"), "");
    about.addCredit(i18n("David Saxton"), i18n("Former developer, project founder, former maintainer"), "david@bluehaze.org");
    about.addCredit(i18n("Daniel Clarke"), i18n("Former developer"), "daniel.jc@gmail.com");
    about.addCredit(i18n("Couriousous"), i18n("JK flip-flop, asynchronous preset/reset in the D flip-flop"), "");
    about.addCredit(i18n("John Myers"), i18n("Rotary Switch"), "");
    about.addCredit(i18n("Ali Akcaagac"), i18n("Glib friendliness"), "");
    about.addCredit(i18n("David Leggett"), i18n("Former website hosting and feedback during early development"), "");
    KAboutData::setApplicationData(about);
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("ktechlab")));

    QCommandLineParser parser;
    about.setupCommandLine(&parser);
    // 2019.10.03 - note: to add options to set icon and caption of the
    //              application's window? currently this is not implemented
    //              but it had references in the .destop file
    parser.addPositionalArgument(QStringLiteral("url"), i18n("Document to open."), QStringLiteral("[url]"));

    parser.process(app);
    about.processCommandLine(&parser);

    if (true) { // TODO add a command line option for debugging the program's visual look
        // app.setStyle(new DiagnosticStyle());
    }

    // register ourselves as a dcop client
    // app.dcopClient()->registerAs(app.name(), false);

    KTechlab *ktechlab = new KTechlab();

    // 2019.10.03 - note: possibly add support for multiple URLs to be opened from
    //              command line?
    if (parser.positionalArguments().count() > 0) {
        const QUrl url = QUrl::fromUserInput(parser.positionalArguments().at(0), QDir::currentPath(), QUrl::AssumeLocalFile);
        ktechlab->load(url);
    }

    ktechlab->show();
    return app.exec();
}
