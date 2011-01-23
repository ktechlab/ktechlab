/***************************************************************************
 *   Copyright (C) 2011 by Zoltan Padrah                                   *
 *   zoltan_padrah@users.sourceforge.net                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <qprocesswitharguments.h>

#include "QDebug"

QProcessWithArguments::QProcessWithArguments(QObject* parent):
    QProcess(parent), m_app("")
{

}

void QProcessWithArguments::start()
{
    qDebug() << "starting executable: " << m_app << " with arguments: "
        << m_args;
    QProcess::start(m_app, m_args);
}

QProcessWithArguments& QProcessWithArguments::operator<<(QString args)
{
    if(m_app == "")
        m_app = args;
    else
        m_args.append(args);
    return *this;
}
