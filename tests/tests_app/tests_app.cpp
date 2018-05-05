#include "../src/ktechlab.h"
#include "config.h"
#include "docmanager.h"
#include "electronics/circuitdocument.h"

#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocalizedstring.h>

#include <qdebug.h>
#include <qtest.h>
#include <qtemporaryfile.h>

static const char description[] =
    I18N_NOOP("An IDE for microcontrollers and electronics");

class KtlTestsAppFixture : public QObject {
    Q_OBJECT

public:
    KApplication *app;
    KTechlab *ktechlab;

private slots:
    void initTestCase() {
        int argc = 1;
        char argv0[] = "tests_app";
        char *argv[] = { argv0, NULL };

        KAboutData about(QByteArray("ktechlab"), QByteArray("ktechlab"), ki18n("KTechLab"), VERSION, ki18n(description),
                    KAboutData::License_GPL, ki18n("(C) 2003-2017, The KTechLab developers"),
                    KLocalizedString(), "https://userbase.kde.org/KTechlab", "ktechlab-devel@kde.org" );
        KCmdLineArgs::init(argc, argv, &about);
        app = new KApplication;
        ktechlab = new KTechlab;

    }
    void cleanupTestCase() {
        delete ktechlab;
        ktechlab = NULL;
        //delete app; // this crashes apparently
        app = NULL;
    }

    void testDocumentOpen() {
        DocManager::self()->closeAll();
        QCOMPARE( DocManager::self()->m_documentList.size(), 0);
        QFile exFile(SRC_TESTS_DATA_DIR "test-document-draw-1.circuit");
        KUrl exUrl(exFile.fileName());
        qDebug() << "open example: " << exUrl;
        DocManager::self()->openURL(exUrl, NULL);
        QCOMPARE( DocManager::self()->m_documentList.size(), 1);
        Document *doc = DocManager::self()->m_documentList.first();
        QVERIFY( doc != NULL );
        QCOMPARE( doc->type(), Document::dt_circuit );
        CircuitDocument *circDoc = static_cast<CircuitDocument*>( doc );
        QVERIFY( circDoc != NULL );
        QVERIFY( circDoc->m_canvas );
        qDebug() << "item list size " << circDoc->m_itemList.size();

        //QRect saveArea = circDoc->m_canvas->rect();   // is empty
        QRect resizeArea(0, -500, 400, 1080);
        qDebug() << " resizeArea " << resizeArea;
        circDoc->m_canvas->resize(resizeArea);

        QRect saveArea(-500, -500, 1040, 1080);
        qDebug() << "save area " << saveArea;
        QPixmap *outputImage = new QPixmap( saveArea.size() );
        outputImage->fill(Qt::green);

        circDoc->exportToImageDraw(saveArea, *outputImage);

        QImage img = dynamic_cast<QPixmap*>(outputImage)->convertToImage();
        img = img.copy();
        QTemporaryFile imgFile("testDocumentOpen_output_XXXXXX.png");
        imgFile.setAutoRemove(false);
        imgFile.open();
        qDebug() << "imgFile.fileName() = " << imgFile.fileName();
        bool saveResult = img.save(imgFile.fileName());
        QCOMPARE( saveResult, true );
    }
};

QTEST_MAIN(KtlTestsAppFixture)
#include "tests_app.moc"
