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
#include <KToggleAction>
#include <KLocalizedString>
#include <KActionCollection>
#include "circuitscene.h"
#include "circuitdocument.h"
#include <interfaces/iplugincontroller.h>
#include <interfaces/icore.h>
#include <interfaces/irouterplugin.h>
#include <KSelectAction>

using namespace KTechLab;

CircuitView::CircuitView ( KTechLab::CircuitDocument* document, QWidget* parent )
    : QGraphicsView ( document->documentScene(), parent ),
      m_document(document),
      m_scene(static_cast<CircuitScene*>(document->documentScene()))
{
#if KDE_ENABLED // FIXME this is ugly
    setXMLFile("ktechlabcircuitui.rc");
#endif
    setRoutingMode(QLatin1String("ktlautomatic_router"));
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

#if KDE_ENABLED
void CircuitView::setupActions()
{
    KActionCollection* ac = actionCollection();
    KAction* action;

    action = ac->addAction( QString("edit_rotate_cw") );
    action->setText( i18n("Rotate Clockwise") );
    action->setIcon( KIcon("object-rotate-right") );
    connect( action, SIGNAL(triggered()), this, SLOT(componentRotateCW()) );

    action = ac->addAction( QString("edit_rotate_ccw") );
    action->setText( i18n("Rotate Counter-Clockwise") );
    action->setIcon( KIcon("object-rotate-left") );
    connect( action, SIGNAL(triggered()), this, SLOT(componentRotateCCW()) );

    action = ac->addAction( QString("edit_flip_horizontally") );
    action->setText( i18n("Flip Horizontally") );
    action->setIcon( KIcon("object-flip-horizontal") );
    connect( action, SIGNAL(triggered()), this, SLOT(componentFlipHorizontal()) );

    action = ac->addAction( QString("edit_flip_vertically") );
    action->setText( i18n("Flip Vertically") );
    action->setIcon( KIcon("object-flip-vertical") );
    connect( action, SIGNAL(triggered()), this, SLOT(componentFlipVertical()) );

    //TODO: move this into a basic document-view to re-use it for flow-code
    action = ac->addAction( KStandardAction::Save );
    connect( action, SIGNAL(triggered()), this, SLOT(save()) );
//     action = ac->addAction( KStandardAction::SaveAs );
//     connect( action, SIGNAL(triggered()), this, SLOT(saveAs()) );
//     action = ac->addAction( KStandardAction::Revert );
//     connect( action, SIGNAL(triggered()), this, SLOT(revert()) );

    m_routerModeActions = new KSelectAction(this);
    m_routerModeActions->setText( i18n("Routing Mode") );
    ac->addAction(QString("routing_mode_actionmenu"), m_routerModeActions);
    connect(m_routerModeActions,SIGNAL(triggered(QAction*)),this,SLOT(routingModeChanged(QAction*)));
    QList<QAction*> routingModeActions;
    KDevelop::IPluginController *pc = KDevelop::ICore::self()->pluginController();
    foreach(KDevelop::IPlugin* plugin, pc->allPluginsForExtension("org.ktechlab.IRouterPlugin")) {
        const KPluginInfo &info = pc->pluginInfo(plugin);
        action = new KToggleAction( KIcon(info.icon()), QString("edit_mode_%1").arg(info.pluginName()), this );
        action->setText( info.name() );
        action->setData( info.pluginName() );
        action->setCheckable(true);
        action->setChecked(m_currentRouterName == info.pluginName());
        m_routerModeActions->addAction(action);
    }
}
#else
void CircuitView::setupActions()
{
}
#endif

void CircuitView::componentFlipHorizontal()
{
    m_scene->flipSelectedComponents(Qt::XAxis);
}

void CircuitView::componentFlipVertical()
{
    m_scene->flipSelectedComponents(Qt::YAxis);
}

void CircuitView::componentRotateCCW()
{
    m_scene->rotateSelectedComponents(-90);
}

void CircuitView::componentRotateCW()
{
    m_scene->rotateSelectedComponents(90);
}

void CircuitView::routingModeChanged(QAction* action)
{
    setRoutingMode(action->data().toString());
}

#if KDE_ENABLED
void CircuitView::save()
{
    m_document->save();
}
#endif

#if KDE_ENABLED
void CircuitView::setRoutingMode(const QString& modeName)
{
    KDevelop::IPluginController *pc = KDevelop::ICore::self()->pluginController();
    IRouterPlugin* router = pc->extensionForPlugin<IRouterPlugin>("org.ktechlab.IRouterPlugin", modeName);
    if (!router) {
        kWarning() << "No Plugin found for extension: org.ktechlab.IRouterPlugin";
        return;
    }
    m_currentRouterName = modeName;
    router->setDocumentScene(m_scene);
}
#else
void CircuitView::setRoutingMode(const QString& modeName)
{
    qWarning() << "CircuitView::setRoutingMode not correctly implemented";
#warning "TODO implement set routing mode in Qt only mode"
    // TODO get an IRouterPlugin here and use it:
    // m_currentRouterName = modeName;
    // router->setDocumentScene(m_scene);
}
#endif

#include "circuitview.moc"
