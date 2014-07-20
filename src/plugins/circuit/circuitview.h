/*
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

#ifndef CIRCUITVIEW_H
#define CIRCUITVIEW_H

#include <QGraphicsView>

#if KDE_ENABLED
#include <KXMLGUIClient>
#endif

class KSelectAction;
class QAction;

namespace KTechLab
{

class ComponentItem;
class Theme;
class CircuitScene;
class CircuitDocument;

/**
 * A @class QGraphicsView widget, displaying a @class KTechLab::CircuitScene
 * representing a @class KTechLab::CircuitDocument. This class does nothing special,
 * yet, but defining some standard values and initial behaviour of the View.
 *
 * @author Julian Bäume
 */
#if KDE_ENABLED
class CircuitView : public QGraphicsView, public KXMLGUIClient
#else
class CircuitView : public QGraphicsView
#endif
{
    Q_OBJECT

public:
    CircuitView ( KTechLab::CircuitDocument* document, QWidget* parent = 0 );
    ~CircuitView();

private slots:
    void componentRotateCW();
    void componentRotateCCW();
    void componentFlipHorizontal();
    void componentFlipVertical();
    void routingModeChanged(QAction* action);
#if KDE_ENABLED
    void save();
#endif

private:
    void init();
    void setupActions();
    void setRoutingMode(const QString& modeName);
    KTechLab::CircuitDocument* m_document;
    KTechLab::CircuitScene* m_scene;
    QString m_currentRouterName;
    KSelectAction* m_routerModeActions;
};

}
#endif

