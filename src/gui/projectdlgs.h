/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PROJECTDLGS_H
#define PROJECTDLGS_H

#include <QDialog>

class CreateSubprojectWidget;
class LinkerOptions;
class LinkerOptionsWidget;
class KUrlRequester;
class NewProjectWidget;
class ProcessingOptions;
class ProcessingOptionsWidget;
class ProjectItem;
class QDialogButtonBox;

/**
A standard dialog for getting project details from the user for a new project
@short Dialog for new project details
@author David Saxton
*/
class NewProjectDlg : public QDialog
{
    Q_OBJECT
public:
    NewProjectDlg(QWidget *parent);

    /**
     * Called when the 'OK' button is pressed.
     * User entered values are read in
     */
    void accept() override;

    QString projectName() const
    {
        return m_projectName;
    }
    QString projectLocation() const
    {
        return m_projectLocation;
    }
    QString location() const
    {
        return m_location;
    }

public Q_SLOTS:
    /**
     * Called when the projectName or projectLocation edit boxes are edited.
     * Checks whether the resultant location combination is a valid path -
     * if so, enables the OK button; otherwise disables it.
     */
    void locationChanged(const QString &);

protected:
    NewProjectWidget *m_pWidget;
    QDialogButtonBox *m_buttonBox;
    QString m_projectName;
    QString m_projectLocation;
    QString m_location;
};

/**
@author David Saxton
*/
class CreateSubprojectDlg : public QDialog
{
    Q_OBJECT
public:
    CreateSubprojectDlg(QWidget *parent = nullptr);
    ~CreateSubprojectDlg() override;

    // The following values should agree with the positions in the combo box
    enum Type { ProgramType = 0, LibraryType = 1 };

    /**
     * Called when the 'OK' button is pressed. User entered values are read
     * in.
     */
    void accept() override;

    Type type() const
    {
        return m_type;
    }
    QString targetFile() const
    {
        return m_targetFile;
    }

protected:
    CreateSubprojectWidget *m_pWidget;
    Type m_type;
    QString m_targetFile;
};

/**
@author David Saxton
*/
class LinkerOptionsDlg : public QDialog
{
    Q_OBJECT
public:
    LinkerOptionsDlg(LinkerOptions *linkingOptions, QWidget *parent = nullptr);
    ~LinkerOptionsDlg() override;

    /**
     * Called when the 'OK' button is pressed. User entered values are read
     * in.
     */
    void accept() override;

protected:
    LinkerOptions *m_pLinkerOptions;
    LinkerOptionsWidget *m_pWidget;
    KUrlRequester *m_pExternalLibraryRequester;
};

/**
@author David Saxton
*/
class ProcessingOptionsDlg : public QDialog
{
public:
    ProcessingOptionsDlg(ProjectItem *projectItem, QWidget *parent = nullptr);
    ~ProcessingOptionsDlg() override;

    /**
     * Called when the 'OK' button is pressed. User entered values are read
     * in.
     */
    void accept() override;

protected:
    ProjectItem *m_pProjectItem;
    ProcessingOptionsWidget *m_pWidget;
};

#endif
