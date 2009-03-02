/*
 * Copyright 2007 Frerich Raabe <raabe@kde.org>
 * Copyright 2007 Aaron Seigo <aseigo@kde.org>
 * Copyright 2008 Aleix Pol <aleixpol@gmail.com>
 * Copyright 2009 Julian Bäume <julian@svg4all.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "circuitview.h"

#include "circuitdocument.h"

#include <QApplication>
#include <QIcon>
#include <QResizeEvent>

#include <KIconLoader>
#include <KStandardDirs>

#include <Plasma/Containment>
#include <Plasma/Wallpaper>

using namespace Plasma;

CircuitView::CircuitView( QWidget *parent )
    : QGraphicsView(parent),
      m_formfactor(Plasma::Planar),
      m_location(Plasma::FullScreen),
      m_containment(0),
      m_applet(0)
{
    setFrameStyle(QFrame::NoFrame);

    setScene(&m_corona);
    connect(&m_corona, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(sceneRectChanged(QRectF)));
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

void CircuitView::addApplet(const QString &name, const QString &containment,
                         const QString& wallpaper, const QVariantList &args)
{
    if ( !m_containment ){
        kDebug() << "adding applet" << name << "in" << containment;
        m_containment = m_corona.addContainment(containment);
        connect(m_containment, SIGNAL(appletRemoved(Plasma::Applet*)), this, SLOT(appletRemoved()));
    }
    m_applet = m_containment->addApplet(name, args, QRectF(0, 0, -1, -1));

    addApplet( m_applet, containment, wallpaper, args );
}

void CircuitView::addApplet( Plasma::Applet *applet, const QString &containment,
                         const QString& wallpaper, const QVariantList &args)
{
    if ( !m_applet ) {
        m_applet = applet;
    }
    if ( !m_containment ){
        kDebug() << "adding applet" << "in" << containment;
        m_containment = m_corona.addContainment(containment);
        connect(m_containment, SIGNAL(appletRemoved(Plasma::Applet*)), this, SLOT(appletRemoved()));
    }

    if (!wallpaper.isEmpty()) {
        m_containment->setWallpaper(wallpaper);
    }

    m_containment->setFormFactor(m_formfactor);
    m_containment->setLocation(m_location);
    setScene(m_containment->scene());

    m_containment->addApplet( applet );
    applet->setFlag(QGraphicsItem::ItemIsMovable, false);

    setSceneRect(applet->geometry());
}

void CircuitView::appletRemoved()
{
    m_applet = 0;
}

void CircuitView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    if (!m_applet) {
        kDebug() << "no applet";
        return;
    }

    //kDebug() << size();
    qreal newWidth = 0;
    qreal newHeight = 0;

    if (m_applet->aspectRatioMode() == Plasma::KeepAspectRatio) {
        // The applet always keeps its aspect ratio, so let's respect it.
        qreal ratio = m_applet->size().width() / m_applet->size().height();
        qreal widthForCurrentHeight = (qreal)size().height() * ratio;
        if (widthForCurrentHeight > size().width()) {
            newHeight = size().width() / ratio;
            newWidth = newHeight * ratio;
        } else {
            newWidth = widthForCurrentHeight;
            newHeight = newWidth / ratio;
        }
    } else {
        newWidth = size().width();
        newHeight = size().height();
    }
    QSizeF newSize(newWidth, newHeight);

    m_containment->resize(size());
    // check if the rect is valid, or else it seems to try to allocate
    // up to infinity memory in exponential increments
    if (newSize.isValid()) {
        m_applet->resize(QSizeF(newWidth, newHeight));
    }
}

void CircuitView::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
}

void CircuitView::sceneRectChanged(const QRectF &rect)
{
    Q_UNUSED(rect)
    if (m_applet) {
        //kDebug() << m_applet->geometry();
        setSceneRect(m_applet->geometry());
    }
}

#include "circuitview.moc"

