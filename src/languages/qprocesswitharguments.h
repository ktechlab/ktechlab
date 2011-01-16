/*
*/
#ifndef _QPROCESSWITHARGUMENTS_H_
#define _QPROCESSWITHARGUMENTS_H_

#include <QObject>
#include <qprocess.h>

class QProcessWithArguments : public QProcess
{
    public:
        QProcessWithArguments(QObject* parent = 0);
        void start();
        QProcessWithArguments & operator<<(QString args);
};


#endif // _QPROCESSWITHARGUMENTS_H_
