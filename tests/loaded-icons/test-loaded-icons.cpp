
#include <KAboutData>
#include <KIconLoader>
#include <KLocalizedString>

#include <QApplication>
#include <QMainWindow>

#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QScrollArea>
#include <QDebug>

#include "config.h"

#include "icon-list-generated.h"

static const char description[] =
    I18N_NOOP("An IDE for microcontrollers and electronics");

    // TODO retun "" if ok, or an error message if it is not ok
static void addIcon(QGridLayout *mainLayout, const char *iconName);

int main(int argc, char **argv) {
    KAboutData about("ktechlab", i18n("KTechLab Icon Tester"), VERSION, i18n(description),
                KAboutLicense::LicenseKey::GPL_V2, i18n("(C) 2003-2009, The KTechLab developers"),
                "", "http://ktechlab.org", "ktechlab-devel@lists.sourceforge.net" );

    QApplication app(argc, argv);
    QMainWindow *mainWnd = new QMainWindow();
    QScrollArea *mainWidget = new QScrollArea();
    QWidget *tableWidget = new QWidget();
    mainWidget->setWidget(tableWidget);
    mainWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    mainWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    mainWidget->setWidgetResizable(true);
    QGridLayout *mainLayout = new QGridLayout();
    tableWidget->setLayout(mainLayout);
    mainWnd->setCentralWidget(mainWidget);

    const int iconCount = sizeof(iconNames)/sizeof(iconNames[0]);
    for (int iconNr = 0; iconNr < iconCount; ++iconNr) {
        addIcon(mainLayout, iconNames[iconNr]);
    }

    mainWnd->show();
    return app.exec();
}

static void addIcon(QGridLayout *mainLayout, const char *iconName) {
    const int atRow = mainLayout->rowCount() + 1;
    {
        QIcon testIconConstr = QIcon::fromTheme(iconName);
        if (testIconConstr.isNull()) {
            qWarning() << "QIcon " << iconName << " is NULL";
        }
    }
    {
        QPixmap testPixmapLoader = KIconLoader::global()->loadIcon(
            QString(iconName), KIconLoader::NoGroup,
            KIconLoader::SizeHuge, KIconLoader::DefaultState,
            QStringList(), 0L, true
        );
        if (testPixmapLoader.isNull()) {
            qWarning() << "loadIcon " << iconName << " is NULL";
        }
    }
    QIcon testIcon(iconName);
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
