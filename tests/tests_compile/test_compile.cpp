/*
 * KTechLab: An IDE for microcontrollers and electronics
 * Copyright 2018  Zoltan Padrah <zoltan_padrah@users.sf.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "../src/ktechlab.h"

#include <KAboutData>
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineParser>

#include <ktechlab_version.h>
#include <ktechlab_debug.h>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    KAboutData aboutData(QByteArray("ktechlab"), i18n("KTechLab"),
                KTECHLAB_VERSION_STRING, i18n("An IDE for microcontrollers and electronics"),
                KAboutLicense::GPL_V2, i18n("(C) 2003-2017, The KTechLab developers"),
                "", "https://userbase.kde.org/KTechlab", "ktechlab-devel@kde.org" );
    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KTechlab *ktechlab = new KTechlab();
    qCInfo(KTL_LOG) << "ktechlab instance:" << ktechlab;
}
