/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef SDCC_H
#define SDCC_H

#include <externallanguage.h>

/**
@author David Saxton
*/
class SDCC : public ExternalLanguage
{
public:
    SDCC(ProcessChain *processChain);
    ~SDCC() override;

    void processInput(ProcessOptions options) override;
    ProcessOptions::ProcessPath::Path outputPath(ProcessOptions::ProcessPath::Path inputPath) const override;

protected:
    bool isError(const QString &message) const override;
    bool isWarning(const QString &message) const override;
    bool isStderrOutputFatal(const QString &message) const override;
};

#endif
