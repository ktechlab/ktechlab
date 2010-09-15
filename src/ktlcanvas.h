//
// C++ Interface: ktlcanvas
//
// Description: 
//
//
// Author: David Saxton, Alan Grimes <agrimes@speakeasy.net>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef KTLCANVAS_H
#define KTLCANVAS_H

#include <canvas.h>

class ItemDocument;

/**
@author David Saxton
*/
class Canvas : public Q3Canvas {
	Q_OBJECT

public:
	Canvas(ItemDocument *itemDocument, const char * name = 0);

	/**
	 * Sets a message to be displayed on the canvas for a brief period of
	 * time. If this is called with an empty message, then any existing
	 * message will be removed.
	 */
	void setMessage(const QString & message);
	virtual void update();
	virtual void resize(const QRect & size);

signals:
	/**
	 * Emitted when the canvas rectangle-size changes.
	 */
	void resized(const QRect & oldSize, const QRect & newSize);

public slots:
	void slotSetAllChanged() {
		setAllChanged();
	}

protected:
	virtual void drawBackground(QPainter & painter, const QRect & clip);
	virtual void drawForeground(QPainter & painter, const QRect & clip);

	ItemDocument *p_itemDocument;

	QString m_message;
	QTimer *m_pMessageTimeout;
};

#endif
