/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "docmanager.h"
#include "filemetainfo.h"
#include "textdocument.h"
#include "outputmethoddlg.h"
#include "microlibrary.h"
#include "microselectwidget.h"
#include "projectmanager.h"

#include <kcombobox.h>
#include <kconfigskeleton.h>
#include <kdebug.h>
#include <ktemporaryfile.h>
#include <kurlrequester.h>

#include <qcheckbox.h>
#include <qfile.h>
#include <qradiobutton.h>

#include <ui_outputmethodwidget.h>


class OutputMethodWidget : public QWidget, public Ui::OutputMethodWidget {
    public:
    OutputMethodWidget(QWidget *parent) : QWidget(parent) {
        setupUi(this);
    }
};


//BEGIN class OutputMethodInfo
OutputMethodInfo::OutputMethodInfo()
{
	m_method = Method::Direct;
	m_bAddToProject = false;
}


void OutputMethodInfo::initialize( OutputMethodDlg * dlg )
{
	if ( dlg->m_widget->displayDirectCheck->isChecked() )
	{
		m_method = Method::Direct;
		//K3TempFile f( QString::null, dlg->m_outputExtension );
        KTemporaryFile f;
        f.setSuffix( dlg->m_outputExtension );
        if (!f.open()) {
            qWarning() << "failed to open " << f.name() << " because " << f.errorString();
        }
		f.close();
		m_outputFile = f.name();
		m_bAddToProject = false;
	}
	
	else
	{
		if ( dlg->m_widget->loadFileCheck->isChecked() )
			m_method = Method::SaveAndLoad;
		
		else
			m_method = Method::SaveAndForget;
		
		m_outputFile = dlg->m_widget->outputFileURL->url();
		m_bAddToProject = dlg->m_widget->addToProjectCheck->isChecked();
	}
	
	m_picID = dlg->m_widget->m_pMicroSelect->micro();
}
//END class OutputMethodInfo



//BEGIN class OutputMethodDlg

OutputMethodDlg::OutputMethodDlg( const QString &caption, const KUrl & inputURL, bool showPICSelect, QWidget *parent, const char *name )
	: // KDialog( parent, name, true, caption, Ok|Cancel )
	KDialog( parent ) //, name, true, caption, Ok|Cancel )
{
    setObjectName(name);
    setModal(true);
    setCaption(caption);
    setButtons(KDialog::Ok | KDialog::Cancel);

	m_inputURL = inputURL;
	m_bAccepted = false;
	m_widget = new OutputMethodWidget(this);
	
	m_widget->addToProjectCheck->setEnabled( ProjectManager::self()->currentProject() );
	
	if (!showPICSelect)
	{
		m_widget->m_pMicroSelect->hide();
		m_widget->adjustSize();
	}
	{
        qDebug() << Q_FUNC_INFO << "outputFileURL: def mode " << m_widget->outputFileURL->mode();
        KFile::Modes openMode = m_widget->outputFileURL->mode();
        openMode &= (~ KFile::ExistingOnly);
        m_widget->outputFileURL->setMode(openMode);
        qDebug() << Q_FUNC_INFO << "outputFileURL: new mode " << m_widget->outputFileURL->mode();
    }

    connect(m_widget->saveFileCheck, SIGNAL(toggled(bool)), m_widget->groupBoxSaveOptions, SLOT(setEnabled(bool)));

	fileMetaInfo()->initializeFromMetaInfo( m_inputURL, this );
	
	setMainWidget(m_widget);
}


OutputMethodDlg::~OutputMethodDlg()
{
}


void OutputMethodDlg::setOutputExtension( const QString & extension )
{
	m_outputExtension = extension;
}


void OutputMethodDlg::setFilter( const QString &filter )
{
	m_widget->outputFileURL->setFilter(filter);
}


void OutputMethodDlg::setMethod( OutputMethodInfo::Method::Type m )
{
	switch (m)
	{
		case OutputMethodInfo::Method::Direct:
			m_widget->displayDirectCheck->setChecked(true);
			break;
			
		case OutputMethodInfo::Method::SaveAndForget:
			m_widget->saveFileCheck->setChecked(true);
			m_widget->loadFileCheck->setChecked(false);
			break;
			
		case OutputMethodInfo::Method::SaveAndLoad:
			m_widget->saveFileCheck->setChecked(true);
			m_widget->loadFileCheck->setChecked(true);
			break;
	};
}


void OutputMethodDlg::setPicID( const QString & id )
{
	m_widget->m_pMicroSelect->setMicro(id);
}


void OutputMethodDlg::setOutputFile( const KUrl & out )
{
	m_widget->outputFileURL->setUrl(out.prettyUrl());
}


void OutputMethodDlg::accept()
{
	m_bAccepted = true;
	m_outputMethodInfo.initialize(this);
	fileMetaInfo()->grabMetaInfo( m_inputURL, this );
	hide();
}


void OutputMethodDlg::reject()
{
    hide();
	m_bAccepted = false;
}


MicroSelectWidget * OutputMethodDlg::microSelect() const
{
	return m_widget->m_pMicroSelect;
}
//END class OutputMethodDlg


#include "outputmethoddlg.moc"
