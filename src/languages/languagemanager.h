/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QList>
#include <QObject>

#include "language.h"
#include "logview.h"

class FlowCode;
class Gpasm;
class Gpdasm;
class KTechlab;
class Language;
class LanguageManager;
class MessageInfo;
class Microbe;
class ProcessChain;
class ProcessListChain;
class ProcessOptions;
namespace KateMDI
{
class ToolView;
}

/**
@author David Saxton
*/
class LanguageManager : public QObject
{
    Q_OBJECT
public:
    static LanguageManager *self(KateMDI::ToolView *parent = nullptr);
    static QString toolViewIdentifier()
    {
        return "LanguageManager";
    }
    ~LanguageManager() override;

    /**
     * Call to compile a file of one type all the way to another type, this can
     * also be used in reverse to disassemble code. Connect to the returned
     * ProcessChain for notification of compile success / failure
     * @return Pointer to the ProcessChain used to compile
     */
    ProcessChain *compile(ProcessOptions options);
    ProcessListChain *compile(ProcessOptionsList pol);
    /**
     * @return Pointer to the LogView that displays the output messages
     */
    LogView *logView() const
    {
        return m_logView;
    }
    /**
     * Clear any errors and clear the log view
     */
    void reset();

public Q_SLOTS:
    /**
     * Called when the user clicks on any text in the LogView
     */
    void slotParaClicked(const QString &message, MessageInfo messageInfo);
    /**
     * Called by languages to report an error message
     * @param error Error message to report
     */
    void slotError(const QString &error, MessageInfo messageInfo);
    /**
     * Called by languages to report a warning message
     * @param warning Warning message to report
     */
    void slotWarning(const QString &warning, MessageInfo messageInfo);
    /**
     * Called by languages to report a general message
     * @param message General message to report
     */
    void slotMessage(const QString &message, MessageInfo messageInfo);

protected:
    LanguageManager(KateMDI::ToolView *parent);

private:
    LogView *m_logView;
    static LanguageManager *m_pSelf;
};

#endif
