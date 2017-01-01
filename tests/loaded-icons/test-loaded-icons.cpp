#include <kapplication.h>
#include <kmainwindow.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kicon.h>
#include <kiconloader.h>

#include <QtGui/qgridlayout.h>
#include <QtGui/qlabel.h>
#include <qscrollarea.h>
#include <qdebug.h>

#include "config.h"

#include "icon-list-generated.h"

static const char description[] =
    I18N_NOOP("An IDE for microcontrollers and electronics");

    // TODO retun "" if ok, or an error message if it is not ok
static void addIcon(QGridLayout *mainLayout, const char *iconName);

int main(int argc, char **argv) {
    KAboutData about(QByteArray("ktechlab"), QByteArray("KTechLab Icon Tester"), ki18n("KTechLab Icon Tester"), VERSION, ki18n(description),
                KAboutData::License_GPL, ki18n("(C) 2003-2009, The KTechLab developers"),
                KLocalizedString(), "http://ktechlab.org", "ktechlab-devel@lists.sourceforge.net" );

    KCmdLineArgs::init(argc, argv, &about);
    KApplication testLoadedIconsApp;
    KMainWindow *mainWnd = new KMainWindow;
    QScrollArea *mainWidget = new QScrollArea;
    QWidget *tableWidget = new QWidget;
    mainWidget->setWidget(tableWidget);
    mainWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    mainWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    mainWidget->setWidgetResizable(true);
    QGridLayout *mainLayout = new QGridLayout;
    tableWidget->setLayout(mainLayout);
    mainWnd->setCentralWidget(mainWidget);

    const int iconCount = sizeof(iconNames)/sizeof(iconNames[0]);
    for (int iconNr = 0; iconNr < iconCount; ++iconNr) {
        addIcon(mainLayout, iconNames[iconNr]);
    }
    // TODO add assertion if all icons have been properly loaded
    /*
    addIcon(mainLayout, "asd");
    addIcon(mainLayout, "object-rotate-right");
    addIcon(mainLayout, "object-rotate-left");
    addIcon(mainLayout, "text");
    addIcon(mainLayout, "rotate");
    addIcon(mainLayout, "media-playback-start");
    addIcon(mainLayout, "media-playback-pause");
    addIcon(mainLayout, "process-stop");
    addIcon(mainLayout, "view-refresh");
    addIcon(mainLayout, "application-x-circuit");
    addIcon(mainLayout, "fork");
    addIcon(mainLayout, "convert_to_microbe");
    addIcon(mainLayout, "convert_to_assembly");
    addIcon(mainLayout, "convert_to_hex");
    addIcon(mainLayout, "convert_to_pic");
    addIcon(mainLayout, "go-down");
    addIcon(mainLayout, "dialog-cancel");
    addIcon(mainLayout, "folder");
    addIcon(mainLayout, "template-source");
    addIcon(mainLayout, "text-x-csrc");
    addIcon(mainLayout, "application-x-circuit");
    addIcon(mainLayout, "application-x-flowcode");
    addIcon(mainLayout, "exec");
    addIcon(mainLayout, "oscilloscope");
    */
    /*  {
        KIcon testIcon("foo");
        QPixmap testPixmap = testIcon.pixmap(64, 64);
        QLabel *ql = new QLabel;
        ql->setPixmap(testPixmap);
        ql->setIconText("label text");
        mainLayout->addWidget(ql, 1, 1);

        QLabel *qt = new QLabel;
        qt->setText("asd");
        mainLayout->addWidget(qt, 1, 2);
    } */

    mainWnd->show();
    return testLoadedIconsApp.exec();
}

static void addIcon(QGridLayout *mainLayout, const char *iconName) {
    const int atRow = mainLayout->numRows() + 1;
    {
        KIcon testIconConstr(iconName);
        if (testIconConstr.isNull()) {
            qWarning() << "KIcon " << iconName << " is NULL";
        }
    }
    {
        QPixmap  testPixmapLoader = KIconLoader::global()->loadIcon(QString(iconName), KIconLoader::NoGroup,
                                                                    KIconLoader::SizeHuge, KIconLoader::DefaultState,
                                                                    QStringList(), 0L, true
                                                                   );
        if (testPixmapLoader.isNull()) {
            qWarning() << "loadIcon " << iconName << " is NULL";
        }
    }
    KIcon testIcon(iconName);
    QPixmap testPixmap = testIcon.pixmap(64, 64);
    QLabel *ql = new QLabel;
    ql->setPixmap(testPixmap);
    ql->setMinimumSize(70, 70);
    mainLayout->addWidget(ql, atRow, 1);

    QLabel *qt = new QLabel;
    qt->setText(iconName);
    qt->setMinimumSize(300, 70);
    mainLayout->addWidget(qt, atRow, 2);
    mainLayout->setRowMinimumHeight(atRow, 70);
}
