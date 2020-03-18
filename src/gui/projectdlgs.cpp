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
#include <qdebug.h>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

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
    : QDialog(parent)
{
    setObjectName("newprojectdlg");
    setModal(true);
    setWindowTitle(i18n("New Project"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

	m_pWidget = new NewProjectWidget(this);
	connect( m_pWidget->projectNameEdit, SIGNAL(textChanged(const QString & )), this, SLOT(locationChanged(const QString& )) );
	connect( m_pWidget->projectLocationURL, SIGNAL(textChanged(const QString & )), this, SLOT(locationChanged(const QString& )) );

    mainLayout->addWidget(m_pWidget);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    mainLayout->addWidget(m_buttonBox);

    QPushButton *okButton = m_buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Check if already valid dir
	locationChanged( QString::null );

	m_pWidget->projectLocationURL->setUrl( QDir::homePath() );
	m_pWidget->projectLocationURL->setMode( KFile::Directory );
}

void NewProjectDlg::accept()
{
	m_projectName = m_pWidget->projectNameEdit->text();
	m_projectLocation = m_pWidget->projectLocationURL->url().toLocalFile();

    QDialog::accept();
}

void NewProjectDlg::locationChanged( const QString & )
{
	m_location = m_pWidget->projectLocationURL->url().toLocalFile();
    qDebug() << "location changed to: " << m_location;
	QDir subDir(m_location);

	if ( !m_location.endsWith("/") )
		m_location.append("/");

	if ( !m_pWidget->projectNameEdit->text().isEmpty() )
		m_location.append( m_pWidget->projectNameEdit->text().toLower() + "/" );

	m_pWidget->locationLabel->setText( m_location );

	QDir dir(m_location);

    qDebug() << "dir.exists: " << dir.exists() << " subdir.exists: " << subDir.exists();

    QPushButton *okButton = m_buttonBox->button(QDialogButtonBox::Ok);
	if ( dir.exists() || !subDir.exists() ) {
		okButton->setEnabled(false);
    } else {
		okButton->setEnabled(true);
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
    : QDialog(parent)
{
    setObjectName("Create Subproject Dialog");
    setModal(true);
    setWindowTitle(i18n("Create Subproject"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

	m_pWidget = new CreateSubprojectWidget(this);

	if ( ProjectManager::self()->currentProject() )
		m_pWidget->m_targetFile->setUrl( ProjectManager::self()->currentProject()->directory() );

	m_type = ProgramType;

    mainLayout->addWidget(m_pWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}


CreateSubprojectDlg::~CreateSubprojectDlg()
{
}


void CreateSubprojectDlg::accept()
{
	m_targetFile = m_pWidget->m_targetFile->url().toLocalFile();
	m_type = (Type)m_pWidget->m_typeCombo->currentIndex();

    QDialog::accept();
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
    : QDialog(parent)
{
    setObjectName("Linker Options Dialog");
    setModal(true);
    setWindowTitle(i18n("Linker Options"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

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

	m_pExternalLibraryRequester = new KUrlRequester( nullptr );
	//m_pExternalLibraryRequester->fileDialog()->setUrl( KUrl( "/usr/share/sdcc/lib" ) );
    m_pExternalLibraryRequester->fileDialog()->setDirectoryUrl( KUrl( "/usr/share/sdcc/lib" ) );
	
	delete m_pWidget->m_pExternalLibraries;
    KEditListBox b;
    
// // 	m_pWidget->m_pExternalLibraries = new KEditListBox( 
// //         i18n("Link libraries outside project"), 
// //         //m_pExternalLibraryRequester->customEditor(), 
// //         KEditListBox::CustomEditor(
// //             m_pExternalLibraryRequester->comboBox(), m_pExternalLibraryRequester->lineEdit()),
// //         m_pWidget );
// //     m_pWidget->m_pExternalLibraries->setTitle(i18n("Link libraries outside project"));
// 	//m_pExternalLibraryRequester->fileDialog()->setUrl( KUrl( "/usr/share/sdcc/lib" ) );
//     m_pExternalLibraryRequester->fileDialog()->setDirectoryUrl( KUrl( "/usr/share/sdcc/lib" ) );
// 	
// 	delete m_pWidget->m_pExternalLibraries;
//     KEditListBox b;
//     
// 	m_pWidget->m_pExternalLibraries = new KEditListBox( 
//         i18n("Link libraries outside project"), 
//         //m_pExternalLibraryRequester->customEditor(), 
//         KEditListBox::CustomEditor(
//             m_pExternalLibraryRequester->comboBox(), m_pExternalLibraryRequester->lineEdit()),
//         m_pWidget );
//     m_pWidget->m_pExternalLibraries->setTitle(i18n("Link libraries outside project"));
// 	m_pExternalLibraryRequester->fileDialog()->selectUrl( KUrl( "/usr/share/sdcc/lib" ) );
// 
	delete m_pWidget->m_pExternalLibraries;
	m_pWidget->m_pExternalLibraries = new KEditListWidget(
        //i18n("Link libraries outside project"),
        m_pExternalLibraryRequester->customEditor(),
        m_pWidget );
	m_pWidget->m_pExternalLibraries->layout()->setMargin(11);
    {
        QGridLayout* grLayout = (dynamic_cast<QGridLayout*>(m_pWidget->layout()));
        //grLayout->addMultiCellWidget( m_pWidget->m_pExternalLibraries, 7, 7, 0, 1 ); // 2018.12.02
        grLayout->addWidget( m_pWidget->m_pExternalLibraries, 7, 0, 1, 2);
    }

	m_pWidget->m_pExternalLibraries->setButtons( KEditListWidget::Add | KEditListWidget::Remove );
	m_pWidget->m_pExternalLibraries->insertStringList( m_pLinkerOptions->linkedExternal() );
	//END Update library widgets


    mainLayout->addWidget(m_pWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}


LinkerOptionsDlg::~LinkerOptionsDlg()
{
	delete m_pExternalLibraryRequester;
}


void LinkerOptionsDlg::accept()
{
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
	m_pLinkerOptions->setHexFormat( (LinkerOptions::HexFormat::type) m_pWidget->m_pHexFormat->currentIndex() );
	m_pLinkerOptions->setOutputMapFile( m_pWidget->m_pOutputMap->isChecked() );
	m_pLinkerOptions->setLibraryDir( m_pWidget->m_pLibraryDir->text() );
	m_pLinkerOptions->setLinkerScript( m_pWidget->m_pLinkerScript->text() );
	m_pLinkerOptions->setLinkerOther( m_pWidget->m_pOther->text() );

    QDialog::accept();
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
    : QDialog(parent)
{
    setObjectName("Processing Options Dialog");
    setModal(true);
    setWindowTitle(i18n("Processing Options"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

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

    mainLayout->addWidget(m_pWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}


ProcessingOptionsDlg::~ProcessingOptionsDlg()
{
}


void ProcessingOptionsDlg::accept()
{
	if ( m_pWidget->m_pOutputURL->isEnabled() )
		m_pProjectItem->setOutputURL( m_pWidget->m_pOutputURL->url() );

	if ( m_pWidget->m_pMicroSelect->isEnabled() )
		m_pProjectItem->setMicroID( m_pWidget->m_pMicroSelect->micro() );

    QDialog::accept();
}


//END class ProcessingOptionsDlg
