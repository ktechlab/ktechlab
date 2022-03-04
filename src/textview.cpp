/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <KXMLGUIClient>

#include "asmformatter.h"
#include "config.h"
#include "filemetainfo.h"
#include "gpsimprocessor.h"
#include "ktechlab.h"
#include "symbolviewer.h"
#include "textdocument.h"
#include "textview.h"
#include "variablelabel.h"
#include "viewiface.h"

//#include <ktexteditor/editinterface.h> // ?
#include <KTextEditor/TextHintInterface>

// #include "kateview.h"
#include <KActionCollection>
#include <KLocalizedString>
// #include <k3popupmenu.h>
#include <KToolBarPopupAction>
#include <KXMLGUIFactory>

#include <QActionGroup>
#include <QApplication>
#include <QCursor>
#include <QVBoxLayout>
//#include <qobjectlist.h>
#include <QClipboard>
#include <QFocusEvent>
#include <QMenu>
#include <QStandardPaths>
#include <QTimer>

#include <ktechlab_debug.h>

// BEGIN class TextView
TextView::TextView(TextDocument *textDocument, ViewContainer *viewContainer, uint viewAreaId)
    : View(textDocument, viewContainer, viewAreaId)
{
    m_view = textDocument->createKateView(this);
    m_view->insertChildClient(this);

    KActionCollection *ac = actionCollection();

    // BEGIN Convert To * Actions
    // KToolBarPopupAction * pa = new KToolBarPopupAction( i18n("Convert to"), "fork", 0, 0, 0, ac, "program_convert" );
    KToolBarPopupAction *pa = new KToolBarPopupAction(QIcon::fromTheme("fork"), i18n("Convert To"), ac);
    pa->setObjectName("program_convert");
    pa->setDelayed(false);
    ac->addAction(pa->objectName(), pa);

    QMenu *m = pa->menu();

    m->setTitle(i18n("Convert To"));
    QAction *actToMicrobe = m->addAction(QIcon::fromTheme("convert_to_microbe"), i18n("Microbe"));
    actToMicrobe->setData(TextDocument::MicrobeOutput);
    m->addAction(QIcon::fromTheme("convert_to_assembly"), i18n("Assembly"))->setData(TextDocument::AssemblyOutput);
    m->addAction(QIcon::fromTheme("convert_to_hex"), i18n("Hex"))->setData(TextDocument::HexOutput);
    m->addAction(QIcon::fromTheme("convert_to_pic"), i18n("PIC (upload)"))->setData(TextDocument::PICOutput);
    connect(m, &QMenu::triggered, textDocument, &TextDocument::slotConvertTo);

    // m->setItemEnabled( TextDocument::MicrobeOutput, false ); // 2018.12.02
    actToMicrobe->setEnabled(false);
    ac->addAction(pa->objectName(), pa);
    // END Convert To * Actions

    {
        // new QAction( i18n("Format Assembly Code"), "", Qt::Key_F12, textDocument, SLOT(formatAssembly()), ac, "format_asm" );
        QAction *action = new QAction(i18n("Format Assembly Code"), ac);
        action->setObjectName("format_asm");
        action->setShortcut(Qt::Key_F12);
        connect(action, &QAction::triggered, textDocument, &TextDocument::formatAssembly);
        ac->addAction(action->objectName(), action);
    }

#ifndef NO_GPSIM
    // BEGIN Debug Actions
    {
        // new QAction( i18n("Set &Breakpoint"), 0, 0, this, SLOT(toggleBreakpoint()), ac, "debug_toggle_breakpoint" );
        QAction *action = new QAction(i18n("Set &Breakpoint"), ac);
        action->setObjectName("debug_toggle_breakpoint");
        connect(action, &QAction::triggered, this, &TextView::toggleBreakpoint);
        ac->addAction(action->objectName(), action);
    }
    {
        // new QAction( i18n("Run"), "debug-run", 0, textDocument, SLOT(debugRun()), ac, "debug_run" );
        QAction *action = new QAction(QIcon::fromTheme("debug-run"), i18n("Run"), ac);
        action->setObjectName("debug_run");
        connect(action, &QAction::triggered, textDocument, &TextDocument::debugRun);
        ac->addAction(action->objectName(), action);
    }
    {
        // new QAction( i18n("Interrupt"), "media-playback-pause", 0, textDocument, SLOT(debugInterrupt()), ac, "debug_interrupt" );
        QAction *action = new QAction(QIcon::fromTheme("media-playback-pause"), i18n("Interrupt"), ac);
        action->setObjectName("debug_interrupt");
        connect(action, &QAction::triggered, textDocument, &TextDocument::debugInterrupt);
        ac->addAction(action->objectName(), action);
    }
    {
        // new QAction( i18n("Stop"), "process-stop", 0, textDocument, SLOT(debugStop()), ac, "debug_stop" );
        QAction *action = new QAction(QIcon::fromTheme("process-stop"), i18n("Stop"), ac);
        action->setObjectName("debug_stop");
        connect(action, &QAction::triggered, textDocument, &TextDocument::debugStop);
        ac->addAction(action->objectName(), action);
    }
    {
        // new QAction( i18n("Step"), "debug-step-instruction", Qt::CTRL|Qt::ALT|Qt::Key_Right, textDocument, SLOT(debugStep()), ac, "debug_step" );
        QAction *action = new QAction(QIcon::fromTheme("debug-step-instruction"), i18n("Step"), ac);
        action->setObjectName("debug_step");
        action->setShortcut(Qt::CTRL | Qt::ALT | Qt::Key_Right);
        connect(action, &QAction::triggered, textDocument, &TextDocument::debugStep);
        ac->addAction(action->objectName(), action);
    }
    {
        // new QAction( i18n("Step Over"), "debug-step-over", 0, textDocument, SLOT(debugStepOver()), ac, "debug_step_over" );
        QAction *action = new QAction(QIcon::fromTheme("debug-step-over"), i18n("Step Over"), ac);
        action->setObjectName("debug_step_over");
        connect(action, &QAction::triggered, textDocument, &TextDocument::debugStepOver);
        ac->addAction(action->objectName(), action);
    }
    {
        // new QAction( i18n("Step Out"), "debug-step-out", 0, textDocument, SLOT(debugStepOut()), ac, "debug_step_out" );
        QAction *action = new QAction(QIcon::fromTheme("debug-step-out"), i18n("Step Out"), ac);
        action->setObjectName("debug_step_out");
        connect(action, &QAction::triggered, textDocument, &TextDocument::debugStepOut);
        ac->addAction(action->objectName(), action);
    }
    // END Debug Actions
#endif

    setXMLFile("ktechlabtextui.rc");

    // m_view->setXMLFile("ktechlabkateui.rc") is protected, replace it with code below
    {
        // see https://github.com/KDE/kxmlgui/blob/master/src/kxmlguiclient.cpp#L219
        QString _file = "ktechlabkateui.rc";
        QStringList allFiles;

        const QString filter = componentName() + QLatin1Char('/') + _file;

        // files on filesystem
        allFiles << QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QLatin1String("kxmlgui5/") + filter); // KF >= 5.1

        // KF >= 5.4 (resource file)
        const QString qrcFile(QLatin1String(":/kxmlgui5/") + filter);
        if (QFile::exists(qrcFile)) {
            allFiles << qrcFile;
        }

        // then compat locations
        const QStringList compatFiles = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, filter) + // kdelibs4, KF 5.0
            QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, _file); // kdelibs4, KF 5.0, caller passes component name

        if (allFiles.isEmpty() && !compatFiles.isEmpty()) {
            qCWarning(KTL_LOG) << "KXMLGUI file found at deprecated location" << compatFiles
                                     << "-- please use ${KDE_INSTALL_KXMLGUI5DIR} to install this file instead.";
        }
        allFiles += compatFiles;

        QString doc;
        if (!allFiles.isEmpty()) {
            QString fileFound = findMostRecentXMLFile(allFiles, doc);
            qCInfo(KTL_LOG) << "KXMLGUI file found at : " << fileFound;
            m_view->replaceXMLFile(fileFound, QString(), false);
        } else {
            qCWarning(KTL_LOG) << "KXMLGUI not found for " << _file;
        }
    }

    m_savedCursorLine = 0;
    m_savedCursorColumn = 0;
    m_pViewIface = new TextViewIface(this);

    setAcceptDrops(true);

    // m_view->installPopup( static_cast<Q3PopupMenu*>( KTechlab::self()->factory()->container( "ktexteditor_popup", KTechlab::self() ) ) );
    m_view->setContextMenu(static_cast<QMenu *>(KTechlab::self()->factory()->container("ktexteditor_popup", KTechlab::self())));

    // TODO this ought to not use internal widgets of the ktexteditor view,
    //      and only use KTextEditor::TextHintInterface for the hint instead
    //      of the own popup logic
    // QWidget *internalView = m_view->findChild<QWidget *>("KateViewInternal");

    connect(m_view, &KTextEditor::View::cursorPositionChanged, this, &TextView::slotCursorPositionChanged);
    connect(m_view, &KTextEditor::View::selectionChanged, this, &TextView::slotSelectionmChanged);

    // setFocusWidget(internalView);
    connect(this, &TextView::focused, this, &TextView::gotFocus);

    m_layout->insertWidget(0, m_view);

    slotCursorPositionChanged();
    slotInitDebugActions();
    initCodeActions();

#ifndef NO_GPSIM
    m_pTextViewLabel = new VariableLabel(this);
    m_pTextViewLabel->hide();

    TextViewEventFilter *eventFilter = new TextViewEventFilter(this);
    connect(eventFilter, &TextViewEventFilter::wordHoveredOver, this, &TextView::slotWordHoveredOver);
    connect(eventFilter, &TextViewEventFilter::wordUnhovered, this, &TextView::slotWordUnhovered);

    // internalView->installEventFilter(eventFilter);
#endif

    // TODO HACK disable some actions which collide with ktechlab's actions.
    //  the proper solution would be to move the actions from KTechLab object level to document level for
    //  all types of documents
    for (QAction *act : actionCollection()->actions()) {
        qCDebug(KTL_LOG) << "act: " << act->text() << " shortcut " << act->shortcut() << ":" << act;

        if (((act->objectName()) == QLatin1String("file_save")) || ((act->objectName()) == QLatin1String("file_save_as")) || ((act->objectName()) == QLatin1String("file_print")) || ((act->objectName()) == QLatin1String("edit_undo")) ||
            ((act->objectName()) == QLatin1String("edit_redo")) || ((act->objectName()) == QLatin1String("edit_cut")) || ((act->objectName()) == QLatin1String("edit_copy")) || ((act->objectName()) == QLatin1String("edit_paste"))) {
            act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
            // act->setShortcutConfigurable(true);
            act->setShortcut(Qt::Key_unknown);
            qCDebug(KTL_LOG) << "action " << act << " disabled";
        }
    }
}

TextView::~TextView()
{
    if (KTechlab::self()) {
        // 2017.01.09: do not crash on document close. factory has its clients removed in TextDocument::~TextDocument()
        // if ( KXMLGUIFactory * f = m_view->factory() )
        //	f->removeClient( m_view );
        KTechlab::self()->addNoRemoveGUIClient(m_view);
    }

    delete m_pViewIface;
}

bool TextView::closeView()
{
    if (textDocument()) {
        const QUrl url = textDocument()->url();
        if (!url.isEmpty())
            fileMetaInfo()->grabMetaInfo(url, this);
    }

    bool doClose = View::closeView();
    if (doClose)
        KTechlab::self()->factory()->removeClient(m_view);
    return View::closeView();
}

bool TextView::gotoLine(const int line)
{
    // return m_view->setCursorPosition( line, 0/*m_view->cursorColumn()*/ );
    return m_view->setCursorPosition(KTextEditor::Cursor(line, 0 /*m_view->cursorColumn()*/));
}

TextDocument *TextView::textDocument() const
{
    return static_cast<TextDocument *>(document());
}
void TextView::undo()
{
    qCDebug(KTL_LOG);
    // note: quite a hack, but could not find any more decent way of getting to undo/redo interface
    // note: quite a hack, but could not find any more decent way of getting to undo/redo interface
    QAction *action = actionByName("edit_undo");
    if (action) {
        action->trigger();
        return;
    }
    qCWarning(KTL_LOG) << "no edit_undo action in text view! no action taken";
}
void TextView::redo()
{
    qCDebug(KTL_LOG);
    // note: quite a hack, but could not find any more decent way of getting to undo/redo interface
    QAction *action = actionByName("edit_redo");
    if (action) {
        action->trigger();
        return;
    }
    qCWarning(KTL_LOG) << "no edit_redo action in text view! no action taken";
}

void TextView::cut()
{
    // m_view-> cut();
    if (!m_view->selection())
        return;
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(m_view->document()->text(m_view->selectionRange()));
    m_view->document()->removeText(m_view->selectionRange());
}

void TextView::copy()
{
    // m_view->copy();
    if (!m_view->selection())
        return;
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(m_view->document()->text(m_view->selectionRange()));
}

void TextView::paste()
{
    // m_view->paste();
    QClipboard *clipboard = QApplication::clipboard();
    m_view->document()->insertText(m_view->cursorPosition(), clipboard->text());
}

void TextView::disableActions()
{
    QMenu *tb = (dynamic_cast<KToolBarPopupAction *>(actionByName("program_convert")))->menu();

    const QList<QAction *> actions = tb->actions();
    for (QAction *a : actions) {
        switch (a->data().toInt()) {
        case TextDocument::AssemblyOutput:
        case TextDocument::HexOutput:
        case TextDocument::PICOutput:
            a->setEnabled(false);
            break;
        default:
            qCDebug(KTL_LOG) << " skip action: " << a;
        }
    }
    // tb->setItemEnabled( TextDocument::AssemblyOutput, false );    // 2018.12.02
    // tb->setItemEnabled( TextDocument::HexOutput, false );
    // tb->setItemEnabled( TextDocument::PICOutput, false );
    actionByName("format_asm")->setEnabled(false);

#ifndef NO_GPSIM
    actionByName("debug_toggle_breakpoint")->setEnabled(false);
#endif
}

// KTextEditor::View::saveResult TextView::save() { return m_view->save(); }
bool TextView::save()
{
    return (m_view->document()->documentSave());
}

// KTextEditor::View::saveResult TextView::saveAs() { return m_view->saveAs(); }
bool TextView::saveAs()
{
    return m_view->document()->documentSaveAs();
}
void TextView::print()
{
    qCDebug(KTL_LOG);
    // note: quite a hack, but could not find any more decent way of getting to undo/redo interface
    QAction *action = actionByName("file_print");
    if (action) {
        action->trigger();
        return;
    }
    qCWarning(KTL_LOG) << "no file_print action in text view! no action taken";
}

void TextView::gotFocus()
{
#ifndef NO_GPSIM
    GpsimDebugger *debugger = textDocument()->debugger();
    if (!debugger || !debugger->gpsim())
        return;

    SymbolViewer::self()->setContext(debugger->gpsim());
#endif
}

void TextView::slotSelectionmChanged()
{
    KTechlab::self()->actionByName("edit_cut")->setEnabled(m_view->selection());
    KTechlab::self()->actionByName("edit_copy")->setEnabled(m_view->selection());
}

void TextView::initCodeActions()
{
    disableActions();

    QMenu *tb = (dynamic_cast<KToolBarPopupAction *>(actionByName("program_convert")))->menu();

    QAction *actHexOut = nullptr;
    QAction *actPicOut = nullptr;
    QAction *actAsmOut = nullptr;
    const QList<QAction *> actions = tb->actions();
    for (QAction *a : actions) {
        switch (a->data().toInt()) {
        case TextDocument::AssemblyOutput:
            actAsmOut = a;
            break;
        case TextDocument::HexOutput:
            actHexOut = a;
            break;
        case TextDocument::PICOutput:
            actPicOut = a;
            break;
        default:
            qCDebug(KTL_LOG) << " skip action: " << a;
        }
    }

    switch (textDocument()->guessedCodeType()) {
    case TextDocument::ct_asm: {
        // tb->setItemEnabled( TextDocument::HexOutput, true );  // 2018.12.02
        // tb->setItemEnabled( TextDocument::PICOutput, true );
        actHexOut->setEnabled(true);
        actPicOut->setEnabled(true);
        actionByName("format_asm")->setEnabled(true);
#ifndef NO_GPSIM
        actionByName("debug_toggle_breakpoint")->setEnabled(true);
        slotInitDebugActions();
#endif
        break;
    }
    case TextDocument::ct_c: {
        // tb->setItemEnabled( TextDocument::AssemblyOutput, true );
        // tb->setItemEnabled( TextDocument::HexOutput, true );
        // tb->setItemEnabled( TextDocument::PICOutput, true );
        actAsmOut->setEnabled(true);
        actHexOut->setEnabled(true);
        actPicOut->setEnabled(true);
        break;
    }
    case TextDocument::ct_hex: {
        // tb->setItemEnabled( TextDocument::AssemblyOutput, true );
        // tb->setItemEnabled( TextDocument::PICOutput, true );
        actAsmOut->setEnabled(true);
        actPicOut->setEnabled(true);
        break;
    }
    case TextDocument::ct_microbe: {
        // tb->setItemEnabled( TextDocument::AssemblyOutput, true );
        // tb->setItemEnabled( TextDocument::HexOutput, true );
        // tb->setItemEnabled( TextDocument::PICOutput, true );
        actAsmOut->setEnabled(true);
        actHexOut->setEnabled(true);
        actPicOut->setEnabled(true);
        break;
    }
    case TextDocument::ct_unknown: {
        break;
    }
    }
}

void TextView::setCursorPosition(uint line, uint col)
{
    // m_view->setCursorPosition( line, col );
    m_view->setCursorPosition(KTextEditor::Cursor(line, col));
}

unsigned TextView::currentLine()
{
    // unsigned l,c ;
    KTextEditor::Cursor curs = m_view->cursorPosition();
    return curs.line();
}
unsigned TextView::currentColumn()
{
    // unsigned l,c ;
    KTextEditor::Cursor curs = m_view->cursorPosition(); // &l, &c );
    return curs.column();
}

void TextView::saveCursorPosition()
{
    KTextEditor::Cursor curs = m_view->cursorPosition(); // &m_savedCursorLine, &m_savedCursorColumn );
    m_savedCursorLine = curs.line();
    m_savedCursorColumn = curs.column();
}

void TextView::restoreCursorPosition()
{
    m_view->setCursorPosition(KTextEditor::Cursor(m_savedCursorLine, m_savedCursorColumn));
}

void TextView::slotCursorPositionChanged()
{
    uint line, column;
    KTextEditor::Cursor curs = m_view->cursorPosition(); //&line, &column );
    line = curs.line();
    column = curs.column();

    m_statusBar->setStatusText(i18n(" Line: %1 Col: %2 ", QString::number(line + 1), QString::number(column + 1)));

    slotUpdateMarksInfo();
}

void TextView::slotUpdateMarksInfo()
{
#ifndef NO_GPSIM
    uint l;
    //int c;
    KTextEditor::Cursor curs = m_view->cursorPosition(); // &l, &c );
    l = curs.line();
    //c = curs.column();

    KTextEditor::MarkInterface *iface = qobject_cast<KTextEditor::MarkInterface *>(m_view->document());
    // if ( m_view->getDoc()->mark(l) & TextDocument::Breakpoint )
    if (iface->mark(l) & TextDocument::Breakpoint)
        actionByName("debug_toggle_breakpoint")->setText(i18n("Clear &Breakpoint"));
    else
        actionByName("debug_toggle_breakpoint")->setText(i18n("Set &Breakpoint"));
#endif
}

void TextView::slotInitDebugActions()
{
#ifndef NO_GPSIM
    bool isRunning = textDocument()->debuggerIsRunning();
    bool isStepping = textDocument()->debuggerIsStepping();
    bool ownDebugger = textDocument()->ownDebugger();

    actionByName("debug_run")->setEnabled(!isRunning || isStepping);
    actionByName("debug_interrupt")->setEnabled(isRunning && !isStepping);
    actionByName("debug_stop")->setEnabled(isRunning && ownDebugger);
    actionByName("debug_step")->setEnabled(isRunning && isStepping);
    actionByName("debug_step_over")->setEnabled(isRunning && isStepping);
    actionByName("debug_step_out")->setEnabled(isRunning && isStepping);
#endif // !NO_GPSIM
}

void TextView::toggleBreakpoint()
{
#ifndef NO_GPSIM
    uint l;
    //int c;
    KTextEditor::Cursor curs = m_view->cursorPosition(); // &l, &c );
    l = curs.line();
    //c = curs.column();
    // const bool isBreakpoint = m_view->getDoc()->mark(l) & TextDocument::Breakpoint;
    KTextEditor::MarkInterface *iface = qobject_cast<KTextEditor::MarkInterface *>(m_view->document());
    if (!iface)
        return;
    const bool isBreakpoint = iface->mark(l) & TextDocument::Breakpoint;
    // textDocument()->setBreakpoint( l, !(m_view->getDoc()->mark(l) & TextDocument::Breakpoint) );
    textDocument()->setBreakpoint(l, !isBreakpoint);
#endif // !NO_GPSIM
}

void TextView::slotWordHoveredOver(const QString &word, int line, int /*col*/)
{
#ifndef NO_GPSIM
    // We're only interested in popping something up if we currently have a debugger running
    GpsimProcessor *gpsim = textDocument()->debugger() ? textDocument()->debugger()->gpsim() : nullptr;
    if (!gpsim) {
        m_pTextViewLabel->hide();
        return;
    }

    // Find out if the word that we are hovering over is the operand data
    // KTextEditor::EditInterface * e = (KTextEditor::EditInterface*)textDocument()->kateDocument()->qt_cast("KTextEditor::EditInterface");
    // InstructionParts parts( e->textLine( unsigned(line) ) );
    InstructionParts parts(textDocument()->kateDocument()->line(line));
    if (!parts.operandData().contains(word))
        return;

    if (RegisterInfo *info = gpsim->registerMemory()->fromName(word))
        m_pTextViewLabel->setRegister(info, info->name());

    else {
        int operandAddress = textDocument()->debugger()->programAddress(textDocument()->debugFile(), line);
        if (operandAddress == -1) {
            m_pTextViewLabel->hide();
            return;
        }

        int regAddress = gpsim->operandRegister(operandAddress);

        if (regAddress != -1)
            m_pTextViewLabel->setRegister(gpsim->registerMemory()->fromAddress(regAddress), word);

        else {
            m_pTextViewLabel->hide();
            return;
        }
    }

    m_pTextViewLabel->move(mapFromGlobal(QCursor::pos()) + QPoint(0, 20));
    m_pTextViewLabel->show();
#else
    Q_UNUSED(word);
    Q_UNUSED(line);
#endif // !NO_GPSIM
}

void TextView::slotWordUnhovered()
{
#ifndef NO_GPSIM
    m_pTextViewLabel->hide();
#endif // !NO_GPSIM
}
// END class TextView

// BEGIN class TextViewEventFilter
TextViewEventFilter::TextViewEventFilter(TextView *textView)
{
    m_hoverStatus = Sleeping;
    m_pTextView = textView;
    m_lastLine = m_lastCol = -1;

    //((KTextEditor::TextHintInterface*)textView->kateView()->qt_cast("KTextEditor::TextHintInterface"))->enableTextHints(0);
    {
        KTextEditor::View *view = textView->kateView();
        KTextEditor::TextHintInterface *iface = qobject_cast<KTextEditor::TextHintInterface *>(view);
        if (iface) {
            // iface->enableTextHints(0);
            iface->registerTextHintProvider(this);
            // connect( textView->kateView(), SIGNAL(needTextHint(int, int, QString &)), this, SLOT(slotNeedTextHint( int, int, QString& )) );
            // 2020.09.10 - no such signal
            // connect( view, SIGNAL(needTextHint(const KTextEditor::Cursor &, QString &)),
            //         this, SLOT(slotNeedTextHint(const KTextEditor::Cursor &, QString &)) );
        } else {
            qCWarning(KTL_LOG) << "KTextEditor::View does not implement TextHintInterface for " << view;
        }
    }

    m_pHoverTimer = new QTimer(this);
    connect(m_pHoverTimer, &QTimer::timeout, this, &TextViewEventFilter::hoverTimeout);

    m_pSleepTimer = new QTimer(this);
    connect(m_pSleepTimer, &QTimer::timeout, this, &TextViewEventFilter::gotoSleep);

    m_pNoWordTimer = new QTimer(this);
    connect(m_pNoWordTimer, &QTimer::timeout, this, &TextViewEventFilter::slotNoWordTimeout);
}
TextViewEventFilter::~TextViewEventFilter()
{
    KTextEditor::View *view = m_pTextView->kateView();
    KTextEditor::TextHintInterface *iface = qobject_cast<KTextEditor::TextHintInterface *>(view);
    if (iface) {
        iface->unregisterTextHintProvider(this);
    }
}

QString TextViewEventFilter::textHint(KTextEditor::View * /*view*/, const KTextEditor::Cursor &position)
{
    qCDebug(KTL_LOG) << "TextViewEventFilter::textHint: position=" << position.toString();
    QString str;
    slotNeedTextHint(position, str);
    return QString();
}

bool TextViewEventFilter::eventFilter(QObject *, QEvent *e)
{
    // 	qCDebug(KTL_LOG) << "e->type() = " << e->type();

    if (e->type() == QEvent::MouseMove) {
        if (!m_pNoWordTimer->isActive())
            m_pNoWordTimer->start(10);
        return false;
    }

    if (e->type() == QEvent::FocusOut || e->type() == QEvent::FocusIn || e->type() == QEvent::MouseButtonPress || e->type() == QEvent::Leave || e->type() == QEvent::Wheel) {
        // user moved focus somewhere - hide the tip and sleep
        if (((QFocusEvent *)e)->reason() != Qt::PopupFocusReason)
            updateHovering(nullptr, -1, -1);
    }

    return false;
}

void TextViewEventFilter::hoverTimeout()
{
    m_pSleepTimer->stop();
    m_hoverStatus = Active;
    emit wordHoveredOver(m_lastWord, m_lastLine, m_lastCol);
}

void TextViewEventFilter::gotoSleep()
{
    m_hoverStatus = Sleeping;
    m_lastWord = QString();
    emit wordUnhovered();
    m_pHoverTimer->stop();
}

void TextViewEventFilter::slotNoWordTimeout()
{
    updateHovering(nullptr, -1, -1);
}

void TextViewEventFilter::updateHovering(const QString &currentWord, int line, int col)
{
    if ((currentWord == m_lastWord) && (line == m_lastLine))
        return;

    m_lastWord = currentWord;
    m_lastLine = line;
    m_lastCol = col;

    if (currentWord.isEmpty()) {
        if (m_hoverStatus == Active)
            m_hoverStatus = Hidden;

        emit wordUnhovered();
        if (!m_pSleepTimer->isActive()) {
            m_pSleepTimer->setSingleShot(true);
            m_pSleepTimer->start(2000 /*, true */);
        }
        return;
    }

    if (m_hoverStatus != Sleeping) {
        emit wordHoveredOver(currentWord, line, col);
    } else {
        m_pHoverTimer->setSingleShot(true);
        m_pHoverTimer->start(700 /*, true */);
    }
}

static inline bool isWordLetter(const QString &s)
{
    return (s.length() == 1) && (s[0].isLetterOrNumber() || s[0] == '_');
}

void TextViewEventFilter::slotNeedTextHint(const KTextEditor::Cursor &position, QString & /*text*/)
{
    int line = position.line();
    int col = position.column();
    m_pNoWordTimer->stop();

    // KTextEditor::EditInterface * e = (KTextEditor::EditInterface*)m_pTextView->textDocument()->kateDocument()->qt_cast("KTextEditor::EditInterface");
    KTextEditor::Document *d = m_pTextView->textDocument()->kateDocument();

    // Return if we aren't currently in a word
    if (!isWordLetter(d->text(KTextEditor::Range(line, col, line, col + 1)))) {
        updateHovering(QString(), line, col);
        return;
    }

    // Find the start of the word
    int wordStart = col;
    do
        wordStart--;
    while (wordStart > 0 && isWordLetter(d->text(KTextEditor::Range(line, wordStart, line, wordStart + 1))));
    wordStart++;

    // Find the end of the word
    int wordEnd = col;
    do
        wordEnd++;
    while (isWordLetter(d->text(KTextEditor::Range(line, wordEnd, line, wordEnd + 1))));

    QString t = d->text(KTextEditor::Range(line, wordStart, line, wordEnd));

    updateHovering(t, line, col);
}
// END class TextViewEventFilter
