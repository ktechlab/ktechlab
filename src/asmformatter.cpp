/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "asmformatter.h"
#include "picinfo12bit.h"
#include "picinfo14bit.h"
#include "picinfo16bit.h"

#include <ktlconfig.h>

static QString extractComment(const QString &line)
{
    int pos = line.indexOf(';');

    if (pos == -1)
        return "";

    return line.right(line.length() - pos);
}

// BEGIN class AsmFormatter
AsmFormatter::AsmFormatter()
{
}

AsmFormatter::~AsmFormatter()
{
}

QString AsmFormatter::tidyAsm(QStringList lines)
{
    // Update our indentation values from config
    m_indentAsmName = KTLConfig::indentAsmName();
    m_indentAsmData = KTLConfig::indentAsmData();
    m_indentEqu = KTLConfig::indentEqu();
    m_indentEquValue = KTLConfig::indentEquValue();
    m_indentComment = m_indentEquComment = KTLConfig::indentComment();

    QStringList::iterator end = lines.end();
    for (QStringList::iterator slit = lines.begin(); slit != end; ++slit) {
        switch (lineType(*slit)) {
        case Other:
            break;

        case Equ:
            *slit = tidyEqu(*slit);
            break;

        case Instruction:
            *slit = tidyInstruction(*slit);
            break;
        }
    }

    QString code;

    for (QStringList::iterator slit = lines.begin(); slit != end; ++slit)
        code.append(*slit + '\n');

    return code;
}

void AsmFormatter::pad(QString &text, int length)
{
    int padLength = length - text.length();
    if (padLength <= 0)
        return;

    QString pad;
    pad.fill(' ', padLength);
    text += pad;
}

QString AsmFormatter::tidyInstruction(const QString &oldLine)
{
    InstructionParts parts(oldLine);
    QString line;

    if (!parts.label().isEmpty())
        line = parts.label() + ' ';
    pad(line, m_indentAsmName);

    if (!parts.operand().isEmpty())
        line += parts.operand() + ' ';
    pad(line, m_indentAsmData);

    if (!parts.operandData().isEmpty())
        line += parts.operandData();
    pad(line, m_indentComment);

    if (parts.comment().isEmpty()) {
        // Remove any whitespace at the end if we're not padding out a comment
        while (!line.isEmpty() && line[line.length() - 1].isSpace())
            line.remove(line.length() - 1, 1);
    } else
        line += parts.comment();

    return line;
}

QString AsmFormatter::tidyEqu(const QString &oldLine)
{
    QString comment = extractComment(oldLine);
    QString code = oldLine;
    code.remove(comment);
    code = code.simplified();

    QStringList parts = code.split(' ', QString::SkipEmptyParts); // QStringList::split( ' ', code ); // 2018.12.01

    QString pad0, pad1, pad2;
    pad0.fill(' ', m_indentEqu - (parts.at(0)).length());
    pad1.fill(' ', m_indentEquValue - m_indentEqu - (parts.at(1)).length());
    pad2.fill(' ', m_indentEquComment - m_indentEquValue - m_indentEqu - (parts.at(2)).length());

    code = parts.at(0) + pad0;
    code += parts.at(1) + pad1;
    code += parts.at(2);
    if (!comment.isEmpty()) {
        code += pad2;
        code += comment;
    }

    return code;
}

AsmFormatter::LineType AsmFormatter::lineType(QString line)
{
    line = line.simplified();

    line.remove(extractComment(line));

    QStringList parts = line.split(' ', QString::SkipEmptyParts); // QStringList::split( ' ', line ); // 2018.12.01
    QStringList::iterator end = parts.end();
    for (QStringList::iterator it = parts.begin(); it != end; ++it) {
        if ((*it).toLower() == "equ")
            return Equ;
    }

    InstructionParts instructionParts(line);
    if (!instructionParts.operand().isEmpty())
        return Instruction;

    return Other;
}
// END class AsmFormatter

// BEGIN class InstructionParts
InstructionParts::InstructionParts(QString line)
{
    m_comment = extractComment(line);
    line.remove(m_comment);

    line = line.simplified();
    QStringList parts = line.split(' ', QString::SkipEmptyParts); // QStringList::split( ' ', line ); // 2018.12.01

    bool foundOperand = false;
    QStringList::iterator end = parts.end();
    for (QStringList::iterator it = parts.begin(); it != end; ++it) {
        if (foundOperand) {
            // Already found the operand, so anything else must be the operand
            // data.

            if (m_operandData.isEmpty())
                m_operandData = *it;
            else
                m_operandData += ' ' + *it;

            continue;
        }

        if (PicAsm12bit::self()->operandList().contains((*it).toUpper()) || PicAsm14bit::self()->operandList().contains((*it).toUpper()) || PicAsm16bit::self()->operandList().contains((*it).toUpper())) {
            m_operand = *it;
            foundOperand = true;
        } else {
            // Must be a label
            m_label = *it;
        }
    }
}
// END class InstructionParts
