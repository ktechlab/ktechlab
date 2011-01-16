/*
*/
#include <qprocesswitharguments.h>

#include "QDebug"

QProcessWithArguments::QProcessWithArguments(QObject* parent): QProcess(parent)
{

}

void QProcessWithArguments::start()
{
    qCritical() << "not implemented"; // FIXME
}

QProcessWithArguments& QProcessWithArguments::operator<<(QString args)
{
    qCritical() << "not implemented"; // FIXME
    return *this;
}
