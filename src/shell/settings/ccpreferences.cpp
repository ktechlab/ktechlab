/* KDevelop Project Settings
*
* Copyright 2006  Matt Rogers <mattr@kde.org>
* Copyright 2007  Hamish Rodda <rodda@kde.org>
* Copyright 2009  David Nolden <david.nolden.kdevelop@art-master.de>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
* 02110-1301, USA.
*/

#include "ccpreferences.h"

#include <QVBoxLayout>

#include <kgenericfactory.h>
#include <kaboutdata.h>

#include <ktexteditor/view.h>
#include <ktexteditor/document.h>
#include <ktexteditor/codecompletioninterface.h>

#include "../core.h"
#include <interfaces/idocumentcontroller.h>
#include <interfaces/idocument.h>

#include "ccconfig.h"

#include "ui_ccsettings.h"
#include <interfaces/ilanguagecontroller.h>
#include <interfaces/icompletionsettings.h>

#include "../completionsettings.h"

using namespace KTextEditor;

namespace KDevelop
{

K_PLUGIN_FACTORY(CCPreferencesFactory, registerPlugin<CCPreferences>();)
K_EXPORT_PLUGIN(CCPreferencesFactory(KAboutData("kcm_kdev_ccsettings", "kdevplatform", ki18n("Language Support Settings"), "0.1")))

CCPreferences::CCPreferences( QWidget *parent, const QVariantList &args )
 : KCModule( CCPreferencesFactory::componentData(), parent, args )
{
    QVBoxLayout * l = new QVBoxLayout( this );
    QWidget* w = new QWidget;
    preferencesDialog = new Ui::CCSettings;
    preferencesDialog->setupUi( w );

    l->addWidget( w );

    addConfig( CCSettings::self(), w );

    load();
}

void CCPreferences::load()
{
    KCModule::load();
    writeToSettings();
}

void CCPreferences::writeToSettings()
{
    CompletionSettings& settings(static_cast<CompletionSettings&>(*ICore::self()->languageController()->completionSettings()));
    settings.m_automatic = preferencesDialog->kcfg_automaticInvocation->isChecked();
    if(preferencesDialog->kcfg_alwaysFullCompletion->isChecked())
        settings.m_level = ICompletionSettings::AlwaysFull;
    if(preferencesDialog->kcfg_minimalAutomaticCompletion->isChecked())
        settings.m_level = ICompletionSettings::MinimalWhenAutomatic;
    if(preferencesDialog->kcfg_alwaysMinimalCompletion->isChecked())
        settings.m_level = ICompletionSettings::Minimal;
    
    settings.m_enableSemanticHighlighting = preferencesDialog->kcfg_enableSemanticHighlighting->isChecked();
    settings.m_localVariableColorizationLevel = preferencesDialog->kcfg_localVariableColorization->value();
    settings.m_highlightSemanticProblems = preferencesDialog->kcfg_highlightSemanticProblems->isChecked();
    
    emit settings.emitChanged();
}

CCPreferences::~CCPreferences( )
{
    delete preferencesDialog;
}

void CCPreferences::save()
{
    KCModule::save();

    foreach (KDevelop::IDocument* doc, Core::self()->documentController()->openDocuments())
        if (Document* textDoc = doc->textDocument())
            foreach (View* view, textDoc->views())
                if (CodeCompletionInterface* cc = dynamic_cast<CodeCompletionInterface*>(view))
                    cc->setAutomaticInvocationEnabled(preferencesDialog->kcfg_automaticInvocation->isChecked());
    
    writeToSettings();
}

}

#include "ccpreferences.moc"

