#include "../src/ktechlab.h"
#include "config.h"

#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocalizedstring.h>

static const char description[] =
    I18N_NOOP("An IDE for microcontrollers and electronics");
    
int main(int argc, char *argv[]) {
    KAboutData about(QByteArray("ktechlab"), QByteArray("ktechlab"), ki18n("KTechLab"), VERSION, ki18n(description),
                KAboutData::License_GPL, ki18n("(C) 2003-2017, The KTechLab developers"),
                KLocalizedString(), "https://userbase.kde.org/KTechlab", "ktechlab-devel@kde.org" );
    KCmdLineArgs::init(argc, argv, &about);
    KApplication app;
    KTechlab *ktechlab = new KTechlab();
}
