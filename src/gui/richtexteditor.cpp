/***************************************************************************
 *   Copyright (C) 2006 by David Saxton - david@bluehaze.org               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "richtexteditor.h"

//#include <kactionclasses.h>
#include <ktoolbarpopupaction.h>
#include <ktoggleaction.h>
#include <kcolordialog.h>
#include <kicon.h>
#include <klocalizedstring.h>
#include <k3popupmenu.h>
#include <ktextedit.h>
#include <ktoolbar.h>
#include <kactioncollection.h>
#include <kmenu.h>
#include <k3textedit.h>

#include <qfont.h>
#include <qlayout.h>
#include <qmime.h>
#include <qregexp.h>
#include <q3vbox.h>
#include <q3textedit.h>
#include <q3stylesheet.h>

//BEGIN class RichTextEditor
RichTextEditor::RichTextEditor(QWidget *parent, const char *name)
	: QWidget(parent, name)
{
	QVBoxLayout * layout = new QVBoxLayout( this, 0, 6 );
	m_pEditor = new K3TextEdit( this ); //, "RichTextEdit" );
	m_pEditor->setName("RichTextEdit");
	layout->addWidget( m_pEditor );
	
	m_pEditor->setTextFormat( Qt::RichText );
	
	connect( m_pEditor, SIGNAL( textChanged() ), SIGNAL( textChanged() ) );
	connect( m_pEditor, SIGNAL( currentFontChanged( const QFont & ) ), this, SLOT( fontChanged( const QFont & ) ) );
	connect( m_pEditor, SIGNAL( currentColorChanged( const QColor & ) ), this, SLOT( colorChanged( const QColor & ) ) );
	connect( m_pEditor, SIGNAL( currentAlignmentChanged( int ) ), this, SLOT( alignmentChanged( int ) ) );
	connect( m_pEditor, SIGNAL( currentVerticalAlignmentChanged( Q3TextEdit::VerticalAlignment ) ), this, SLOT(verticalAlignmentChanged()) );
	
	KToolBar * tools = new KToolBar( this, "RichTextEditorToops" );
	layout->add( tools );
	KActionCollection * ac = new KActionCollection( m_pEditor );
	
	
	//m_pTextBold = new KToggleAction( i18n("Bold"), "format-text-bold", Qt::CTRL + Qt::Key_B, 0, 0, ac, "format_bold" );
    m_pTextBold = new KToggleAction( i18n("Bold"), ac);
    m_pTextBold->setName("text_bold");
    m_pTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    m_pTextBold->setIcon( KIcon("format-text-bold") );
	connect( m_pTextBold, SIGNAL(toggled(bool)), m_pEditor, SLOT(setBold(bool)) );
	//m_pTextBold->plug( tools );
    tools->addAction(m_pTextBold);

	//m_pTextItalic = new KToggleAction( i18n("Italic"), "format-text-italic", Qt::CTRL + Qt::Key_I, 0, 0, ac, "format_italic" );
    m_pTextItalic = new KToggleAction( i18n("Italic"), ac);
    m_pTextItalic->setName("text_italic");
    m_pTextItalic->setShortcut( Qt::CTRL + Qt::Key_I );
    m_pTextItalic->setIcon( KIcon("format-text-italic") );
	connect( m_pTextItalic, SIGNAL(toggled(bool)), m_pEditor, SLOT(setItalic(bool)) );
	//m_pTextItalic->plug( tools );
    tools->addAction(m_pTextItalic);

	//m_pTextUnderline = new KToggleAction( i18n("Underline"), "format-text-underline", Qt::CTRL + Qt::Key_U, 0, 0, ac, "format_underline" );
    m_pTextUnderline = new KToggleAction( i18n("Underline"), ac);
    m_pTextUnderline->setName("text_under");
    m_pTextUnderline->setShortcut( Qt::CTRL + Qt::Key_U );
    m_pTextItalic->setIcon( KIcon("format-text-underline") );
	connect( m_pTextUnderline, SIGNAL(toggled(bool)), m_pEditor, SLOT(setUnderline(bool)) );
	//m_pTextUnderline->plug( tools );
    tools->addAction(m_pTextUnderline);
	
	//m_pTextList = new KToggleAction( i18n("List"), "unsorted_list", Qt::CTRL + Qt::Key_L, 0, 0, ac, "format_list" );
    m_pTextList = new KToggleAction( i18n("List"), ac);
    m_pTextList->setName("unsorted_list");
    m_pTextList->setShortcut( Qt::CTRL + Qt::Key_L );
    m_pTextItalic->setIcon( KIcon("format-list-unordered") );
	connect( m_pTextList, SIGNAL(toggled(bool)), SLOT(slotSetList(bool)) );
	//m_pTextList->plug( tools );
    tools->addAction( m_pTextList );
	
	
	//BEGIN Text horizontal-alignment actions
	//m_pTextAlignment = new KToolBarPopupAction( i18n("Text Alignment"), "format-justify-left", 0, 0, 0, ac, "text_alignment" );
    m_pTextAlignment = new KToolBarPopupAction(
            KIcon("format-justify-left"),
            i18n("Text Alignment"),
            ac);
    m_pTextAlignment->setName("text_left");
	//m_pTextAlignment->plug( tools );
    tools->addAction(m_pTextAlignment);
	m_pTextAlignment->setDelayed(false);
	
	//K3PopupMenu * m = m_pTextAlignment->menu();
    QMenu * m = m_pTextAlignment->menu();
    //m->insertTitle( i18n("Text Alignment") );
    m->setTitle( i18n("Text Alignment"));
	m->setCheckable( true );
	
	m->insertItem( KIcon( "format-justify-left" ), i18n("Align Left"),		Qt::AlignLeft );
	m->insertItem( KIcon( "format-justify-center"), i18n("Align Center"),	Qt::AlignHCenter );
	m->insertItem( KIcon( "format-justify-right" ), i18n("Align Right"),	Qt::AlignRight );
	m->insertItem( KIcon( "format-justify-fill" ), i18n("Align Block"),	Qt::AlignJustify );
	connect( m, SIGNAL(activated(int)), m_pEditor, SLOT(setAlignment(int)) );
	//END Text horizontal-alignment actions
	
	
	//BEGIN Text vertical-alignment actions
	//m_pTextVerticalAlignment = new KToolBarPopupAction( i18n("Text Vertical Alignment"), "text", 0, 0, 0, ac, "text_vertical_alignment" );
    m_pTextVerticalAlignment = new KToolBarPopupAction(
            KIcon(QString("text_vertical_alignment")),
            i18n("Text Vertical Alignment"),
            ac);
    m_pTextVerticalAlignment->setName("text");
	//m_pTextVerticalAlignment->plug( tools );
    tools->addAction(m_pTextVerticalAlignment);
	m_pTextVerticalAlignment->setDelayed(false);
	
	m = m_pTextVerticalAlignment->menu();
	//m->insertTitle( i18n("Text Vertical Alignment") );
    m->setTitle( i18n("Text Vertical Alignment") );
	m->setCheckable( true );
	
	m->insertItem( KIcon( "format-text-superscript" ), i18n("Superscript"),	Q3TextEdit::AlignSuperScript );
	m->insertItem(						i18n("Normal"),			Q3TextEdit::AlignNormal );
	m->insertItem( KIcon( "format-text-subscript" ), i18n("Subscript"),		Q3TextEdit::AlignSubScript );
	connect( m, SIGNAL(activated(int)), this, SLOT(slotSetVerticalAlignment(int)) );
	//END Text vertical-alignment actions
	
	
	QPixmap pm( 16, 16 );
	pm.fill( Qt::black );
	//m_pTextColor = new KAction( i18n("Text Color..."), pm, 0, this, SLOT(textColor()), ac, "format_color" );
    m_pTextColor = new KAction( i18n("Text Color..."), this);
    m_pTextColor->setIcon(pm);
    m_pTextColor->setName("format_color");
    connect(m_pTextColor, SIGNAL(activated(int)), this, SLOT(textColor()));
	//m_pTextColor->plug( tools );
    ac->addAction("format_color", m_pTextColor);
    tools->addAction(m_pTextColor);
	
	
}


RichTextEditor::~RichTextEditor()
{
}


void RichTextEditor::makeUseStandardFont( QString * html )
{
	if ( !html )
		return;
	
	QFont f;
	QString bodyString = QString("<body style=\"font-size:%1pt;font-family:%2\">").arg( f.pointSize() ).arg( f.family() );
	
	if ( html->contains("<body>") )
	{
		// Set the correct font size
		QFont f;
		html->replace( "<body>", bodyString );
	}
	
	else if ( !html->startsWith("<html>") )
	{
		html->prepend( "<html>" + bodyString );
		html->append( "</body></html>" );
	}
}


QWidget * RichTextEditor::editorViewport() const
{
	return m_pEditor->viewport();
}


void RichTextEditor::setText( QString text )
{
	if ( !Q3StyleSheet::mightBeRichText( text ) )
	{
		// Format the text to be HTML
		text.replace( '\n', "<br>" );
	}
	
	m_pEditor->setText( text );
}



QString RichTextEditor::text() const
{
	QString text = m_pEditor->text().stripWhiteSpace();
	
	// Remove the style info (e.g. style="font-size:8pt;font-family:DejaVu Sans") inserted into the body tag.
	text.replace( QRegExp( "<body style=\"[^\"]*\">"), "<body>" );
	
	// Replace all non-latin1 characters with HTML codes to represent them
	QString nonAsciiChars;
	for ( unsigned i = 0; i < text.length(); ++i )
	{
		QChar current = text[i];
		if ( (current.latin1() == 0) && (current.unicode() != 0) )
		{
			// A non-latin1 character
			if ( !nonAsciiChars.contains( current ) )
				nonAsciiChars.append( current );
		}
	}
	for ( unsigned i = 0; i < nonAsciiChars.length(); ++i )
	{
		text.replace( nonAsciiChars[i], QString("&#%1;").arg( nonAsciiChars[i].unicode() ) );
	}
	
	return text;
}


void RichTextEditor::insertURL( const QString & url, const QString & text )
{
	insertHTML( QString("<a href=\"%1\">%2</a>").arg( url ).arg( text ) );
}


void RichTextEditor::insertHTML( const QString & html )
{
	// Save cursor position
	int cursorPara, cursorIndex;
	m_pEditor->getCursorPosition( & cursorPara, & cursorIndex );
	
	// replaceString is used so that the inserted text is at the cursor position.
	// it's just a random set of characters, so that the chance of them actually being
	// used is about zero.
	QString replaceString = "SXbCk2CtqJ83";
	
	m_pEditor->insert( replaceString );
	QString editorText = m_pEditor->text();
	editorText.replace( replaceString, html, (uint)0 );
	m_pEditor->setText( editorText );
	
	// Restore cursor position
	m_pEditor->setCursorPosition( cursorPara, cursorIndex );
}


void RichTextEditor::slotSetVerticalAlignment( int a )
{
	m_pEditor->setVerticalAlignment( (Q3TextEdit::VerticalAlignment)a );
}


void RichTextEditor::slotSetList( bool set )
{
	m_pEditor->setParagType( set ? Q3StyleSheetItem::DisplayListItem : Q3StyleSheetItem::DisplayBlock, Q3StyleSheetItem::ListDisc );
}


void RichTextEditor::fontChanged( const QFont & f )
{
	m_pTextBold->setChecked( f.bold() );
	m_pTextItalic->setChecked( f.italic() );
	m_pTextUnderline->setChecked( f.underline() );
}


void RichTextEditor::textColor()
{
	QColor c = m_pEditor->color();
	int ret = KColorDialog::getColor( c, this );
	if ( ret == QDialog::Accepted )
		m_pEditor->setColor( c );
}


void RichTextEditor::colorChanged( const QColor & c )
{
	QPixmap pix( 16, 16 );
	pix.fill( c );
	m_pTextColor->setIconSet( pix );
}


void RichTextEditor::alignmentChanged( int a )
{
	if ( ( a == Qt::AlignAuto ) || ( a & Qt::AlignLeft ))
		m_pTextAlignment->setIcon( KIcon("format-justify-left") );
	else if ( ( a & Qt::AlignHCenter ) )
		m_pTextAlignment->setIcon( KIcon("format-justify-center") );
	else if ( ( a & Qt::AlignRight ) )
		m_pTextAlignment->setIcon( KIcon("format-justify-right") );
	else if ( ( a & Qt::AlignJustify ) )
		m_pTextAlignment->setIcon( KIcon("format-justify-fill") );
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


void RichTextEditor::setResourcePaths( const QStringList & paths )
{
	m_pEditor->mimeSourceFactory()->setFilePath( paths );
}
//END class RichTextEditor


//BEGIN class RichTextEditorDlg
RichTextEditorDlg::RichTextEditorDlg( QWidget * parent, const QString & caption )
	: //KDialog( parent, "RichTextEditorDlg", true, caption, KDialog::Ok|KDialog::Cancel, KDialog::Ok, true )
	KDialog( parent )
{
    setName("RichTextEditorDlg");
    setModal(true);
    setCaption(caption);
    setButtons(KDialog::Ok | KDialog::Cancel);
    setDefaultButton(KDialog::Ok);
    showButtonSeparator(true);


	//QVBox * page = makeVBoxMainWidget();
    Q3VBox * page = new Q3VBox(this);
    setMainWidget(page);
	m_pEditor = new RichTextEditor( page );
}
//END class RichTextEditorDlg

#include "richtexteditor.moc"
