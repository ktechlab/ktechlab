/*
    Copyright (C) 2010 Julian Bäume <julian@svg4all.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <interfaces/idocument.h>

#include "componenteditorview.h"
#include "circuitdocument.h"
#include "circuitscene.h"
#include <circuitmodel.h>
#include <componentitem.h>
#include <QHeaderView>

using namespace KTechLab;

KTechLab::ComponentEditorView::ComponentEditorView(QWidget* parent)
    : QTableView(parent),
      m_scene(0),
      m_model(0)
{
    verticalHeader()->hide();
    setSelectionMode(SingleSelection);
    setEditTriggers(CurrentChanged);
    setAcceptDrops( false );
    setDragEnabled( false );
}

void ComponentEditorView::activated(KDevelop::IDocument* document)
{
    if (m_scene) disconnect(m_scene);

    CircuitDocument* doc = dynamic_cast<CircuitDocument*>(document);
    if (!doc) {
        m_scene = 0;
        m_model = 0;
    } else {
        m_scene = static_cast<CircuitScene*>(doc->documentScene());
        m_model = static_cast<CircuitModel*>(doc->documentModel());

        connect( m_scene, SIGNAL( selectionChanged() ),
                 this, SLOT( updateModel() ) );
    }

    updateModel();
}

void ComponentEditorView::updateModel()
{
    if (m_scene && m_scene->selectedItems().size() == 1) {
        QGraphicsItem* item = m_scene->selectedItems().first();
        setComponentItem(qgraphicsitem_cast<ComponentItem*>(item));
    } else {
        setModel(0);
    }
}

void ComponentEditorView::setComponentItem(ComponentItem* item)
{
    if (!item) {
        setModel(0);
        return;
    }

    setModel(m_model);
    setRootIndex(m_model->index(item->data()));
}

#include "componenteditorview.moc"