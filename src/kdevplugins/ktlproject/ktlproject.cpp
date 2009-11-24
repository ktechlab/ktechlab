/*
    Represent a KTechLab project
    Copyright (C) 2009  Julian Bäume <julian@svg4all.de>

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

#include "ktlproject.h"

using namespace KDevelop;

KTLProject::KTLProject( QObject *parent )
  : IProject( parent )
{
}

KTLProject::~KTLProject()
{
}

IBuildSystemManager* KTLProject::buildSystemManager() const
{
  return 0;
}

ProjectFileItem* KTLProject::fileAt( int pos ) const
{
  return 0;
}

int KTLProject::fileCount() const
{
  return 0;
}

QList<ProjectFileItem*> KTLProject::files() const
{
  return QList<ProjectFileItem*>();
}

QList<ProjectFileItem*> KTLProject::filesForUrl( const KUrl& file ) const
{
  return QList<ProjectFileItem*>();
}

QList<ProjectFolderItem*> KTLProject::foldersForUrl( const KUrl& folder ) const
{
  return QList<ProjectFolderItem*>();
}

IPlugin* KTLProject::managerPlugin() const
{
  return 0;
}

IProjectFileManager* KTLProject::projectFileManager() const
{
  return 0;
}

KUrl KTLProject::projectFileUrl() const
{
  return KUrl();
}

ProjectFolderItem* KTLProject::projectItem() const
{
}

void KTLProject::reloadModel()
{
}

IPlugin* KTLProject::versionControlPlugin() const
{
  return 0;
}

const KUrl KTLProject::folder() const
{
  return KUrl();
}

bool KTLProject::inProject( const KUrl& url ) const
{
  return false;
}

QString KTLProject::name() const
{
  return QString();
}

KUrl KTLProject::relativeUrl( const KUrl& absoluteUrl ) const
{
  return KUrl();
}

#include "ktlproject.moc"