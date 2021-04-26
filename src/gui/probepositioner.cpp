/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "probepositioner.h"
#include "oscilloscope.h"
#include "oscilloscopedata.h"
#include "oscilloscopeview.h"

#include <QPaintEvent>
#include <QPainter>
// #include <q3pointarray.h> // 2018.08.14

#include <algorithm>
#include <cmath>

#include <ktechlab_debug.h>

ProbePositioner::ProbePositioner(QWidget *parent)
    : QWidget(parent /* , Qt::WNoAutoErase */)
{
    m_probePosOffset = 0;
    p_draggedProbe = nullptr;
    setFixedWidth(int(probeArrowWidth));
    // setBackgroundMode(Qt::NoBackground); // 2018.12.07
    setBackgroundRole(QPalette::NoRole);
    b_needRedraw = true;
    m_pixmap = nullptr;
}

ProbePositioner::~ProbePositioner()
{
    delete m_pixmap;
}

void ProbePositioner::forceRepaint()
{
    b_needRedraw = true;
    repaint(/* false - 2018.12.07 */);
}

int ProbePositioner::probeOutputHeight() const
{
    int height = int(Oscilloscope::self()->oscilloscopeView->height() - probeArrowHeight);
    int numProbes = Oscilloscope::self()->numberOfProbes();
    if (numProbes == 0)
        numProbes = 1;
    return height / numProbes;
}

int ProbePositioner::probePosition(ProbeData *probeData) const
{
    if (!probeData)
        return -1;

    int spacing = probeOutputHeight();
    int probeNum = Oscilloscope::self()->probeNumber(probeData->id());

    return int(probeArrowHeight / 2 + spacing * (probeNum + probeData->drawPosition()));
}

void ProbePositioner::setProbePosition(ProbeData *probeData, int position)
{
    if (!probeData)
        return;

    int height = int(Oscilloscope::self()->oscilloscopeView->height() - probeArrowHeight);
    int numProbes = Oscilloscope::self()->numberOfProbes();
    int spacing = height / numProbes;
    int probeNum = Oscilloscope::self()->probeNumber(probeData->id());

    int minPos = int(probeArrowHeight / 2);
    int maxPos = int(Oscilloscope::self()->oscilloscopeView->height() - (probeArrowHeight / 2)) - 1;
    if (position < minPos)
        position = minPos;
    else if (position > maxPos)
        position = maxPos;

    probeData->setDrawPosition(float(position - probeArrowHeight / 2) / float(spacing) - probeNum);

    forceRepaint();
    Oscilloscope::self()->oscilloscopeView->updateView();
}

ProbeData *ProbePositioner::probeAtPosition(const QPoint &pos)
{
    int relativeArrowHeight = int(probeArrowHeight * (1. - float(pos.x() / probeArrowWidth)));

    const ProbeDataMap::const_iterator end = m_probeDataMap.end();
    for (ProbeDataMap::const_iterator it = m_probeDataMap.begin(); it != end; ++it) {
        ProbeData *probeData = it.value();
        int currentPos = probePosition(probeData);
        m_probePosOffset = pos.y() - currentPos;
        if (std::abs(m_probePosOffset) <= relativeArrowHeight)
            return probeData;
    }
    m_probePosOffset = 0;
    return nullptr;
}

void ProbePositioner::slotProbeDataRegistered(int id, ProbeData *probe)
{
    m_probeDataMap[id] = probe;
    connect(probe, &ProbeData::displayAttributeChanged, this, &ProbePositioner::forceRepaint);

    // This connect doesn't really belong here, but it save a lot of code
    connect(probe, &ProbeData::displayAttributeChanged, Oscilloscope::self()->oscilloscopeView, &OscilloscopeView::updateView);
    forceRepaint();
    Oscilloscope::self()->oscilloscopeView->updateView();
}

void ProbePositioner::slotProbeDataUnregistered(int id)
{
    m_probeDataMap.remove(id);
    // We "set" the position of each probe to force it into proper bounds

    const ProbeDataMap::const_iterator end = m_probeDataMap.end();
    for (ProbeDataMap::const_iterator it = m_probeDataMap.begin(); it != end; ++it)
        setProbePosition(it.value(), probePosition(it.value()));

    forceRepaint();
}

void ProbePositioner::resizeEvent(QResizeEvent *e)
{
    delete m_pixmap;
    m_pixmap = new QPixmap(e->size());
    QWidget::resizeEvent(e);
    forceRepaint();
}

void ProbePositioner::mousePressEvent(QMouseEvent *e)
{
    p_draggedProbe = probeAtPosition(e->pos());
    if (p_draggedProbe)
        e->accept();
    else
        e->ignore();
}

void ProbePositioner::mouseReleaseEvent(QMouseEvent *e)
{
    if (p_draggedProbe)
        e->accept();
    else
        e->ignore();
}

void ProbePositioner::mouseMoveEvent(QMouseEvent *e)
{
    if (!p_draggedProbe) {
        e->ignore();
        return;
    }
    e->accept();

    setProbePosition(p_draggedProbe, e->pos().y() - m_probePosOffset);
    forceRepaint();
}

void ProbePositioner::paintEvent(QPaintEvent *e)
{
    QRect r = e->rect();

    if (b_needRedraw) {
        if (!m_pixmap) {
            qCWarning(KTL_LOG) << " unexpected null m_pixmap in " << this;
            return;
        }

        QPainter p;
        // m_pixmap->fill( paletteBackgroundColor() );
        m_pixmap->fill(palette().color(backgroundRole()));
        const bool startSuccess = p.begin(m_pixmap);
        if ((!startSuccess) || (!p.isActive())) {
            qCWarning(KTL_LOG) << " painter is not active";
        }

        p.setClipRegion(e->region());

        const ProbeDataMap::const_iterator end = m_probeDataMap.end();
        for (ProbeDataMap::const_iterator it = m_probeDataMap.begin(); it != end; ++it) {
            ProbeData *probeData = it.value();
            p.setBrush(probeData->color());
            int currentPos = probePosition(probeData);

            QPolygon pa(3);
            pa[0] = QPoint(0, int(currentPos - (probeArrowHeight / 2)));
            pa[1] = QPoint(int(probeArrowWidth), currentPos);
            pa[2] = QPoint(0, int(currentPos + (probeArrowHeight / 2)));

            p.drawPolygon(pa);
        }
        b_needRedraw = false;
    }

    // bitBlt( this, r.x(), r.y(), m_pixmap, r.x(), r.y(), r.width(), r.height() ); // 2018.12.07
    QPainter p;
    const bool paintStarted = p.begin(this);
    if (!paintStarted) {
        qCWarning(KTL_LOG) << " failed to start painting ";
    }
    p.drawImage(r, m_pixmap->toImage(), r);
}
