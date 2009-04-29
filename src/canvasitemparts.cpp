/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "canvasitemparts.h"
#include "cells.h"
#include "cnitem.h"
#include "icndocument.h"

#include <qpainter.h>

//BEGIN Class GuiPart
GuiPart::GuiPart(CNItem *parent, const QRect &r, QCanvas *canvas)
		: QObject(parent),
		QCanvasRectangle(r, canvas),
		m_angleDegrees(0),
		p_parent(parent),
		b_pointsAdded(false),
		m_originalRect(r) {
	connect(parent, SIGNAL(movedBy(double, double)), this, SLOT(slotMoveBy(double, double)));
	setZ(parent->z() + 0.5);
}

GuiPart::~GuiPart() {
	hide();
}

void GuiPart::setAngleDegrees(int angleDegrees) {
	m_angleDegrees = angleDegrees;
	posChanged();

	if (canvas())
		canvas()->setChanged(boundingRect());
}

void GuiPart::setGuiPartSize(int width, int height) {
	updateConnectorPoints(false);
	setSize(width, height);
	posChanged();
}

void GuiPart::initPainter(QPainter &p) {
	if ((m_angleDegrees % 180) == 0)
		return;

	p.translate(int(x() + (width() / 2)), int(y() + (height() / 2)));
	p.rotate(m_angleDegrees);
	p.translate(-int(x() + (width() / 2)), -int(y() + (height() / 2)));
}

void GuiPart::deinitPainter(QPainter &p) {
	if ((m_angleDegrees % 180) == 0)
		return;

	p.translate(int(x() + (width() / 2)), int(y() + (height() / 2)));
	p.rotate(-m_angleDegrees);
	p.translate(-int(x() + (width() / 2)), -int(y() + (height() / 2)));
}

void GuiPart::slotMoveBy(double dx, double dy) {
	if (dx == 0 && dy == 0)
		return;

	moveBy(dx, dy);
	posChanged();
}

void GuiPart::updateConnectorPoints(bool add) {
	ICNDocument *icnd = dynamic_cast<ICNDocument*>(p_parent->itemDocument());

	if (!icnd)
		return;

	Cells *cells = icnd->cells();

	if (!cells)
		return;

	if (!isVisible())
		add = false;

	if (add == b_pointsAdded)
		return;

	b_pointsAdded = add;

	int mult = add ? 1 : -1;

	int sx = roundDown(x(), 8);
	int sy = roundDown(y(), 8);

	int ex = roundDown(x() + width(), 8);
	int ey = roundDown(y() + height(), 8);

	for (int x = sx; x <= ex; ++x) {
		for (int y = sy; y <= ey; ++y) {
			if (cells->haveCell(x, y))
				cells->cell(x, y).addCIPenalty(mult * ICNDocument::hs_item / 2);
		}
	}
}

QRect GuiPart::drawRect() {
	QRect dr = rect();

	if (m_angleDegrees % 180 != 0) {
		QWMatrix m;
		m.translate(int(x() + (width() / 2)), int(y() + (height() / 2)));

		if ((m_angleDegrees % 180) != 0)
			m.rotate(-m_angleDegrees);

		m.translate(-int(x() + (width() / 2)), -int(y() + (height() / 2)));

		dr = m.mapRect(dr);
	}

	return dr;
}
//END Class GuiPart

//BEGIN Class Text
Text::Text(const QString &text, CNItem *parent, const QRect & r, QCanvas * canvas, int flags)
		: GuiPart(parent, r, canvas) {
	m_flags = flags;
	setText(text);
}

Text::~Text() {
}

bool Text::setText(const QString & text) {
	if (m_text == text)
		return false;

	updateConnectorPoints(false);

	m_text = text;

	return true;
}

void Text::setFlags(int flags) {
	updateConnectorPoints(false);
	m_flags = flags;
}

void Text::drawShape(QPainter & p) {
	initPainter(p);
	p.setFont(p_parent->font());
	p.drawText(drawRect(), m_flags, m_text);
	deinitPainter(p);
}

QRect Text::recommendedRect() const {
	return QFontMetrics(p_parent->font()).boundingRect(m_originalRect.x(), m_originalRect.y(), m_originalRect.width(), m_originalRect.height(), m_flags, m_text);
}
//END Class Text

//BEGIN Class Widget
Widget::Widget(const QString & id, CNItem * parent, const QRect & r, QCanvas * canvas)
		: GuiPart(parent, r, canvas) {
	m_id = id;
	show();
}

Widget::~Widget() {
}

void Widget::setEnabled(bool enabled) {
	widget()->setEnabled(enabled);
}

void Widget::posChanged() {
	// Swap around the width / height if we are rotated at a non-half way around
	if (m_angleDegrees % 90 != 0)
		widget()->setFixedSize(QSize(height(), width()));
	else	widget()->setFixedSize(size());

	widget()->move(int(x()), int(y()));
}

void Widget::drawShape(QPainter &p) {
// 	initPainter(p);
	p.drawPixmap(int(x()), int(y()), QPixmap::grabWidget(widget()));
// 	deinitPainter(p);
}
//END Class Widget

//BEGIN Class ToolButton
ToolButton::ToolButton(QWidget *parent)
		: QToolButton(parent) {
	m_angleDegrees = 0;

	if (QFontInfo(m_font).pixelSize() > 11)   // It has to be > 11, not > 12, as (I think) pixelSize() rounds off the actual size
		m_font.setPixelSize(12);
}

void ToolButton::drawButtonLabel(QPainter * p) {
	if (m_angleDegrees % 180 == 0 || text().isEmpty()) {
		QToolButton::drawButtonLabel(p);
		return;
	}

	double dx = size().width() / 2;
	double dy = size().height() / 2;

	p->translate(dx, dy);
	p->rotate(m_angleDegrees);
	p->translate(-dx, -dy);
	p->translate(-dy + dx, 0);

	int m = width() > height() ? width() : height();

	p->setPen(Qt::black);
	p->drawText(isDown() ? 1 : 0, isDown() ? 1 : 0, m, m, Qt::AlignVCenter | Qt::AlignHCenter, text());
	p->translate(dy - dx, 0);
	p->translate(dx, dy);
	p->rotate(-m_angleDegrees);
	p->translate(-dx, -dy);
}
//END Class ToolButton

//BEGIN Class Button
Button::Button(const QString & id, CNItem * parent, bool isToggle, const QRect & r, QCanvas * canvas)
		: Widget(id, parent, r, canvas) {
	b_isToggle = isToggle;
	m_button = new ToolButton(0l);
	m_button->setUsesTextLabel(false);
	m_button->setToggleButton(b_isToggle);
	connect(m_button, SIGNAL(pressed()), this, SLOT(slotStateChanged()));
	connect(m_button, SIGNAL(released()), this, SLOT(slotStateChanged()));
	posChanged();
}

Button::~Button() {
	delete m_button;
}

void Button::setToggle(bool toggle) {
	if (b_isToggle == toggle)
		return;

	if (b_isToggle) {
		// We must first untoggle it, else it'll be forever stuck...
		setState(false);
	}

	b_isToggle = toggle;
	m_button->setToggleButton(b_isToggle);
}

void Button::posChanged() {
	Widget::posChanged();
	m_button->setAngleDegrees(m_angleDegrees);
}

void Button::slotStateChanged() {
	parent()->buttonStateChanged(id(), m_button->isDown() || m_button->isOn());
}

QWidget* Button::widget() const {
	return m_button;
}

void Button::setPixmap(const QPixmap &p) {
	m_button->setPixmap(p);
}

void Button::setState(bool state) {
	if (this->state() == state)
		return;

	if (isToggle())
		m_button->setOn(state);
	else	m_button->setDown(state);

	slotStateChanged();
}

bool Button::state() const {
	if (isToggle())
		return m_button->state();
	else	return m_button->isDown();
}

QRect Button::recommendedRect() const {
	QSize sizeHint = m_button->sizeHint();

	if (sizeHint.width() < m_originalRect.width())
		sizeHint.setWidth(m_originalRect.width());

	// Hmm...for now, lets just keep the recomended rect the same height as the original rect
	sizeHint.setHeight(m_originalRect.height());

	int hdw = (sizeHint.width() - m_originalRect.width()) / 2;
	int hdh = (sizeHint.height() - m_originalRect.height()) / 2;

	return QRect(m_originalRect.x() - hdw, m_originalRect.y() - hdh, sizeHint.width(), sizeHint.height());
}

void Button::setText(const QString &text) {
	if (m_button->text() == text)
		return;

	updateConnectorPoints(false);

	m_button->setUsesTextLabel(true);
	m_button->setText(text);
	m_button->setTextLabel(text);

	canvas()->setChanged(rect());

	p_parent->updateAttachedPositioning();
}

void Button::mousePressEvent(QMouseEvent *e) {
	if (!m_button->isEnabled())
		return;

	QMouseEvent event(QEvent::MouseButtonPress, e->pos() - QPoint(int(x()), int(y())), e->button(), e->state());

	m_button->mousePressEvent(&event);

	if (event.isAccepted())
		e->accept();

	canvas()->setChanged(rect());
}

void Button::mouseReleaseEvent(QMouseEvent *e) {
	QMouseEvent event(QEvent::MouseButtonRelease, e->pos() - QPoint(int(x()), int(y())), e->button(), e->state());
	m_button->mouseReleaseEvent(&event);

	if (event.isAccepted())
		e->accept();

	canvas()->setChanged(rect());
}

void Button::enterEvent() {
	m_button->enterEvent();
// 	m_button->setFocus();
// 	bool hasFocus = m_button->hasFocus();
// 	m_button->setAutoRaise(true);
// 	m_button->setOn(true);
}

void Button::leaveEvent() {
	m_button->leaveEvent();
// 	m_button->clearFocus();
// 	bool hasFocus = m_button->hasFocus();
// 	m_button->setAutoRaise(false);
// 	m_button->setOn(false);
}
//END Class Button

//BEGIN Class SliderWidget
SliderWidget::SliderWidget(QWidget *parent)
		: QSlider(parent) {
	setWFlags(WNoAutoErase | WRepaintNoErase);
}
//END Class SliderWidget

//BEGIN Class Slider
Slider::Slider(const QString & id, CNItem * parent, const QRect & r, QCanvas * canvas)
		: Widget(id, parent, r, canvas) {
	m_orientation = Qt::Vertical;
	m_bSliderInverted = false;

	m_slider = new SliderWidget(0l);
	m_slider->setPaletteBackgroundColor(Qt::white);
	m_slider->setPaletteForegroundColor(Qt::white);
	m_slider->setEraseColor(Qt::white);
	m_slider->setBackgroundMode(Qt::NoBackground);
	connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged(int)));
	posChanged();
}

Slider::~Slider() {
	delete m_slider;
}

QWidget* Slider::widget() const {
	return m_slider;
}

int Slider::value() const {
	if (m_bSliderInverted) {
		// Return the value as if the slider handle was reflected along through
		// the center of the slide.
		return m_slider->maxValue() + m_slider->minValue() - m_slider->value();
	} else	return m_slider->value();
}

void Slider::setValue(int value) {
	if (m_bSliderInverted) {
		value = m_slider->maxValue() + m_slider->minValue() - value;
	}

	m_slider->setValue(value);

	if (canvas())
		canvas()->setChanged(rect());
}

void Slider::mousePressEvent(QMouseEvent *e) {
	QMouseEvent event(QEvent::MouseButtonPress, e->pos() - QPoint(int(x()), int(y())), e->button(), e->state());
	m_slider->mousePressEvent(&event);

	if (event.isAccepted())
		e->accept();

	canvas()->setChanged(rect());
}

void Slider::mouseReleaseEvent(QMouseEvent *e) {
	QMouseEvent event(QEvent::MouseButtonRelease, e->pos() - QPoint(int(x()), int(y())), e->button(), e->state());
	m_slider->mouseReleaseEvent(&event);

	if (event.isAccepted())
		e->accept();

	canvas()->setChanged(rect());
}

void Slider::mouseDoubleClickEvent(QMouseEvent *e) {
	QMouseEvent event(QEvent::MouseButtonDblClick, e->pos() - QPoint(int(x()), int(y())), e->button(), e->state());
	m_slider->mouseDoubleClickEvent(&event);

	if (event.isAccepted())
		e->accept();

	canvas()->setChanged(rect());
}

void Slider::mouseMoveEvent(QMouseEvent *e) {
	QMouseEvent event(QEvent::MouseMove, e->pos() - QPoint(int(x()), int(y())), e->button(), e->state());
	m_slider->mouseMoveEvent(&event);

	if (event.isAccepted())
		e->accept();
}

void Slider::wheelEvent(QWheelEvent *e) {
	QWheelEvent event(e->pos() - QPoint(int(x()), int(y())), e->delta(), e->state(), e->orientation());
	m_slider->wheelEvent(&event);

	if (event.isAccepted())
		e->accept();

	canvas()->setChanged(rect());
}

void Slider::enterEvent() {
	m_slider->enterEvent();
}

void Slider::leaveEvent() {
	m_slider->leaveEvent();
}

void Slider::slotValueChanged(int value) {
	if (parent()->itemDocument())
		parent()->itemDocument()->setModified(true);

	// Note that we do not use value as we want to take into account rotation
	(void)value;

	parent()->sliderValueChanged(id(), this->value());

	if (canvas())
		canvas()->setChanged(rect());
}

void Slider::setOrientation(Qt::Orientation o) {
	m_orientation = o;
	posChanged();
}

void Slider::posChanged() {
	Widget::posChanged();

	bool nowInverted;

	if (m_orientation == Qt::Vertical) {
		nowInverted = angleDegrees() == 90 || angleDegrees() == 180;
		m_slider->setOrientation((m_angleDegrees % 180 == 0) ? Qt::Vertical : Qt::Horizontal);
	} else {
		nowInverted = angleDegrees() == 0 || angleDegrees() == 90;
		m_slider->setOrientation((m_angleDegrees % 180 == 0) ? Qt::Horizontal : Qt::Vertical);
	}

	if (nowInverted != m_bSliderInverted) {
		int prevValue = value();
		m_bSliderInverted = nowInverted;
		setValue(prevValue);
	}
}

//END Class Slider

#include "canvasitemparts.moc"
