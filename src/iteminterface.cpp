/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "iteminterface.h"
#include "circuitview.h"
#include "cnitem.h"
#include "cnitemgroup.h"
#include "colorutils.h"
#include "contexthelp.h"
#include "doublespinbox.h"
#include "itemdocument.h"
#include "itemeditor.h"
#include "itemview.h"
#include "ktechlab.h"

#include <KColorCombo>
#include <KComboBox>
#include <KLineEdit>
#include <KToolBar>
#include <KUrlRequester>
#include <KXMLGUIFactory>
#include <kio_version.h>

#include <QApplication>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>

#include <cassert>

#include <ktechlab_debug.h>

ItemInterface *ItemInterface::m_pSelf = nullptr;

ItemInterface *ItemInterface::self()
{
    if (!m_pSelf)
        m_pSelf = new ItemInterface();

    return m_pSelf;
}

ItemInterface::ItemInterface()
    : QObject(KTechlab::self())
    , m_isInTbDataChanged(false)
{
    m_pActiveItemEditorToolBar = nullptr;
    p_cvb = nullptr;
    p_itemGroup = nullptr;
    p_lastItem = nullptr;
    m_currentActionTicket = -1;
    m_toolBarWidgetID = -1;
}

ItemInterface::~ItemInterface()
{
}

void ItemInterface::slotGetActionTicket()
{
    m_currentActionTicket = p_cvb ? p_cvb->getActionTicket() : -1;
}

void ItemInterface::slotItemDocumentChanged(ItemDocument *doc)
{
    slotClearAll();
    if (ItemDocument *itemDocument = dynamic_cast<ItemDocument *>((Document *)p_cvb)) {
        disconnect(itemDocument, &ItemDocument::selectionChanged, this, &ItemInterface::slotUpdateItemInterface);
    }

    p_itemGroup = nullptr;
    p_cvb = doc;

    slotGetActionTicket();

    if (!p_cvb)
        return;

    connect(p_cvb, &ItemDocument::selectionChanged, this, &ItemInterface::slotUpdateItemInterface);

    p_itemGroup = p_cvb->selectList();

    slotUpdateItemInterface();
}

void ItemInterface::clearItemEditorToolBar()
{
    if (m_pActiveItemEditorToolBar && m_toolBarWidgetID != -1) {
        // m_pActiveItemEditorToolBar->removeItem(m_toolBarWidgetID); // TODO add proper replacmenet
        m_pActiveItemEditorToolBar->clear();
    }
    m_toolBarWidgetID = -1;
    itemEditTBCleared();
}

void ItemInterface::slotClearAll()
{
    ContextHelp::self()->slotClear();
    ItemEditor::self()->slotClear();
    clearItemEditorToolBar();
    p_lastItem = nullptr;
}

void ItemInterface::slotMultipleSelected()
{
    ContextHelp::self()->slotMultipleSelected();
    ItemEditor::self()->slotMultipleSelected();
    clearItemEditorToolBar();
    p_lastItem = nullptr;
}

void ItemInterface::slotUpdateItemInterface()
{
    if (!p_itemGroup)
        return;

    slotGetActionTicket();
    updateItemActions();

    if (!p_itemGroup->itemsAreSameType()) {
        slotMultipleSelected();
        return;
    }
    if (p_lastItem && p_itemGroup->activeItem()) {
        ItemEditor::self()->itemGroupUpdated(p_itemGroup);
        return;
    }

    p_lastItem = p_itemGroup->activeItem();
    if (!p_lastItem) {
        slotClearAll();
        return;
    }

    ContextHelp::self()->slotUpdate(p_lastItem);
    ItemEditor::self()->slotUpdate(p_itemGroup);
    if (CNItem *cnItem = dynamic_cast<CNItem *>((Item *)p_lastItem)) {
        ItemEditor::self()->slotUpdate(cnItem);
    }

    // Update item editor toolbar
    if (ItemView *itemView = dynamic_cast<ItemView *>(p_cvb->activeView())) {
        if (KTechlab *ktl = KTechlab::self()) {
            if ((m_pActiveItemEditorToolBar = dynamic_cast<KToolBar *>(ktl->factory()->container("itemEditorTB", itemView)))) {
                // m_pActiveItemEditorToolBar->setFullSize( true ); // TODO proper replacement
                m_pActiveItemEditorToolBar->adjustSize();
                QWidget *widget = configWidget();
                m_toolBarWidgetID = 1;
                // m_pActiveItemEditorToolBar->insertWidget( m_toolBarWidgetID, 0, widget ); // TODO properly fix
                m_pActiveItemEditorToolBar->addWidget(widget);
            }
        }
    }
}

void ItemInterface::updateItemActions()
{
    ItemView *itemView = ((ItemDocument *)p_cvb) ? dynamic_cast<ItemView *>(p_cvb->activeView()) : nullptr;
    if (!itemView)
        return;

    bool itemsSelected = p_itemGroup && p_itemGroup->itemCount();

    itemView->actionByName("edit_raise")->setEnabled(itemsSelected);
    itemView->actionByName("edit_lower")->setEnabled(itemsSelected);

    if (KTechlab::self()) {
        KTechlab::self()->actionByName("edit_cut")->setEnabled(itemsSelected);
        KTechlab::self()->actionByName("edit_copy")->setEnabled(itemsSelected);
    }

    CNItemGroup *cnItemGroup = dynamic_cast<CNItemGroup *>((ItemGroup *)p_itemGroup);
    CircuitView *circuitView = dynamic_cast<CircuitView *>(itemView);

    if (cnItemGroup && circuitView) {
        bool canFlip = cnItemGroup->canFlip();
        circuitView->actionByName("edit_flip_horizontally")->setEnabled(canFlip);
        circuitView->actionByName("edit_flip_vertically")->setEnabled(canFlip);

        bool canRotate = cnItemGroup->canRotate();
        circuitView->actionByName("edit_rotate_ccw")->setEnabled(canRotate);
        circuitView->actionByName("edit_rotate_cw")->setEnabled(canRotate);
    }
}

void ItemInterface::setFlowPartOrientation(unsigned orientation)
{
    CNItemGroup *cnItemGroup = dynamic_cast<CNItemGroup *>((ItemGroup *)p_itemGroup);
    if (!cnItemGroup)
        return;

    cnItemGroup->setFlowPartOrientation(orientation);
}

void ItemInterface::setComponentOrientation(int angleDegrees, bool flipped)
{
    CNItemGroup *cnItemGroup = dynamic_cast<CNItemGroup *>((ItemGroup *)p_itemGroup);
    if (!cnItemGroup)
        return;

    cnItemGroup->setComponentOrientation(angleDegrees, flipped);
}

void ItemInterface::itemEditTBCleared()
{
    m_stringLineEditMap.clear();
    m_stringComboBoxMap.clear();
    m_stringURLReqMap.clear();
    m_intSpinBoxMap.clear();
    m_doubleSpinBoxMap.clear();
    m_colorComboMap.clear();
    m_boolCheckMap.clear();
}

// The bool specifies whether advanced data should be shown
QWidget *ItemInterface::configWidget()
{
    if (!p_itemGroup || !p_itemGroup->activeItem() || !m_pActiveItemEditorToolBar)
        return nullptr;

    VariantDataMap *variantMap = p_itemGroup->activeItem()->variantMap();

    QWidget *parent = m_pActiveItemEditorToolBar;

    // Create new widget with the toolbar or dialog as the parent
    QWidget *configWidget = new QWidget(parent /*, "tbConfigWidget" */);
    configWidget->setObjectName("tbConfigWidget");
    {
        // 2018.12.02
        // configWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding, 1, 1 ) );
        QSizePolicy p(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        p.setHorizontalStretch(1);
        p.setVerticalStretch(1);
        configWidget->setSizePolicy(p);
    }

    QHBoxLayout *configLayout = new QHBoxLayout(configWidget);
    // 	configLayout->setAutoAdd( true );
    configLayout->setSpacing(6);
    configLayout->setMargin(0);

    // 	configLayout->addItem( new QSpacerItem( 0, 0,  QSizePolicy::Expanding, QSizePolicy::Fixed ) );

    const VariantDataMap::iterator vaEnd = variantMap->end();
    for (VariantDataMap::iterator vait = variantMap->begin(); vait != vaEnd; ++vait) {
        if (vait.value()->isHidden() || vait.value()->isAdvanced())
            continue;

        const Variant::Type::Value type = vait.value()->type();

        // common to all types apart from bool
        QString toolbarCaption = vait.value()->toolbarCaption();
        if (type != Variant::Type::Bool && !toolbarCaption.isEmpty())
            configLayout->addWidget(new QLabel(toolbarCaption, configWidget));

        QWidget *editWidget = nullptr; // Should be set to the created widget

        switch (type) {
        case Variant::Type::Port:
        case Variant::Type::Pin:
        case Variant::Type::VarName:
        case Variant::Type::Combo:
        case Variant::Type::Select:
        case Variant::Type::KeyPad:
        case Variant::Type::SevenSegment: {
            QString value = vait.value()->displayString();
            if (!value.isEmpty() && !vait.value()->allowed().contains(value))
                vait.value()->appendAllowed(value);

            const QStringList allowed = vait.value()->allowed();

            KComboBox *box = new KComboBox(configWidget);

            box->insertItems(box->count(), allowed);
            box->setCurrentItem(value);

            if (type == Variant::Type::VarName || type == Variant::Type::Combo)
                box->setEditable(true);

            m_stringComboBoxMap[vait.key()] = box;
            connectMapWidget(box, SIGNAL(editTextChanged(const QString &)));
            connectMapWidget(box, SIGNAL(activated(const QString &)));

            connect(*vait, SIGNAL(valueChangedStrAndTrue(const QString &, bool)), box, SLOT(setCurrentItem(const QString &, bool)));

            editWidget = box;
            break;
        }
        case Variant::Type::FileName: {
            qCDebug(KTL_LOG) << "create FileName";
            QString value = vait.value()->value().toString();
            if (!vait.value()->allowed().contains(value))
                vait.value()->appendAllowed(value);

            const QStringList allowed = vait.value()->allowed();

            KUrlComboRequester *urlreq = new KUrlComboRequester(configWidget);
#if KIO_VERSION >= QT_VERSION_CHECK(5, 108, 0)
            urlreq->setNameFilters(vait.value()->fileFilters().toQtStyleStringList());
#else
            urlreq->setFilter(vait.value()->fileFilters().toKDEStyleString());
#endif
            connectMapWidget(urlreq, SIGNAL(urlSelected(QUrl)));
            m_stringURLReqMap[vait.key()] = urlreq;

            KComboBox *box = urlreq->comboBox();
            box->insertItems(box->count(), allowed);
            box->setEditable(true);

            // Note this has to be called after inserting the allowed list
            urlreq->setUrl(QUrl::fromLocalFile(vait.value()->value().toString()));

            // Generally we only want a file name once the user has finished typing out the full file name.
            connectMapWidget(box, SIGNAL(returnPressed(const QString &)));
            connectMapWidget(box, SIGNAL(activated(const QString &)));

            connect(*vait, SIGNAL(valueChanged(const QString &)), box, SLOT(setEditText(const QString &)));

            editWidget = urlreq;
            break;
        }
        case Variant::Type::String: {
            KLineEdit *edit = new KLineEdit(configWidget);

            edit->setText(vait.value()->value().toString());
            connectMapWidget(edit, SIGNAL(textChanged(const QString &)));
            m_stringLineEditMap[vait.key()] = edit;
            editWidget = edit;

            connect(*vait, SIGNAL(valueChanged(const QString &)), edit, SLOT(setText(const QString &)));

            break;
        }
        case Variant::Type::Int: {
            QSpinBox *spin = new QSpinBox(configWidget);
            spin->setMinimum((int)vait.value()->minValue());
            spin->setMaximum((int)vait.value()->maxValue());
            spin->setValue(vait.value()->value().toInt());

            connectMapWidget(spin, SIGNAL(valueChanged(int)));
            m_intSpinBoxMap[vait.key()] = spin;
            editWidget = spin;

            connect(*vait, SIGNAL(valueChanged(int)), spin, SLOT(setValue(int)));

            break;
        }
        case Variant::Type::Double: {
            DoubleSpinBox *spin = new DoubleSpinBox(vait.value()->minValue(), vait.value()->maxValue(), vait.value()->minAbsValue(), vait.value()->value().toDouble(), vait.value()->unit(), configWidget);

            connectMapWidget(spin, SIGNAL(valueChanged(double)));
            m_doubleSpinBoxMap[vait.key()] = spin;
            editWidget = spin;

            connect(*vait, SIGNAL(valueChanged(double)), spin, SLOT(setValue(double)));

            break;
        }
        case Variant::Type::Color: {
            QColor value = vait.value()->value().value<QColor>();

            KColorCombo *colorBox = ColorUtils::createColorCombo((ColorUtils::ColorScheme)vait.value()->colorScheme(), configWidget);

            colorBox->setColor(value);
            connectMapWidget(colorBox, SIGNAL(activated(const QColor &)));
            m_colorComboMap[vait.key()] = colorBox;

            connect(*vait, SIGNAL(valueChanged(const QColor &)), colorBox, SLOT(setColor(const QColor &)));

            editWidget = colorBox;
            break;
        }
        case Variant::Type::Bool: {
            const bool value = vait.value()->value().toBool();
            QCheckBox *box = new QCheckBox(vait.value()->toolbarCaption(), configWidget);

            box->setChecked(value);
            connectMapWidget(box, SIGNAL(toggled(bool)));
            m_boolCheckMap[vait.key()] = box;

            connect(*vait, SIGNAL(valueChanged(bool)), box, SLOT(setChecked(bool)));

            editWidget = box;
            break;
        }
        case Variant::Type::Raw:
        case Variant::Type::PenStyle:
        case Variant::Type::PenCapStyle:
        case Variant::Type::Multiline:
        case Variant::Type::RichText:
        case Variant::Type::None: {
            // Do nothing, as these data types are not handled in the toolbar
            break;
        }
        }

        if (!editWidget)
            continue;

        const int widgetH = QFontMetrics(configWidget->font()).height() + 2;
        editWidget->setMinimumHeight(widgetH); // note: this is hack-ish; something is not ok with the layout
        editWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        // In the case of the toolbar, we don't want it too high
        if (editWidget->height() > parent->height() - 2)
            editWidget->setMaximumHeight(parent->height() - 2);

        switch (type) {
        case Variant::Type::VarName:
        case Variant::Type::Combo:
        case Variant::Type::String: {
            QSizePolicy p(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed /*, 1, 1 */);
            p.setHorizontalStretch(1);
            p.setVerticalStretch(1);

            editWidget->setSizePolicy(p);
            editWidget->setMaximumWidth(250);
            break;
        }

        case Variant::Type::FileName:
        case Variant::Type::Port:
        case Variant::Type::Pin:
        case Variant::Type::Select:
        case Variant::Type::KeyPad:
        case Variant::Type::SevenSegment:
        case Variant::Type::Int:
        case Variant::Type::Double:
        case Variant::Type::Color:
        case Variant::Type::Bool:
        case Variant::Type::Raw:
        case Variant::Type::PenStyle:
        case Variant::Type::PenCapStyle:
        case Variant::Type::Multiline:
        case Variant::Type::RichText:
        case Variant::Type::None:
            break;
        }

        configLayout->addWidget(editWidget);
    }

    configLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));

    return configWidget;
}

void ItemInterface::connectMapWidget(QWidget *widget, const char *_signal)
{
    connect(widget, _signal, this, SLOT(tbDataChanged()));
}

// TODO move to separate file
struct BoolLock {
    bool *m_flagPtr;
    BoolLock(bool *flagPtr)
        : m_flagPtr(flagPtr)
    {
        if (m_flagPtr == nullptr) {
            qCCritical(KTL_LOG) << "nullptr flagPtr";
            return;
        }
        if (*m_flagPtr == true) {
            qCWarning(KTL_LOG) << "flag expected to be false, addr=" << m_flagPtr << " Doing nothing";
            m_flagPtr = nullptr;
        } else {
            *m_flagPtr = true;
        }
    }
    ~BoolLock()
    {
        if (m_flagPtr != nullptr) {
            *m_flagPtr = false;
        }
    }
};

void ItemInterface::tbDataChanged()
{
    qCDebug(KTL_LOG) << "begin";
    if (m_isInTbDataChanged) {
        qCDebug(KTL_LOG) << "avoiding recursion, returning";
        return;
    }
    BoolLock inTbChangedLock(&m_isInTbDataChanged);
    // Manual string values
    const KLineEditMap::iterator m_stringLineEditMapEnd = m_stringLineEditMap.end();
    for (KLineEditMap::iterator leit = m_stringLineEditMap.begin(); leit != m_stringLineEditMapEnd; ++leit) {
        slotSetData(leit.key(), leit.value()->text());
    }

    // String values from comboboxes
    const KComboBoxMap::iterator m_stringComboBoxMapEnd = m_stringComboBoxMap.end();
    for (KComboBoxMap::iterator cmit = m_stringComboBoxMap.begin(); cmit != m_stringComboBoxMapEnd; ++cmit) {
        qCDebug(KTL_LOG) << "set KCombo data for " << cmit.key() << " to " << cmit.value()->currentText();
        slotSetData(cmit.key(), cmit.value()->currentText());
    }

    // Colors values from colorcombos
    const KColorComboMap::iterator m_colorComboMapEnd = m_colorComboMap.end();
    for (KColorComboMap::iterator ccit = m_colorComboMap.begin(); ccit != m_colorComboMapEnd; ++ccit) {
        slotSetData(ccit.key(), ccit.value()->color());
    }

    // Bool values from checkboxes
    const QCheckBoxMap::iterator m_boolCheckMapEnd = m_boolCheckMap.end();
    for (QCheckBoxMap::iterator chit = m_boolCheckMap.begin(); chit != m_boolCheckMapEnd; ++chit) {
        slotSetData(chit.key(), chit.value()->isChecked());
    }

    const IntSpinBoxMap::iterator m_intSpinBoxMapEnd = m_intSpinBoxMap.end();
    for (IntSpinBoxMap::iterator it = m_intSpinBoxMap.begin(); it != m_intSpinBoxMapEnd; ++it) {
        slotSetData(it.key(), it.value()->value());
    }

    // (?) Combined values from spin boxes and combo boxes
    // (?) Get values from all spin boxes

    const DoubleSpinBoxMap::iterator m_doubleSpinBoxMapEnd = m_doubleSpinBoxMap.end();
    for (DoubleSpinBoxMap::iterator sbit = m_doubleSpinBoxMap.begin(); sbit != m_doubleSpinBoxMapEnd; ++sbit) {
        // 		VariantDataMap::iterator vait = variantData.find(sbit.key());
        slotSetData(sbit.key(), sbit.value()->value());
    }

    // Filenames from KUrlRequesters
    const KUrlReqMap::iterator m_stringURLReqMapEnd = m_stringURLReqMap.end();
    for (KUrlReqMap::iterator urlit = m_stringURLReqMap.begin(); urlit != m_stringURLReqMapEnd; ++urlit) {
        qCDebug(KTL_LOG) << "set kurlrequester data for " << urlit.key() << " to " << urlit.value()->url();
        QVariant urlVar(urlit.value()->url().path());
        qCDebug(KTL_LOG) << "urlVar=" << urlVar << " urlVar.toUrl=" << urlVar.toUrl();
        slotSetData(urlit.key(), urlVar);
    }

    if (p_cvb)
        p_cvb->setModified(true);
}

void ItemInterface::setProperty(Variant *v)
{
    slotSetData(v->id(), v->value());
}

void ItemInterface::slotSetData(const QString &id, QVariant value)
{
    if (!p_itemGroup || (p_itemGroup->itemCount() == 0)) {
        qCDebug(KTL_LOG) << "p_itemGroup not valid:" << p_itemGroup;
        return;
    }

    if (!p_itemGroup->itemsAreSameType()) {
        qCDebug(KTL_LOG) << "Items are not the same type!";
        return;
    }
    qCDebug(KTL_LOG) << "id=" << id << " value=" << value;

    const ItemList itemList = p_itemGroup->items(true);
    const ItemList::const_iterator end = itemList.end();
    for (ItemList::const_iterator it = itemList.begin(); it != end; ++it) {
        if (*it)
            (*it)->property(id)->setValue(value);
    }

    if (p_cvb)
        p_cvb->setModified(true);

    ItemEditor::self()->itemGroupUpdated(p_itemGroup);

    if (p_cvb)
        p_cvb->requestStateSave(m_currentActionTicket);
}

#include "moc_iteminterface.cpp"
