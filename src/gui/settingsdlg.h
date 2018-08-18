/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <kconfigdialog.h>
#include <qmap.h>
#include <qvalidator.h>

class AsmFormattingWidget;
class GeneralOptionsWidget;
class GpasmSettingsWidget;
class GplinkSettingsWidget;
class LogicWidget;
class PicProgrammerConfigWidget;
class PicProgrammerSettings;
class SDCCOptionsWidget;



/**
@author David Saxton
*/
class SettingsDlg : public KConfigDialog
{
	Q_OBJECT
	public:
		SettingsDlg( QWidget *parent, const char *name, KConfigSkeleton *config );
		~SettingsDlg();
	
		static int refreshRateToSliderValue( int refreshRate );
		static int sliderValueToRefreshRate( int sliderValue );
		
		virtual void show();

	public slots:
		void slotUpdateRefreshRateLabel( int sliderValue );
		void slotUpdatePicProgrammerDescription();
		void slotAddProgrammerConfig();
		void slotRemoveProgrammerConfig();
		void slotSaveCurrentProgrammerConfig();
		
	protected slots:
		void slotUpdateSettings();
		void slotUpdateWidgets();
	
	protected:
		virtual void updateSettings();
		virtual void updateWidgets();
		virtual void updateWidgetsDefault();
		virtual bool hasChanged();
		virtual bool isDefault();
	
		PicProgrammerSettings * m_pPicProgrammerSettings;
		
		GeneralOptionsWidget * m_generalOptionsWidget;
		GpasmSettingsWidget * m_gpasmSettingsWidget;
		SDCCOptionsWidget * m_sdccOptionsWidget;
		AsmFormattingWidget * m_asmFormattingWidget;
		LogicWidget * m_logicWidget;
		PicProgrammerConfigWidget * m_picProgrammerConfigWidget;
		GplinkSettingsWidget * m_gplinkSettingsWidget;
};


class NameValidator : public QValidator
{
	public:
		NameValidator( QStringList unallowed )
			: QValidator(0) {
			m_unallowed = unallowed;
		}
		
		virtual State validate( QString & input, int & ) const {
			return (input.isEmpty() || m_unallowed.contains( input.toLower() )) ? Intermediate : Acceptable;
		}
		
	protected:
		QStringList m_unallowed;
};


#endif
