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
#include <qvaluevector.h>

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
class CanvasTip : public QCanvasRectangle {

public:
	CanvasTip(ItemDocument *itemDocument, QCanvas *qcanvas);
	virtual ~CanvasTip();

	void displayVI(ECNode *node, const QPoint &pos);
	void displayVI(Connector *connector, const QPoint &pos);

protected:
	virtual void draw(QPainter &p);
	void setText(const QString & text);
	bool updateVI();
	void display(const QPoint &pos);
	QString displayText(unsigned num) const;

	QValueVector<electrical_info> info;

	ItemDocument *p_itemDocument;
	QString m_text;
};

#endif