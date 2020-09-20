/***************************************************************************
 *   Copyright (C) 2020 by Friedrich W. H. Kossebau - kossebau@kde.org     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

class QString;
class QValidator;
class QWidget;

namespace InputDialog
{
QString getText(const QString &windowTitle, const QString &labelText, const QString &value, bool *ok, QWidget *parent, QValidator *validator);

}

#endif
