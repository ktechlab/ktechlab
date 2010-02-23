#include "tested/matrix.h"
#include "tested/quickmatrix.h"
#include "tested/quickvector.h"

#include "Eigen/Core"
#include "Eigen/LU"
#include "Eigen/Array"

#include <QtTest/QtTest>

// using namespace Eigen;

#define     VERY_SMALL_THRESHOLD    1e-12
#define     MULTIPLY_ERROR          1e-10
#define     SOLVE_ERROR             1e-10

#undef LOT_OF_DEBUG

class MatrixTester : public QObject
{
    Q_OBJECT

     // create a new vector from eigen one
     QuickVector * vectorFromEigen(const Eigen::VectorXd & eig){
        QuickVector *ret = new QuickVector( eig.size() );
        for(int i=0; i<eig.size(); i++)
            ret->atPut(i, eig[i]);
        return ret;
     }
 
    // create a new matrix from an eigen matrix
    Matrix * matrixFromEigen(const Eigen::MatrixXd & eig){
        if( eig.rows() != eig.cols() ){
            qDebug("Broken testcase! matrix not square!\n");
            return NULL;
        }
 
        Matrix *ret = new Matrix(eig.rows()) ;
        for(int x=0; x<eig.cols(); x++)
            for(int y=0; y<eig.rows(); y++)
                ret->g(x,y) = eig(x,y);
 
        // some testing
        #ifdef LOT_OF_DEBUG
        std::cout << "original matrix:\n" << eig << "\n";
        std::cout << "new matrix:\n";
        ret->displayMatrix(std::cout);
        std::cout << "\n";
        #endif
        return ret;
    }
 
    // compare 2 matrxies. take the sum of absolute values of diffrences
    double differenceOfMatrixes(const Eigen::MatrixXd &eig, Matrix &our){
                    // FIXME can't make "our" constant, as no there is no "get" api 
        // if( (eig.cols() != our. ??? // TODO no API to get matrxi size!
        double ret = 0;
        for(int x = 0; x<eig.cols(); x++)
            for(int y=0; y<eig.rows(); y++)
                ret += abs( our.g(x,y) - eig(x,y));
        return ret;
    }
 
    // compare an eigen vector to quickvector
    double differenceOfVectors(const Eigen::VectorXd &eig, const QuickVector &our){
        if( eig.size() != our.size()){
            qDebug("test case broken. comparing vectors of different size!");
            return 1e12;
        }
        double ret = 0;
        for(int x=0; x<our.size(); x++)
            ret += abs( our.at(x) - eig(x));
        return ret;
    }
 
 
private slots:
 
    void initTestCase() { 
        qDebug("called before everything else"); 
    }

    void mySecondTest() { 
        QVERIFY(1 != 2); 
    }

    void getSetVectorFuzzerTest() {
        qDebug("starting");
 
        for( int i = 1; i < 10 ; i ++ ){
            Eigen::VectorXd ref = Eigen::VectorXd::Random(i);
 
        }
    }
 
    void getSetMatrixFuzzerTest() {
        qDebug("starting");
 
        for( int i = 1; i < 10 ; i++ ) {
            Eigen::MatrixXd ref = Eigen::MatrixXd::Random(i,i);
 
            Matrix * tested = matrixFromEigen( ref );
            QVERIFY( tested != NULL );
 
            double dif =  differenceOfMatrixes( ref, *tested );
 
            if( dif >= VERY_SMALL_THRESHOLD )
                tested->displayMatrix(std::cerr);
            QVERIFY( dif < VERY_SMALL_THRESHOLD );
 
            delete tested;
        }
     }
 
     void multiplyFuzzerTest() {
        qDebug("starting");
 
        for( int i = 1; i < 10; i++ ){
 
            // create matrix
            Eigen::MatrixXd refM = Eigen::MatrixXd::Random(i,i);
 
            Matrix * testedM = matrixFromEigen( refM );
            QVERIFY( testedM != NULL );
 
            // create vector
            Eigen::VectorXd refV = Eigen::VectorXd::Random(i);
            QuickVector * testedV = vectorFromEigen( refV );
            QVERIFY( testedV != NULL );
 
            // multiply them
            Eigen::VectorXd refRes = refM * refV;
 
            QuickVector * testedRes = new QuickVector(i);
            QVERIFY( testedRes != NULL );
            testedM->multiply( testedV, testedRes );
 
            double multError = differenceOfVectors(refRes, testedRes);
            if( multError >= MULTIPLY_ERROR ) {
                std::cout << "multiply error. \nrefV: \n" << refV << 
                            "\nrefM:\n" << refM << "\nrefRes:\n" << refRes << 
                            "\ntestedM:\n" << std::endl;
                testedM->displayMatrix(std::cout);
                std::cout << "\ntestedV:\n";
                testedV->dumpToAux();
                std::cout << "\ntestedRes:\n";
                testedRes->dumpToAux();
                std::cout <<"\n";
 
            }
            QVERIFY( multError < MULTIPLY_ERROR );
 
            delete testedM;
            delete testedRes;
        }
     }
 
    void solveFuzzerTest() {
        qDebug("starting");
 
        for( int i = 1; i < 10; i++ ){
 
            // create matrix
            Eigen::MatrixXd refM = Eigen::MatrixXd::Random(i,i);
 
            Matrix * testedM = matrixFromEigen( refM );
            QVERIFY( testedM != NULL );
 
            // create vector
 
            // perform LU
            Eigen::LU<Eigen::MatrixXd> refLU(refM);
            testedM->performLU();
 
            for(int j = 0; j < 5; j++) {
                // get some vectors
                Eigen::VectorXd refV = Eigen::VectorXd::Random(i);
                QuickVector * testedV = vectorFromEigen( refV );
                QVERIFY( testedV != NULL );
 
                // solve the eq.
                Eigen::VectorXd refSol(i);
                refLU.solve(refV, &refSol);
 
                testedM->fbSub(testedV);
 
                //check
                double diff = differenceOfVectors(refSol, testedV );
                if( diff >= SOLVE_ERROR){
                    std::cerr << "LU solve error.\n refV:\n" << refV << std::endl;
                    std::cerr << "revM:\n" << refM << std::endl;
                }
                QVERIFY( diff < SOLVE_ERROR);
            }
            delete testedM;
 
       }
   }
 
   void cleanupTestCase(){ 
        qDebug("called after myFirstTest and mySecondTest"); 
     }
};
 
QTEST_MAIN(MatrixTester)
#include "matrixtester.moc"

