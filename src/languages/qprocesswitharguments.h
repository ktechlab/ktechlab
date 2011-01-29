/***************************************************************************
 *   Copyright (C) 2011 by Zoltan Padrah                                   *
 *   zoltan_padrah@users.sourceforge.net                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef _QPROCESSWITHARGUMENTS_H_
#define _QPROCESSWITHARGUMENTS_H_

#include <QObject>
#include <qprocess.h>

/**
    A process class that allows building its argument list with
    the help of << operator.
*/
class QProcessWithArguments : public QProcess
{
    public:
        QProcessWithArguments(QObject* parent = 0);
        QStringList args();
        void start();
        QProcessWithArguments & operator<<(QString args);
    private:
        QString m_app;
        QStringList m_args;
};


#endif // _QPROCESSWITHARGUMENTS_H_
