/***************************************************************************
 *   Copyright (C) 2003-2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DOUBLESPINBOX_H
#define DOUBLESPINBOX_H

#include <QDoubleSpinBox>

/**
Where appropriate, function names with value in them should
be prefixed with "real" - e.g. realValue() - to get the value stored in the
spin box plus the SI magnitude symbol it is showing

@author David Saxton
*/
class DoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    DoubleSpinBox(double lower, double upper, double minAbs, double value, const QString &unit, QWidget *parent = nullptr);
    DoubleSpinBox(QWidget *parent = nullptr);
    ~DoubleSpinBox() override;

    /**
     * The minimum value is the lowest number that the user can enter.
     */
    // 		double minValue() const { return m_minValue; }
    /**
     * @see minValue
     */
    // 		void setMinValue( double minValue ) { m_minValue = minValue; }
    /**
     * The minimum value is the lowest number that the user can enter.
     */
    // 		void setMinValue( int minValue ) { m_minValue = minValue; }
    /**
     * The maximum value is the highest number that the user can enter.
     */
    // 		double maxValue() const { return m_maxValue; }
    /**
     * @see maxValue
     */
    // 		void setMaxValue( double maxValue ) { m_maxValue = maxValue; }
    /**
     * @see maxValue
     */
    // 		void setMaxValue( int maxValue ) { m_maxValue = maxValue; }
    /**
     * The minimum absolute value is the smallest value that the user can
     * enter before the value is considered 0.
     */
    void setMinAbsValue(double minAbsValue)
    {
        m_minAbsValue = minAbsValue;
    }
    /**
     * The actual value that the user has entered - e.g. if the spinbox
     * displays "100 kF", then the value returned will be 1e5.
     */
    // 		double value();
    /**
     * Sets the unit used, e.g. "F"
     */
    // 		void setUnit( const QString & unit );

    QValidator::State validate(QString &text, int &pos) const override;

public Q_SLOTS:
    // 		virtual void stepUp(); // QDoubleSpinBox has these
    // 		virtual void stepDown();
    /**
     * Set the value to be displayed - e.g. if value is 1e5, then the
     * spinbox might display "100 kF".
     */
    // 		void setValue( double value );

Q_SIGNALS:
    /**
     * This value is emitted whenever the value of the spinbox changes.
     */
    // 		void valueChanged( double value ); // exists in QDoubleSpinBox

protected Q_SLOTS:
    /**
     * Checks if the value has changed - and if so, emits a valueChanged
     * signal.
     */
    // 		void checkIfChanged(double value);
    /**
     * Sets the suffix from m_queuedSuffix. Called from QTimer::singleShot
     * to avoid strange recursion problems.
     */
    // 		void setQueuedSuffix();

protected:
    /**
     * make Qt enable the up/down step arrows
     */
    StepEnabled stepEnabled() const override;
    /**
     * Change the value of the spin box, because of user interaction
     */
    void stepBy(int steps) override;

    double getNextUpStepValue(double in);
    double getNextDownStepValue(double in);

    /**
     * Updates the suffix using m_unit and value.
     */
    void updateSuffix(double value);
    /**
     * Returns the multiplication number from what is displayed
     * in the box, e.g. "10 kV" will return "1000" due to the letter "k" presence
     */
    // 		double getMult();
    /**
     * Returns the number currently displayed in the spin box.
     */
    // 		double getDisplayedNumber( bool * ok );
    /**
     *  Overloaded the method in QSpinxBox to allow SI prefixes to be entered
     */
    // 		virtual int mapTextToValue( bool * ok );
    double valueFromText(const QString &text) const override;
    /**
     *  Overloaded the method in QSpinxBox to allow SI prefixes to be entered
     */
    // 		virtual QString mapValueToText( int v );
    QString textFromValue(double value) const override;
    /**
     * Returns value rounded off to one significant figure.
     */
    static double roundToOneSF(double value);

    void init();

    // 		QString m_queuedSuffix; ///< Used
    QString m_unit;
    // 		double m_minValue;
    // 		double m_maxValue;
    double m_minAbsValue;
    double m_lastEmittedValue;
};

#endif
