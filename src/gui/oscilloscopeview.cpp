/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "oscilloscope.h"
#include "oscilloscopedata.h"
#include "oscilloscopeview.h"
#include "probepositioner.h"
#include "simulator.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <QActionGroup>
#include <QCheckBox>
#include <QCursor>
#include <QLabel>
#include <QMenu>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QScrollBar>
#include <QTimer>

#include <algorithm>
#include <cmath>

#include <ktechlab_debug.h>

using namespace std;

inline uint64_t min(uint64_t a, uint64_t b)
{
    return a < b ? a : b;
}

OscilloscopeView::OscilloscopeView(QWidget *parent)
    : QFrame(parent /*, Qt::WNoAutoErase */)
    , b_needRedraw(true)
    , m_pixmap(nullptr)
    , m_fps(10)
    , m_sliderValueAtClick(-1)
    , m_clickOffsetPos(-1)
    , m_pSimulator(Simulator::self())
    , m_halfOutputHeight(0.0)
{
    auto config = KSharedConfig::openConfig();
    KConfigGroup grOscill(config, "Oscilloscope");
    m_fps = grOscill.readEntry("FPS", 25);

    setBackgroundRole(QPalette::NoRole);
    setMouseTracking(true);

    m_updateViewTmr = new QTimer(this);
    connect(m_updateViewTmr, SIGNAL(timeout()), this, SLOT(updateViewTimeout()));
}

OscilloscopeView::~OscilloscopeView()
{
    delete m_pixmap;
}

void OscilloscopeView::updateView()
{
    if (m_updateViewTmr->isActive())
        return;

    m_updateViewTmr->setSingleShot(true);
    m_updateViewTmr->start(1000 / m_fps);
}

void OscilloscopeView::updateViewTimeout()
{
    b_needRedraw = true;
    repaint();
}

void OscilloscopeView::resizeEvent(QResizeEvent *e)
{
    delete m_pixmap;
    m_pixmap = new QPixmap(e->size());
    b_needRedraw = true;
    QFrame::resizeEvent(e);
}

void OscilloscopeView::mousePressEvent(QMouseEvent *event)
{
    switch (event->button()) {
    case Qt::LeftButton: {
        event->accept();

        m_clickOffsetPos = event->pos().x();
        m_sliderValueAtClick = Oscilloscope::self()->horizontalScroll->value();
        return;
    }

    case Qt::RightButton: {
        event->accept();

        QMenu fpsMenu;
        QAction *titleAction = fpsMenu.addAction(i18n("Framerate"));
        titleAction->setEnabled(false);
        fpsMenu.addSeparator();

        QActionGroup *fpsGroup = new QActionGroup(&fpsMenu);
        fpsGroup->setExclusive(true);

        const std::vector<int> fpsOptions = {10, 25, 50, 75, 100};
        int currentFps = m_fps;

        for (int num : fpsOptions) {
            QAction *a = fpsMenu.addAction(i18n("%1 fps", QString::number(num)));
            a->setData(num);
            a->setCheckable(true);
            if (num == currentFps) {
                a->setChecked(true);
            }
            fpsGroup->addAction(a);
        }

        connect(&fpsMenu, SIGNAL(triggered(QAction *)), this, SLOT(slotSetFrameRate(QAction *)));
        fpsMenu.exec(event->globalPosition().toPoint());
        return;
    }

    default: {
        QFrame::mousePressEvent(event);
        return;
    }
    }
}

void OscilloscopeView::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();

    m_mouseXPos = event->pos().x();

    if (m_sliderValueAtClick != -1) {
        int dx = event->pos().x() - m_clickOffsetPos;
        int dTick = int(dx * Oscilloscope::self()->sliderTicksPerSecond() / Oscilloscope::self()->pixelsPerSecond());
        Oscilloscope::self()->horizontalScroll->setValue(m_sliderValueAtClick - dTick);
    }

    update();
}

void OscilloscopeView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_sliderValueAtClick == -1)
        return QFrame::mouseReleaseEvent(event);

    event->accept();
    m_sliderValueAtClick = -1;
}

void OscilloscopeView::slotSetFrameRate(QAction *action)
{
    int fps = action->data().toInt();
    m_fps = fps;
    KConfigGroup grOscill(KSharedConfig::openConfig(), "Oscilloscope");
    grOscill.writeEntry("FPS", m_fps);
}

void OscilloscopeView::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    if (!p.isActive()) {
        qCWarning(KTL_LOG) << "Painter is not active";
        return;
    }

    if (b_needRedraw) {
        updateOutputHeight();
        if (!m_pixmap) {
            qCWarning(KTL_LOG) << "Unexpected null m_pixmap";
            return;
        }

        QPainter pixmapPainter(m_pixmap);
        if (!pixmapPainter.isActive()) {
            qCWarning(KTL_LOG) << "Pixmap painter is not active";
            return;
        }

        m_pixmap->fill(palette().color(backgroundRole()));
        drawGrid(pixmapPainter);
        drawLogicData(pixmapPainter);
        drawFloatingData(pixmapPainter);
        pixmapPainter.setPen(Qt::black);
        pixmapPainter.drawRect(frameRect());
        b_needRedraw = false;
    }

    QRect r = e->rect();
    p.drawPixmap(r, *m_pixmap, r);

    if (testAttribute(Qt::WA_UnderMouse)) {
        drawTimeCursorLine(p);
    }
}

void OscilloscopeView::drawGrid(QPainter &p)
{
    QPen gridPen(QColor(128, 128, 128, 128)); // Light grey color, semi-transparent
    gridPen.setStyle(Qt::SolidLine);
    p.setPen(gridPen);

    const double verticalSpacing = 50.0;
    for (double x = 0; x <= m_pixmap->width(); x += verticalSpacing) {
        p.drawLine(QPointF(x, 0), QPointF(x, m_pixmap->height()));
    }

    const double horizontalSpacing = 50.0;
    for (double y = 0; y <= m_pixmap->height(); y += horizontalSpacing) {
        p.drawLine(QPointF(0, y), QPointF(m_pixmap->width(), y));
    }
}

void OscilloscopeView::drawTimeCursorLine(QPainter &p)
{
    QPen linePen(Qt::red);
    linePen.setStyle(Qt::DashLine);
    p.setPen(linePen);
    p.drawLine(QPoint(m_mouseXPos, 0), QPoint(m_mouseXPos, height()));

    // Fetch the time from Oscilloscope and prepare for display.
    int x = mapFromGlobal(QCursor::pos()).x();
    double time = (double(Oscilloscope::self()->scrollTime()) / LOGIC_UPDATE_RATE) + (x / Oscilloscope::self()->pixelsPerSecond());
    QString timeText = QString::number(time, 'f', 6);

    QFont font = p.font();
    font.setPointSize(10); // Set font size.
    p.setFont(font);

    QRect textRect = p.fontMetrics().boundingRect(timeText);
    textRect.moveCenter(QPoint(m_mouseXPos, 20));
    textRect.adjust(-2, -2, 2, 2);
    p.fillRect(textRect.adjusted(-2, -2, 2, 2), Qt::white);
    p.setPen(Qt::black);
    p.drawText(textRect, Qt::AlignCenter, timeText);
}

void OscilloscopeView::updateOutputHeight()
{
    m_halfOutputHeight = int((Oscilloscope::self()->probePositioner->probeOutputHeight() - (probeArrowWidth / Oscilloscope::self()->numberOfProbes())) / 2) - 1;
}

void OscilloscopeView::drawLogicData(QPainter &p)
{
    const double pixelsPerSecond = Oscilloscope::self()->pixelsPerSecond();

    const LogicProbeDataMap::iterator end = Oscilloscope::self()->m_logicProbeDataMap.end();
    for (LogicProbeDataMap::iterator it = Oscilloscope::self()->m_logicProbeDataMap.begin(); it != end; ++it) {
        // When searching for the next logic value to display, we look along
        // until there is a recorded point which is at least one pixel along
        // If we are zoomed out far, there might be thousands of data points
        // between each pixel. It is time consuming searching for the next point
        // to display one at a time, so we record the average number of data points
        // between pixels ( = deltaAt / totalDeltaAt)
        int64_t deltaAt = 1;
        int totalDeltaAt = 1;

        LogicProbeData *probe = it.value();

        vector<LogicDataPoint> *data = probe->m_data;
        if (!data->size())
            continue;

        const int midHeight = Oscilloscope::self()->probePositioner->probePosition(probe);
        const int64_t timeOffset = Oscilloscope::self()->scrollTime();

        // Draw the horizontal line indicating the midpoint of our output
        p.setPen(QColor(228, 228, 228));
        p.drawLine(0, midHeight, width(), midHeight);

        // Set the pen colour according to the colour the user has selected for the probe
        p.setPen(probe->color());

        // The smallest time step that will display in our oscilloscope
        const int minTimeStep = int(LOGIC_UPDATE_RATE / pixelsPerSecond);

        int64_t at = probe->findPos(timeOffset);
        const int64_t maxAt = probe->m_data->size();
        int64_t prevTime = (*data)[at].time;
        int prevX = (at > 0) ? 0 : int((prevTime - timeOffset) * (pixelsPerSecond / LOGIC_UPDATE_RATE));
        bool prevHigh = (*data)[at].value;
        int prevY = midHeight + int(prevHigh ? -m_halfOutputHeight : +m_halfOutputHeight);

        while (at < maxAt) {
            // Search for the next pos which will show up at our zoom level
            int64_t previousAt = at;
            int64_t dAt = deltaAt / totalDeltaAt;

            while ((dAt > 1) && (at < maxAt) && ((int64_t((*data)[at].time) - prevTime) != minTimeStep)) {
                // Search forwards until we overshoot
                while (at < maxAt && (int64_t((*data)[at].time) - prevTime) < minTimeStep)
                    at += dAt;
                dAt /= 2;

                // Search backwards until we undershoot
                while ((at < maxAt) && (int64_t((*data)[at].time) - prevTime) > minTimeStep) {
                    at -= dAt;
                    if (at < 0)
                        at = 0;
                }
                dAt /= 2;
            }

            // Possibly increment the value of at found by one (or more if this is the first go)
            while ((previousAt == at) || ((at < maxAt) && (int64_t((*data)[at].time) - prevTime) < minTimeStep))
                at++;

            if (at >= maxAt)
                break;

            // Update the average values
            deltaAt += at - previousAt;
            totalDeltaAt++;

            bool nextHigh = (*data)[at].value;
            if (nextHigh == prevHigh)
                continue;

            int64_t nextTime = (*data)[at].time;
            int nextX = int((nextTime - timeOffset) * (pixelsPerSecond / LOGIC_UPDATE_RATE));
            int nextY = midHeight + int(nextHigh ? -m_halfOutputHeight : +m_halfOutputHeight);

            p.drawLine(prevX, prevY, nextX, prevY);
            p.drawLine(nextX, prevY, nextX, nextY);

            prevHigh = nextHigh;
            prevTime = nextTime;
            prevX = nextX;
            prevY = nextY;

            if (nextX > width())
                break;
        };

        // If we could not draw right to the end; it is because we exceeded
        // maxAt
        if (prevX < width())
            p.drawLine(prevX, prevY, width(), prevY);
    }
}

#define v_to_y int(midHeight - (logarithmic ? ((v > 0) ? log(v / lowerAbsValue) : -log(-v / lowerAbsValue)) : v) * sf)

void OscilloscopeView::drawFloatingData(QPainter &p)
{
    const double pixelsPerSecond = Oscilloscope::self()->pixelsPerSecond();

    const FloatingProbeDataMap::iterator end = Oscilloscope::self()->m_floatingProbeDataMap.end();
    for (FloatingProbeDataMap::iterator it = Oscilloscope::self()->m_floatingProbeDataMap.begin(); it != end; ++it) {
        FloatingProbeData *probe = it.value();
        vector<float> *data = probe->m_data;

        if (!data->size())
            continue;

        bool logarithmic = probe->scaling() == FloatingProbeData::Logarithmic;
        double lowerAbsValue = probe->lowerAbsValue();
        double sf = m_halfOutputHeight / (logarithmic ? log(probe->upperAbsValue() / lowerAbsValue) : probe->upperAbsValue());

        const int midHeight = Oscilloscope::self()->probePositioner->probePosition(probe);
        const int64_t timeOffset = Oscilloscope::self()->scrollTime();

        // Draw the horizontal line indicating the midpoint of our output
        p.setPen(QColor(228, 228, 228));
        p.drawLine(0, midHeight, width(), midHeight);

        // Set the pen colour according to the colour the user has selected for the probe
        p.setPen(probe->color());

        int64_t at = probe->findPos(timeOffset);
        const int64_t atEnd = probe->m_data->size();
        if (at > atEnd)
            at = atEnd;
        int64_t prevTime = probe->toTime(at);

        double v = 0;
        if (at < atEnd) {
            v = (*data)[(at > 0) ? at : 0];
        }
        int prevY = v_to_y;
        int prevX = int((prevTime - timeOffset) * (pixelsPerSecond / LOGIC_UPDATE_RATE));

        while (at < atEnd - 1) {
            at++;

            uint64_t nextTime = prevTime + uint64_t(LOGIC_UPDATE_RATE * LINEAR_UPDATE_PERIOD);

            double v = (*data)[(at > 0) ? at : 0];
            int nextY = v_to_y;
            int nextX = int((nextTime - timeOffset) * (pixelsPerSecond / LOGIC_UPDATE_RATE));

            p.drawLine(prevX, prevY, nextX, nextY);

            prevTime = nextTime;
            prevX = nextX;
            prevY = nextY;

            if (nextX > width())
                break;
        };

        // If we could not draw right to the end; it is because we exceeded
        // maxAt
        if (prevX < width())
            p.drawLine(prevX, prevY, width(), prevY);
    }
}

#include "moc_oscilloscopeview.cpp"
