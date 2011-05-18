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


#ifndef KTECHLAB_COMPONENTEDITORVIEW_H
#define KTECHLAB_COMPONENTEDITORVIEW_H

#include <QtGui/QTableView>

namespace KDevelop {
    class IDocument;
}


namespace KTechLab {

class ComponentItem;
class CircuitModel;
class CircuitScene;

class ComponentEditorView : public QTableView
{
    Q_OBJECT
public:
    explicit ComponentEditorView(QWidget* parent = 0);

public slots:
    void activated( KDevelop::IDocument* document );

private:
    void setComponentItem( KTechLab::ComponentItem* item );
    KTechLab::CircuitScene* m_scene;
    KTechLab::CircuitModel* m_model;

private slots:
    void updateModel();
};

}

#endif // KTECHLAB_COMPONENTEDITORVIEW_H
