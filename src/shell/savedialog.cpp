/* This file is part of the KDE project
   Copyright (C) 2002 Harald Fernengel <harry@kdevelop.org>
   Copyright (C) 2008 Hamish Rodda <rodda@kde.org>

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

#include "savedialog.h"

#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>

#include <klocale.h>
#include <kstdguiitem.h>

#include <interfaces/idocument.h>

using namespace KDevelop;

class DocumentItem : public QListWidgetItem
{
public:
    DocumentItem( IDocument* doc, QListWidget* parent )
        : QListWidgetItem(parent)
        , m_doc( doc )
    {
        setFlags(Qt::ItemIsUserCheckable | flags());
        setData(Qt::CheckStateRole, true);
        setText(m_doc->url().prettyUrl());
    }

    IDocument* doc() const { return m_doc; }

private:
    IDocument* m_doc;
};

KSaveSelectDialog::KSaveSelectDialog( const QList<IDocument*>& files, QWidget * parent )
    : KDialog( parent )
{
    setCaption( i18n("Save Modified Files?") );
    setButtons( Ok | User1 | Cancel );

    QWidget* mainWidget = new QWidget(this);
    setMainWidget( mainWidget );
    connect( this, SIGNAL( okClicked() ), this, SLOT( save() ) );

    QVBoxLayout* layout = new QVBoxLayout(mainWidget);
    mainWidget->setLayout(layout);

    layout->addWidget(new QLabel( i18n("The following files have been modified. Save them?"), this ));

    m_listWidget = new QListWidget( this );
    layout->addWidget(m_listWidget);
//     m_listWidget->addColumn( "" );
//     m_listWidget->header()->hide();
//     m_listWidget->setResizeMode( QListView::LastColumn );

    setButtonGuiItem( Ok, KGuiItem(i18n("Save &Selected"), QString(), i18n("Saves all selected files")) );
    setButtonText( User1, i18n("Save &None") );
    setButtonToolTip( User1, i18n("Discard all modifications") );
    setDefaultButton( Ok );

    foreach (IDocument* doc, files)
        new DocumentItem( doc, m_listWidget );

    connect( this, SIGNAL(okClicked()), this, SLOT(save()) );
    connect( this, SIGNAL(user1Clicked()), this, SLOT(accept()) );
}

KSaveSelectDialog::~KSaveSelectDialog()
{
}

void KSaveSelectDialog::save( )
{
    for (int i = 0; i < m_listWidget->count(); ++i) {
        DocumentItem* item = static_cast<DocumentItem*>(m_listWidget->item(i));
        if (item->data(Qt::CheckStateRole).toBool())
            item->doc()->save(IDocument::Silent);
    }

    accept();
}

#include "savedialog.moc"
