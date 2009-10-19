//
// C++ Implementation: ktlcanvas
//
// Description:
//
//
// Author: David Saxton, Alan Grimes <agrimes@speakeasy.net>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//

#include <qapplication.h>
#include <canvas.h>
#include <qtimer.h>
#include <qsimplerichtext.h>

#include "ktlcanvas.h"
#include "itemdocument.h"
#include "view.h"


//BEGIN class Canvas
Canvas::Canvas(ItemDocument *itemDocument, const char *name)
		: QCanvas(itemDocument, name) {
	p_itemDocument = itemDocument;
	m_pMessageTimeout = new QTimer(this);
	connect(m_pMessageTimeout, SIGNAL(timeout()), this, SLOT(slotSetAllChanged()));
}

void Canvas::resize(const QRect & size) {
	if (rect() == size)
		return;

	QRect oldSize = rect();
	QCanvas::resize(size);

	emit resized(oldSize, size);
}

void Canvas::setMessage(const QString & message) {
	m_message = message;

	if (message.isEmpty())
		m_pMessageTimeout->stop();
	else	m_pMessageTimeout->start(2000, true);

	setAllChanged();
}

void Canvas::drawBackground(QPainter &p, const QRect & clip) {
	QCanvas::drawBackground(p, clip);
#if 0
	const int scx = (int)((clip.left() - 4) / 8);
	const int ecx = (int)((clip.right() + 4) / 8);
	const int scy = (int)((clip.top() - 4) / 8);
	const int ecy = (int)((clip.bottom() + 4) / 8);

	ICNDocument * icnd = dynamic_cast<ICNDocument*>(p_itemDocument);

	if (!icnd)
		return;

	Cells * c = icnd->cells();

	if (!c->haveCell(scx, scy) || !c->haveCell(ecx, ecy))
		return;

	for (int x = scx; x <= ecx; x++) {
		for (int y = scy; y <= ecy; y++) {
			const double score = c->cell(x, y).CIpenalty + c->cell(x, y).Cpenalty;
			int value = (int)std::log(score) * 20;

			if (value > 255)
				value = 255;
			else if (value < 0)
				value = 0;

			p.setBrush(QColor(255, (255 - value), (255 - value)));

			p.setPen(Qt::NoPen);

			p.drawRect((x*8), (y*8), 8, 8);
		}
	}

#endif
}

void Canvas::drawForeground(QPainter &p, const QRect & clip) {
	QCanvas::drawForeground(p, clip);

	if (!m_pMessageTimeout->isActive())
		return;

	// Following code stolen and adapted from amarok/src/playlist.cpp :)

	// Find out width of smallest view
	QSize minSize;

	const ViewList viewList = p_itemDocument->viewList();

	ViewList::const_iterator end = viewList.end();
	View *firstView = 0;
	for (ViewList::const_iterator it = viewList.begin(); it != end; ++it) {
		if (!*it) continue;

		if (!firstView) {
			firstView = *it;
			minSize = (*it)->size();
		} else	minSize = minSize.boundedTo((*it)->size());
	}

	if (!firstView) return;

	QSimpleRichText *t = new QSimpleRichText(m_message, QApplication::font());

	const int w = t->width();
	const int h = t->height();
	const int x = rect().left() + 15;
	const int y = rect().top() + 15;
	const int b = 10; // text padding

	if (w + 2 * b >= minSize.width() || h + 2 * b >= minSize.height()) {
		delete t;
		return;
	}

	p.setBrush(firstView->colorGroup().background());

	p.drawRoundRect(x, y,
			w + 2 * b,
			h + 2 * b,
			(8 * 200) / (w + 2 * b),
			(8 * 200) / (h + 2 * b));

	t->draw(&p, x + b, y + b, QRect(), firstView->colorGroup());
	delete t;
}

void Canvas::update() {
	p_itemDocument->update();
	QCanvas::update();
}
//END class Canvas

