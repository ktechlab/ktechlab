#include <kapplication.h>
#include <kmainwindow.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kicon.h>
#include <QtGui/qgridlayout.h>
#include <QtGui/qlabel.h>
#include <qscrollarea.h>

#include "config.h"

static const char description[] =
    I18N_NOOP("An IDE for microcontrollers and electronics");

static void addIcon(QGridLayout *mainLayout, const char *iconName);

int main(int argc, char **argv) {
    KAboutData about(QByteArray("ktechlab"), QByteArray("KTechLab"), ki18n("KTechLab"), VERSION, ki18n(description),
                KAboutData::License_GPL, ki18n("(C) 2003-2009, The KTechLab developers"),
                ki18n(""), "http://ktechlab.org", "ktechlab-devel@lists.sourceforge.net" );

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

    addIcon(mainLayout, "asd");
    addIcon(mainLayout, "rotate_cw");
    addIcon(mainLayout, "rotate_ccw");
    addIcon(mainLayout, "text");
    addIcon(mainLayout, "rotate");
    addIcon(mainLayout, "player_play");
    addIcon(mainLayout, "player_pause");
    addIcon(mainLayout, "stop");
    addIcon(mainLayout, "reload");
    addIcon(mainLayout, "ktechlab_circuit");
    addIcon(mainLayout, "fork");
    addIcon(mainLayout, "convert_to_microbe");
    addIcon(mainLayout, "convert_to_assembly");
    addIcon(mainLayout, "convert_to_hex");
    addIcon(mainLayout, "convert_to_pic");
    addIcon(mainLayout, "down");
    addIcon(mainLayout, "button_cancel");
    addIcon(mainLayout, "folder");
    addIcon(mainLayout, "template-source");
    addIcon(mainLayout, "source_c");
    addIcon(mainLayout, "ktechlab_circuit");
    addIcon(mainLayout, "ktechlab_flowcode");
    addIcon(mainLayout, "exec");
    addIcon(mainLayout, "oscilloscope");
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
    mainLayout->setRowMinimumHeight(atRow, 100);
}
