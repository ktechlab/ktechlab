/***************************************************************************
 *   Copyright (C) 2006 by David Saxton - david@bluehaze.org               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "richtexteditor.h"

#include <kactionclasses.h>
#include <kcolordialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <ktextedit.h>
#include <ktoolbar.h>

#include <qfont.h>
#include <qlayout.h>
#include <qmime.h>
#include <qregexp.h>
#include <qvbox.h>


//BEGIN class RichTextEditor
RichTextEditor::RichTextEditor(QWidget *parent, const char *name)
	: QWidget(parent, name)
{
	QVBoxLayout * layout = new QVBoxLayout( this, 0, 6 );
	m_pEditor = new KTextEdit( this, "RichTextEdit" );
	layout->addWidget( m_pEditor );
	
	m_pEditor->setTextFormat( QTextEdit::RichText );
	
	connect( m_pEditor, SIGNAL( textChanged() ), SIGNAL( textChanged() ) );
	connect( m_pEditor, SIGNAL( currentFontChanged( const QFont & ) ), this, SLOT( fontChanged( const QFont & ) ) );
	connect( m_pEditor, SIGNAL( currentColorChanged( const QColor & ) ), this, SLOT( colorChanged( const QColor & ) ) );
	connect( m_pEditor, SIGNAL( currentAlignmentChanged( int ) ), this, SLOT( alignmentChanged( int ) ) );
	connect( m_pEditor, SIGNAL( currentVerticalAlignmentChanged( VerticalAlignment ) ), this, SLOT(verticalAlignmentChanged()) );
	
	KToolBar * tools = new KToolBar( this, "RichTextEditorToops" );
	layout->add( tools );
	KActionCollection * ac = new KActionCollection( m_pEditor );
	
	
	m_pTextBold = new KToggleAction( i18n("Bold"), "text_bold", CTRL + Key_B, 0, 0, ac, "format_bold" );
	connect( m_pTextBold, SIGNAL(toggled(bool)), m_pEditor, SLOT(setBold(bool)) );
	m_pTextBold->plug( tools );

	m_pTextItalic = new KToggleAction( i18n("Italic"), "text_italic", CTRL + Key_I, 0, 0, ac, "format_italic" );
	connect( m_pTextItalic, SIGNAL(toggled(bool)), m_pEditor, SLOT(setItalic(bool)) );
	m_pTextItalic->plug( tools );

	m_pTextUnderline = new KToggleAction( i18n("Underline"), "text_under", CTRL + Key_U, 0, 0, ac, "format_underline" );
	connect( m_pTextUnderline, SIGNAL(toggled(bool)), m_pEditor, SLOT(setUnderline(bool)) );
	m_pTextUnderline->plug( tools );
	
	m_pTextList = new KToggleAction( i18n("List"), "unsorted_list", CTRL + Key_L, 0, 0, ac, "format_list" );
	connect( m_pTextList, SIGNAL(toggled(bool)), SLOT(slotSetList(bool)) );
	m_pTextList->plug( tools );
	
	
	//BEGIN Text horizontal-alignment actions
	m_pTextAlignment = new KToolBarPopupAction( i18n("Text Alignment"), "text_left", 0, 0, 0, ac, "text_alignment" );
	m_pTextAlignment->plug( tools );
	m_pTextAlignment->setDelayed(false);
	
	KPopupMenu * m = m_pTextAlignment->popupMenu();
	m->insertTitle( i18n("Text Alignment") );
	m->setCheckable( true );
	
	m->insertItem( KGlobal::iconLoader()->loadIcon( "text_left",	KIcon::Small ), i18n("Align Left"),		AlignLeft );
	m->insertItem( KGlobal::iconLoader()->loadIcon( "text_center",	KIcon::Small ), i18n("Align Center"),	AlignHCenter );
	m->insertItem( KGlobal::iconLoader()->loadIcon( "text_right",	KIcon::Small ), i18n("Align Right"),	AlignRight );
	m->insertItem( KGlobal::iconLoader()->loadIcon( "text_block",	KIcon::Small ), i18n("Align Block"),	AlignJustify );
	connect( m, SIGNAL(activated(int)), m_pEditor, SLOT(setAlignment(int)) );
	//END Text horizontal-alignment actions
	
	
	//BEGIN Text vertical-alignment actions
	m_pTextVerticalAlignment = new KToolBarPopupAction( i18n("Text Vertical Alignment"), "text", 0, 0, 0, ac, "text_vertical_alignment" );
	m_pTextVerticalAlignment->plug( tools );
	m_pTextVerticalAlignment->setDelayed(false);
	
	m = m_pTextVerticalAlignment->popupMenu();
	m->insertTitle( i18n("Text Vertical Alignment") );
	m->setCheckable( true );
	
	m->insertItem( KGlobal::iconLoader()->loadIcon( "text_super",	KIcon::Small ), i18n("Superscript"),	QTextEdit::AlignSuperScript );
	m->insertItem(																	i18n("Normal"),			QTextEdit::AlignNormal );
	m->insertItem( KGlobal::iconLoader()->loadIcon( "text_sub",		KIcon::Small ), i18n("Subscript"),		QTextEdit::AlignSubScript );
	connect( m, SIGNAL(activated(int)), this, SLOT(slotSetVerticalAlignment(int)) );
	//END Text vertical-alignment actions
	
	
	QPixmap pm( 16, 16 );
	pm.fill( black );
	m_pTextColor = new KAction( i18n("Text Color..."), pm, 0, this, SLOT(textColor()), ac, "format_color" );
	m_pTextColor->plug( tools );
	
	
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
	if ( !QStyleSheet::mightBeRichText( text ) )
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
	m_pEditor->setVerticalAlignment( (QTextEdit::VerticalAlignment)a );
}


void RichTextEditor::slotSetList( bool set )
{
	m_pEditor->setParagType( set ? QStyleSheetItem::DisplayListItem : QStyleSheetItem::DisplayBlock, QStyleSheetItem::ListDisc );
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
	if ( ( a == AlignAuto ) || ( a & AlignLeft ))
		m_pTextAlignment->setIcon( "text_left" );
	else if ( ( a & AlignHCenter ) )
		m_pTextAlignment->setIcon( "text_center" );
	else if ( ( a & AlignRight ) )
		m_pTextAlignment->setIcon( "text_right" );
	else if ( ( a & AlignJustify ) )
		m_pTextAlignment->setIcon( "text_block" );
}


void RichTextEditor::verticalAlignmentChanged()
{
// 	QTextEdit::VerticalAlignment a =
// 	if ( a == KTextEdit::AlignNormal )
// 		m_pTextVerticalAlignment->setIcon( "text" );
// 	else if ( a == KTextEdit::AlignSuperScript )
// 		m_pTextVerticalAlignment->setIcon( "text_super" );
// 	else if ( a == KTextEdit::AlignSubScript )
// 		m_pTextVerticalAlignment->setIcon( "text_sub" );
}


void RichTextEditor::setResourcePaths( const QStringList & paths )
{
	m_pEditor->mimeSourceFactory()->setFilePath( paths );
}
//END class RichTextEditor


//BEGIN class RichTextEditorDlg
RichTextEditorDlg::RichTextEditorDlg( QWidget * parent, const QString & caption )
	: KDialogBase( parent, "RichTextEditorDlg", true, caption, KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true )
{
	QVBox * page = makeVBoxMainWidget();
	m_pEditor = new RichTextEditor( page );
}
//END class RichTextEditorDlg

#include "richtexteditor.moc"
