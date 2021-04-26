/***************************************************************************
 *   Copyright (C) 2006 by David Saxton - david@bluehaze.org               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "richtexteditor.h"

#include <KToggleAction>
#include <KToolBarPopupAction>

#include <KLocalizedString>
// #include <k3popupmenu.h>
#include <KActionCollection>
#include <KTextEdit>
#include <KToolBar>

#include <QFont>
#include <QIcon>
#include <QTextEdit>
//#include <qmime.h>
#include <QColorDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QMenu>
#include <QPushButton>
#include <QRegExp>
#include <QTextList>
#include <QTextListFormat>
#include <QVBoxLayout>

#include <ktechlab_debug.h>

// #include <q3vbox.h>
// #include <q3textedit.h>
// #include <q3stylesheet.h>

// BEGIN class RichTextEditor
RichTextEditor::RichTextEditor(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this /*, 0, 6 */);
    layout->setMargin(0);
    layout->setSpacing(6);
    m_pEditor = new QTextEdit(this); //, "RichTextEdit" );
    m_pEditor->setObjectName("RichTextEdit");
    layout->addWidget(m_pEditor);

    // m_pEditor->setTextFormat( Qt::RichText ); // 2018.12.07 - just use toHtml() and html()

    connect(m_pEditor, &QTextEdit::textChanged, this, &RichTextEditor::textChanged);
    connect(m_pEditor, &QTextEdit::currentCharFormatChanged, this, &RichTextEditor::slotCurrentCharFormatChanged);
    // connect( m_pEditor, SIGNAL( currentFontChanged( const QFont & ) ), this, SLOT( fontChanged( const QFont & ) ) ); // 2018.01.03 - use slotCurrentCharFormatChanged
    // connect( m_pEditor, SIGNAL( currentColorChanged( const QColor & ) ), this, SLOT( colorChanged( const QColor & ) ) ); // 2018.01.03 - use slotCurrentCharFormatChanged
    // connect( m_pEditor, SIGNAL( currentAlignmentChanged( int ) ), this, SLOT( alignmentChanged( int ) ) ); // 2018.01.03 - use slotCurrentCharFormatChanged
    // connect( m_pEditor, SIGNAL( currentVerticalAlignmentChanged( Q3TextEdit::VerticalAlignment ) ), this, SLOT(verticalAlignmentChanged()) ); // 2018.01.03 - use slotCurrentCharFormatChanged

    KToolBar *tools = new KToolBar(QStringLiteral("RichTextEditorToops"), this);
    layout->addWidget(tools);
    KActionCollection *ac = new KActionCollection(m_pEditor);

    // m_pTextBold = new KToggleAction( i18n("Bold"), "format-text-bold", Qt::CTRL + Qt::Key_B, 0, 0, ac, "format_bold" );
    m_pTextBold = new KToggleAction(i18n("Bold"), ac);
    m_pTextBold->setObjectName("text_bold");
    m_pTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    m_pTextBold->setIcon(QIcon::fromTheme("format-text-bold"));
    connect(m_pTextBold, &KToggleAction::toggled, this, &RichTextEditor::slotSetBold);
    // m_pTextBold->plug( tools );
    tools->addAction(m_pTextBold);

    // m_pTextItalic = new KToggleAction( i18n("Italic"), "format-text-italic", Qt::CTRL + Qt::Key_I, 0, 0, ac, "format_italic" );
    m_pTextItalic = new KToggleAction(i18n("Italic"), ac);
    m_pTextItalic->setObjectName("text_italic");
    m_pTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    m_pTextItalic->setIcon(QIcon::fromTheme("format-text-italic"));
    connect(m_pTextItalic, &KToggleAction::toggled, this, &RichTextEditor::slotSetItalic);
    // m_pTextItalic->plug( tools );
    tools->addAction(m_pTextItalic);

    // m_pTextUnderline = new KToggleAction( i18n("Underline"), "format-text-underline", Qt::CTRL + Qt::Key_U, 0, 0, ac, "format_underline" );
    m_pTextUnderline = new KToggleAction(i18n("Underline"), ac);
    m_pTextUnderline->setObjectName("text_under");
    m_pTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    m_pTextItalic->setIcon(QIcon::fromTheme("format-text-underline"));
    connect(m_pTextUnderline, &KToggleAction::toggled, this, &RichTextEditor::slotSetUnderline);
    // m_pTextUnderline->plug( tools );
    tools->addAction(m_pTextUnderline);

    // m_pTextList = new KToggleAction( i18n("List"), "unsorted_list", Qt::CTRL + Qt::Key_L, 0, 0, ac, "format_list" );
    m_pTextList = new KToggleAction(i18n("List"), ac);
    m_pTextList->setObjectName("unsorted_list");
    m_pTextList->setShortcut(Qt::CTRL + Qt::Key_L);
    m_pTextItalic->setIcon(QIcon::fromTheme("format-list-unordered"));
    connect(m_pTextList, &KToggleAction::toggled, this, &RichTextEditor::slotSetList);
    // m_pTextList->plug( tools );
    tools->addAction(m_pTextList);

    // BEGIN Text horizontal-alignment actions
    // m_pTextAlignment = new KToolBarPopupAction( i18n("Text Alignment"), "format-justify-left", 0, 0, 0, ac, "text_alignment" );
    m_pTextAlignment = new KToolBarPopupAction(QIcon::fromTheme("format-justify-left"), i18n("Text Alignment"), ac);
    m_pTextAlignment->setObjectName("text_left");
    // m_pTextAlignment->plug( tools );
    tools->addAction(m_pTextAlignment);
    m_pTextAlignment->setDelayed(false);

    // K3PopupMenu * m = m_pTextAlignment->menu();
    QMenu *m = m_pTextAlignment->menu();
    // m->insertTitle( i18n("Text Alignment") );
    m->setTitle(i18n("Text Alignment"));
    // m->setCheckable( true ); // 2018.12.07

    // m->insertItem( QIcon::fromTheme( "format-justify-left" ), i18n("Align Left"),		Qt::AlignLeft );
    m->addAction(QIcon::fromTheme("format-justify-left"), i18n("Align Left"))->setData(Qt::AlignLeft);
    // m->insertItem( QIcon::fromTheme( "format-justify-center"), i18n("Align Center"),	Qt::AlignHCenter );
    m->addAction(QIcon::fromTheme("format-justify-center"), i18n("Align Center"))->setData(Qt::AlignHCenter);
    // m->insertItem( QIcon::fromTheme( "format-justify-right" ), i18n("Align Right"),	Qt::AlignRight );
    m->addAction(QIcon::fromTheme("format-justify-right"), i18n("Align Right"))->setData(Qt::AlignRight);
    // m->insertItem( QIcon::fromTheme( "format-justify-fill" ), i18n("Align Block"),	Qt::AlignJustify );
    m->addAction(QIcon::fromTheme("format-justify-fill"), i18n("Align Block"))->setData(Qt::AlignJustify);
    connect(m, &QMenu::triggered, this, &RichTextEditor::slotSetAlignment);
    // END Text horizontal-alignment actions

    // BEGIN Text vertical-alignment actions
    // m_pTextVerticalAlignment = new KToolBarPopupAction( i18n("Text Vertical Alignment"), "text", 0, 0, 0, ac, "text_vertical_alignment" );
    m_pTextVerticalAlignment = new KToolBarPopupAction(QIcon::fromTheme(QString("text_vertical_alignment")), i18n("Text Vertical Alignment"), ac);
    m_pTextVerticalAlignment->setObjectName("text");
    // m_pTextVerticalAlignment->plug( tools );
    tools->addAction(m_pTextVerticalAlignment);
    m_pTextVerticalAlignment->setDelayed(false);

    m = m_pTextVerticalAlignment->menu();
    // m->insertTitle( i18n("Text Vertical Alignment") );
    m->setTitle(i18n("Text Vertical Alignment"));
    // m->setCheckable( true ); // 2018.12.07

    m->addAction(QIcon::fromTheme("format-text-superscript"), i18n("Superscript"))->setData(QTextCharFormat::AlignSuperScript);
    // m->insertItem( QIcon::fromTheme( "format-text-superscript" ), i18n("Superscript"),	QTextCharFormat::AlignSuperScript );
    m->addAction(i18n("Normal"))->setData(QTextCharFormat::AlignNormal);
    // m->insertItem(						i18n("Normal"),			QTextCharFormat::AlignNormal );
    m->addAction(QIcon::fromTheme("format-text-subscript"), i18n("Subscript"))->setData(QTextCharFormat::AlignSubScript);
    // m->insertItem( QIcon::fromTheme( "format-text-subscript" ), i18n("Subscript"),		QTextCharFormat::AlignSubScript );
    connect(m, &QMenu::triggered, this, &RichTextEditor::slotSetVerticalAlignment);
    // END Text vertical-alignment actions

    QPixmap pm(16, 16);
    pm.fill(Qt::black);
    // m_pTextColor = new KAction( i18n("Text Color..."), pm, 0, this, SLOT(textColor()), ac, "format_color" );
    m_pTextColor = new QAction(i18n("Text Color..."), this);
    m_pTextColor->setIcon(pm);
    m_pTextColor->setObjectName("format_color");
    connect(m_pTextColor, &QAction::triggered, this, &RichTextEditor::textColor);
    // m_pTextColor->plug( tools );
    ac->addAction("format_color", m_pTextColor);
    tools->addAction(m_pTextColor);
}

RichTextEditor::~RichTextEditor()
{
}

void RichTextEditor::makeUseStandardFont(QString *html)
{
    if (!html)
        return;

    QFont f;
    QString bodyString = QString("<body style=\"font-size:%1pt;font-family:%2\">").arg(f.pointSize()).arg(f.family());

    if (html->contains("<body>")) {
        // Set the correct font size
        QFont f;
        html->replace("<body>", bodyString);
    }

    else if (!html->startsWith("<html>")) {
        html->prepend("<html>" + bodyString);
        html->append("</body></html>");
    }
}

QWidget *RichTextEditor::editorViewport() const
{
    return m_pEditor->viewport();
}

void RichTextEditor::setText(QString text)
{
    if (!Qt::mightBeRichText(text)) {
        // Format the text to be HTML
        text.replace('\n', "<br>");
    }

    m_pEditor->setText(text);
}

QString RichTextEditor::text() const
{
    QString text = m_pEditor->toHtml().trimmed();

    // Remove the style info (e.g. style="font-size:8pt;font-family:DejaVu Sans") inserted into the body tag.
    text.replace(QRegExp("<body style=\"[^\"]*\">"), "<body>");

    // Replace all non-latin1 characters with HTML codes to represent them
    QString nonAsciiChars;
    for (int i = 0; i < text.length(); ++i) {
        QChar current = text[i];
        if ((current.toLatin1() == 0) && (current.unicode() != 0)) {
            // A non-latin1 character
            if (!nonAsciiChars.contains(current))
                nonAsciiChars.append(current);
        }
    }
    for (int i = 0; i < nonAsciiChars.length(); ++i) {
        text.replace(nonAsciiChars[i], QString("&#%1;").arg(nonAsciiChars[i].unicode()));
    }

    return text;
}

void RichTextEditor::insertURL(const QString &url, const QString &text)
{
    insertHTML(QString("<a href=\"%1\">%2</a>").arg(url).arg(text));
}

void RichTextEditor::insertHTML(const QString &html)
{
    // 2018.12.07
    // 	// Save cursor position
    // 	//int cursorPara, cursorIndex;
    // 	//m_pEditor->getCursorPosition( & cursorPara, & cursorIndex );
    //     QPoint cursorPos;
    //     cursorPos = m_pEditor->cursor().pos();
    //
    // 	// replaceString is used so that the inserted text is at the cursor position.
    // 	// it's just a random set of characters, so that the chance of them actually being
    // 	// used is about zero.
    // 	QString replaceString = "SXbCk2CtqJ83";
    //
    // 	m_pEditor->insert( replaceString );
    // 	QString editorText = m_pEditor->text();
    // 	//editorText.replace( replaceString, html, (uint)0 ); // 2018.12.07
    //     editorText.replace( replaceString, html, Qt::CaseInsensitive );
    // 	m_pEditor->setText( editorText );
    //
    // 	// Restore cursor position
    // 	//m_pEditor->setCursorPosition( cursorPara, cursorIndex );
    //     m_pEditor->cursor().setPos(cursorPos);

    m_pEditor->insertHtml(html);
}

void RichTextEditor::slotSetBold(bool isBold)
{
    QTextCharFormat format;
    if (isBold) {
        format.setFontWeight(QFont::Bold);
    } else {
        format.setFontWeight(QFont::Normal);
    }
    m_pEditor->textCursor().mergeCharFormat(format);
}
void RichTextEditor::slotSetItalic(bool isItalic)
{
    QTextCharFormat format;
    format.setFontItalic(isItalic);
    m_pEditor->textCursor().mergeCharFormat(format);
}
void RichTextEditor::slotSetUnderline(bool isUnderline)
{
    QTextCharFormat format;
    format.setFontUnderline(isUnderline);
    m_pEditor->textCursor().mergeCharFormat(format);
}
void RichTextEditor::slotSetAlignment(QAction *act)
{
    int alignment = act->data().toInt();
    QTextBlockFormat format = m_pEditor->textCursor().blockFormat();
    format.setAlignment((Qt::AlignmentFlag)alignment);
    m_pEditor->textCursor().mergeBlockFormat(format);
}

void RichTextEditor::slotSetVerticalAlignment(QAction *action)
{
    int a = action->data().toInt();
    // m_pEditor->setVerticalAlignment( (Q3TextEdit::VerticalAlignment)a );
    // m_pEditor->setAlignment(a);
    QTextCharFormat format;
    format.setVerticalAlignment((QTextCharFormat::VerticalAlignment)a);
    m_pEditor->mergeCurrentCharFormat(format);
}

void RichTextEditor::slotSetList(bool set)
{
    // m_pEditor->setParagType( set ? Q3StyleSheetItem::DisplayListItem : Q3StyleSheetItem::DisplayBlock, Q3StyleSheetItem::ListDisc );
    if (set) {
        m_pEditor->textCursor().createList(QTextListFormat::ListDisc);
    } else {
        QTextCursor cursor = m_pEditor->textCursor();
        QTextList *list = cursor.currentList();
        if (list) {
            QTextListFormat::Style style = QTextListFormat::ListStyleUndefined;
            QTextListFormat listFormat;
            listFormat.setIndent(0);
            listFormat.setStyle(style);
            list->setFormat(listFormat);

            for (int i = list->count() - 1; i >= 0; --i)
                list->removeItem(i);
        }
    }
}

void RichTextEditor::slotCurrentCharFormatChanged(const QTextCharFormat &f)
{
    fontChanged(f.font());
    // colorChanged( m_pEditor->foregroundColor() ); // 2018.12.07
    colorChanged(m_pEditor->palette().color(m_pEditor->foregroundRole()));
    alignmentChanged(m_pEditor->alignment());
    verticalAlignmentChanged(); // note: consider removing this method
}

void RichTextEditor::fontChanged(const QFont &f)
{
    m_pTextBold->setChecked(f.bold());
    m_pTextItalic->setChecked(f.italic());
    m_pTextUnderline->setChecked(f.underline());
}

void RichTextEditor::textColor()
{
    const QColor c = QColorDialog::getColor(m_pEditor->textColor(), this);
    if (c.isValid()) {
        m_pEditor->setTextColor(c);
    }
}

void RichTextEditor::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    m_pTextColor->setIcon(pix);
}

void RichTextEditor::alignmentChanged(int a)
{
    if (/*( a == Qt::AlignAuto ) || */ (a & Qt::AlignLeft))
        m_pTextAlignment->setIcon(QIcon::fromTheme("format-justify-left"));
    else if ((a & Qt::AlignHCenter))
        m_pTextAlignment->setIcon(QIcon::fromTheme("format-justify-center"));
    else if ((a & Qt::AlignRight))
        m_pTextAlignment->setIcon(QIcon::fromTheme("format-justify-right"));
    else if ((a & Qt::AlignJustify))
        m_pTextAlignment->setIcon(QIcon::fromTheme("format-justify-fill"));
}

void RichTextEditor::verticalAlignmentChanged()
{
    // 	QTextEdit::VerticalAlignment a =
    // 	if ( a == KTextEdit::AlignNormal )
    // 		m_pTextVerticalAlignment->setIcon( "text" );
    // 	else if ( a == KTextEdit::AlignSuperScript )
    // 		m_pTextVerticalAlignment->setIcon( "format-text-superscript" );
    // 	else if ( a == KTextEdit::AlignSubScript )
    // 		m_pTextVerticalAlignment->setIcon( "format-text-subscript" );
}

void RichTextEditor::setResourcePaths(const QStringList &paths)
{
    // m_pEditor->mimeSourceFactory()->setFilePath( paths );

    for (QStringList::const_iterator itStr = paths.begin(); itStr != paths.end(); ++itStr) {
        QString dirName = *itStr;
        QDir dir(dirName);
        dir.setFilter(QDir::Files);
        QStringList l;
        l << "*.png";
        dir.setNameFilters(l);
        QFileInfoList fileInfoList = dir.entryInfoList();
        qCDebug(KTL_LOG) << " list size " << fileInfoList.size();
        for (QFileInfoList::iterator itFile = fileInfoList.begin(); itFile != fileInfoList.end(); ++itFile) {
            QFileInfo &fi = *itFile;

            QString fullPath = fi.path() + "/" + fi.fileName();
            QPixmap img(fullPath);
            if (img.isNull()) {
                qCWarning(KTL_LOG) << " img is null " << fullPath;
            }

            m_pEditor->document()->addResource(QTextDocument::ImageResource, QUrl(fi.fileName()), QVariant(img));
            qCDebug(KTL_LOG) << " added resource: " << fi.fileName() << " to " << fullPath;
        }
    }
}
// END class RichTextEditor

// BEGIN class RichTextEditorDlg
RichTextEditorDlg::RichTextEditorDlg(QWidget *parent, const QString &caption)
    : QDialog(parent)
{
    setObjectName("RichTextEditorDlg");
    setModal(true);
    setWindowTitle(caption);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    m_pEditor = new RichTextEditor(this);
    mainLayout->addWidget(m_pEditor);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
// END class RichTextEditorDlg
