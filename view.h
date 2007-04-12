/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef VIEW_H
#define VIEW_H

#include <kstatusbar.h>
#include <kurl.h>
#include <kxmlguiclient.h>
#include <qguardedptr.h>
#include <qpixmap.h>

class DCOPObject;
class Document;
class KSqueezedTextLabel;
class KTechlab;
class View;
class ViewContainer;
class ViewIface;
class QVBoxLayout;

class ViewStatusBar : public KStatusBar
{
Q_OBJECT
public:
	ViewStatusBar( View *view );
	
	enum InfoId
	{
		SimulationState,
		LineCol,
		InsertMode,
		SelectionMode
	};
	
public slots:
	void slotModifiedStateChanged();
	void slotFileNameChanged( const KURL &url );
	void slotViewFocused( View * );
	void slotViewUnfocused();
	
protected:
	View *p_view;
	QLabel* m_modifiedLabel;
	KSqueezedTextLabel* m_fileNameLabel;
	QPixmap m_modifiedPixmap;
	QPixmap m_unmodifiedPixmap;
};

/**
@author David Saxton
*/
class View : public QWidget, public KXMLGUIClient
{
Q_OBJECT
public:
	View( Document *document, ViewContainer *viewContainer, uint viewAreaId, const char *name = 0 );
	virtual ~View();
	
	KAction * action( const QString & name ) const;
	bool isFocused() const { return b_isFocused; }
	/**
	 * Pointer to the parent document
	 */
	Document *document() const { return m_pDocument; }
	/**
	 * Returns the DCOP object from this view
	 */
	DCOPObject * dcopObject() const;
	/**
	 * Returns the dcop suffix for this view - a unique ID for the current the
	 * view within all views associated with the parent document. DCOP name
	 * will become "View#docID#viewID".
	 */
	unsigned dcopID() const { return m_dcopID; }
	/**
	 * Sets the dcop suffix. The DCOP object for this view will be renamed.
	 * @see dcopID
	 */
	void setDCOPID( unsigned id );
	/**
	 * Pointer to the ViewContainer that we're in
	 */
	ViewContainer *viewContainer() const { return p_viewContainer; }
	/**
	 * Tells the view container which contains this view to close this view,
	 * returning true if successful (i.e. not both last view and unsaved, etc)
	 */
	virtual bool closeView();
	/**
	 * Returns the unique (for the view container) view area id associated with this view
	 */
	uint viewAreaId() const { return m_viewAreaId; }
	/**
	 * Zoom in
	 */
	virtual void viewZoomIn() {};
	/**
	 * Zoom out
	 */
	virtual void viewZoomOut() {};
	virtual bool canZoomIn() const { return true; }
	virtual bool canZoomOut() const { return true; }
	/**
	 * Restore view to actual size
	 */
	virtual void actualSize() {};
	
	virtual void toggleBreakpoint() {};
	/**
	 * Called by ktechlab when it has entered its destructor to avoid calls to
	 * it (such as from the TextView destructor).
	 */
	void setKTechlabDeleted() { p_ktechlab = 0; }
	
public slots:
	/**
	 * Called when the view is to be focused (enables actions, etc)
	 */
	virtual void setFocused();
	/**
	 * Called when the view is to be unfocused (disables actions, etc)
	 */
	virtual void setUnfocused();

protected slots:
	/**
	 * Called when the user changes the configuration.
	 */
	virtual void slotUpdateConfiguration() {};
	
signals:
	void viewFocused( View *view );
	void viewUnfocused();
	
protected:
	QGuardedPtr<Document> m_pDocument;
	KTechlab * p_ktechlab;
	QGuardedPtr<ViewContainer> p_viewContainer;
	uint m_viewAreaId;
	bool b_isFocused;
	ViewStatusBar *m_statusBar;
	QVBoxLayout *m_layout;
	ViewIface * m_pViewIface;
	unsigned m_dcopID;
};

#endif
