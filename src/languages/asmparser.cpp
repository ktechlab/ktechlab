/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "asmparser.h"
#include "config.h"
#include "gpsimprocessor.h"

#include <QFile>
// TODO revmoe:
#include <QRegExp>
#include <QRegularExpression>
#include <QStringList>

#include <ktechlab_debug.h>

AsmParser::AsmParser(const QString &url)
    : m_url(url)
{
    m_bContainsRadix = false;
    m_type = Absolute;
}

AsmParser::~AsmParser()
{
}

bool AsmParser::parse(GpsimDebugger *debugger)
{
    QFile file(m_url);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QTextStream stream(&file);

    m_type = Absolute;
    m_bContainsRadix = false;
    m_picID = QString();

    // QStringList nonAbsoluteOps = QStringList::split( ",",
    //		"code,.def,.dim,.direct,endw,extern,.file,global,idata,.ident,.line,.type,udata,udata_acs,udata_ovr,udata_shr" );
    QStringList nonAbsoluteOps = QString("code,.def,.dim,.direct,endw,extern,.file,global,idata,.ident,.line,.type,udata,udata_acs,udata_ovr,udata_shr").split(",");

    unsigned inputAtLine = 0;
    while (!stream.atEnd()) {
        const QString line = stream.readLine().trimmed();
        if (m_type != Relocatable) {
            QString col0 = line.section(QRegularExpression("[; ]"), 0, 0);
            col0 = col0.trimmed();
            if (nonAbsoluteOps.contains(col0))
                m_type = Relocatable;
        }

        if (!m_bContainsRadix) {
            if (line.contains(QRegularExpression("^RADIX[\\s]*")) || line.contains(QRegularExpression("^radix[\\s]*")))
                m_bContainsRadix = true;
        }

        if (m_picID.isEmpty()) {
            // We look for "list p = ", and "list p = picid ", and subtract the positions / lengths away from each other to get the picid text position
            QRegExp fullRegExp("[lL][iI][sS][tT][\\s]+[pP][\\s]*=[\\s]*[\\d\\w]+");
            QRegExp halfRegExp("[lL][iI][sS][tT][\\s]+[pP][\\s]*=[\\s]*");

            int startPos = fullRegExp.indexIn(line);
            if ((startPos != -1) && (startPos == halfRegExp.indexIn(line))) {
                m_picID = line.mid(startPos + halfRegExp.matchedLength(), fullRegExp.matchedLength() - halfRegExp.matchedLength());
                m_picID = m_picID.toUpper();
                if (!m_picID.startsWith("P"))
                    m_picID.prepend("P");
            }
        }
#ifndef NO_GPSIM
        if (debugger && line.startsWith(";#CSRC\t")) {
            // Assembly file produced (by sdcc) from C, line is in format:
            // ;#CSRC\t[file-name] [file-line]
            // The filename can contain spaces.
            int fileLineAt = line.lastIndexOf(" ");

            if (fileLineAt == -1)
                qCWarning(KTL_LOG) << "Syntax error in line \"" << line << "\" while looking for file-line";
            else {
                // 7 = length_of(";#CSRC\t")
                QString fileName = line.mid(7, fileLineAt - 7);
                QString fileLineString = line.mid(fileLineAt + 1, line.length() - fileLineAt - 1);

                if (fileName.startsWith("\"")) {
                    // Newer versions of SDCC insert " around the filename
                    fileName.remove(0, 1);                     // First "
                    fileName.remove(fileName.length() - 1, 1); // Last "
                }

                bool ok;
                int fileLine = fileLineString.toInt(&ok) - 1;
                if (ok && fileLine >= 0)
                    debugger->associateLine(fileName, fileLine, m_url, inputAtLine);
                else
                    qCDebug(KTL_LOG) << "Not a valid line number: \"" << fileLineString << "\"";
            }
        }

        if (debugger && (line.startsWith(".line\t") || line.startsWith(";#MSRC"))) {
            // Assembly file produced by either sdcc or microbe, line is in format:
            // \t[".line"/"#MSRC"]\t[file-line]; [file-name]\t[c/microbe source code for that line]
            // We're screwed if the file name contains tabs, but hopefully not many do...
            // QStringList lineParts = QStringList::split( '\t', line ); // 2018.12.01
            QStringList lineParts = line.split('\t', Qt::SkipEmptyParts);
            if (lineParts.size() < 2)
                qCWarning(KTL_LOG) << "Line is in wrong format for extracting source line and file: \"" << line << "\"";
            else {
                const QString lineAndFile = lineParts[1];
                int lineFileSplit = lineAndFile.indexOf("; ");
                if (lineFileSplit == -1)
                    qCDebug(KTL_LOG) << "Could not find file / line split in \"" << lineAndFile << "\"";
                else {
                    QString fileName = lineAndFile.mid(lineFileSplit + 2);
                    QString fileLineString = lineAndFile.left(lineFileSplit);

                    if (fileName.startsWith("\"")) {
                        // Newer versions of SDCC insert " around the filename
                        fileName.remove(0, 1);                     // First "
                        fileName.remove(fileName.length() - 1, 1); // Last "
                    }

                    bool ok;
                    int fileLine = fileLineString.toInt(&ok) - 1;
                    if (ok && fileLine >= 0)
                        debugger->associateLine(fileName, fileLine, m_url, inputAtLine);
                    else
                        qCDebug(KTL_LOG) << "Not a valid line number: \"" << fileLineString << "\"";
                }
            }
        }
#else
        Q_UNUSED(debugger);
#endif // !NO_GPSIM
        inputAtLine++;
    }

    return true;
}
