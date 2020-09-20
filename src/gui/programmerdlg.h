/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PROGRAMMERDLG_H
#define PROGRAMMERDLG_H

#include <QDialog>

class MicroSelectWidget;
class PicProgrammerSettings;
class ProcessOptions;
class ProgrammerWidget;

/**
@author David Saxton
*/
class ProgrammerDlg : public QDialog
{
    Q_OBJECT
public:
    /**
     * Create a new ProgrammerDlg with the PIC type set to picID. Other
     * options (such as the program to use) will be read in from the
     * settings.
     */
    ProgrammerDlg(const QString &picID, QWidget *parent = nullptr, const char *name = nullptr);
    ~ProgrammerDlg() override;

    /**
     * Initialises options with the values that the user has entered into
     * the widgets.
     */
    void initOptions(ProcessOptions *options);

    MicroSelectWidget *microSelect() const;

protected:
    ProgrammerWidget *m_pProgrammerWidget;
    PicProgrammerSettings *m_pProgrammerSettings;
};

#endif
