/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2015  Zoltan Padrah <zoltan_padrah@users.sf.net>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include "logtofilemsghandler.h"

#include <QtCore/QtGlobal>
#include <QtCore/QDebug>
#include <QtCore/QDate>
#include <Qt/qapplication.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

static FILE *logFile = 0;

static void ktlLogToFile(QtMsgType type, const char *msg) {
    if (!logFile) {
        return;
    }
    QString nowStr = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm,zzz");
    const char *nowCStr = nowStr.toLatin1().data();
    switch (type) {
    case QtDebugMsg:
        fprintf(logFile, "%s (DD) %s\n", nowCStr, msg);
        fflush(logFile);
        break;
    case QtWarningMsg:
        fprintf(logFile, "%s (WW) %s\n", nowCStr, msg);
        fflush(logFile);
        break;
    case QtCriticalMsg:
        fprintf(logFile, "%s (Critical) %s\n", nowCStr, msg);
        fflush(logFile);
        break;
    case QtFatalMsg:
        fprintf(stderr, " %s (Fatal) %s\n", nowCStr, msg);
        fflush(logFile);
    }
}

static void ktlLogToStderr(QtMsgType type, const char *msg) {
    switch (type) {
    case QtDebugMsg:
        //fprintf(stderr, "(DD) %s\n", msg); // quite noisy
        break;
    case QtWarningMsg:
        fprintf(stderr, "(WW) %s\n", msg);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "(Critical) %s\n", msg);
        break;
    case QtFatalMsg:
        fprintf(stderr, "(Fatal) %s\n", msg);
    }
}

static void ktlMessageOutput(QtMsgType type, const char *msg) {
    ktlLogToFile(type, msg);
    ktlLogToStderr(type, msg);
    if (QtFatalMsg == type) {
        abort();
    }
}

LogToFileMsgHandler::LogToFileMsgHandler()
{
    logFile = 0;
    qint64 appPid = QApplication::applicationPid();
    QString logFileName = QString("/tmp/ktechlab-pid-%1-log").arg(appPid);
    qDebug() << "Starting logging to " << logFileName;
    logFile = fopen(logFileName.toLatin1().data(), "w+");
    if (!logFile) {
        const int lastErrno = errno;
        qWarning() << "Failed to create log file" << logFileName
            << ". errno=" << lastErrno << ", strerror=" << strerror(lastErrno);
        return;
    }
    qInstallMsgHandler(ktlMessageOutput);
    qDebug() << "logging started to " << logFileName << " by " << this;
}

LogToFileMsgHandler::~LogToFileMsgHandler()
{
    qDebug() << "logging ending by " << this;
    const int closeRet = fclose(logFile);
    logFile = 0;
    if (closeRet) {
        const int lastErrno = errno;
        qCritical() << "failed to close log file, errno=" << lastErrno;
    }
    qInstallMsgHandler(0);
    if (closeRet) {
        const int lastErrno = errno;
        qCritical() << "failed to close log file, errno=" << lastErrno;
    }
}
