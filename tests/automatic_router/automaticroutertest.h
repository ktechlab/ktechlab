/*
 *    Test automatic_router plugin
 *    Copyright (C) 2010 Julian Bäume <julian@svg4all.de>
 * 
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 */

#ifndef AUTOMATICROUTERTEST_H
#define AUTOMATICROUTERTEST_H

#include <QtCore/QObject>
#include <QMap>

namespace KDevelop
{
    class TestCore;
    class IProject;
    class ProjectFolderItem;
    class ProjectFileItem;
}

namespace KTechLab
{

class IDocumentScene;
class IRouterPlugin;

class AutomaticRouterTest : public QObject
{
Q_OBJECT
private slots:
    void initTestCase();
    void mapLineRoute();
    void visualizeData();
    void cleanupTestCase();

private:
    void initDocumentScenes();

    QMap<QString,IDocumentScene*> m_testScenes;
    IDocumentScene* m_currentScene;
    KDevelop::TestCore* m_core;
    IRouterPlugin* m_router;
};
}
#endif // AUTOMATICROUTERTEST_H
