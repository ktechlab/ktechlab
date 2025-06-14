/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef NEWFILEDLG_H
#define NEWFILEDLG_H

#include <QDialog>

class MicroSelectWidget;
class NewFileWidget;
// class Q3IconViewItem; // 2018.06.02 - not used

/**
A standard dialog for getting file details from the user for a new project
@short Dialog for new file details
@author David Saxton
*/
class NewFileDlg : public QDialog
{
    Q_OBJECT
public:
    NewFileDlg(QWidget *parent);

    void accept() override;

    int fileType() const
    {
        return m_fileType;
    }
    int codeType() const
    {
        return m_codeType;
    }
    bool addToProject() const
    {
        return m_bAddToProject;
    }
    QString microID() const
    {
        return m_microID;
    }
    MicroSelectWidget *microSelectWidget() const;

public Q_SLOTS:
    void fileTypeChanged();

protected:
    int m_fileType;
    int m_codeType;
    bool m_bAddToProject;
    QString m_microID;

    NewFileWidget *m_pNewFileWidget;
    QWidget *m_pMainParent;
};

#endif
