/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef OUTPUTMETHODDLG_H
#define OUTPUTMETHODDLG_H

#include <QDialog>
#include <QUrl>

class TextDocument;
class KTechlab;
class MicroSelectWidget;
class OutputMethodDlg;
class OutputMethodWidget;

class OutputMethodInfo
{
public:
    class Method
    {
    public:
        enum Type { Direct, SaveAndForget, SaveAndLoad };
    };

    OutputMethodInfo();
    void initialize(OutputMethodDlg *dlg);

    Method::Type method() const
    {
        return m_method;
    }
    void setMethod(Method::Type method)
    {
        m_method = method;
    }

    bool addToProject() const
    {
        return m_bAddToProject;
    }
    void setAddToProject(bool add)
    {
        m_bAddToProject = add;
    }

    QString picID() const
    {
        return m_picID;
    }
    void setPicID(const QString &id)
    {
        m_picID = id;
    }

    /// Only local filesystem urls supported
    QUrl outputFile() const
    {
        return m_outputFile;
    }
    void setOutputFile(const QUrl &outputFile)
    {
        m_outputFile = outputFile;
    }

protected:
    Method::Type m_method;
    bool m_bAddToProject;
    QString m_picID;
    QUrl m_outputFile;
};

/**
@author David Saxton
*/
class OutputMethodDlg : public QDialog
{
    Q_OBJECT
public:
    /**
     * @param caption The caption of the dialog window
     * @param inputURL Used for saving/restoring previous options the user has selected for this file; set this to null if temporary file
     * @param showPICSelect Whether to show the combo boxes for selecting a PIC
     */
    OutputMethodDlg(const QString &caption, const QUrl &inputURL, bool showPICSelect = false, QWidget *parent = nullptr);
    ~OutputMethodDlg() override;

    void setOutputExtension(const QString &outputExtension);
    void setFilter(const QString &filter);
    void setMethod(OutputMethodInfo::Method::Type m);
    void setOutputFile(const QUrl &out);
    void setPicID(const QString &id);

    void accept() override;

    OutputMethodInfo info() const
    {
        return m_outputMethodInfo;
    }

    MicroSelectWidget *microSelect() const;

protected:
    OutputMethodWidget *m_widget;
    QString m_outputExtension;
    QUrl m_inputURL;
    OutputMethodInfo m_outputMethodInfo;

    friend class OutputMethodInfo;
};

#endif
