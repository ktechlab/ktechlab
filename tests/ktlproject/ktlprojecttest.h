/*
    Test handling of KTechLab projects
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

#ifndef KTLPROJECTTEST_H
#define KTLPROJECTTEST_H

#include <QtCore/QObject>

namespace KDevelop
{
    class TestCore;
    class IProject;
    class ProjectFolderItem;
    class ProjectFileItem;
}

namespace KTechLab
{

class KTLProjectTest : public QObject
{
Q_OBJECT

private slots:
    void initTestCase();
    void loadKTLProject();
    void addSubProject();
    void renameSubProject();
    void removeSubProject();
    void addFile();
    void renameFile();
    void removeFile();
    void cleanupTestCase();

private:
    KDevelop::TestCore* m_core;
    KDevelop::IProject* m_project;
    KDevelop::ProjectFolderItem* m_testFolderItem;
    KDevelop::ProjectFileItem* m_testFileItem;
};
}
#endif // KTLPROJECTTEST_H
