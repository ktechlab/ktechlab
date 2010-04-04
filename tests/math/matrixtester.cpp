#include <matrix.h>
#include <quickmatrix.h>
#include <quickvector.h>

#include "Eigen/Core"
#include "Eigen/LU"
#include "Eigen/Array"

#include <QtTest/QtTest>

#include <math.h>

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
      if( (unsigned int)eig.size() != (unsigned int)our.size()){
            qDebug("test case broken. comparing vectors of different size!");
            return 1e12;
        }

        double ret = 0;
        for(unsigned int x=0; x<our.size(); x++){

            if( isnan( eig(x) ) )
                qWarning("NaN value in vector");
            else
                if( isnan( our.at(x) )  ){
                    qCritical("illegal NaN value in solution vector");
                    return 2e12;
                }

            if( isinf( eig(x) ) )
                qWarning("Inf value in vector");
            else
                if( isinf( our.at(x) ) ){
                    qCritical("illegal Inf value in solution vector");
                    return 3e12;
                }

            ret += abs( our.at(x) - eig(x));
        }

        if( ret < 0 ){
            qDebug("we have a problem with absolute difference");
            std::cout << "eig: " << eig << "\n";
            std::cout << "our: \n" ;
            our.dumpToAux();
        }
        return ret;
    }
 
    // create matrix and vector from eigen expression, then perform solving
    // and check the results
    void solveTest(Eigen::MatrixXd &em, Eigen::VectorXd &ev){
        Matrix *pm = matrixFromEigen(em);
        QVERIFY( pm != 0 );
        // FIXME can't get the size of our matrix
        // QVERIFY( pm ?? , em->size() );

        // for debugging this method/function
        // #define DEBUG_SOLVETEST

        #ifdef DEBUG_SOLVETEST
        std::cout << "pm = ";
        pm->displayMatrix(std::cout);
        std::cout << "\n";
        #endif

        QuickVector *pv = vectorFromEigen(ev);
        QVERIFY( pv != 0 );
        QVERIFY( pv->size() == (unsigned int)ev.size() );

        #ifdef DEBUG_SOLVETEST
        std::cout << "pv = ";
        pv->dumpToAux();
        std::cout << "\n";
        #endif

        Eigen::LU<Eigen::MatrixXd> eLU(em);
        pm->performLU();

        // solve
        Eigen::VectorXd esol( em.cols() );
        eLU.solve(ev, &esol);

        pm->fbSub(pv);

        #ifdef DEBUG_SOLVETEST
        std::cout << "pv(solved) = ";
        pv->dumpToAux();
        std::cout << "\n";
        #endif

        double diff ;
        diff = differenceOfVectors(esol, pv);

        #ifdef DEBUG_SOLVETEST
        std::cout << "diff = " << diff << "\n";
        #endif

        if( diff > SOLVE_ERROR) {
            qDebug("solving test failed. dumping matrixes:\n");
            std::cout << "Eigen stuff: \nem, matrix =\n" << em
                << "\nev other side vector = \n" << ev
                << "\nesol unknown vector = \n" << esol
                << "\nCurrent matrixes:\npm matrix = \n";
            pm->displayMatrix(std::cout);
            std::cout << "\npv(sol) solution of equation = \n";
            pv->dumpToAux();
            //           std::cout << "\ndiff = " << diff << "\n";
        }
        QVERIFY(diff < SOLVE_ERROR);

        // multiply back, just for fun
        QuickVector res( em.rows() );
        pm->multiply(pv, &res);

        Eigen::VectorXd eres = em * esol;
        diff = differenceOfVectors(eres, res);
        if( diff > MULTIPLY_ERROR ) {
            qDebug("solving test, multiplying the result back failed. \
                Dumping matrixes");
            std::cout << "Eigen stuff: \nem, matrix =\n" << em
                << "\nev other side vector = \n" << ev
                << "\nesol unknown vector = \n" << esol
                << "\nCurrent matrixes:\npm matrix = \n";
            pm->displayMatrix(std::cout);
            std::cout << "\npv(sol) solution of equation = \n";
            pv->dumpToAux();
            //            std::cout << "\ndiff = " << diff << "\n";

        }
        QVERIFY(diff < MULTIPLY_ERROR);

        delete pm;
        delete pv;
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
 
   void test1x1_0(){
        // to make this test verbose
        // #define TEST1X1_VERBOSE

        Matrix m(1);
        m.g(0,0) = 1;

        QuickVector v(1);
        v.atPut(0, 1);

        #ifdef TEST1X1_VERBOSE
        std::cout <<"v = \n";
        v.dumpToAux();
        std::cout << "\n";
        #endif

        // FIXME the matrix solver doesn't complain if the LU has not been performed since last change
        m.performLU();

        m.fbSub(&v);

        #ifdef TEST1X1_VERBOSE
        std::cout << "matirx LU:\n";
        m.displayLU(std::cout);
        std::cout << "\nmatrix = \n";
        m.displayMatrix(std::cout);
        std::cout << "\nresult = \n";
        v.dumpToAux();
        std::cout << "\n";
        #endif

        QVERIFY( v.at(0) == 1 );
    }

    void test1x1_1(){
        Eigen::MatrixXd m(1,1);
        m << 1 ;
        Eigen::VectorXd v(1);
        v << 1;
        solveTest(m,v);
    }

    void test2x2_1(){
        Eigen::MatrixXd t(2,2);
        t(0,0) = 1;
        t(0,1) = 0;
        t(1,0) = 0;
        t(1,1) = 1;

        Eigen::VectorXd v(2);
        v(0) = 0;
        v(1) = 0;
        solveTest(t, v);

        v(0) = 1;
        v(1) = 1;
        solveTest(t, v);

        v(0) = 0;
        v(1) = 1;
        solveTest(t, v);

        v(0) = 1;
        v(1) = 0;
        solveTest(t, v);

        t(1,0) = 1;
        solveTest(t, v);

        t(0,0) = 0;
        t(0,1) = 1;
        t(1,0) = 1;
        t(1,1) = 0;
        solveTest(t, v);

    }

    void testFuzzySparse(){
        const int maxcol = 7;
        const int maxrow = 7;
        Eigen::MatrixXd m(maxrow,maxcol);
        Eigen::VectorXd v(maxrow);
        v.setRandom();

        int row, col;
        srand( time(NULL) );
        // nonz = number of nonzero elements
        for(int nonz=1;nonz<10; nonz++){
            // fill m with nonz nonzero elements
        m.setZero(maxrow,maxcol);
        m.setZero();
        for(int i=0; i<nonz; i++){
            do {
            row = (int) (5.0*rand()/RAND_MAX);
            col = (int) (5.0*rand()/RAND_MAX);
            } while( m(row, col) != 0 );
            m( row, col) = 1;
        }
            // got m, now test
        solveTest(m, v);
        }
    }

    void cleanupTestCase(){ 
        qDebug("called after myFirstTest and mySecondTest"); 
    }
};
 
QTEST_MAIN(MatrixTester)
#include "matrixtester.moc"

