/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "config.h"
#ifndef NO_GPSIM

#include "gpsimprocessor.h"
#include "katemdi.h"
#include "symbolviewer.h"

#include <KComboBox>
#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>

#include <QDebug>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>

#include <cassert>

static const int NAME_COLUMN = 0;
static const int VALUE_COLUMN = 1;

// BEGIN class SymbolViewerItem
SymbolViewerItem::SymbolViewerItem(SymbolViewer *symbolViewer, const RegisterInfo *registerInfo, int intendedColumn)
    : QObject()
    , QTableWidgetItem()
    , m_pRegisterInfo(registerInfo)
    , m_pSymbolViewer(symbolViewer)
{
    qDebug() << Q_FUNC_INFO << " reg info name " << m_pRegisterInfo->name();
    qDebug() << Q_FUNC_INFO << " row " << row() << " column " << column();

    assert(registerInfo);
    m_pRegisterInfo = registerInfo;
    m_pSymbolViewer = symbolViewer;

    // note: at initial update the column is set to -1, so don't rely on that
    if (intendedColumn == NAME_COLUMN) {
        setText(m_pRegisterInfo->name());
    } else { // VALUE_COLUMN...
        setText(m_pSymbolViewer->toDisplayString(m_pRegisterInfo->value()));
    }

    connect(m_pRegisterInfo, SIGNAL(valueChanged(unsigned)), this, SLOT(valueChanged(unsigned)));
    connect(m_pSymbolViewer, SIGNAL(valueRadixChanged(SymbolViewer::Radix)), this, SLOT(radixChanged()));
}

void SymbolViewerItem::valueChanged(unsigned newValue)
{
    if (column() == VALUE_COLUMN) {
        setText(m_pSymbolViewer->toDisplayString(newValue));
    }
}

void SymbolViewerItem::radixChanged()
{
    if (column() == VALUE_COLUMN) {
        valueChanged(m_pRegisterInfo->value());
    }
}
// END class SymbolViewerItem

// BEGIN class SymbolView
SymbolViewer *SymbolViewer::m_pSelf = nullptr;
SymbolViewer *SymbolViewer::self(KateMDI::ToolView *parent)
{
    if (!m_pSelf) {
        assert(parent);
        m_pSelf = new SymbolViewer(parent);
    }
    return m_pSelf;
}

SymbolViewer::SymbolViewer(KateMDI::ToolView *parent)
    : QWidget((QWidget *)parent)
{
    if (parent->layout()) {
        parent->layout()->addWidget(this);
        qDebug() << Q_FUNC_INFO << " added item selector to parent's layout " << parent;
    } else {
        qWarning() << Q_FUNC_INFO << " unexpected null layout on parent " << parent;
    }

    QGridLayout *grid = new QGridLayout(this /*, 1, 1, 0, 6 */);
    grid->setMargin(0);
    grid->setSpacing(6);

    m_pSymbolList = new QTableWidget(this);
    m_pSymbolList->setFocusPolicy(Qt::NoFocus);
    // grid->addMultiCellWidget( m_pSymbolList, 0, 0, 0, 1 ); // 2018.12.02
    grid->addWidget(m_pSymbolList, 0, 0, 1, 2);

    grid->addWidget(new QLabel(i18n("Value radix:"), this), 1, 0);

    m_pRadixCombo = new KComboBox(false, this);
    grid->addWidget(m_pRadixCombo, 1, 1);
    m_pRadixCombo->insertItem(m_pRadixCombo->count(), i18n("Binary"));
    m_pRadixCombo->insertItem(m_pRadixCombo->count(), i18n("Octal"));
    m_pRadixCombo->insertItem(m_pRadixCombo->count(), i18n("Decimal"));
    m_pRadixCombo->insertItem(m_pRadixCombo->count(), i18n("Hexadecimal"));
    m_valueRadix = Decimal;
    m_pRadixCombo->setCurrentIndex(2);
    connect(m_pRadixCombo, SIGNAL(activated(int)), this, SLOT(selectRadix(int)));

    m_pGpsim = nullptr;
    m_pCurrentContext = nullptr;

    m_pSymbolList->verticalHeader()->setVisible(false);
    m_pSymbolList->horizontalHeader()->setVisible(true);
    // m_pSymbolList->addColumn( i18n("Name") ); // 2018.06.02 - use QTableWidget
    // m_pSymbolList->addColumn( i18n("Value") ); // 2018.06.02 - use QTableWidget
    m_pSymbolList->setColumnCount(2);
    m_pSymbolList->setHorizontalHeaderItem(0, new QTableWidgetItem(i18n("Name")));
    m_pSymbolList->setHorizontalHeaderItem(1, new QTableWidgetItem(i18n("Value")));
    m_pSymbolList->horizontalHeaderItem(0)->setText(i18n("Name"));
    m_pSymbolList->horizontalHeaderItem(1)->setText(i18n("Value"));
    // m_pSymbolList->setFullWidth(true);
    // m_pSymbolList->setColumnWidthMode(1, Q3ListView::Maximum);    // 2018.06.02 - use QTableWidget
    m_pSymbolList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    // m_pSymbolList->setAllColumnsShowFocus( true );    // 2018.06.02 - use QTableWidget
    m_pSymbolList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QHeaderView *symbolListHeader = m_pSymbolList->horizontalHeader();
    symbolListHeader->setSectionResizeMode(0, QHeaderView::Stretch);
    symbolListHeader->setSectionResizeMode(1, QHeaderView::Stretch);
}

SymbolViewer::~SymbolViewer()
{
}

void SymbolViewer::saveProperties(KConfig *config)
{
    // QString oldGroup = config->group();

    KConfigGroup grSym = config->group("SymbolEditor");
    grSym.writeEntry("Radix", (int)m_valueRadix);

    // config->setGroup( oldGroup );
}

void SymbolViewer::readProperties(KConfig *config)
{
    // QString oldGroup = config->group();

    KConfigGroup grSym = config->group("SymbolEditor");

    m_valueRadix = (SymbolViewer::Radix)grSym.readEntry("Radix", (int)Decimal);

    int pos = 4;
    switch (m_valueRadix) {
    case Binary:
        pos--;
    case Octal:
        pos--;
    case Decimal:
        pos--;
    case Hexadecimal:
        pos--;
    }
    m_pRadixCombo->setCurrentIndex(pos);

    // config->setGroup( oldGroup );
}

void SymbolViewer::setContext(GpsimProcessor *gpsim)
{
    RegisterSet *registerSet = gpsim ? gpsim->registerMemory() : nullptr;

    if (registerSet == m_pCurrentContext)
        return;

    m_pSymbolList->clear();
    m_pSymbolList->setColumnCount(2);
    m_pSymbolList->setRowCount(0);

    m_pSymbolList->setHorizontalHeaderItem(0, new QTableWidgetItem(i18n("Name")));
    m_pSymbolList->setHorizontalHeaderItem(1, new QTableWidgetItem(i18n("Value")));
    m_pSymbolList->horizontalHeaderItem(0)->setText(i18n("Name"));
    m_pSymbolList->horizontalHeaderItem(1)->setText(i18n("Value"));

    m_pGpsim = gpsim;
    m_pCurrentContext = registerSet;

    if (!m_pCurrentContext)
        return;

    connect(gpsim, SIGNAL(destroyed()), m_pSymbolList, SLOT(clearContents()));

    unsigned count = m_pCurrentContext->size();
    for (unsigned i = 0; i < count; ++i) {
        RegisterInfo *reg = m_pCurrentContext->fromAddress(i);

        if (!reg) {
            qDebug() << " skip null register at " << i;
            continue;
        }

        if ((reg->type() == RegisterInfo::Generic) || (reg->type() == RegisterInfo::Invalid)) {
            continue;
        }

        qDebug() << Q_FUNC_INFO << " add reg at " << i << " info " << reg;

        m_pSymbolList->insertRow(i);

        SymbolViewerItem *itemName = new SymbolViewerItem(this, reg, 0);
        m_pSymbolList->setItem(i, 0, itemName);
        SymbolViewerItem *itemVal = new SymbolViewerItem(this, reg, 1);
        m_pSymbolList->setItem(i, 1, itemVal);
    }
}

void SymbolViewer::selectRadix(int selectIndex)
{
    if ((selectIndex < 0) || (selectIndex > 3)) {
        qWarning() << Q_FUNC_INFO << "Invalid select position for radix: " << selectIndex << endl;
        return;
    }

    Radix radii[] = {Binary, Octal, Decimal, Hexadecimal};
    Radix newRadix = radii[selectIndex];

    if (newRadix == m_valueRadix)
        return;

    m_valueRadix = newRadix;

    emit valueRadixChanged(m_valueRadix);
}

QString SymbolViewer::toDisplayString(unsigned value) const
{
    switch (m_valueRadix) {
    case Binary:
        return QString::number(value, 2).rightJustified(8, '0', false);

    case Octal:
        return "0" + QString::number(value, 8);

    case Decimal:
        return QString::number(value, 10);

    case Hexadecimal:
        return "0x" + QString::number(value, 16);
    }

    return "?";
}
// END class SymbolView

#endif
