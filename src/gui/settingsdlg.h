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

#include <KConfigDialog>
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
		SettingsDlg( QWidget *parent, const char *name, KCoreConfigSkeleton *config );
		~SettingsDlg() override;
	
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
		void updateSettings() override;
		void updateWidgets() override;
		void updateWidgetsDefault() override;
		bool hasChanged() override;
		bool isDefault() override;

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
			: QValidator(nullptr) {
			m_unallowed = unallowed;
		}
		
		State validate( QString & input, int & ) const override {
			return (input.isEmpty() || m_unallowed.contains( input.toLower() )) ? Intermediate : Acceptable;
		}

	protected:
		QStringList m_unallowed;
};


#endif
