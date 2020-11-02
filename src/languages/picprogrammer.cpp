/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "picprogrammer.h"
#include "languagemanager.h"
#include "logview.h"

#include <KLocalizedString>
#include <KMessageBox>
#include <KProcess>
#include <KSharedConfig>
#include <KShell>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QRegExp>
#include <QTextStream>

#include <stdio.h>

#include <ktlconfig.h>

// BEGIN class ProgrammerConfig
ProgrammerConfig::ProgrammerConfig()
{
}

void ProgrammerConfig::reset()
{
    initCommand = QString();
    readCommand = QString();
    writeCommand = QString();
    verifyCommand = QString();
    blankCheckCommand = QString();
    eraseCommand = QString();
}
// END class ProgrammerConfig

// BEGIN class PicProgrammerSettings
bool PicProgrammerSettings::m_bDoneStaticConfigsInit = false;
ProgrammerConfigMap PicProgrammerSettings::m_staticConfigs = ProgrammerConfigMap();

PicProgrammerSettings::PicProgrammerSettings()
{
    if (!m_bDoneStaticConfigsInit)
        initStaticConfigs();
}

void PicProgrammerSettings::initStaticConfigs()
{
    m_bDoneStaticConfigsInit = true;
    ProgrammerConfig config;

    config.description = i18n("Supported programmers: %1", QString("JuPic, PICStart Plus, Warp-13"));
    config.description += i18n("<br>Interface: Serial Port");
    config.initCommand = "";
    config.readCommand = "picp %port %device -rp %file";
    config.writeCommand = "picp %port %device -wp %file";
    config.verifyCommand = "";
    config.blankCheckCommand = "picp %port %device -b";
    config.eraseCommand = "picp %port %device -e";
    // 	config.executable = "picp";
    m_staticConfigs["PICP"] = config;

    config.description = i18n("Supported programmers: %1", QString("Epic Plus"));
    config.description += i18n("<br>Interface: Parallel Port");
    config.initCommand = "odyssey init";
    config.readCommand = "odyssey %device read %file";
    config.writeCommand = "odyssey %device write %file";
    config.verifyCommand = "odyssey %device verify %file";
    config.blankCheckCommand = "odyssey %device blankcheck";
    config.eraseCommand = "odyssey %device erase";
    // 	config.executable = "odyssey";
    m_staticConfigs["Odyssey"] = config;

    config.description = i18n("Supported programmers: %1", QString("JDM PIC-Programmer 2, PIC-PG2C"));
    config.description += i18n("<br>Interface: Serial Port");
    config.initCommand = "";
    config.readCommand = "picprog --output %file --pic %port";
    config.writeCommand = "picprog --burn --input %file --pic %port --device %device";
    config.verifyCommand = "";
    config.blankCheckCommand = "";
    config.eraseCommand = "picprog --erase --pic %device";
    m_staticConfigs["PICProg"] = config;

    config.description = i18n("Supported programmers: %1", QString("Epic Plus"));
    config.description += i18n("<br>Interface: Parallel Port");
    config.initCommand = "";
    config.readCommand = "dump84 --dump-all --output=%file";
    config.writeCommand = "prog84 --intel16=%file";
    config.verifyCommand = "";
    config.blankCheckCommand = "";
    config.eraseCommand = "prog84 --clear";
    m_staticConfigs["prog84"] = config;

    config.description = i18n("Supported programmers: %1", QString("Kit 149, Kit 150"));
    config.description += i18n("<br>Interface: USB Port");
    config.initCommand = "";
    config.readCommand = "pp -d %device -r %file";
    config.writeCommand = "pp -d %device -w %file";
    config.verifyCommand = "pp -d %device -v %file";
    config.blankCheckCommand = "";
    config.eraseCommand = "pp -d %device -e";
    m_staticConfigs["PP"] = config;

    config.description = i18n("Supported programmers: %1", QString("Wisp628"));
    config.description += i18n("<br>Interface: Serial Port");
    config.initCommand = "";
    config.readCommand = "xwisp ID %device PORT %device DUMP";
    config.writeCommand = "xwisp ID %device PORT %device WRITE %file";
    config.verifyCommand = "";
    config.blankCheckCommand = "";
    config.eraseCommand = "xwisp ID %device PORT %device ERASE";
    m_staticConfigs["XWisp"] = config;

#if 0
	config.description = i18n("Supported programmers: %1", QString("Epic Plus, JDM PIC-Programmer 2, PICCOLO, PICCOLO Grande, Trivial HVP Programmer"));
	config.description += i18n("<br>Interface: Serial Port and Parallel Port");
	config.initCommand = "";
	config.readCommand = "";
	config.writeCommand = "";
	config.verifyCommand = "";
	config.blankCheckCommand = "";
	config.eraseCommand = "";
	config.executable = "pkp";
	m_staticConfigs[ "PiKdev" ] = config;
	config.executable = "";


	config.description = i18n("Supported programmers: %1", QString("Trivial LVP programmer, Trivial HVP Programmer"));
	config.description += i18n("<br>Interface: Parallel Port");
	config.initCommand = "";
	config.readCommand = "";
	config.writeCommand = "";
	config.verifyCommand = "";
	config.blankCheckCommand = "";
	config.eraseCommand = "";
	m_staticConfigs[ "PicPrg2" ] = config;


	config.description = i18n("Supported programmers: %1", QString("El Cheapo"));
	config.description += i18n("<br>Interface: Parallel Port");
	config.initCommand = "";
	config.readCommand = "";
	config.writeCommand = "";
	config.verifyCommand = "";
	config.blankCheckCommand = "";
	config.eraseCommand = "";
	m_staticConfigs[ "PP06" ] = config;


	config.description = i18n("Supported programmers: %1", QString("NOPPP"));
	config.description += i18n("<br>Interface: Parallel Port");
	config.initCommand = "";
	config.readCommand = "";
	config.writeCommand = "";
	config.verifyCommand = "";
	config.blankCheckCommand = "";
	config.eraseCommand = "";
	m_staticConfigs[ "NOPPP" ] = config;


	config.description = i18n("Supported programmers: %1", QString("SNOPPP"));
	config.description += i18n("<br>Interface: Parallel Port");
	config.initCommand = "";
	config.readCommand = "";
	config.writeCommand = "";
	config.verifyCommand = "";
	config.blankCheckCommand = "";
	config.eraseCommand = "";
	m_staticConfigs[ "SNOPPP" ] = config;
#endif
}

void PicProgrammerSettings::load(KConfig *config)
{
    QStringList oldCustomProgrammers = config->groupList().filter("CustomProgrammer_");
    QStringList::iterator ocpEnd = oldCustomProgrammers.end();
    for (QStringList::iterator it = oldCustomProgrammers.begin(); it != ocpEnd; ++it) {
        // The CustomProgrammer_ string we searched for might appear half way through... (don't want)
        if ((*it).startsWith("CustomProgrammer_")) {
            // config->setGroup(*it);
            KConfigGroup grProg = config->group(*it);

            ProgrammerConfig pc;
            pc.initCommand = grProg.readEntry("InitCommand");
            pc.readCommand = grProg.readEntry("ReadCommand");
            pc.writeCommand = grProg.readEntry("WriteCommand");
            pc.verifyCommand = grProg.readEntry("VerifyCommand");
            pc.blankCheckCommand = grProg.readEntry("BlankCheckCommand");
            pc.eraseCommand = grProg.readEntry("EraseCommand");

            QString name = grProg.readEntry("Name");
            m_customConfigs[name] = pc;
        }
    }
}

void PicProgrammerSettings::save(KConfig *config)
{
    QStringList oldCustomProgrammers = config->groupList().filter("CustomProgrammer_");
    QStringList::iterator ocpEnd = oldCustomProgrammers.end();
    for (QStringList::iterator it = oldCustomProgrammers.begin(); it != ocpEnd; ++it) {
        // The CustomProgrammer_ string we searched for might appear half way through... (don't want)
        if ((*it).startsWith("CustomProgrammer_"))
            config->deleteGroup(*it);
    }

    int at = 0;
    ProgrammerConfigMap::iterator end = m_customConfigs.end();
    for (ProgrammerConfigMap::iterator it = m_customConfigs.begin(); it != end; ++it) {
        // config->setGroup( QString("CustomProgrammer_%1").arg(at++) );
        QString grName = QString("CustomProgrammer_%1").arg(at++);
        KConfigGroup gr = config->group(grName);

        gr.writeEntry("Name", it.key());
        gr.writeEntry("InitCommand", it.value().initCommand);
        gr.writeEntry("ReadCommand", it.value().readCommand);
        gr.writeEntry("WriteCommand", it.value().writeCommand);
        gr.writeEntry("VerifyCommand", it.value().verifyCommand);
        gr.writeEntry("BlankCheckCommand", it.value().blankCheckCommand);
        gr.writeEntry("EraseCommand", it.value().eraseCommand);
    }
}

ProgrammerConfig PicProgrammerSettings::config(const QString &name)
{
    if (name.isEmpty())
        return ProgrammerConfig();

    QString l = name.toLower();

    ProgrammerConfigMap::const_iterator end = m_customConfigs.end();
    for (ProgrammerConfigMap::const_iterator it = m_customConfigs.begin(); it != end; ++it) {
        if (it.key().toLower() == l)
            return *it;
    }

    end = m_staticConfigs.end();
    for (ProgrammerConfigMap::const_iterator it = m_staticConfigs.begin(); it != end; ++it) {
        if (it.key().toLower() == l)
            return *it;
    }

    return m_customConfigs[name];
}

void PicProgrammerSettings::removeConfig(const QString &name)
{
    if (isPredefined(name)) {
        qWarning() << Q_FUNC_INFO << "Cannot remove a predefined PIC programmer configuration." << endl;
        return;
    }

    QString l = name.toLower();

    ProgrammerConfigMap::iterator end = m_customConfigs.end();
    for (ProgrammerConfigMap::iterator it = m_customConfigs.begin(); it != end; ++it) {
        if (it.key().toLower() == l) {
            m_customConfigs.erase(it);
            return;
        }
    }
}

void PicProgrammerSettings::saveConfig(const QString &name, const ProgrammerConfig &config)
{
    if (isPredefined(name)) {
        qWarning() << Q_FUNC_INFO << "Cannot save to a predefined PIC programmer configuration." << endl;
        return;
    }

    QString l = name.toLower();

    ProgrammerConfigMap::iterator end = m_customConfigs.end();
    for (ProgrammerConfigMap::iterator it = m_customConfigs.begin(); it != end; ++it) {
        if (it.key().toLower() == l) {
            *it = config;
            return;
        }
    }

    m_customConfigs[name] = config;
}

QStringList PicProgrammerSettings::configNames(bool makeLowercase) const
{
    if (!makeLowercase)
        return m_customConfigs.keys() + m_staticConfigs.keys();

    QStringList names;

    ProgrammerConfigMap::const_iterator end = m_customConfigs.end();
    for (ProgrammerConfigMap::const_iterator it = m_customConfigs.begin(); it != end; ++it)
        names << it.key().toLower();

    end = m_staticConfigs.end();
    for (ProgrammerConfigMap::const_iterator it = m_staticConfigs.begin(); it != end; ++it)
        names << it.key().toLower();

    return names;
}

bool PicProgrammerSettings::isPredefined(const QString &name) const
{
    QString l = name.toLower();

    ProgrammerConfigMap::const_iterator end = m_staticConfigs.end();
    for (ProgrammerConfigMap::const_iterator it = m_staticConfigs.begin(); it != end; ++it) {
        if (it.key().toLower() == l)
            return true;
    }

    return false;
}
// END class PicProgrammerSettings

// BEGIN class PicProgrammer
PicProgrammer::PicProgrammer(ProcessChain *processChain)
    : ExternalLanguage(processChain, "PicProgrammer")
{
    m_successfulMessage = i18n("*** Programming successful ***");
    m_failedMessage = i18n("*** Programming failed ***");
}

PicProgrammer::~PicProgrammer()
{
}

void PicProgrammer::processInput(ProcessOptions options)
{
    resetLanguageProcess();
    m_processOptions = options;

    PicProgrammerSettings settings;
    // settings.load( kapp->config() );
    KSharedConfigPtr cfgPtr = KSharedConfig::openConfig();
    settings.load(cfgPtr.data());

    QString program = options.m_program;
    if (!settings.configNames(true).contains(program.toLower())) {
        qCritical() << Q_FUNC_INFO << "Invalid program" << endl;
        finish(false);
        return;
    }

    ProgrammerConfig config = settings.config(program);

    QString command = config.writeCommand;
    command.replace("%port", options.m_port);
    command.replace("%device", QString(options.m_picID).remove("P"));
    command.replace("%file", KShell::quoteArg(options.inputFiles().first()));

    // m_languageProcess->setUseShell( true ); // 2017.10.08 - port to KProcess
    //*m_languageProcess << command;
    m_languageProcess->setShellCommand(command);

    if (!start()) {
        // 		KMessageBox::sorry( LanguageManager::self()->logView(), i18n("Could not program PIC.") );
        processInitFailed();
        return;
    }
}

bool PicProgrammer::isError(const QString &message) const
{
    return message.contains("Error", Qt::CaseInsensitive);
}

bool PicProgrammer::isWarning(const QString &message) const
{
    return message.contains("Warning", Qt::CaseInsensitive);
}

ProcessOptions::ProcessPath::Path PicProgrammer::outputPath(ProcessOptions::ProcessPath::Path inputPath) const
{
    switch (inputPath) {
    case ProcessOptions::ProcessPath::PIC_AssemblyAbsolute:
    case ProcessOptions::ProcessPath::Program_PIC:
        return ProcessOptions::ProcessPath::None;

    case ProcessOptions::ProcessPath::AssemblyAbsolute_PIC:
    case ProcessOptions::ProcessPath::AssemblyAbsolute_Program:
    case ProcessOptions::ProcessPath::AssemblyRelocatable_Library:
    case ProcessOptions::ProcessPath::AssemblyRelocatable_Object:
    case ProcessOptions::ProcessPath::AssemblyRelocatable_PIC:
    case ProcessOptions::ProcessPath::AssemblyRelocatable_Program:
    case ProcessOptions::ProcessPath::C_AssemblyRelocatable:
    case ProcessOptions::ProcessPath::C_Library:
    case ProcessOptions::ProcessPath::C_Object:
    case ProcessOptions::ProcessPath::C_PIC:
    case ProcessOptions::ProcessPath::C_Program:
    case ProcessOptions::ProcessPath::FlowCode_AssemblyAbsolute:
    case ProcessOptions::ProcessPath::FlowCode_Microbe:
    case ProcessOptions::ProcessPath::FlowCode_PIC:
    case ProcessOptions::ProcessPath::FlowCode_Program:
    case ProcessOptions::ProcessPath::Microbe_AssemblyAbsolute:
    case ProcessOptions::ProcessPath::Microbe_PIC:
    case ProcessOptions::ProcessPath::Microbe_Program:
    case ProcessOptions::ProcessPath::Object_Disassembly:
    case ProcessOptions::ProcessPath::Object_Library:
    case ProcessOptions::ProcessPath::Object_PIC:
    case ProcessOptions::ProcessPath::Object_Program:
    case ProcessOptions::ProcessPath::Program_Disassembly:
    case ProcessOptions::ProcessPath::Invalid:
    case ProcessOptions::ProcessPath::None:
        return ProcessOptions::ProcessPath::Invalid;
    }

    return ProcessOptions::ProcessPath::Invalid;
}
// END class PicProgrammer
