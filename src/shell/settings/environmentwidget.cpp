/* This file is part of KDevelop
Copyright 2006 Adam Treat <treat@kde.org>
Copyright 2007 Dukju Ahn <dukjuahn@gmail.com>
Copyright 2008 Andreas Pakuat <apaku@gmx.de>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.
*/

#include "environmentwidget.h"

#include <QLabel>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMap>
#include <QProcess>

#include <kdebug.h>
#include <kdialog.h>
#include <klineedit.h>
#include <ktextedit.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>
#include <kconfigdialogmanager.h>

#include "environmentgroupmodel.h"

namespace KDevelop
{

EnvironmentWidget::EnvironmentWidget( QWidget *parent )
        : QWidget( parent ), groupModel( new EnvironmentGroupModel() ), proxyModel( new QSortFilterProxyModel() )
{

    // setup ui
    ui.setupUi( this );
    ui.variableTable->verticalHeader()->hide();
    proxyModel->setSourceModel( groupModel );
    ui.variableTable->setModel( proxyModel );
    ui.variableTable->horizontalHeader()->setResizeMode( 1, QHeaderView::Stretch );

    connect( ui.newButton, SIGNAL( clicked() ),
             SLOT( newButtonClicked() ) );
    connect( ui.deleteButton, SIGNAL( clicked() ),
             SLOT( deleteButtonClicked() ) );

    connect( ui.addgrpBtn, SIGNAL( clicked() ), SLOT( addGroupClicked() ) );
    connect( ui.addgrpBtn, SIGNAL( clicked() ), SIGNAL(changed() ) );
    connect( ui.removegrpBtn, SIGNAL( clicked() ), SLOT( removeGroupClicked() ) );
    connect( ui.removegrpBtn, SIGNAL( clicked() ), SIGNAL(changed() ) );
    connect( ui.setAsDefaultBtn, SIGNAL( clicked() ), SLOT( setAsDefault() ) );
    connect( ui.setAsDefaultBtn, SIGNAL( clicked() ), SIGNAL( changed() ) );
    connect( ui.activeCombo, SIGNAL(currentIndexChanged(int)),
             SLOT( activeGroupChanged(int)) );
    connect( groupModel, SIGNAL( rowsRemoved( const QModelIndex&, int, int ) ), SIGNAL( changed() ) );
    connect( groupModel, SIGNAL( rowsInserted( const QModelIndex&, int, int ) ), SIGNAL( changed() ) );
    connect( groupModel, SIGNAL( rowsRemoved( const QModelIndex&, int, int ) ), SLOT( enableDeleteButton() ) );
    connect( groupModel, SIGNAL( rowsInserted( const QModelIndex&, int, int ) ), SLOT( enableDeleteButton() ) );
    connect( groupModel, SIGNAL( modelReset() ), SLOT( enableDeleteButton() ) );
}

void EnvironmentWidget::enableDeleteButton()
{
    ui.deleteButton->setEnabled( groupModel->rowCount( QModelIndex() ) > 0 );
}

void EnvironmentWidget::setAsDefault()
{
    groupModel->changeDefaultGroup( ui.activeCombo->currentText() );
    emit changed();
}

void EnvironmentWidget::loadSettings( KConfig* config )
{
    kDebug() << "Loading groups from config";
    groupModel->loadFromConfig( config );

    ui.activeCombo->clear();

    QStringList groupList = groupModel->groups();
    kDebug() << "Grouplist:" << groupList << "default group:" << groupModel->defaultGroup();
    ui.activeCombo->addItems( groupList );
    if( !groupList.contains( groupModel->defaultGroup() ) )
    {
        ui.activeCombo->addItem( groupModel->defaultGroup() );
    }
    int idx = ui.activeCombo->findText( groupModel->defaultGroup() );
    ui.activeCombo->setCurrentIndex( idx );
}

void EnvironmentWidget::saveSettings( KConfig* config )
{
    groupModel->saveToConfig( config );
}

void EnvironmentWidget::defaults( KConfig* config )
{
    loadSettings( config );
}

void EnvironmentWidget::newButtonClicked()
{
    KDialog * dialog = new KDialog( this );
    dialog->setCaption( i18n( "New Environment Variable" ) );
    dialog->setButtons( KDialog::Ok | KDialog::Cancel );
    dialog->setDefaultButton( KDialog::Ok );

    QWidget *main = new QWidget( dialog );
    QGridLayout *layout = new QGridLayout( main );

    QLabel* l = new QLabel( i18nc( "Name of an environment variable", "Variable Name:" ), main );
    l->setAlignment( Qt::AlignRight | Qt::AlignTop );
    layout->addWidget( l, 0, 0 );
    KLineEdit *nameEdit = new KLineEdit( main );
    layout->addWidget( nameEdit, 0, 1 );
    l = new QLabel( i18nc( "Value of an environment variable", "Variable Value:" ), main );
    l->setAlignment( Qt::AlignRight | Qt::AlignTop );
    layout->addWidget( l, 1, 0 );
    KTextEdit *valueEdit = new KTextEdit( main );
    layout->addWidget( valueEdit, 1, 1 );
    nameEdit->setFocus();
    dialog->setMainWidget( main );

    if ( dialog->exec() == QDialog::Accepted )
    {
        QString _name = nameEdit->text();
        QString _value = valueEdit->toPlainText();
        if ( _name.isEmpty() )
            return;

        groupModel->addVariable( _name, _value );
    }
}

void EnvironmentWidget::deleteButtonClicked()
{
    kDebug() << "delete button clicked";
    QModelIndexList selected = ui.variableTable->selectionModel()->selectedRows();
    if( selected.isEmpty() )
        return;
    foreach( const QModelIndex &idx, selected )
    {
        kDebug() << "deleting index" << idx << proxyModel->mapToSource( idx );
        groupModel->removeVariable( proxyModel->mapToSource( idx ) );
    }
}

void EnvironmentWidget::addGroupClicked()
{
    QString curText = ui.activeCombo->currentText();
    if( groupModel->groups().contains( curText ) )
    {
        return; // same group name cannot be added twice.
    }
    ui.activeCombo->addItem( curText );
}

void EnvironmentWidget::removeGroupClicked()
{
    int idx = ui.activeCombo->currentIndex();
    if( idx < 0 || ui.activeCombo->count() == 1 )
    {
        return;
    }

    QString curText = ui.activeCombo->currentText();
    groupModel->removeGroup( curText );
    ui.activeCombo->removeItem( idx );
}

void EnvironmentWidget::activeGroupChanged( int /*idx*/ )
{

    groupModel->setCurrentGroup( ui.activeCombo->currentText() );
}

}

#include "environmentwidget.moc"

