#include <QtTest/QtTest>

#include "Eigen/LU"
// #include "tested/matrix.h"

 class MyFirstTest : public QObject
 {
     Q_OBJECT
 private slots:
     void initTestCase() { 
        qDebug("called before everything else"); 
     }
     
     void myFirstTest()   { 
        QVERIFY(1 == 1); 
     }
     
     void mySecondTest() { 
        QVERIFY(1 != 2); 
     }
     
     void cleanupTestCase(){ 
        qDebug("called after myFirstTest and mySecondTest"); 
     }
 };
 
QTEST_MAIN(MyFirstTest)
#include "matrixtester.moc"
 
 
