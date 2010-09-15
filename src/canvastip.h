//
// C++ Interface: canvastip
//
// Description:
//
//
// Author: David Saxton, Alan Grimes <agrimes@speakeasy.net>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CANVASTIP_H
#define CANVASTIP_H

#include <canvas.h>
#include <q3valuevector.h>

class ItemDocument;
class ECNode;
class Connector;

typedef struct { 
	double I; 
	double V;
	int id;
} electrical_info;

/**
@author David Saxton
*/
class CanvasTip : public Q3CanvasRectangle {

public:
	CanvasTip(ItemDocument *itemDocument, Q3Canvas *qcanvas);
	virtual ~CanvasTip();

	void displayVI(ECNode *node, const QPoint &pos);
	void displayVI(Connector *connector, const QPoint &pos);

protected:
	virtual void draw(QPainter &p);
	void setText(const QString & text);
	bool updateVI();
	void display(const QPoint &pos);
	QString displayText(unsigned num) const;

	Q3ValueVector<electrical_info> info;

	ItemDocument *p_itemDocument;
	QString m_text;
};

#endif