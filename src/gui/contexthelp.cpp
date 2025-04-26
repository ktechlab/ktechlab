/***************************************************************************
 *   Copyright (C) 2003-2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "contexthelp.h"
#include "cnitem.h"
#include "cnitemgroup.h"
#include "docmanager.h"
#include "itemlibrary.h"
#include "itemselector.h"
#include "katemdi.h"
#include "libraryitem.h"
#include "richtexteditor.h"

#include <KIO/Global>
#include <KIconLoader>
#include <KLocalizedString>
#include <KMessageBox>
// #include <k3popupmenu.h>
// #include <KRun> // 2024.04.21 - see below
#include <KIO/CommandLauncherJob>
#include <KIO/OpenUrlJob>
#include <KIO/JobUiDelegate>
#include <KIO/JobUiDelegateFactory>
// #include <k3iconview.h>

#include <QDropEvent>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QIcon>
#include <QLabel>
#include <QRegExp>
#include <QTimer>
#include <QToolButton>
#include <QValidator>
// #include <q3widgetstack.h>
#include <QMimeData>
#include <QStandardPaths>
#include <QTextBrowser>

#include <cassert>

#include <ktechlab_debug.h>

ContextHelp *ContextHelp::m_pSelf = nullptr;

ContextHelp *ContextHelp::self(KateMDI::ToolView *parent)
{
    if (!m_pSelf) {
        assert(parent);
        m_pSelf = new ContextHelp(parent);
    }
    return m_pSelf;
}

ContextHelp::ContextHelp(KateMDI::ToolView *parent)
    : QWidget(parent)
    , Ui::ContextHelpWidget(/* parent */)
{
    setupUi(this);

    setWhatsThis(i18n("Provides context-sensitive help relevant to the current editing being performed."));
    setAcceptDrops(true);

    if (parent->layout()) {
        parent->layout()->addWidget(this);
        qCDebug(KTL_LOG) << " added item selector to parent's layout " << parent;
    } else {
        qCWarning(KTL_LOG) << " unexpected null layout on parent " << parent;
    }

    QFont font;
    font.setBold(true);
    if (font.pointSize() != 0)
        font.setPointSize(int(font.pointSize() * 1.4));
    m_pNameLabel->setFont(font);
    m_pNameLabel->setTextFormat(Qt::RichText);

    m_pBrowserView = new QTextBrowser;
    m_pBrowserView->setOpenLinks(false);

    m_pBrowserView->setFocusPolicy(Qt::NoFocus);
    m_pBrowserLayout->addWidget(m_pBrowserView);

    connect(m_pBrowserView, &QTextBrowser::anchorClicked, this, &ContextHelp::openURL);

    m_pEditor = new RichTextEditor(m_pWidgetStack->widget(1));
    m_pEditor->setObjectName("ContextHelpEditor");
    m_pTopLayout->addWidget(m_pEditor);

    m_pEditor->installEventFilter(this);
    m_pEditor->editorViewport()->installEventFilter(this);
    slotClear();

    connect(m_pEditButton, &QPushButton::clicked, this, &ContextHelp::slotEdit);
    connect(m_pSaveButton, &QPushButton::clicked, this, &ContextHelp::slotSave);
    connect(m_pResetButton, &QToolButton::clicked, this, &ContextHelp::slotEditReset);
    connect(m_pChangeDescriptionsDirectory, &QToolButton::clicked, this, &ContextHelp::requestItemDescriptionsDirectory);

    connect(m_pLanguageSelect, qOverload<int>(&QComboBox::activated), this, &ContextHelp::setCurrentLanguage);

    m_pResetButton->setIcon(QIcon::fromTheme("dialog-cancel"));
    m_pChangeDescriptionsDirectory->setIcon(QIcon::fromTheme("folder"));

    connect(ComponentSelector::self(), &ComponentSelector::itemSelected, this, &ContextHelp::setBrowserItem);

    connect(FlowPartSelector::self(), &FlowPartSelector::itemSelected, this, &ContextHelp::setBrowserItem);
#ifdef MECHANICS
    connect(MechanicsSelector::self(), &MechanicsSelector::itemSelected, this, &MechanicsSelector::setBrowserItem);
#endif

    QTimer::singleShot(10, this, &ContextHelp::slotInitializeLanguageList);
}

ContextHelp::~ContextHelp()
{
}

bool ContextHelp::eventFilter(QObject *watched, QEvent *e)
{
    // 	qCDebug(KTL_LOG) << "watched="<<watched;

    if ((watched != m_pEditor) && (watched != m_pEditor->editorViewport()))
        return false;

    switch (e->type()) {
    case QEvent::DragEnter: {
        QDragEnterEvent *dragEnter = static_cast<QDragEnterEvent *>(e);
        if (!dragEnter->mimeData()->text().startsWith("ktechlab/"))
            break;

        // dragEnter->acceptAction(); // 2018.12.07
        dragEnter->acceptProposedAction();
        return true;
    }

    case QEvent::Drop: {
        QDropEvent *dropEvent = static_cast<QDropEvent *>(e);
        const QMimeData *mimeData = dropEvent->mimeData();

        if (!mimeData->text().startsWith("ktechlab/"))
            break;

        dropEvent->accept();

        QString type;
        QDataStream stream(mimeData->data(mimeData->text()) /*, QIODevice::ReadOnly */);
        stream >> type;

        LibraryItem *li = itemLibrary()->libraryItem(type);
        if (!li)
            return true;

        m_pEditor->insertURL("ktechlab-help:///" + type, li->name());
        return true;
    }

    default:
        break;
    }

    return false;
}

void ContextHelp::slotInitializeLanguageList()
{
    const QStringList descriptionLanguages = itemLibrary()->descriptionLanguages();
    for (const QString &languageCode : descriptionLanguages) {
        QString text = languageCode;
        QLocale locale(languageCode);
        if (locale != QLocale::c()) {
            text = locale.nativeLanguageName();
            // For some languages the native name might be empty.
            // In this case use the non native language name as fallback.
            // See: QTBUG-51323
            if (text.isEmpty()) {
                text = QLocale::languageToString(locale.language());
            }
        }
        m_pLanguageSelect->addItem(text, languageCode);
    }
    m_currentLanguage = QLocale().name();
    const int currentIndex = m_pLanguageSelect->findData(m_currentLanguage);
    m_pLanguageSelect->setCurrentIndex(currentIndex);
}

bool ContextHelp::isEditChanged()
{
    if (m_lastItemType.isEmpty())
        return false;

    // Is the edit widget raised?
    if (m_pWidgetStack->currentIndex() != 1) {
        return false;
    }

    // We are currently editing an item. Is it changed?
    return (m_pEditor->text() != itemLibrary()->description(m_lastItemType, m_currentLanguage).trimmed());
}

void ContextHelp::slotUpdate(Item *item)
{
    if (isEditChanged()) {
        return;
    }

    m_lastItemType = item ? item->type() : QString();
    m_pEditButton->setEnabled(item);

    if (!item) {
        slotClear();
        return;
    }

    m_pWidgetStack->setCurrentIndex(0);
    setContextHelp(item->name(), itemLibrary()->description(m_lastItemType, QLocale().name()));
}

void ContextHelp::setBrowserItem(const QString &type)
{
    if (isEditChanged())
        return;

    QString description = itemLibrary()->description(type, QLocale().name());
    if (description.isEmpty())
        return;

    QString name;
    LibraryItem *li = itemLibrary()->libraryItem(type);
    if (li)
        name = li->name();
    else
        name = type;

    m_lastItemType = type;
    setContextHelp(name, description);
    m_pEditButton->setEnabled(true);
}

void ContextHelp::slotClear()
{
    setContextHelp(i18n("No Item Selected"), nullptr);
    m_pEditButton->setEnabled(false);

    // Can we go hide the edit widget?
    if (!isEditChanged())
        m_pWidgetStack->setCurrentIndex(0);
}

void ContextHelp::slotMultipleSelected()
{
    setContextHelp(i18n("Multiple Items"), nullptr);
}

void ContextHelp::setContextHelp(QString name, QString help)
{
    // BEGIN modify help string as appropriate
    help = help.trimmed();
    parseInfo(help);
    RichTextEditor::makeUseStandardFont(&help);
    addLinkTypeAppearances(&help);
    // END modify help string as appropriate

    m_pNameLabel->setText(name);
    m_pBrowserView->setSearchPaths({itemLibrary()->itemDescriptionsDirectory()});
    m_pBrowserView->clear();
    if (help.startsWith("<html>")) {
        m_pBrowserView->insertHtml(help);
    } else {
        m_pBrowserView->insertPlainText(help);
    }
}

void ContextHelp::parseInfo(QString &info)
{
    info.replace("<example>", "<br><br><b>Example:</b><blockquote>");
    info.replace("</example>", "</blockquote>");
}

void ContextHelp::slotEdit()
{
    if (m_lastItemType.isEmpty())
        return;

    QStringList resourcePaths;
    QString currentResourcePath = itemLibrary()->itemDescriptionsDirectory();
    QString defaultResourcePath = QStandardPaths::locate(QStandardPaths::AppDataLocation, "contexthelp/", QStandardPaths::LocateDirectory);

    resourcePaths << currentResourcePath;
    if (currentResourcePath != defaultResourcePath)
        resourcePaths << defaultResourcePath;

    m_pEditor->setResourcePaths(resourcePaths);
    QString description = itemLibrary()->description(m_lastItemType, m_currentLanguage);
    m_pEditor->setText(description);
    m_pWidgetStack->setCurrentIndex(1);
}

void ContextHelp::setCurrentLanguage(int languageIndex)
{
    const QString language = m_pLanguageSelect->itemData(languageIndex).toString();
    if (!saveDescription(m_currentLanguage)) {
        m_pLanguageSelect->blockSignals(true);
        const int currentIndex = m_pLanguageSelect->findData(m_currentLanguage);
        m_pLanguageSelect->setCurrentIndex(currentIndex);
        m_pLanguageSelect->blockSignals(false);
        return;
    }

    m_currentLanguage = language;
    slotEdit();
}

void ContextHelp::requestItemDescriptionsDirectory()
{
    const QString path = QFileDialog::getExistingDirectory(nullptr, QString(), itemLibrary()->itemDescriptionsDirectory());
    if (!path.isEmpty()) {
        itemLibrary()->setItemDescriptionsDirectory(path);
    }
}

void ContextHelp::slotEditReset()
{
    if (isEditChanged()) {
        KGuiItem continueItem = KStandardGuiItem::cont(); // KStandardGuiItem::cont();
        continueItem.setText(i18n("Reset"));
        int answer = KMessageBox::warningContinueCancel(this, i18n("Reset item help to last saved changes?"), i18n("Reset"), continueItem);
        if (answer == KMessageBox::Cancel)
            return;
    }

    m_pWidgetStack->setCurrentIndex(0);
}

void ContextHelp::slotSave()
{
    if (!saveDescription(m_currentLanguage))
        return;

    setContextHelp(m_pNameLabel->text(), itemLibrary()->description(m_lastItemType, QLocale().name()));
    m_pWidgetStack->setCurrentIndex(0);
}

bool ContextHelp::saveDescription(const QString &language)
{
    if (m_lastItemType.isEmpty()) {
        KMessageBox::error(nullptr, i18n("Cannot save item description."));
        return false;
    }

    return itemLibrary()->setDescription(m_lastItemType, m_pEditor->text(), language);
}

// static function
void ContextHelp::addLinkTypeAppearances(QString *html)
{
    QRegularExpression rx("<a href=\"([^\"]*)\">([^<]*)</a>");

    //int pos = 0;
    //
    // while ((pos = rx.indexIn(*html, pos)) >= 0) {
    //     QString anchorText = rx.cap(0);      // contains e.g.: <a href="http://ktechlab.org/">KTechlab website</a>
    //     const QString urlString = rx.cap(1); // contains e.g.: http://ktechlab.org/
    //     QString text = rx.cap(2);            // contains e.g.: KTechlab website

    QRegularExpressionMatchIterator itMatch = rx.globalMatch(*html);
    while (itMatch.hasNext()) {
        QRegularExpressionMatch match = itMatch.next();
        QString anchorText = match.captured(0);      // contains e.g.: <a href="http://ktechlab.org/">KTechlab website</a>
        const QString urlString = match.captured(1); // contains e.g.: http://ktechlab.org/
        QString text = match.captured(2);            // contains e.g.: KTechlab website

        int pos = match.capturedStart(0);

        int length = anchorText.length();

        const QUrl url(urlString);
        LinkType lt = extractLinkType(url);

        QColor color; // default constructor gives an "invalid" color
        QString imageURL;

        switch (lt) {
        case HelpLink:
            break;

        case NewHelpLink:
            color = Qt::red;
            break;

        case ExampleLink: {
            // QString iconName = KMimeType::iconNameForURL( examplePathToFullPath( KUrl( url ).path() ) );
            QString iconName = KIO::iconNameForUrl(QUrl::fromLocalFile(examplePathToFullPath(url.path())));
            imageURL = KIconLoader::global()->iconPath(iconName, -KIconLoader::SizeSmall);
            break;
        }

        case ExternalLink: {
            imageURL = QStandardPaths::locate(QStandardPaths::AppDataLocation, "icons/external_link.png");
            break;
        }
        }

        QString newAnchorText;

        if (color.isValid()) {
            newAnchorText = QString("<a href=\"%1\" style=\"color: %2;\">%3</a>").arg(urlString, color.name(), text);
        } else if (!imageURL.isEmpty()) {
            newAnchorText = anchorText;
            newAnchorText += QString(" <img src=\"%1\"/>").arg(imageURL);
        }

        if (!newAnchorText.isEmpty())
            html->replace(pos, length, newAnchorText);

        //pos++; // avoid the string we just found
    }
}

// static function
ContextHelp::LinkType ContextHelp::extractLinkType(const QUrl &url)
{
    QString path = url.path();

    if (url.scheme() == "ktechlab-help") {
        if (itemLibrary()->haveDescription(path, QLocale().name()))
            return HelpLink;
        else
            return NewHelpLink;
    }

    if (url.scheme() == "ktechlab-example")
        return ExampleLink;

    return ExternalLink;
}

// static function
QString ContextHelp::examplePathToFullPath(QString path)
{
    // quick security check
    path.remove("..");

    if (path.startsWith("/"))
        path.remove(0, 1);

    return QStandardPaths::locate(QStandardPaths::AppDataLocation, "examples/" + path);
}

void ContextHelp::openURL(const QUrl &url /*, const KParts::OpenUrlArguments & */)
{
    QString path = url.path();

    switch (extractLinkType(url)) {
    case HelpLink:
    case NewHelpLink:
        setBrowserItem(path);
        break;

    case ExampleLink:
        DocManager::self()->openURL(QUrl::fromLocalFile(examplePathToFullPath(path)));
        break;

    case ExternalLink: {
        // external url
        // KRun *r = new KRun(url, this);
        // connect(r, &KRun::finished, r, &KRun::deleteLater);
        // connect(r, &KRun::error, r, &KRun::deleteLater);

        KIO::OpenUrlJob *r = new KIO::OpenUrlJob(url, this);
        // for "open with" functionality
        r->setUiDelegate(KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, this));
        connect(r, &KJob::finished, r, &KJob::deleteLater);
        connect(r, &KJob::error, r, &KJob::deleteLater);
        break;
    }
    }
}

#include "moc_contexthelp.cpp"
