/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "asmformattingwidget.h"
#include "generaloptionswidget.h"
#include "gpasmsettingswidget.h"
#include "gplinksettingswidget.h"
#include "logicwidget.h"
#include "picprogrammerconfigwidget.h"
#include "picprogrammer.h"
#include "port.h"
#include "sdccoptionswidget.h"
#include "settingsdlg.h"
#include "src/core/ktlconfig.h"

#include <kapplication.h>
#include <kcolorbutton.h>
#include <kcombobox.h>
#include <kconfig.h>
#include <kinputdialog.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qslider.h>
#include <qtimer.h>
#include <qtooltip.h>


// Make sure that this value is the same as that in ktechlab.kcfg
const int defaultRefreshRate = 50;

SettingsDlg::SettingsDlg( QWidget *parent, const char *name, KConfigSkeleton *config )
	: KConfigDialog( parent, name, config )
{
	m_generalOptionsWidget = new GeneralOptionsWidget( this, "generalOptionsWidget" );
	m_gpasmSettingsWidget = new GpasmSettingsWidget( this, "gpasmSettingsWidget" );
	m_sdccOptionsWidget = new SDCCOptionsWidget( this, "sdccOptionsWidget" );
	m_asmFormattingWidget = new AsmFormattingWidget( this, "asmFormattingWidget" );
	m_logicWidget = new LogicWidget( this, "logicWidget" );
	m_picProgrammerConfigWidget = new PicProgrammerConfigWidget( this, "picProgrammerConfigWidget" );
	m_gplinkSettingsWidget = new GplinkSettingsWidget( this, "gplinkSettingsWidget" );
	
	m_pPicProgrammerSettings = new PicProgrammerSettings;
	
	m_logicWidget->kcfg_LogicOutputHighImpedance->setSuffix( QString(" ") + QChar(0x3a9) );
	m_logicWidget->kcfg_LogicOutputLowImpedance->setSuffix( QString(" ") + QChar(0x3a9) );
	
	addPage( m_generalOptionsWidget, i18n("General"), "misc", i18n("General Options") );
	addPage( m_picProgrammerConfigWidget, i18n("Programmer"), "memory", i18n("PIC Programmer") );
	addPage( m_asmFormattingWidget, i18n("Formatter"), "indent_asm", i18n("Assembly Formatter") );
	addPage( m_logicWidget, i18n("Logic"), "logic_or", i18n("Electronic Logic Values") );
	addPage( m_gpasmSettingsWidget, "Gpasm", "convert_to_hex", "gpasm" );
	addPage( m_gplinkSettingsWidget, "Gplink", "gplink", "Gpasm" );
	addPage( m_sdccOptionsWidget, "SDCC", "source_c", "SDCC" );
	
	connect( m_generalOptionsWidget->refreshRateSlider, SIGNAL(valueChanged(int)), this, SLOT(slotUpdateRefreshRateLabel(int)) );
	connect( m_picProgrammerConfigWidget->kcfg_PicProgrammerProgram, SIGNAL(activated(const QString &)), this, SLOT(slotUpdatePicProgrammerDescription()) );
	connect( m_picProgrammerConfigWidget->removeButton, SIGNAL(clicked()), this, SLOT(slotRemoveProgrammerConfig()) );
	connect( m_picProgrammerConfigWidget->addButton, SIGNAL(clicked()), this, SLOT(slotAddProgrammerConfig()) );
	
	
	connect( m_picProgrammerConfigWidget->initCommand, SIGNAL(textChanged(const QString &)), this, SLOT(slotSaveCurrentProgrammerConfig()) );
	connect( m_picProgrammerConfigWidget->readCommand, SIGNAL(textChanged(const QString &)), this, SLOT(slotSaveCurrentProgrammerConfig()) );
	connect( m_picProgrammerConfigWidget->writeCommand, SIGNAL(textChanged(const QString &)), this, SLOT(slotSaveCurrentProgrammerConfig()) );
	connect( m_picProgrammerConfigWidget->verifyCommand, SIGNAL(textChanged(const QString &)), this, SLOT(slotSaveCurrentProgrammerConfig()) );
	connect( m_picProgrammerConfigWidget->blankCheckCommand, SIGNAL(textChanged(const QString &)), this, SLOT(slotSaveCurrentProgrammerConfig()) );
	connect( m_picProgrammerConfigWidget->eraseCommand, SIGNAL(textChanged(const QString &)), this, SLOT(slotSaveCurrentProgrammerConfig()) );
	
	
	m_generalOptionsWidget->kcfg_GridColor->setEnabled( KTLConfig::showGrid() );
	
	m_picProgrammerConfigWidget->kcfg_PicProgrammerPort->insertStringList( Port::ports( Port::ExistsAndRW ) );
	slotUpdatePicProgrammerDescription();
}


SettingsDlg::~SettingsDlg()
{
	delete m_pPicProgrammerSettings;
}


void SettingsDlg::show()
{
	KComboBox * combo = m_picProgrammerConfigWidget->kcfg_PicProgrammerProgram;
	combo->setEditable( true );
	KConfigDialog::show();
	combo->setEditable( false );
}


void SettingsDlg::slotUpdateRefreshRateLabel( int sliderValue )
{
	const QString number = QString::number( sliderValueToRefreshRate(sliderValue) );
	switch(sliderValue)
	{
		case 0:
			m_generalOptionsWidget->refreshRateLabel->setText( i18n("Lowest (%1 FPS)").arg(number) );
			break;
		case 1:
			m_generalOptionsWidget->refreshRateLabel->setText( i18n("Low (%1 FPS)").arg(number) );
			break;
		case 2:
			m_generalOptionsWidget->refreshRateLabel->setText( i18n("Medium (%1 FPS)").arg(number) );
			break;
		case 3:
			m_generalOptionsWidget->refreshRateLabel->setText( i18n("High (%1 FPS)").arg(number) );
			break;
		case 4:
			m_generalOptionsWidget->refreshRateLabel->setText( i18n("Highest (%1 FPS)").arg(number) );
			break;
		default:
			m_generalOptionsWidget->refreshRateLabel->setText( i18n("Unknown value") );
			break;
	}
	updateButtons();
}


void SettingsDlg::slotUpdatePicProgrammerDescription()
{
	QString program = m_picProgrammerConfigWidget->kcfg_PicProgrammerProgram->currentText();
	
	ProgrammerConfig config = m_pPicProgrammerSettings->config( program );
	QString description = config.description;
	
	bool customProgrammer = ! m_pPicProgrammerSettings->isPredefined( program );
	
	QString executable = config.executable;
	if ( executable.isEmpty() )
		executable = program.lower();
	
	QString programLocation = KStandardDirs::findExe( executable );
	if ( programLocation.isNull() )
		description.prepend( i18n("<b>%1</b> cannot be found.<br>").arg( executable ) );
	else
		description.prepend( i18n("<b>%1</b> found: %2<br>").arg( executable ).arg(programLocation) );
	
	m_picProgrammerConfigWidget->m_pProgrammerDescription->setText( description );
	m_picProgrammerConfigWidget->removeButton->setEnabled( customProgrammer );
	
	KLineEdit * edit;
	
#define SETUP_COMMAND( name ) \
	edit = m_picProgrammerConfigWidget->name; \
	edit->setText( config.name ); \
	edit->setEnabled(customProgrammer); \
	QToolTip::add( edit, customProgrammer ? 0 : config.name )
	
	SETUP_COMMAND( initCommand );
	SETUP_COMMAND( readCommand );
	SETUP_COMMAND( writeCommand );
	SETUP_COMMAND( verifyCommand );
	SETUP_COMMAND( blankCheckCommand );
	SETUP_COMMAND( eraseCommand );
	
#undef SETUP_COMMAND
}


void SettingsDlg::slotSaveCurrentProgrammerConfig()
{
	QString program = m_picProgrammerConfigWidget->kcfg_PicProgrammerProgram->currentText();
	
	if ( m_pPicProgrammerSettings->isPredefined( program ) )
		return;
	
	ProgrammerConfig config;
	
	config.initCommand = m_picProgrammerConfigWidget->initCommand->text();
	config.readCommand = m_picProgrammerConfigWidget->readCommand->text();
	config.writeCommand = m_picProgrammerConfigWidget->writeCommand->text();
	config.verifyCommand = m_picProgrammerConfigWidget->verifyCommand->text();
	config.blankCheckCommand = m_picProgrammerConfigWidget->blankCheckCommand->text();
	config.eraseCommand = m_picProgrammerConfigWidget->eraseCommand->text();
	
	m_pPicProgrammerSettings->saveConfig( program, config );
}


void SettingsDlg::slotRemoveProgrammerConfig()
{
	KComboBox * combo = m_picProgrammerConfigWidget->kcfg_PicProgrammerProgram;
	
	QString program = combo->currentText();
	
	KMessageBox::ButtonCode confirm = (KMessageBox::ButtonCode)KMessageBox::warningContinueCancel( this, i18n("Remove programmer configuration \"%1\"?").arg(program), i18n("Remove \"%1\"").arg(program), i18n("Remove") );
	if ( confirm == KMessageBox::Cancel )
		return;
	
	m_pPicProgrammerSettings->removeConfig( program );
	combo->removeItem( combo->currentItem() );
	slotUpdatePicProgrammerDescription();
}


void SettingsDlg::slotAddProgrammerConfig()
{
	KComboBox * combo = m_picProgrammerConfigWidget->kcfg_PicProgrammerProgram;
	
	QStringList takenNames;
	int count = combo->count();
	for ( int i = 0; i < count; ++i )
		takenNames << combo->text(i).lower();
	
	NameValidator * nv = new NameValidator( takenNames );
	
	bool ok = false;
	QString name = KInputDialog::getText( i18n("Configuration Name"), i18n("Name"), 0, &ok, this, 0, nv );
	
	delete nv;
	
	if (!ok)
		return;
	
	ProgrammerConfig config;
	config.executable = name.lower();
	
	m_pPicProgrammerSettings->saveConfig( name, config );
	
	combo->insertItem( name );
	combo->setCurrentItem( count );
	slotUpdatePicProgrammerDescription();
}


int SettingsDlg::refreshRateToSliderValue( int refreshRate )
{
	switch (refreshRate)
	{
		case 10: return 0;
		case 25: return 1;
		case 50: return 2;
		case 75: return 3;
		case 100: return 4;
		default: return -1;
	}
}


int SettingsDlg::sliderValueToRefreshRate( int sliderValue )
{
	switch (sliderValue)
	{
		case 0: return 10;
		case 1: return 25;
		case 2: return 50;
		case 3: return 75;
		case 4: return 100;
		default: return -1;
	}
}


void SettingsDlg::updateSettings()
{
	KConfig * config = kapp->config();
	
	KConfigSkeleton::ItemInt *item = dynamic_cast<KConfigSkeleton::ItemInt*>(KTLConfig::self()->findItem( "RefreshRate" ));
	if ( !item )
		return;
	
	int newRefreshRate = sliderValueToRefreshRate(m_generalOptionsWidget->refreshRateSlider->value());
	
	if ( newRefreshRate != KTLConfig::refreshRate() )
	{
		item->setValue(newRefreshRate);
		config->setGroup("WorkArea");
		if ( newRefreshRate != defaultRefreshRate )
			config->writeEntry("RefreshRate",newRefreshRate);
		else
			config->deleteEntry("RefreshRate");
		
		emit settingsChanged();
	}
	
	QTimer::singleShot( 0, this, SLOT(slotUpdateSettings()) );
}


void SettingsDlg::slotUpdateSettings()
{
	KConfig * config = kapp->config();
	
	KConfigSkeleton::ItemString * item = dynamic_cast<KConfigSkeleton::ItemString*>(KTLConfig::self()->findItem( "PicProgrammerProgram" ));
	if ( !item )
		return;
	
	KComboBox * combo = m_picProgrammerConfigWidget->kcfg_PicProgrammerProgram;
	QString newProgram = combo->currentText();
	
	if ( newProgram != KTLConfig::picProgrammerProgram() )
	{
		item->setValue( newProgram );
		config->setGroup( "PicProgramming" );
		if ( newProgram != "picp" )
			config->writeEntry( "PicProgrammerProgram", newProgram );
		else
			config->deleteEntry( "PicProgrammerProgram" );
		
		emit settingsChanged();
	}
	
	m_pPicProgrammerSettings->save( config );
	
	config->sync();
}


void SettingsDlg::updateWidgets()
{
	m_generalOptionsWidget->refreshRateSlider->setValue( refreshRateToSliderValue( KTLConfig::refreshRate() ) );
	
	m_pPicProgrammerSettings->load( kapp->config() );
	
	QStringList programmerNames = m_pPicProgrammerSettings->configNames( false );
	
	KComboBox * combo = m_picProgrammerConfigWidget->kcfg_PicProgrammerProgram;
	combo->clear();
	combo->insertStringList( programmerNames );
	combo->setSizeLimit( programmerNames.size() );
	
	QTimer::singleShot( 0, this, SLOT(slotUpdateWidgets()) );
}


void SettingsDlg::slotUpdateWidgets()
{
	KComboBox * combo = m_picProgrammerConfigWidget->kcfg_PicProgrammerProgram;
	
	combo->setCurrentText( KTLConfig::picProgrammerProgram() );
	slotUpdatePicProgrammerDescription();
}


void SettingsDlg::updateWidgetsDefault()
{
	m_generalOptionsWidget->refreshRateSlider->setValue( refreshRateToSliderValue( defaultRefreshRate ) );
	slotUpdatePicProgrammerDescription();
}


bool SettingsDlg::hasChanged()
{
	if ( sliderValueToRefreshRate( m_generalOptionsWidget->refreshRateSlider->value() ) == KTLConfig::refreshRate() )
		return KConfigDialog::hasChanged();
	return true;
}


bool SettingsDlg::isDefault()
{
	if ( sliderValueToRefreshRate( m_generalOptionsWidget->refreshRateSlider->value() ) == defaultRefreshRate )
		return KConfigDialog::isDefault();
	
	return false;
}


#include "settingsdlg.moc"
