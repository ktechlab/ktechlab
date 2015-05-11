/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include "config.h"
#include "view.h"

// #include <kate/view.h>
#include <ktexteditor/view.h>

#include <Qt/qpointer.h>
#include <Qt/qlabel.h>

class QMouseEvent;
class RegisterInfo;
class TextDocument;
class TextView;
class VariableLabel;


/**
@author David Saxton
*/
class TextView : public View
{
	Q_OBJECT
	public:
		TextView( TextDocument *textDocument, ViewContainer *viewContainer, uint viewAreaId, const char *name = 0 );
		virtual ~TextView();
		virtual bool closeView();
		/**
		 * Brings up the goto line dialog.
		 */
		bool gotoLine( const int line ) ; // { return m_view->setCursorPosition( line, 0/*m_view->cursorColumn()*/ ); }
		/**
		 * Returns a pointer to the document as a text document
		 */
		TextDocument *textDocument() const;
		void cut(); // { m_view->cut(); }
		void copy(); // { m_view->copy(); }
		void paste(); // { m_view->paste(); }
		void saveCursorPosition();
		void restoreCursorPosition();
		/**
		 * Enable code actions depending on the type of code being edited
		 */
		void initCodeActions();
		void setCursorPosition( uint line, uint col ); // { m_view->setCursorPosition( line, col ); }
		unsigned currentLine();
		unsigned currentColumn();
		void disableActions();
	
		KTextEditor::View * kateView() const { return m_view; }
	
		//KTextEditor::View::saveResult save(); // { return m_view->save(); }
		bool save();
		//KTextEditor::View::saveResult saveAs(); // { return m_view->saveAs(); }
        bool saveAs();
	
	public slots:
		/**
		 * Called when change line / toggle marks
		 */
		void slotUpdateMarksInfo();
		void slotCursorPositionChanged();
		void toggleBreakpoint();
		/**
		 * Initialize the actions appropriate for when the debugger is running
		 * or stepping
		 */
		void slotInitDebugActions();
	
	protected slots:
		void slotWordHoveredOver( const QString & word, int line, int col );
		void slotWordUnhovered();
		void gotFocus();
	
	protected:
		KTextEditor::View * m_view;
#ifndef NO_GPSIM
		VariableLabel * m_pTextViewLabel; ///< Pops up when the user hovers his mouse over a word
#endif
	
	private:
		uint m_savedCursorLine;
		uint m_savedCursorColumn;
};


/**
This class is an event filter to be installed in the kate view, and is used to
do stuff like poping up menus and telling TextView that a word is being hovered
over (used in the debugger).

@author David Saxton
*/
class TextViewEventFilter : public QObject
{
	Q_OBJECT
	public:
		TextViewEventFilter( TextView * textView );
		
		bool eventFilter( QObject * watched, QEvent * e );
		
	signals:
		/**
		 * When the user hovers the mouse for more than 700 milliseconds over a
		 * word, "hover mode" is entered. When the user presses a key, clicks
		 * mouse, etc, this mode is left. During the mode, any word that is
		 * under the mouse cursor will be emitted as hoveredOver( word ).
		 */
		void wordHoveredOver( const QString & word, int line, int col );
		/**
		 * Emitted when focus is lost, the mouse moves to a different word, etc.
		 */
		void wordUnhovered();
		
	protected slots:
		void slotNeedTextHint( int line, int col, QString & text );
		/**
		 * Called when we are not in hover mode, but the user has had his mouse
		 * in the same position for some time.
		 */
		void hoverTimeout();
		/**
		 * Called (from m_pSleepTimer) when we are in hover mode, but no word
		 * has been hovered over for some time.
		 */
		void gotoSleep();
		/**
		 * @see m_pNoWordTimer
		 */
		void slotNoWordTimeout();
		
	protected:
		enum HoverStatus
		{
			/**
			 * We are currently hovering - wordHoveredOver was emitted, and
			 * wordUnhovered hasn't been emitted yet.
			 */
			Active,
			/**
			 * A word was unhovered recently. We will go straight to PoppedUp
			 * mode if a word is hovered over again.
			 */
			Hidden,
			/**
			 * A word was not unhovered recentely. There will be a short display
			 * before going to PoppedUp mode if a word is hovered over.
			 */
			Sleeping
		};
		
		/**
		 * Starts / stops timers, emits signals, etc. See other functions for an
		 * idea of what this does.
		 */
		void updateHovering( const QString & currentWord, int line, int col );
		/**
		 * Started when the user moves his mouse over a word, and we are in
		 * Sleeping mode. Reset when the user moves his mouse, etc.
		 */
		QTimer * m_pHoverTimer;
		/**
		 * Started when a word is unhovered. When this timeouts, we will go to
		 * Sleeping mode.
		 */
		QTimer * m_pSleepTimer;
		/**
		 * Activated by the user moving the mouse. Reset by a call to
		 * slotNeedTextHint. This timer is needed as KateViewInternal doesn't
		 * bother updating us if the mouse cursor isn't over text.
		 */
		QTimer * m_pNoWordTimer;
		
		TextView * m_pTextView;
		QString m_lastWord;
		int m_lastLine;
		int m_lastCol;
		HoverStatus m_hoverStatus;
};

#endif
