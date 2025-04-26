/***************************************************************************
 *   Copyright (C) 2003-2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "itemselector.h"
#include "circuitdocument.h"
#include "docmanager.h"
#include "flowcodedocument.h"
#include "itemdocument.h"
#include "itemlibrary.h"
#include "katemdi.h"
#include "libraryitem.h"
#include "mechanicsdocument.h"
#include <vector> // Temporary fix for pthread.h problem

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

// #include <q3dragobject.h>
// #include <q3popupmenu.h>
#include <QIODevice>
#include <QLayout>
#include <QMenu>
#include <QMimeData>

#include <cassert>

#include <ktechlab_debug.h>

ILVItem::ILVItem(QTreeWidget *parent, const QString &id)
    : QTreeWidgetItem(parent, 0 /* note: add item types */)
{
    setData(0, DataRole_ID, QVariant(id));
    // 	m_id = id;  // 2018.08.12 - use value()
    b_isRemovable = false;
    m_pProjectItem = nullptr;
}

ILVItem::ILVItem(QTreeWidgetItem *parent, const QString &id)
    : QTreeWidgetItem(parent, 0 /* note: add item types */)
{
    // m_id = id;    // 2018.08.12 - use value()
    setData(0, DataRole_ID, QVariant(id));
    b_isRemovable = false;
    m_pProjectItem = nullptr;
}

ItemSelector::ItemSelector(QWidget *parent)
    : QTreeWidget(parent)
{
    qCDebug(KTL_LOG) << " this=" << this;

    setDragDropMode(QAbstractItemView::DragOnly);
    setColumnCount(1);
    setHeaderLabel(i18n("Component"));
    // addColumn( i18n( "Component" ) ); // 2018.08.12 - use setHeaderLabel()
    // setFullWidth(true);       // 2018.06.02 - need to be fixed
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    // setSorting( -1, false ); // 2018.08.12 - use setSortingEnabled
    setSortingEnabled(false);
    setRootIsDecorated(true);
    // setDragEnabled(true);     // 2018.06.02 - needed?
    setFocusPolicy(Qt::NoFocus);

    setSelectionMode(QAbstractItemView::SingleSelection); // 2015.12.10 - need to allow selection for removing items

    if (parent->layout()) {
        parent->layout()->addWidget(this);
        qCDebug(KTL_LOG) << " added item selector to parent's layout " << parent;
    } else {
        qCWarning(KTL_LOG) << " unexpected null layout on parent " << parent;
    }

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // ?

    // 	connect( this, SIGNAL(executed(K3ListViewItem*) ), this, SLOT(slotItemExecuted(K3ListViewItem*)) );
    connect(this, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(slotItemClicked(QTreeWidgetItem *, int)));
    /*TODO Can't connect to itemClicked(QTreeWidgetItem *, int)
    connect(this, qOverload<QTreeWidgetItem*, int>(&ItemSelector::itemClicked),
            this, qOverload<QTreeWidgetItem*, int>(&ItemSelector::slotItemClicked));*/
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(slotItemDoubleClicked(QTreeWidgetItem *, int)));
    /*TODO The same problem as above
    connect(this, qOverload<QTreeWidgetItem*, int>(&ItemSelector::itemClicked),
        this, qOverload<QTreeWidgetItem*, int>(&ItemSelector::slotItemDoubleClicked));*/
    // 	connect( this, SIGNAL(contextMenuRequested(Q3ListViewItem*, const QPoint&, int )), this,
    //              SLOT(slotContextMenuRequested(Q3ListViewItem*, const QPoint&, int )) ); // 2018.08.12 - use signal from below
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &ItemSelector::customContextMenuRequested, this, &ItemSelector::slotContextMenuRequested);

    connect(this, &ItemSelector::itemSelectionChanged, this, &ItemSelector::slotItemSelected);
}

ItemSelector::~ItemSelector()
{
    writeOpenStates();
}

void ItemSelector::clear()
{
    m_categories.clear();
    QTreeWidget::clear();
}

void ItemSelector::addItem(const QString &caption, const QString &id, const QString &_category, const QIcon &icon, bool removable)
{
    qCDebug(KTL_LOG) << "id=" << id;
    ILVItem *parentItem = nullptr;

    QString category = _category;
    if (!category.startsWith("/")) {
        category.prepend('/');
    }

    do {
        category.remove(0, 1);
        QString cat;
        category.replace("\\/", "|");
        int pos = category.indexOf('/');
        if (pos == -1)
            cat = category;
        else
            cat = category.left(pos);

        cat.replace("|", "/");

        if (m_categories.indexOf(cat) == -1) {
            m_categories.append(cat);

            if (parentItem) {
                parentItem = new ILVItem(parentItem, "");
            } else {
                parentItem = new ILVItem(this, "");
            }
            // parentItem->setExpandable(true); // 2018.08.12 - is it needed?

            parentItem->setExpanded(readOpenState(cat));

            parentItem->setText(0, cat);
        } else {
            QList<QTreeWidgetItem *> foundList = findItems(cat, Qt::MatchExactly);
            if (foundList.size() > 1) {
                qCWarning(KTL_LOG) << "found multiple categories for '" << cat << "'";
            }
            parentItem = dynamic_cast<ILVItem *>(foundList.front());
        }

        category.remove(0, pos);
    } while (category.contains('/'));

    if (!parentItem) {
        qCCritical(KTL_LOG) << "Unexpected error in finding parent item for category list";
        return;
    }

    ILVItem *item = new ILVItem(parentItem, id);
    // item->setPixmap( 0, icon );  // 2018.08.12 - replaced with line below
    item->setIcon(0, icon);
    item->setText(0, caption);
    // item->setDragEnabled(true); // 2018.08.12 - replaced with line below
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
    item->setRemovable(removable);
}

void ItemSelector::writeOpenStates()
{
    // KConfig *config = kapp->config();
    KSharedConfigPtr configPtr = KSharedConfig::openConfig();
    // config->setGroup( name() );
    KConfigGroup configGroup = configPtr->group(objectName());

    const QStringList::iterator end = m_categories.end();
    for (QStringList::iterator it = m_categories.begin(); it != end; ++it) {
        QList<QTreeWidgetItem *> itemsFound = findItems(*it, Qt::MatchExactly);
        if (itemsFound.size() > 1) {
            qCWarning(KTL_LOG) << " too many items " << itemsFound.size() << " for category '" << *it << "'";
        }
        QTreeWidgetItem *item = itemsFound.first() /* findItem( *it, 0 ) */;
        if (item) {
            configGroup.writeEntry(*it + "IsOpen", item->isExpanded() /* isOpen() */);
        }
    }
}

bool ItemSelector::readOpenState(const QString &id)
{
    // KConfig *config = kapp->config();
    KSharedConfigPtr configPtr = KSharedConfig::openConfig();
    // config->setGroup( name() );
    KConfigGroup configGroup = configPtr->group(objectName());

    return configGroup.readEntry<bool>(id + "IsOpen", true);
}

QTreeWidgetItem *ItemSelector::selectedItem() const
{
    QList<QTreeWidgetItem *> selectedList = selectedItems();
    if (selectedList.empty()) {
        return nullptr;
    }
    if (selectedList.size() > 1) {
        qCWarning(KTL_LOG) << " expected 1 item in selection, got " << selectedList.size();
    }
    return selectedList.first();
}

QMimeData *ItemSelector::mimeData(const QList<QTreeWidgetItem *> & items) const
{
    qCDebug(KTL_LOG) << " begin ";
    if (items.size() > 1) {
        qCWarning(KTL_LOG) << "expected 1 item, got " << items.size();
    }
    QTreeWidgetItem *theItem = items.first();
    if (!theItem) {
        qCWarning(KTL_LOG) << "unexpected null item";
        return nullptr;
    }
    qCDebug(KTL_LOG) << " theItem = " << theItem;
    QVariant idAsVariant = theItem->data(0, ILVItem::DataRole_ID);
    qCDebug(KTL_LOG) << " idAsVariant = " << idAsVariant;
    const QString id = idAsVariant.toString();
    qCDebug(KTL_LOG) << "id='" << id << "'";

    QMimeData *mime = new QMimeData();

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << id;

    if (id.startsWith("flow/")) {
        mime->setData("ktechlab/flowpart", data);
    } else if (id.startsWith("ec/")) {
        mime->setData("ktechlab/component", data);
    } else if (id.startsWith("sc/")) {
        mime->setData("ktechlab/subcircuit", data);
    } else if (id.startsWith("mech/")) {
        mime->setData("ktechlab/mechanical", data);
    } else {
        qCWarning(KTL_LOG) << "returning unset mime; unknown id '" << id << "'";
    }

    // A pixmap cursor is often hard to make out
    //  QPixmap *pixmap = const_cast<QPixmap*>(currentItem()->pixmap(0));
    //  if (pixmap) d->setPixmap(*pixmap);

    return mime;
}

void ItemSelector::slotContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item = itemAt(pos);
    if (!item || !(static_cast<ILVItem *>(item))->isRemovable()) {
        return;
    }

    QMenu *menu = new QMenu(this);
    /* menu->insertItem(
        //, Qt::Key_Delete // 2015.12.29 - do not specify shortcut key, because it does not work
    ); - 2018.12.01 */
    menu->addAction(i18n("Remove %1", item->text(0)), this, SLOT(slotRemoveSelectedItem()));
    QPoint globalPos = mapToGlobal(pos);
    menu->popup(globalPos);
}

void ItemSelector::slotRemoveSelectedItem()
{
    qCDebug(KTL_LOG) << "removing selected item";
    QList<QTreeWidgetItem *> selectedList = selectedItems();
    if (selectedList.empty()) {
        qCDebug(KTL_LOG) << "selection is empty";
        return;
    }
    QTreeWidgetItem *selectedItem = selectedList.first();
    ILVItem *item = dynamic_cast<ILVItem *>(selectedItem);
    if (!item) {
        qCDebug(KTL_LOG) << "no selected item to remove";
        return;
    }

    emit itemRemoved(item->data(0, ILVItem::DataRole_ID).toString() /*key( 0, 0 ) */);
    ILVItem *parent = dynamic_cast<ILVItem *>(item->QTreeWidgetItem::parent());
    delete item;
    // Get rid of the category as well if it has no children
    if (parent && !parent->childCount() /* firstChild() */) {
        m_categories.removeAll(parent->text(0));
        delete parent;
    }
}

void ItemSelector::setListCaption(const QString &caption)
{
    // setColumnText( 0, caption ); // 2018.08.12 - see below
    setHeaderLabel(caption);
}

void ItemSelector::slotItemSelected()
{
    QTreeWidgetItem *item = selectedItem();
    if (!item) {
        return;
    }

    emit itemSelected(item->data(0, ILVItem::DataRole_ID).toString() /* item->key( 0, 0 ) */);
}

void ItemSelector::slotItemClicked(QTreeWidgetItem *item, int)
{
    if (!item)
        return;

    if (ItemDocument *itemDocument = dynamic_cast<ItemDocument *>(DocManager::self()->getFocusedDocument()))
        itemDocument->slotUnsetRepeatedItemId();

    const QString &itemIdString = item->data(0, ILVItem::DataRole_ID).toString();

    emit itemClicked(itemIdString /* item->key( 0, 0 ) */);
}

void ItemSelector::slotItemDoubleClicked(QTreeWidgetItem *item, int)
{
    if (!item)
        return;

    // QString id = item->key( 0, 0 );
    const QString &id = item->data(0, ILVItem::DataRole_ID).toString();

    if (Document *doc = DocManager::self()->getFocusedDocument()) {
        if (doc->type() == Document::dt_flowcode && id.startsWith("flow/"))
            (static_cast<FlowCodeDocument *>(doc))->slotSetRepeatedItemId(id);

        else if (doc->type() == Document::dt_circuit && (id.startsWith("ec/") || id.startsWith("sc/")))
            (static_cast<CircuitDocument *>(doc))->slotSetRepeatedItemId(id);

        else if (doc->type() == Document::dt_mechanics && id.startsWith("mech/"))
            (static_cast<MechanicsDocument *>(doc))->slotSetRepeatedItemId(id);
    }

    emit itemDoubleClicked(id);
}

#if 0 // 2018.08.12 - needed?
// Q3DragObject* ItemSelector::dragObject()
// {
// 	const QString &id = currentItem()->data(0, ILVItem::DataRole_ID).toString() /* key(0,0) */;
//
// 	Q3StoredDrag * d = nullptr;
//
// 	if ( id.startsWith("flow/") )
// 		d = new Q3StoredDrag( "ktechlab/flowpart", this );
//
// 	else if ( id.startsWith("ec/") )
// 		d = new Q3StoredDrag( "ktechlab/component", this );
//
// 	else if ( id.startsWith("sc/") )
// 		d = new Q3StoredDrag( "ktechlab/subcircuit", this );
//
// 	else if ( id.startsWith("mech/") )
// 		d = new Q3StoredDrag( "ktechlab/mechanical", this );
//
// 	if (d)
// 	{
// 		QByteArray data;
// 		QDataStream stream( &data, QIODevice::WriteOnly );
// 		stream << id;
// 		d->setEncodedData(data);
// 	} else {
//         qCWarning(KTL_LOG) << " null drag returned";
//     }
//
// 	// A pixmap cursor is often hard to make out
// // 	QPixmap *pixmap = const_cast<QPixmap*>(currentItem()->pixmap(0));
// // 	if (pixmap) d->setPixmap(*pixmap);
//
//     return d;
// }
#endif

// BEGIN class ComponentSelector
ComponentSelector *ComponentSelector::m_pSelf = nullptr;

ComponentSelector *ComponentSelector::self(KateMDI::ToolView *parent)
{
    if (!m_pSelf) {
        assert(parent);
        m_pSelf = new ComponentSelector(parent);
        m_pSelf->setObjectName("Component Selector");
    }
    return m_pSelf;
}

ComponentSelector::ComponentSelector(KateMDI::ToolView *parent)
    : ItemSelector(parent)
{
    qCDebug(KTL_LOG) << " creating " << this;

    setWhatsThis(
        i18n("Add components to the circuit diagram by dragging them into the circuit.<br><br>"

             "To add more than one component of the same type, doubleclick on a component, and click repeatedly in the circuit to place the component. Right click to stop placement.<br><br>"

             "Some components (such as subcircuits) can be removed by right clicking on the item and selecting \"Remove\"."));

    setListCaption(i18n("Component"));

    LibraryItemList *items = itemLibrary()->items();
    qCDebug(KTL_LOG) << " there are " << items->count() << " items";
    const LibraryItemList::iterator end = items->end();
    for (LibraryItemList::iterator it = items->begin(); it != end; ++it) {
        if ((*it)->type() == LibraryItem::lit_component)
            addItem((*it)->name(), (*it)->activeID(), (*it)->category(), (*it)->icon());
    }
}
// END class ComponentSelector

// BEGIN class FlowPartSelector
FlowPartSelector *FlowPartSelector::m_pSelf = nullptr;

FlowPartSelector *FlowPartSelector::self(KateMDI::ToolView *parent)
{
    if (!m_pSelf) {
        assert(parent);
        m_pSelf = new FlowPartSelector(parent);
        m_pSelf->setObjectName("Part Selector");
    }
    return m_pSelf;
}

FlowPartSelector::FlowPartSelector(KateMDI::ToolView *parent)
    : ItemSelector(static_cast<QWidget *>(parent))
{
    setWhatsThis(
        i18n("Add FlowPart to the FlowCode document by dragging them there.<br><br>To add more than one FlowPart of the same type, doubleclick on a FlowPart, and click repeatedly in the FlowChart to place the component. Right click to "
             "stop placement."));

    setListCaption(i18n("Flow Part"));

    LibraryItemList *items = itemLibrary()->items();
    const LibraryItemList::iterator end = items->end();
    for (LibraryItemList::iterator it = items->begin(); it != end; ++it) {
        if ((*it)->type() == LibraryItem::lit_flowpart)
            addItem((*it)->name(), (*it)->activeID(), (*it)->category(), (*it)->icon());
    }
}
// END class FlowPartSelector

// BEGIN class MechanicsSelector
MechanicsSelector *MechanicsSelector::m_pSelf = nullptr;

MechanicsSelector *MechanicsSelector::self(KateMDI::ToolView *parent)
{
    if (!m_pSelf) {
        assert(parent);
        m_pSelf = new MechanicsSelector(static_cast<QWidget *>(parent));
        m_pSelf->setObjectName("Mechanics Selector");
    }
    return m_pSelf;
}

MechanicsSelector::MechanicsSelector(QWidget *parent)
    : ItemSelector(static_cast<QWidget *>(parent))
{
    setWhatsThis(i18n("Add mechanical parts to the mechanics work area by dragging them there."));

    LibraryItemList *items = itemLibrary()->items();
    const LibraryItemList::iterator end = items->end();
    for (LibraryItemList::iterator it = items->begin(); it != end; ++it) {
        if ((*it)->type() == LibraryItem::lit_mechanical) {
            addItem((*it)->name(), (*it)->activeID(), (*it)->category(), (*it)->icon());
        }
    }
}
// END class MechanicsSelector

#include "moc_itemselector.cpp"
