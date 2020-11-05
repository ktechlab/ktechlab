
/***************************************************************************
 *   Copyright (C) 2006 by David Saxton - david@bluehaze.org               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef RICHTEXTEDITOR_H
#define RICHTEXTEDITOR_H

#include <QDialog>

class QTextEdit;
class KToggleAction;
class KToolBarPopupAction;
class QFont;
class QTextCharFormat;

/**
@author David Saxton
*/
class RichTextEditor : public QWidget
{
    Q_OBJECT
public:
    RichTextEditor(QWidget *parent = nullptr);

    ~RichTextEditor() override;
    /**.
     * @return the text in the editor (tidied up).
     */
    QString text() const;
    /**
     * Sets the text.
     */
    void setText(QString text);
    /**
     * Inserts formatting into the body tag that will ensure the text used
     * is the default for the system. If the text does not have a body tag,
     * it will be given one.
     */
    static void makeUseStandardFont(QString *html);
    /**
     * These are the paths that are used for to search for images, etc.
     */
    void setResourcePaths(const QStringList &paths);

    QWidget *editorViewport() const;

signals:
    void textChanged();

public slots:
    /**
     * Inserts a url in the editor at the current cursor position with the
     * given location and text.
     */
    void insertURL(const QString &url, const QString &text);
    /**
     * Inserts the given HTML code at the current cursor position (this
     * function is required as QTextEdit will try to format any inserted
     * HTML code, replacing less-thans, etc.
     */
    void insertHTML(const QString &html);

protected slots:
    void slotSetBold(bool);
    void slotSetItalic(bool);
    void slotSetUnderline(bool);
    void slotSetAlignment(QAction *);
    /**
     * Called when a vertical alignment is selected (subscript, normal or
     * superscript).
     */
    void slotSetVerticalAlignment(QAction *a);
    void slotSetList(bool set);
    void slotCurrentCharFormatChanged(const QTextCharFormat &f);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(int a);
    void verticalAlignmentChanged();
    void textColor();

protected:
    KToggleAction *m_pTextBold;
    KToggleAction *m_pTextItalic;
    KToggleAction *m_pTextUnderline;
    KToggleAction *m_pTextList;
    KToolBarPopupAction *m_pTextAlignment;
    KToolBarPopupAction *m_pTextVerticalAlignment;
    QAction *m_pTextColor;
    QTextEdit *m_pEditor;
};

/**
Popup dialog for editing rich text
@author David Saxton
*/
class RichTextEditorDlg : public QDialog
{
public:
    RichTextEditorDlg(QWidget *parent = nullptr, const QString &caption = QString());

    /**
     * Sets the text being edited (passes it to RichTextEditor).
     */
    void setText(const QString &text)
    {
        m_pEditor->setText(text);
    }
    /**
     * @return the text in the RichTextEditor.
     */
    QString text() const
    {
        return m_pEditor->text();
    }

protected:
    RichTextEditor *m_pEditor;
};

#endif
