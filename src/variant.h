/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef VARIANT_H
#define VARIANT_H

#include <QObject>
#include <QStringList>
#include <QVariant>

#include "filefilters.h"

/// \todo Replace "Variant" with "Property"
class Variant;
typedef Variant Property;

class QColor;
class QString;

typedef QMap<QString, QString> QStringMap;

/**
For information:
QVariant::type() returns an enum for the current data type
contained. e.g. returns QVariant::Color or QVariant::Rect
@author Daniel Clarke
@author David Saxton
*/
class Variant : public QObject
{
    Q_OBJECT
public:
    class Type
    {
    public:
        enum Value {
            None,
            Int,          // Integer
            Raw,          // QByteArray
            Double,       // Real number
            String,       // Editable string
            Multiline,    // String that may contain linebreaks
            RichText,     // HTML formatted text
            Select,       // Selection of strings
            Combo,        // Editable combination of strings
            FileName,     // Filename on local filesystem
            Color,        // Color
            Bool,         // Boolean
            VarName,      // Variable name
            Port,         // Port name
            Pin,          // Pin name
            PenStyle,     // Pen Style
            PenCapStyle,  // Pen Cap Style
            SevenSegment, // Pin Map for Seven Segment Display
            KeyPad        // Pin Map for Keypad
        };
    };

    Variant(const QString &id, Type::Value type);
    ~Variant() override;

    QString id() const
    {
        return m_id;
    }

    /**
     * Returns the type of Variant (see Variant::Type::Value)
     */
    Variant::Type::Value type() const
    {
        return m_type;
    }
    /**
     * Sets the variant type
     */
    void setType(Type::Value type);
    /**
     * Returns the filters used for file dialogs (if this is of type Type::FileName)
     */
    FileFilters fileFilters() const
    {
        return m_fileFilters;
    }
    void setFileFilters(const FileFilters &fileFilters)
    {
        m_fileFilters = fileFilters;
    }
    /**
     * The selection of colours to be used in the combo box - e.g.
     * ColorCombo::LED.
     * @see ColorCombo::ColorScheme
     */
    int colorScheme() const
    {
        return m_colorScheme;
    }
    void setColorScheme(int colorScheme)
    {
        m_colorScheme = colorScheme;
    }
    /**
     * This function is for convenience; it sets both the toolbar and editor
     * caption.
     */
    void setCaption(const QString &caption)
    {
        setToolbarCaption(caption);
        setEditorCaption(caption);
    }
    /**
     * This text is displayed to the left of the entry widget in the toolbar
     */
    QString toolbarCaption() const
    {
        return m_toolbarCaption;
    }
    void setToolbarCaption(const QString &caption)
    {
        m_toolbarCaption = caption;
    }
    /**
     * This text is displayed to the left of the entry widget in the item editor
     */
    QString editorCaption() const
    {
        return m_editorCaption;
    }
    void setEditorCaption(const QString &caption)
    {
        m_editorCaption = caption;
    }
    /**
     * Unit of number, (e.g. V (volts) / F (farads))
     */
    QString unit() const
    {
        return m_unit;
    }
    void setUnit(const QString &unit)
    {
        m_unit = unit;
    }
    /**
     * The smallest (as in negative, not absoluteness) value that the user can
     * set this to.
     */
    double minValue() const
    {
        return m_minValue;
    }
    void setMinValue(double value);
    /**
     * The largest (as in positive, not absoluteness) value that the user can
     * set this to.
     */
    double maxValue() const
    {
        return m_maxValue;
    }
    void setMaxValue(double value);
    /**
     * The smallest absolute value that the user can set this to, before the
     * value is considered zero.
     */
    double minAbsValue() const
    {
        return m_minAbsValue;
    }
    void setMinAbsValue(double val);
    QVariant defaultValue() const
    {
        return m_defaultValue;
    }
    /**
     * If this data is marked as advanced, it will only display in the item
     * editor (and not in the toolbar)
     */
    void setAdvanced(bool advanced)
    {
        m_bAdvanced = advanced;
    }
    bool isAdvanced() const
    {
        return m_bAdvanced;
    }
    /**
     * If this data is marked as hidden, it will not be editable from anywhere
     * in the user interface
     */
    void setHidden(bool hidden)
    {
        m_bHidden = hidden;
    }
    bool isHidden() const
    {
        return m_bHidden;
    }
    /**
     * Returns the best possible attempt at representing the data in a string
     * for display. Used by the properties list view.
     */
    QString displayString() const;
    /**
     * The list of values that the data is allowed to take (if it is string)
     * that is displayed to the user.
     */
    QStringList allowed() const
    {
        return m_allowed.values();
    }
    /**
     * @param allowed A list of pairs of (id, i18n-name) of allowed values.
     */
    void setAllowed(const QStringMap &allowed)
    {
        m_allowed = allowed;
    }
    void setAllowed(const QStringList &allowed);
    void appendAllowed(const QString &id, const QString &i18nName);
    void appendAllowed(const QString &allowed);
    /**
     * @return whether the current value is different to the default value.
     */
    bool changed() const;
    QVariant value() const
    {
        return m_value;
    }
    void setValue(QVariant val);

signals:
    /**
     * Emitted when the value changes.
     * NOTE: The order of data given is the new value, and then the old value
     * This is done so that slots that don't care about the old value don't
     * have to accept it
     */
    void valueChanged(QVariant newValue, QVariant oldValue);
    /**
     * Emitted for variants of string-like type.
     */
    void valueChanged(const QString &newValue);
    /**
     * Emitted for variants of string-like type.
     * This signal is needed for updating values in KComboBox-es, see KComboBox::setCurrentItem(),
     *   second bool parameter, insert.
     */
    void valueChangedStrAndTrue(const QString &newValue, bool trueBool);
    /**
     * Emitted for variants of int-like type.
     */
    void valueChanged(int newValue);
    /**
     * Emitted for variants of double-like type.
     */
    void valueChanged(double newValue);
    /**
     * Emitted for variants of color-like type.
     */
    void valueChanged(const QColor &newValue);
    /**
     * Emitted for variants of bool-like type.
     */
    void valueChanged(bool newValue);

private:
    QVariant m_value; // the actual data
    QVariant m_defaultValue;
    QString m_unit;
    const QString m_id;
    double m_minAbsValue;
    double m_minValue;
    double m_maxValue;
    QString m_toolbarCaption; // Short description shown in e.g. properties dialog
    QString m_editorCaption;  // Text displayed before the data entry widget in the toolbar
    bool m_bAdvanced;         // If advanced, only display data in item editor
    bool m_bHidden;           // If hidden, do not allow user to change data
    FileFilters m_fileFilters; // If type() == Type::FileName these are the filters used in file dialogs.
    bool m_bSetDefault;       // If false, then the default will be set to the first thing this variant is set to
    Type::Value m_type;
    QStringMap m_allowed;
    int m_colorScheme;
};

#endif
