/***************************************************************************
 *   Copyright (C) 2006 by William Hillerby - william.hillerby@ntlworld.com*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef LEDBARGRAPHDISPLAY_H
#define LEDBARGRAPHDISPLAY_H

#include "dipcomponent.h"
#include "diode.h"

#include <qstringlist.h>

/**
@author William Hillerby
@short Simulates an LED Bar Graph Display
TODO: refactor this so that the same basic code can be used in LED and matrixDisplay too. 
caveat: matrix display requires a slowed response time to function properly. We want to get the LED code right once and forever. 
*/
const unsigned int max_LED_rows = 24;
class LEDPart {
public:
	LEDPart(Component *pParent, const QString& strPNode, const QString& strNNode);
	~LEDPart();

	void setDiodeSettings(const DiodeSettings& ds);
	void setColor(const QColor &color);
	void step();
	void draw(QPainter &p, int x, int y, int w, int h);

private:
	Component *m_pParent;
	Diode m_pDiode;
	DiodeSettings ds;
	QString m_strPNode, m_strNNode;

	float r, g, b;
	double lastUpdatePeriod;
	double avg_brightness;
	uint last_brightness;
};

class LEDBarGraphDisplay : public DIPComponent {
public:
	LEDBarGraphDisplay(ICNDocument* icnDocument, bool newItem, const QString& id = 0);
	~LEDBarGraphDisplay();

	static Item* construct(ItemDocument *itemDocument, bool newItem, const char *id);
	static LibraryItem *libraryItem();

private:
	void initPins();
	void dataChanged();

	virtual void stepNonLogic();
	virtual bool doesStepNonLogic() const {
		return true;
	}

	virtual void drawShape(QPainter &p);

	LEDPart* m_LEDParts[max_LED_rows];
	unsigned int m_numRows;
};

#endif
