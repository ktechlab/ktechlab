/*
 * Copyright 2009-2010 Julian Bäume <julian@svg4all.de>
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
#include <KIcon>
#include <KActionMenu>
#include <KLocalizedString>
#include <KActionCollection>
#include "circuitscene.h"

using namespace KTechLab;

CircuitView::CircuitView ( CircuitScene* scene, QWidget* parent )
    : QGraphicsView ( scene, parent ),
      m_scene(scene)
{
    setXMLFile("ktechlabcircuitui.rc");
    setupActions();

    init();
}

CircuitView::~CircuitView()
{
}

void CircuitView::init()
{
    setFrameStyle(QFrame::NoFrame);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setRenderHints(QPainter::Antialiasing);
    setAcceptDrops( true );
    setDragMode( QGraphicsView::RubberBandDrag );
}

void CircuitView::setupActions()
{
    KActionCollection* ac = actionCollection();
    KAction* action;

    action = ac->addAction( QString("edit_rotate_cw") );
    action->setText( i18n("Rotate Clockwise") );
    action->setIcon( KIcon("object-rotate-right") );
    connect( action, SIGNAL(triggered()), this, SLOT(slotComponentRotateCW()) );

    action = ac->addAction( QString("edit_rotate_ccw") );
    action->setText( i18n("Rotate Counter-Clockwise") );
    action->setIcon( KIcon("object-rotate-left") );
    connect( action, SIGNAL(triggered()), this, SLOT(slotComponentRotateCCW()) );

    action = ac->addAction( QString("edit_flip_horizontally") );
    action->setText( i18n("Flip Horizontally") );
    action->setIcon( KIcon("object-flip-horizontal") );
    connect( action, SIGNAL(triggered()), this, SLOT(slotComponentFlipHorizontal()) );

    action = ac->addAction( QString("edit_flip_vertically") );
    action->setText( i18n("Flip Vertically") );
    action->setIcon( KIcon("object-flip-vertical") );
    connect( action, SIGNAL(triggered()), this, SLOT(slotComponentFlipVertical()) );
}

void CircuitView::slotComponentFlipHorizontal()
{
    m_scene->flipSelectedComponents(Qt::XAxis);
}

void CircuitView::slotComponentFlipVertical()
{
    m_scene->flipSelectedComponents(Qt::YAxis);
}

void CircuitView::slotComponentRotateCCW()
{
    m_scene->rotateSelectedComponents(-90);
}

void CircuitView::slotComponentRotateCW()
{
    m_scene->rotateSelectedComponents(90);
}

#include "circuitview.moc"

