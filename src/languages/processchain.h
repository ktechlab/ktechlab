/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PROCESSCHAIN_H
#define PROCESSCHAIN_H

#include "language.h"
#include <QList>
#include <QObject>

class FlowCode;
class Gpasm;
class Gpdasm;
class Gplib;
class Gplink;
class KTechlab;
class Microbe;
class PicProgrammer;
class ProcesOptions;
class SDCC;

typedef QList<ProcessOptions> ProcessOptionsList;

/**
@author Daniel Clarke
@author David Saxton
*/
class ProcessChain : public QObject
{
    Q_OBJECT
public:
    ProcessChain(ProcessOptions options);
    ~ProcessChain() override;

    void setProcessOptions(ProcessOptions options)
    {
        m_processOptions = options;
    }

public Q_SLOTS:
    /**
     * Adds the output file to project if requested in the options, and opens
     * the file in a code editor. Called to signal that a language in the last
     * step of a compile has finished its compiling successfully.
     */
    void slotFinishedCompile(Language *language);
    /**
     * Call to compile a file of one type all the way to another type. This
     * uses the ProcessOptions given in the constructor of this function, or
     * later in setProcessOptions.
     */
    void compile();

Q_SIGNALS:
    /**
     * Emitted when compiling has successfully gone all the way through to the
     * specified 'typeTo'
     * @param options The ProcessOptions holding the output filename
     * @see compile
     */
    void successful(ProcessOptions options);
    /**
     * Convenience signal
     */
    void successful();
    /**
     * Emitted if not successful
     */
    void failed();

protected:
    FlowCode *flowCode();
    Gpasm *gpasm();
    Gpdasm *gpdasm();
    Gplib *gplib();
    Gplink *gplink();
    Microbe *microbe();
    PicProgrammer *picProgrammer();
    SDCC *sdcc();

    int m_errorCount;
    ProcessOptions m_processOptions;

private:
    FlowCode *m_pFlowCode;
    Microbe *m_pMicrobe;
    Gpasm *m_pGpasm;
    Gpdasm *m_pGpdasm;
    Gplib *m_pGplib;
    Gplink *m_pGplink;
    PicProgrammer *m_pPicProgrammer;
    SDCC *m_pSDCC;
};

class ProcessListChain : public QObject
{
    Q_OBJECT

public:
    ProcessListChain(ProcessOptionsList pol);

Q_SIGNALS:
    /**
     * Emitted if successful
     */
    void successful();
    /**
     * Emitted if not successful
     */
    void failed();

protected Q_SLOTS:
    void slotProcessChainSuccessful();
    void slotProcessChainFailed();

protected:
    ProcessOptionsList m_processOptionsList;
};

#endif
