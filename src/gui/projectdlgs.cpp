/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "microlibrary.h"
#include "microselectwidget.h"
#include "projectdlgs.h"
#include "projectmanager.h"

#include <cassert>

#include <kcombobox.h>
#include <kfiledialog.h>
#include <klineedit.h>
#include <klocalizedstring.h>
#include <kurlrequester.h>
// #include <k3listview.h>

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qdebug.h>

#include <ui_createsubprojectwidget.h>
#include <ui_linkeroptionswidget.h>
#include <ui_newprojectwidget.h>
#include <ui_processingoptionswidget.h>

class NewProjectWidget : public QWidget, public Ui::NewProjectWidget {
    public:
    NewProjectWidget(QWidget *parent) : QWidget(parent) {
        setupUi(this);
    }
};

//BEGIN class NewProjectDlg
NewProjectDlg::NewProjectDlg( QWidget * parent )
	: // KDialog( parent, "newprojectdlg", true, "New Project", KDialog::Ok|KDialog::Cancel, KDialog::Ok, true )
      KDialog( parent ) //, "newprojectdlg", true, "New Project", KDialog::Ok|KDialog::Cancel, KDialog::Ok, true )
{
    setObjectName("newprojectdlg");
    setModal(true);
    setCaption(i18n("New Project"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setDefaultButton(KDialog::Ok);
    showButtonSeparator(true);

	m_pWidget = new NewProjectWidget(this);
	connect( m_pWidget->projectNameEdit, SIGNAL(textChanged(const QString & )), this, SLOT(locationChanged(const QString& )) );
	connect( m_pWidget->projectLocationURL, SIGNAL(textChanged(const QString & )), this, SLOT(locationChanged(const QString& )) );
    
    // Check if already valid dir
	locationChanged( QString::null );
    
	m_pWidget->projectLocationURL->setUrl( QDir::homeDirPath() );
	m_pWidget->projectLocationURL->setMode( KFile::Directory );
    
	setMainWidget( m_pWidget );
	setInitialSize( m_pWidget->rect().size() );
}

void NewProjectDlg::accept()
{
	hide();

	m_bAccepted = true;

	m_projectName = m_pWidget->projectNameEdit->text();
	m_projectLocation = m_pWidget->projectLocationURL->url().toLocalFile();
}

void NewProjectDlg::reject()
{
	m_bAccepted = false;
    hide();
}

void NewProjectDlg::locationChanged( const QString & )
{
	m_location = m_pWidget->projectLocationURL->url().toLocalFile();
    qDebug() << "location changed to: " << m_location;
	QDir subDir(m_location);
    
	if ( !m_location.endsWith("/") )
		m_location.append("/");
    
	if ( !m_pWidget->projectNameEdit->text().isEmpty() )
		m_location.append( m_pWidget->projectNameEdit->text().lower() + "/" );
    
	m_pWidget->locationLabel->setText( m_location );
    
	QDir dir(m_location);

    qDebug() << "dir.exists: " << dir.exists() << " subdir.exists: " << subDir.exists();
	
	if ( dir.exists() || !subDir.exists() ) {
		enableButtonOk(false);
    } else {
		enableButtonOk(true);
    }
}
//END class NewProjectDlg


class CreateSubprojectWidget : public QWidget, public Ui::CreateSubprojectWidget {
    public:
    CreateSubprojectWidget(QWidget *parent) : QWidget(parent) {
        setupUi(this);
    }
};

//BEGIN class CreateSubprojectDlg
CreateSubprojectDlg::CreateSubprojectDlg( QWidget * parent )
	: // KDialog( parent, "Create Subproject Dialog", true, "Create Subproject", KDialog::Ok|KDialog::Cancel, KDialog::Ok, true )
	KDialog( parent ) // , "Create Subproject Dialog", true, "Create Subproject", KDialog::Ok|KDialog::Cancel, KDialog::Ok, true )
{
    setObjectName("Create Subproject Dialog");
    setModal(true);
    setCaption(i18n("Create Subproject"));
    setButtons(KDialog::Ok|KDialog::Cancel);
    setDefaultButton(KDialog::Ok);
    showButtonSeparator(true);

	m_pWidget = new CreateSubprojectWidget(this);
	
	if ( ProjectManager::self()->currentProject() )
		m_pWidget->m_targetFile->setUrl( ProjectManager::self()->currentProject()->directory() );
	
	m_type = ProgramType;
    
	setMainWidget( m_pWidget );
	setInitialSize( m_pWidget->rect().size() );
}


CreateSubprojectDlg::~CreateSubprojectDlg()
{
}


void CreateSubprojectDlg::accept()
{
	hide();

	m_bAccepted = true;

	m_targetFile = m_pWidget->m_targetFile->url().toLocalFile();
	m_type = (Type)m_pWidget->m_typeCombo->currentItem();
}


void CreateSubprojectDlg::reject()
{
	m_bAccepted = false;
    hide();
}
//END class CreateSubprojectDlg

class LinkerOptionsWidget : public QWidget, public Ui::LinkerOptionsWidget {
    public:
    LinkerOptionsWidget(QWidget *parent) : QWidget(parent) {
        setupUi(this);
    }
};

//BEGIN class LinkerOptionsDlg
LinkerOptionsDlg::LinkerOptionsDlg( LinkerOptions * linkingOptions, QWidget *parent )
	: // KDialog( parent, "Linker Options Dialog", true, "Linker Options", KDialog::Ok|KDialog::Cancel, KDialog::Ok, true )
	KDialog( parent) //, "Linker Options Dialog", true, "Linker Options", KDialog::Ok|KDialog::Cancel, KDialog::Ok, true )
{
    setObjectName("Linker Options Dialog");
    setModal(true);
    setCaption(i18n("Linker Options"));
    setButtons(KDialog::Ok|KDialog::Cancel);
    setDefaultButton(KDialog::Ok);
    showButtonSeparator(true);


	m_pLinkerOptions = linkingOptions;
	m_pWidget = new LinkerOptionsWidget(this);
	
	ProjectInfo * pi = ProjectManager::self()->currentProject();
	assert(pi);
	
	
	//BEGIN Update gplink options
	m_pWidget->m_pHexFormat->setCurrentIndex( m_pLinkerOptions->hexFormat() );
	m_pWidget->m_pOutputMap->setChecked( m_pLinkerOptions->outputMapFile() );
	m_pWidget->m_pLibraryDir->setText( m_pLinkerOptions->libraryDir() );
	m_pWidget->m_pLinkerScript->setText( m_pLinkerOptions->linkerScript() );
	m_pWidget->m_pOther->setText( m_pLinkerOptions->linkerOther() );
	//END Update gplink options
	
	
	
	//BEGIN Update library widgets
	const KUrl::List availableInternal = pi->childOutputURLs( ProjectItem::LibraryType );
	const QStringList linkedInternal = m_pLinkerOptions->linkedInternal();
	
	KUrl::List::const_iterator end = availableInternal.end();
	for ( KUrl::List::const_iterator it = availableInternal.begin(); it != end; ++it )
	{
		QString relativeURL = KUrl::relativeUrl( pi->url(), *it );
        // 2017.12.1 - convert to QListWidgetItem
		//Q3CheckListItem * item = new Q3CheckListItem( m_pWidget->m_pInternalLibraries, relativeURL, Q3CheckListItem::CheckBox );
        QListWidgetItem * item = new QListWidgetItem( relativeURL, m_pWidget->m_pInternalLibraries );
        item->setCheckState( (linkedInternal.contains(relativeURL)) ? Qt::Checked : Qt::Unchecked );
		//item->setOn( linkedInternal.contains(relativeURL) ); // 2017.12.1 - convert to QListWidgetItem
	}
	
	m_pExternalLibraryRequester = new KUrlRequester( 0l );
	m_pExternalLibraryRequester->fileDialog()->setUrl( KUrl( "/usr/share/sdcc/lib" ) );
	
	delete m_pWidget->m_pExternalLibraries;
	m_pWidget->m_pExternalLibraries = new KEditListBox( i18n("Link libraries outside project"), m_pExternalLibraryRequester->customEditor(), m_pWidget );
	m_pWidget->m_pExternalLibraries->layout()->setMargin(11);
	(dynamic_cast<QGridLayout*>(m_pWidget->layout()))->addMultiCellWidget( m_pWidget->m_pExternalLibraries, 7, 7, 0, 1 );
	
	m_pWidget->m_pExternalLibraries->setButtons( KEditListBox::Add | KEditListBox::Remove );
	m_pWidget->m_pExternalLibraries->insertStringList( m_pLinkerOptions->linkedExternal() );
	//END Update library widgets
	
	
	setMainWidget( m_pWidget );
	setInitialSize( m_pWidget->rect().size() );
}


LinkerOptionsDlg::~LinkerOptionsDlg()
{
	delete m_pExternalLibraryRequester;
}


void LinkerOptionsDlg::accept()
{
	hide();
	
	QStringList linkedInternal;
	for (int itemNr = 0; itemNr < m_pWidget->m_pInternalLibraries->count(); ++itemNr)
	{
        QListWidgetItem * item = m_pWidget->m_pInternalLibraries->item(itemNr);
		if ( item->checkState() == Qt::Checked ) {
			linkedInternal << item->text();
        }
	}
	m_pLinkerOptions->setLinkedInternal( linkedInternal );
	
	m_pLinkerOptions->setLinkedExternal( m_pWidget->m_pExternalLibraries->items() );
	m_pLinkerOptions->setHexFormat( (LinkerOptions::HexFormat::type) m_pWidget->m_pHexFormat->currentItem() );
	m_pLinkerOptions->setOutputMapFile( m_pWidget->m_pOutputMap->isChecked() );
	m_pLinkerOptions->setLibraryDir( m_pWidget->m_pLibraryDir->text() );
	m_pLinkerOptions->setLinkerScript( m_pWidget->m_pLinkerScript->text() );
	m_pLinkerOptions->setLinkerOther( m_pWidget->m_pOther->text() );
}


void LinkerOptionsDlg::reject()
{
    hide();
}
//END class LinkerOptionsDlg

class ProcessingOptionsWidget : public QWidget, public Ui::ProcessingOptionsWidget {
    public:
    ProcessingOptionsWidget(QWidget *parent) : QWidget(parent) {
        setupUi(this);
    }
};


//BEGIN class ProcessingOptionsDlg
ProcessingOptionsDlg::ProcessingOptionsDlg( ProjectItem * projectItem, QWidget *parent )
	: // KDialog( parent, "Processing Options Dialog", true, "Processing Options", KDialog::Ok|KDialog::Cancel, KDialog::Ok, true )
	KDialog( parent ) // , "Processing Options Dialog", true, "Processing Options", KDialog::Ok|KDialog::Cancel, KDialog::Ok, true )
{
    setObjectName("Processing Options Dialog");
    setModal(true);
    setCaption(i18n("Processing Options"));
    setButtons(KDialog::Ok|KDialog::Cancel);
    setDefaultButton(KDialog::Ok);
    showButtonSeparator(true);

	m_pProjectItem = projectItem;
	m_pWidget = new ProcessingOptionsWidget(this);
	
	m_pWidget->m_pMicroSelect->setEnabled( !projectItem->useParentMicroID() );
	
	switch ( projectItem->type() )
	{
		case ProjectItem::ProjectType:
			m_pWidget->m_pOutputURL->setEnabled(false);
			break;
			
		case ProjectItem::FileType:
			m_pWidget->m_pOutputURL->setEnabled(true);
			break;
			
		case ProjectItem::ProgramType:
		case ProjectItem::LibraryType:
			m_pWidget->m_pOutputURL->setEnabled(false);
			break;
	}
	
	m_pWidget->m_pOutputURL->setUrl( projectItem->outputURL().path() );
	m_pWidget->m_pMicroSelect->setMicro( projectItem->microID() );
	
	setMainWidget( m_pWidget );
	setInitialSize( m_pWidget->rect().size() );
}


ProcessingOptionsDlg::~ProcessingOptionsDlg()
{
}


void ProcessingOptionsDlg::accept()
{
	hide();
	
	if ( m_pWidget->m_pOutputURL->isEnabled() )
		m_pProjectItem->setOutputURL( m_pWidget->m_pOutputURL->url() );
	
	if ( m_pWidget->m_pMicroSelect->isEnabled() )
		m_pProjectItem->setMicroID( m_pWidget->m_pMicroSelect->micro() );
}


void ProcessingOptionsDlg::reject()
{
    hide();
}
//END class ProcessingOptionsDlg


#include "projectdlgs.moc"
