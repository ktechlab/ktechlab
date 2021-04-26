/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "subcircuits.h"
#include "circuitdocument.h"
#include "ecsubcircuit.h"
#include "itemdocumentdata.h"
#include "itemlibrary.h"
#include "itemselector.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>

#include <ktechlab_debug.h>

Subcircuits::Subcircuits()
    : QObject()
{
    connect(ComponentSelector::self(), &ComponentSelector::itemRemoved, this, &Subcircuits::slotItemRemoved);
}

Subcircuits::~Subcircuits()
{
}

static QList<int> asIntList(const QString &string)
{
    QList<QByteArray> entries = string.toLatin1().split(',');
    QList<int> list;
    for (const QByteArray &s : entries)
        list << s.toInt();
    return list;
}

void Subcircuits::initECSubcircuit(int subcircuitId, ECSubcircuit *ecSubcircuit)
{
    const QString fileName = genFileName(subcircuitId);
    if (!QFile::exists(fileName)) {
        qCDebug(KTL_LOG) << "Subcircuits::createSubcircuit: Subcircuit \"" << fileName << "\" was not found.";
        return;
    }

    SubcircuitData subcircuit;
    if (!subcircuit.loadData(QUrl::fromLocalFile(genFileName(subcircuitId))))
        return;

    subcircuit.initECSubcircuit(ecSubcircuit);
}

ECSubcircuit *Subcircuits::createSubcircuit(int id, CircuitDocument *circuitDocument, bool newItem, const char *newId)
{
    // I pass finishCreation = false here because the subcircuit was getting
    // finished twice, causing a segfault in CircuitDocument::assignCircuits()
    //--electronerd
    ECSubcircuit *ecSubcircuit = static_cast<ECSubcircuit *>(itemLibrary()->createItem("ec/subcircuit", circuitDocument, newItem, newId, false));
    ecSubcircuit->property("id")->setValue(id);
    return ecSubcircuit;
}

void Subcircuits::loadSubcircuits()
{
    // KConfig *config = kapp->config();
    KSharedConfigPtr config = KSharedConfig::openConfig();
    // config->setGroup("Subcircuits");
    KConfigGroup configGrSubcirc = config->group("Subcircuits");

    QList<int> idList = asIntList(configGrSubcirc.readEntry<QString>(QString("Ids"), QString()));
    const QList<int>::iterator idListEnd = idList.end();
    for (QList<int>::iterator it = idList.begin(); it != idListEnd; ++it) {
        QFile file(genFileName(*it));
        if (file.open(QIODevice::ReadOnly) == false) {
            // File has mysteriously disappeared....
            *it = -1;
        } else {
            KConfigGroup configGrSubcNr = config->group("Subcircuit_" + QString::number(*it));
            updateComponentSelector(*it, configGrSubcNr.readEntry("Name"));
        }
        file.close();
    }
    idList.removeAll(-1);

    // Update the config file if any ids have been removed
    // config->setGroup("Subcircuits");
    configGrSubcirc.writeEntry("Ids", idList);
}

QString Subcircuits::genFileName(const int nextId)
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/subcircuit_" + QString::number(nextId) + ".circuit";
}

void Subcircuits::updateComponentSelector(int id, const QString &name)
{
    if (name.isEmpty())
        return;

    ComponentSelector::self()->addItem(name, "sc/" + QString::number(id), i18n("Subcircuits"), QIcon::fromTheme("application-x-circuit"), true);
}

void Subcircuits::addSubcircuit(const QString &name, const QString &subcircuitXml)
{
    // KConfig *config = kapp->config();
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup subcircGroup = config->group("Subcircuits");

    int nextId = subcircGroup.readEntry<int>("NextId", 0);

    // ensure dir exists
    QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    while (QFile::exists(genFileName(nextId))) {
        nextId++;
    }

    const int id = nextId;

    const QString fileName = genFileName(id);
    QFile file(fileName);

    if (file.open(QIODevice::WriteOnly) == false) {
        qCCritical(KTL_LOG) << "Subcircuits::addSubcircuit: couldn't open subcircuit save file: " << fileName;
        return;
    }

    QTextStream stream(&file);
    stream << subcircuitXml;
    file.close();

    QList<int> idList = asIntList(subcircGroup.readEntry<QString>(QString("Ids"), QString()));
    idList += id;
    subcircGroup.writeEntry("Ids", idList);
    subcircGroup.writeEntry("NextId", ++nextId);

    KConfigGroup grSubcircNr = config->group("Subcircuit_" + QString::number(id));
    grSubcircNr.writeEntry("Name", name);

    // It's important that we write the configuration *now*, lest the subcircuits be lost
    grSubcircNr.sync();

    updateComponentSelector(id, name);
}

void Subcircuits::slotItemRemoved(const QString &id)
{
    if (!id.startsWith("sc/")) {
        return;
    }

    QString temp = id;
    temp.remove("sc/");
    const int id_num = temp.toInt();
    const QString fileName = genFileName(id_num);
    QFile file(fileName);
    file.remove();

    // KConfig *config = kapp->config();
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup grSc = config->group("Subcircuits");
    QList<int> idList = asIntList(grSc.readEntry<QString>(QString("Ids"), QString()));
    idList.removeAll(id_num);
    grSc.writeEntry("Ids", idList);
}
