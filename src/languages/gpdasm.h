/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef GPDASM_H
#define GPDASM_H

#include <externallanguage.h>

/**
Interface to the GNU Pic Disassembler
@author David Saxton
*/
class Gpdasm : public ExternalLanguage
{
public:
    Gpdasm(ProcessChain *processChain);
    ~Gpdasm() override;

    void processInput(ProcessOptions options) override;
    MessageInfo extractMessageInfo(const QString &text) override;
    ProcessOptions::ProcessPath::Path outputPath(ProcessOptions::ProcessPath::Path inputPath) const override;

protected:
    void outputtedMessage(const QString &message) override;
    bool isError(const QString &message) const override;
    bool isWarning(const QString &message) const override;
    bool processExited(bool successfully) override;

    QString m_asmOutput; // Outputed by gpdasm
};

#endif
