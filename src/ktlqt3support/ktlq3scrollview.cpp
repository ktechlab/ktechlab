/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "ktlq3scrollview.h"

#include "qwidget.h"
#include "qscrollbar.h"
#include "qpainter.h"
#include "qpixmap.h"
#include "qcursor.h"
#include "q3ptrdict.h"
#include "qapplication.h"
#include "qtimer.h"
#include "qstyle.h"
#include "q3ptrlist.h"
#include "qevent.h"
//#include "q3listview.h"
// #ifdef Q_WS_MAC      // 2018.10.18 - do not depend on internal headers
// # include "private/qt_mac_p.h"
// #endif

// QT_BEGIN_NAMESPACE

// using namespace Qt;

static const int coord_limit = 4000;
static const int autoscroll_margin = 16;
static const int initialScrollTime = 30;
static const int initialScrollAccel = 5;

struct QSVChildRec {
    QSVChildRec(QWidget* c, int xx, int yy) :
        child(c),
        x(xx), y(yy)
    {
    }

    void hideOrShow(KtlQ3ScrollView* sv, QWidget* clipped_viewport);
    void moveTo(KtlQ3ScrollView* sv, int xx, int yy, QWidget* clipped_viewport)
    {
        if (x != xx || y != yy) {
            x = xx;
            y = yy;
            hideOrShow(sv,clipped_viewport);
        }
    }
    QWidget* child;
    int x, y;
};

void QSVChildRec::hideOrShow(KtlQ3ScrollView* sv, QWidget* clipped_viewport)
{
    if (clipped_viewport) {
        if (x+child->width() < sv->contentsX()+clipped_viewport->x()
             || x > sv->contentsX()+clipped_viewport->width()
             || y+child->height() < sv->contentsY()+clipped_viewport->y()
             || y > sv->contentsY()+clipped_viewport->height()) {
            child->move(clipped_viewport->width(),
                        clipped_viewport->height());
        } else {
            child->move(x-sv->contentsX()-clipped_viewport->x(),
                        y-sv->contentsY()-clipped_viewport->y());
        }
    } else {
        child->move(x-sv->contentsX(), y-sv->contentsY());
    }
}

// QT_BEGIN_INCLUDE_NAMESPACE
// #include "ktlq3scrollview.moc"
// QT_END_INCLUDE_NAMESPACE

class KtlQ3ScrollViewData {
public:
    KtlQ3ScrollViewData(KtlQ3ScrollView* parent, int vpwflags) :
        hbar(new QScrollBar(Qt::Horizontal, parent /*, "qt_hbar"*/)),
        vbar(new QScrollBar(Qt::Vertical, parent /*, "qt_vbar" */)),
        viewport(new KtlQAbstractScrollAreaWidget(parent, "qt_viewport", QFlag(vpwflags))),
        clipped_viewport(0),
        flags(vpwflags),
        vx(0), vy(0), vwidth(1), vheight(1),
#ifndef QT_NO_DRAGANDDROP
        autoscroll_timer(parent /* , "scrollview autoscroll timer" */),
        drag_autoscroll(true),
#endif
        scrollbar_timer(parent /*, "scrollview scrollbar timer" */),
        inresize(false), use_cached_size_hint(true)
    {
        hbar->setObjectName("qt_hbar");
        vbar->setObjectName("qt_vbar");
#ifndef QT_NO_DRAGANDDROP
        autoscroll_timer.setObjectName("scrollview autoscroll timer");
#endif
        scrollbar_timer.setObjectName("scrollview scrollbar timer");
        l_marg = r_marg = t_marg = b_marg = 0;
        viewport->ensurePolished();
        vMode = KtlQ3ScrollView::Auto;
        hMode = KtlQ3ScrollView::Auto;
        corner = 0;
        // vbar->setSteps(20, 1/*set later*/);
        vbar->setSingleStep(20); vbar->setPageStep(1);
        //hbar->setSteps(20, 1/*set later*/);
        hbar->setSingleStep(20); hbar->setPageStep(1);
        policy = KtlQ3ScrollView::Default;
        signal_choke = false;
        static_bg = false;
        fake_scroll = false;
        hbarPressed = false;
        vbarPressed = false;
        hbar->setLayoutDirection(Qt::LeftToRight);
    }
    ~KtlQ3ScrollViewData();

    QSVChildRec* rec(QWidget* w) { return childDict.find(w); }
    QSVChildRec* ancestorRec(QWidget* w);
    QSVChildRec* addChildRec(QWidget* w, int x, int y)
    {
        QSVChildRec *r = new QSVChildRec(w,x,y);
        children.append(r);
        childDict.insert(w, r);
        return r;
    }
    void deleteChildRec(QSVChildRec* r)
    {
        childDict.remove(r->child);
        children.removeRef(r);
        delete r;
    }

    void hideOrShowAll(KtlQ3ScrollView* sv, bool isScroll = false);
    void moveAllBy(int dx, int dy);
    bool anyVisibleChildren();
    void autoMove(KtlQ3ScrollView* sv);
    void autoResize(KtlQ3ScrollView* sv);
    void autoResizeHint(KtlQ3ScrollView* sv);
    void viewportResized(int w, int h);

    QScrollBar*  hbar;
    QScrollBar*  vbar;
    bool hbarPressed;
    bool vbarPressed;
    KtlQAbstractScrollAreaWidget*    viewport;
    KtlQClipperWidget*     clipped_viewport;
    int         flags;
    Q3PtrList<QSVChildRec>       children;
    Q3PtrDict<QSVChildRec>       childDict;
    QWidget*    corner;
    int         vx, vy, vwidth, vheight; // for drawContents-style usage
    int         l_marg, r_marg, t_marg, b_marg;
    KtlQ3ScrollView::ResizePolicy policy;
    KtlQ3ScrollView::ScrollBarMode  vMode;
    KtlQ3ScrollView::ScrollBarMode  hMode;
#ifndef QT_NO_DRAGANDDROP
    QPoint cpDragStart;
    QTimer autoscroll_timer;
    int autoscroll_time;
    int autoscroll_accel;
    bool drag_autoscroll;
#endif
    QTimer scrollbar_timer;

    uint static_bg : 1;
    uint fake_scroll : 1;

    // This variable allows ensureVisible to move the contents then
    // update both the sliders.  Otherwise, updating the sliders would
    // cause two image scrolls, creating ugly flashing.
    //
    uint signal_choke : 1;

    // This variables indicates in updateScrollBars() that we are
    // in a resizeEvent() and thus don't want to flash scroll bars
    uint inresize : 1;
    uint use_cached_size_hint : 1;
    QSize cachedSizeHint;

    inline int contentsX() const { return -vx; }
    inline int contentsY() const { return -vy; }
    inline int contentsWidth() const { return vwidth; }
};

inline KtlQ3ScrollViewData::~KtlQ3ScrollViewData()
{
    children.setAutoDelete(true);
}

QSVChildRec* KtlQ3ScrollViewData::ancestorRec(QWidget* w)
{
    if (clipped_viewport) {
        while (w->parentWidget() != clipped_viewport) {
            w = w->parentWidget();
            if (!w) return 0;
        }
    } else {
        while (w->parentWidget() != viewport) {
            w = w->parentWidget();
            if (!w) return 0;
        }
    }
    return rec(w);
}

void KtlQ3ScrollViewData::hideOrShowAll(KtlQ3ScrollView* sv, bool isScroll)
{
    if (!clipped_viewport)
        return;
    if (clipped_viewport->x() <= 0
         && clipped_viewport->y() <= 0
         && clipped_viewport->width()+clipped_viewport->x() >=
         viewport->width()
         && clipped_viewport->height()+clipped_viewport->y() >=
         viewport->height()) {
        // clipped_viewport still covers viewport
        if(static_bg) {
            //clipped_viewport->repaint(true); // 2018.11.30
            clipped_viewport->repaint();
        }
        else if ((!isScroll && !clipped_viewport->testAttribute(Qt::WA_StaticContents)) || static_bg)
            clipped_viewport->update();
    } else {
        // Re-center
        int nx = (viewport->width() - clipped_viewport->width()) / 2;
        int ny = (viewport->height() - clipped_viewport->height()) / 2;
        clipped_viewport->move(nx,ny);
        clipped_viewport->update();
    }
    for (QSVChildRec *r = children.first(); r; r=children.next()) {
        r->hideOrShow(sv, clipped_viewport);
    }
}

void KtlQ3ScrollViewData::moveAllBy(int dx, int dy)
{
    if (clipped_viewport && !static_bg) {
        clipped_viewport->move(clipped_viewport->x()+dx,
                                clipped_viewport->y()+dy);
    } else {
        for (QSVChildRec *r = children.first(); r; r=children.next()) {
            r->child->move(r->child->x()+dx,r->child->y()+dy);
        }
        if (static_bg) {
            //viewport->repaint(true); // 2018.11.30
            viewport->repaint();
        }
    }
}

bool KtlQ3ScrollViewData::anyVisibleChildren()
{
    for (QSVChildRec *r = children.first(); r; r=children.next()) {
        if (r->child->isVisible()) return true;
    }
    return false;
}

void KtlQ3ScrollViewData::autoMove(KtlQ3ScrollView* sv)
{
    if (policy == KtlQ3ScrollView::AutoOne) {
        QSVChildRec* r = children.first();
        if (r)
            sv->setContentsPos(-r->child->x(),-r->child->y());
    }
}

void KtlQ3ScrollViewData::autoResize(KtlQ3ScrollView* sv)
{
    if (policy == KtlQ3ScrollView::AutoOne) {
        QSVChildRec* r = children.first();
        if (r)
            sv->resizeContents(r->child->width(),r->child->height());
    }
}

void KtlQ3ScrollViewData::autoResizeHint(KtlQ3ScrollView* sv)
{
    if (policy == KtlQ3ScrollView::AutoOne) {
        QSVChildRec* r = children.first();
        if (r) {
            QSize s = r->child->sizeHint();
            if (s.isValid())
                r->child->resize(s);
        }
    } else if (policy == KtlQ3ScrollView::AutoOneFit) {
        QSVChildRec* r = children.first();
        if (r) {
            QSize sh = r->child->sizeHint();
            sh = sh.boundedTo(r->child->maximumSize());
            sv->resizeContents(sh.width(), sh.height());
        }
    }
}

void KtlQ3ScrollViewData::viewportResized(int w, int h)
{
    if (policy == KtlQ3ScrollView::AutoOneFit) {
        QSVChildRec* r = children.first();
        if (r) {
            QSize sh = r->child->sizeHint();
            sh = sh.boundedTo(r->child->maximumSize());
            r->child->resize(QMAX(w,sh.width()), QMAX(h,sh.height()));
        }

    }
}


/*!
    \class KtlQ3ScrollView
    \brief The KtlQ3ScrollView widget provides a scrolling area with on-demand scroll bars.

    \compat

    The KtlQ3ScrollView is a large canvas - potentially larger than the
    coordinate system normally supported by the underlying window
    system. This is important because it is quite easy to go beyond
    these limitations (e.g. many web pages are more than 32000 pixels
    high). Additionally, the KtlQ3ScrollView can have QWidgets positioned
    on it that scroll around with the drawn content. These sub-widgets
    can also have positions outside the normal coordinate range (but
    they are still limited in size).

    To provide content for the widget, inherit from KtlQ3ScrollView,
    reimplement drawContents() and use resizeContents() to set the
    size of the viewed area. Use addChild() and moveChild() to
    position widgets on the view.

    To use KtlQ3ScrollView effectively it is important to understand its
    widget structure in the three styles of use: a single large child
    widget, a large panning area with some widgets and a large panning
    area with many widgets.

    \section1 Using One Big Widget

    \img qscrollview-vp2.png

    The first, simplest usage of KtlQ3ScrollView (depicted above), is
    appropriate for scrolling areas that are never more than about
    4000 pixels in either dimension (this is about the maximum
    reliable size on X11 servers). In this usage, you just make one
    large child in the KtlQ3ScrollView. The child should be a child of the
    viewport() of the scrollview and be added with addChild():
    \snippet doc/src/snippets/code/src_qt3support_widgets_q3scrollview.cpp 0
    You can go on to add arbitrary child widgets to the single child
    in the scrollview as you would with any widget:
    \snippet doc/src/snippets/code/src_qt3support_widgets_q3scrollview.cpp 1

    Here the KtlQ3ScrollView has four children: the viewport(), the
    verticalScrollBar(), the horizontalScrollBar() and a small
    cornerWidget(). The viewport() has one child: the QWidget. The
    QWidget has the three QLabel objects as child widgets. When the view
    is scrolled, the QWidget is moved; its children move with it as
    child widgets normally do.

    \section1 Using a Very Big View with Some Widgets

    \img qscrollview-vp.png

    The second usage of KtlQ3ScrollView (depicted above) is appropriate
    when few, if any, widgets are on a very large scrolling area that
    is potentially larger than 4000 pixels in either dimension. In
    this usage you call resizeContents() to set the size of the area
    and reimplement drawContents() to paint the contents. You may also
    add some widgets by making them children of the viewport() and
    adding them with addChild() (this is the same as the process for
    the single large widget in the previous example):
    \snippet doc/src/snippets/code/src_qt3support_widgets_q3scrollview.cpp 2
    Here, the KtlQ3ScrollView has the same four children: the viewport(),
    the verticalScrollBar(), the horizontalScrollBar() and a small
    cornerWidget(). The viewport() has the three QLabel objects as
    child widgets. When the view is scrolled, the scrollview moves the
    child widgets individually.

    \section1 Using a Very Big View with Many Widgets

    \img qscrollview-cl.png

    The final usage of KtlQ3ScrollView (depicted above) is appropriate
    when many widgets are on a very large scrolling area that is
    potentially larger than 4000 pixels in either dimension. In this
    usage you call resizeContents() to set the size of the area and
    reimplement drawContents() to paint the contents. You then call
    enableClipper(true) and add widgets, again by making them children
    of the viewport(), and adding them with addChild():
    \snippet doc/src/snippets/code/src_qt3support_widgets_q3scrollview.cpp 3

    Here, the KtlQ3ScrollView has four children:  the clipper() (not the
    viewport() this time), the verticalScrollBar(), the
    horizontalScrollBar() and a small cornerWidget(). The clipper()
    has one child: the viewport(). The viewport() has the same three
    labels as child widgets. When the view is scrolled the viewport()
    is moved; its children move with it as child widgets normally do.

    \target allviews
    \section1 Details Relevant for All Views

    Normally you will use the first or third method if you want any
    child widgets in the view.

    Note that the widget you see in the scrolled area is the
    viewport() widget, not the KtlQ3ScrollView itself. So to turn mouse
    tracking on, for example, use viewport()->setMouseTracking(true).

    To enable drag-and-drop, you would setAcceptDrops(true) on the
    KtlQ3ScrollView (because drag-and-drop events propagate to the
    parent). But to work out the logical position in the view, you
    would need to map the drop co-ordinate from being relative to the
    KtlQ3ScrollView to being relative to the contents; use the function
    viewportToContents() for this.

    To handle mouse events on the scrolling area, subclass scrollview
    as you would subclass other widgets, but rather than
    reimplementing mousePressEvent(), reimplement
    contentsMousePressEvent() instead. The contents specific event
    handlers provide translated events in the coordinate system of the
    scrollview. If you reimplement mousePressEvent(), you'll get
    called only when part of the KtlQ3ScrollView is clicked: and the only
    such part is the "corner" (if you don't set a cornerWidget()) and
    the frame; everything else is covered up by the viewport, clipper
    or scroll bars.

    When you construct a KtlQ3ScrollView, some of the window flags apply
    to the viewport() instead of being sent to the QWidget constructor
    for the KtlQ3ScrollView.

    \list

    \i An image-manipulation widget would use \c
    WNoAutoErase|WStaticContents because the widget draws all pixels
    itself, and when its size increases, it only needs a paint event
    for the new part because the old part remains unchanged.

    \i A scrolling game widget in which the background scrolls as the
    characters move might use \c WNoAutoErase (in addition to \c
    WStaticContents) so that the window system background does not
    flash in and out during scrolling.

    \i A word processing widget might use \c WNoAutoErase and repaint
    itself line by line to get a less-flickery resizing. If the widget
    is in a mode in which no text justification can take place, it
    might use \c WStaticContents too, so that it would only get a
    repaint for the newly visible parts.

    \endlist

    Child widgets may be moved using addChild() or moveChild(). Use
    childX() and childY() to get the position of a child widget.

    A widget may be placed in the corner between the vertical and
    horizontal scroll bars with setCornerWidget(). You can get access
    to the scroll bars using horizontalScrollBar() and
    verticalScrollBar(), and to the viewport with viewport(). The
    scroll view can be scrolled using scrollBy(), ensureVisible(),
    setContentsPos() or center().

    The visible area is given by visibleWidth() and visibleHeight(),
    and the contents area by contentsWidth() and contentsHeight(). The
    contents may be repainted using one of the repaintContents() or
    updateContents() functions.

    Coordinate conversion is provided by contentsToViewport() and
    viewportToContents().

    The contentsMoving() signal is emitted just before the contents
    are moved to a new position.

    \warning KtlQ3ScrollView currently does not erase the background when
    resized, i.e. you must always clear the background manually in
    scrollview subclasses. This will change in a future version of Qt
    and we recommend specifying the \c WNoAutoErase flag explicitly.
*/


/*!
    \enum KtlQ3ScrollView::ResizePolicy

    This enum type is used to control a KtlQ3ScrollView's reaction to
    resize events.

    \value Default  the KtlQ3ScrollView selects one of the other settings
    automatically when it has to. In this version of Qt, KtlQ3ScrollView
    changes to \c Manual if you resize the contents with
    resizeContents() and to \c AutoOne if a child is added.

    \value Manual  the contents stays the size set by resizeContents().

    \value AutoOne  if there is only one child widget the contents stays
    the size of that widget. Otherwise the behavior is undefined.

    \value AutoOneFit if there is only one child widget the contents stays
    the size of that widget's sizeHint(). If the scrollview is resized
    larger than the child's sizeHint(), the child will be resized to
    fit. If there is more than one child, the behavior is undefined.

*/
//####  The widget will be resized to its sizeHint() when a LayoutHint event
//#### is received

/*!
    Constructs a KtlQ3ScrollView called \a name with parent \a parent and
    widget flags \a f.

    The widget flags \c WStaticContents, \c WNoAutoErase and \c
    WPaintClever are propagated to the viewport() widget. The other
    widget flags are propagated to the parent constructor as usual.
*/

KtlQ3ScrollView::KtlQ3ScrollView(QWidget *parent, const char *name, Qt::WindowFlags f) :
    KtlQ3Frame(parent, name, f & (~Qt::WStaticContents) & (~Qt::WNoAutoErase) & (~Qt::WResizeNoErase))
{
    Qt::WindowFlags flags = Qt::WResizeNoErase | (f&Qt::WPaintClever) | (f&Qt::WRepaintNoErase) | (f&Qt::WStaticContents);
    d = new KtlQ3ScrollViewData(this, flags);

#ifndef QT_NO_DRAGANDDROP
    connect(&d->autoscroll_timer, SIGNAL(timeout()),
             this, SLOT(doDragAutoScroll()));
#endif

    connect(d->hbar, SIGNAL(valueChanged(int)),
        this, SLOT(hslide(int)));
    connect(d->vbar, SIGNAL(valueChanged(int)),
        this, SLOT(vslide(int)));

    connect(d->hbar, SIGNAL(sliderPressed()), this, SLOT(hbarIsPressed()));
    connect(d->hbar, SIGNAL(sliderReleased()), this, SLOT(hbarIsReleased()));
    connect(d->vbar, SIGNAL(sliderPressed()), this, SLOT(vbarIsPressed()));
    connect(d->vbar, SIGNAL(sliderReleased()), this, SLOT(vbarIsReleased()));


    d->viewport->installEventFilter(this);

    connect(&d->scrollbar_timer, SIGNAL(timeout()),
             this, SLOT(updateScrollBars()));

    setFrameStyle(KtlQ3Frame::StyledPanel | KtlQ3Frame::Sunken);
    setLineWidth(style()->pixelMetric(QStyle::PM_DefaultFrameWidth));
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
}


/*!
    Destroys the KtlQ3ScrollView. Any children added with addChild() will
    be deleted.
*/
KtlQ3ScrollView::~KtlQ3ScrollView()
{
    // Be careful not to get all those useless events...
    if (d->clipped_viewport)
        d->clipped_viewport->removeEventFilter(this);
    else
        d->viewport->removeEventFilter(this);

    // order is important
    // ~QWidget may cause a WM_ERASEBKGND on Windows
    delete d->vbar;
    d->vbar = 0;
    delete d->hbar;
    d->hbar = 0;
    delete d->viewport;
    d->viewport = 0;
    delete d;
    d = 0;
}

/*!
    \fn void KtlQ3ScrollView::horizontalSliderPressed()

    This signal is emitted whenever the user presses the horizontal slider.
*/
/*!
    \fn void KtlQ3ScrollView::horizontalSliderReleased()

    This signal is emitted whenever the user releases the horizontal slider.
*/
/*!
    \fn void KtlQ3ScrollView::verticalSliderPressed()

    This signal is emitted whenever the user presses the vertical slider.
*/
/*!
    \fn void KtlQ3ScrollView::verticalSliderReleased()

    This signal is emitted whenever the user releases the vertical slider.
*/
void KtlQ3ScrollView::hbarIsPressed()
{
    d->hbarPressed = true;
    emit(horizontalSliderPressed());
}

void KtlQ3ScrollView::hbarIsReleased()
{
    d->hbarPressed = false;
    emit(horizontalSliderReleased());
}

/*!
    Returns true if horizontal slider is pressed by user; otherwise returns false.
*/
bool KtlQ3ScrollView::isHorizontalSliderPressed()
{
    return d->hbarPressed;
}

void KtlQ3ScrollView::vbarIsPressed()
{
    d->vbarPressed = true;
    emit(verticalSliderPressed());
}

void KtlQ3ScrollView::vbarIsReleased()
{
    d->vbarPressed = false;
    emit(verticalSliderReleased());
}

/*!
    Returns true if vertical slider is pressed by user; otherwise returns false.
*/
bool KtlQ3ScrollView::isVerticalSliderPressed()
{
    return d->vbarPressed;
}

/*!
    \internal
*/
void KtlQ3ScrollView::styleChange(QStyle& old)
{
    QWidget::styleChange(old);
    updateScrollBars();
    d->cachedSizeHint = QSize();
}

/*!
    \internal
*/
void KtlQ3ScrollView::fontChange(const QFont &old)
{
    QWidget::fontChange(old);
    updateScrollBars();
    d->cachedSizeHint = QSize();
}

void KtlQ3ScrollView::hslide(int pos)
{
    if (!d->signal_choke) {
        moveContents(-pos, -d->contentsY());
        QApplication::syncX();
    }
}

void KtlQ3ScrollView::vslide(int pos)
{
    if (!d->signal_choke) {
        moveContents(-d->contentsX(), -pos);
        QApplication::syncX();
    }
}

/*!
    Called when the horizontal scroll bar geometry changes. This is
    provided as a protected function so that subclasses can do
    interesting things such as providing extra buttons in some of the
    space normally used by the scroll bars.

    The default implementation simply gives all the space to \a hbar.
    The new geometry is given by \a x, \a y, \a w and \a h.

    \sa setVBarGeometry()
*/
void KtlQ3ScrollView::setHBarGeometry(QScrollBar& hbar,
    int x, int y, int w, int h)
{
    hbar.setGeometry(x, y, w, h);
}

/*!
    Called when the vertical scroll bar geometry changes. This is
    provided as a protected function so that subclasses can do
    interesting things such as providing extra buttons in some of the
    space normally used by the scroll bars.

    The default implementation simply gives all the space to \a vbar.
    The new geometry is given by \a x, \a y, \a w and \a h.

    \sa setHBarGeometry()
*/
void KtlQ3ScrollView::setVBarGeometry(QScrollBar& vbar,
    int x, int y, int w, int h)
{
    vbar.setGeometry(x, y, w, h);
}


/*!
    Returns the viewport size for size (\a x, \a y).

    The viewport size depends on (\a x, \a y) (the size of the contents),
    the size of this widget and the modes of the horizontal and
    vertical scroll bars.

    This function permits widgets that can trade vertical and
    horizontal space for each other to control scroll bar appearance
    better. For example, a word processor or web browser can control
    the width of the right margin accurately, whether or not there
    needs to be a vertical scroll bar.
*/

QSize KtlQ3ScrollView::viewportSize(int x, int y) const
{
    int fw = frameWidth();
    int lmarg = fw+d->l_marg;
    int rmarg = fw+d->r_marg;
    int tmarg = fw+d->t_marg;
    int bmarg = fw+d->b_marg;

    int w = width();
    int h = height();

    bool needh, needv;
    bool showh, showv;
    int hsbExt = horizontalScrollBar()->sizeHint().height();
    int vsbExt = verticalScrollBar()->sizeHint().width();

    if (d->policy != AutoOne || d->anyVisibleChildren()) {
        // Do we definitely need the scroll bar?
        needh = w-lmarg-rmarg < x;
        needv = h-tmarg-bmarg < y;

        // Do we intend to show the scroll bar?
        if (d->hMode == AlwaysOn)
            showh = true;
        else if (d->hMode == AlwaysOff)
            showh = false;
        else
            showh = needh;

        if (d->vMode == AlwaysOn)
            showv = true;
        else if (d->vMode == AlwaysOff)
            showv = false;
        else
            showv = needv;

        // Given other scroll bar will be shown, NOW do we need one?
        if (showh && h-vsbExt-tmarg-bmarg < y) {
            if (d->vMode == Auto)
                showv=true;
        }
        if (showv && w-hsbExt-lmarg-rmarg < x) {
            if (d->hMode == Auto)
                showh=true;
        }
    } else {
        // Scroll bars not needed, only show scroll bar that are always on.
        showh = d->hMode == AlwaysOn;
        showv = d->vMode == AlwaysOn;
    }

    return QSize(w-lmarg-rmarg - (showv ? vsbExt : 0),
                  h-tmarg-bmarg - (showh ? hsbExt : 0));
}


/*!
    Updates scroll bars: all possibilities are considered. You should
    never need to call this in your code.
*/
void KtlQ3ScrollView::updateScrollBars()
{
    if(!horizontalScrollBar() && !verticalScrollBar())
        return;

    // I support this should use viewportSize()... but it needs
    // so many of the temporary variables from viewportSize.  hm.
    int fw = frameWidth();
    int lmarg = fw+d->l_marg;
    int rmarg = fw+d->r_marg;
    int tmarg = fw+d->t_marg;
    int bmarg = fw+d->b_marg;

    int w = width();
    int h = height();

    int portw, porth;

    bool needh;
    bool needv;
    bool showh;
    bool showv;
    bool showc = false;

    int hsbExt = horizontalScrollBar()->sizeHint().height();
    int vsbExt = verticalScrollBar()->sizeHint().width();

    QSize oldVisibleSize(visibleWidth(), visibleHeight());

    if (d->policy != AutoOne || d->anyVisibleChildren()) {
        // Do we definitely need the scroll bar?
        needh = w-lmarg-rmarg < d->contentsWidth();
        if (d->inresize)
            needh  = !horizontalScrollBar()->isHidden();
        needv = h-tmarg-bmarg < contentsHeight();

        // Do we intend to show the scroll bar?
        if (d->hMode == AlwaysOn)
            showh = true;
        else if (d->hMode == AlwaysOff)
            showh = false;
        else
            showh = needh;

        if (d->vMode == AlwaysOn)
            showv = true;
        else if (d->vMode == AlwaysOff)
            showv = false;
        else
            showv = needv;

#ifdef Q_WS_MAC
        bool mac_need_scroll = false;
        if(!parentWidget()) {
            mac_need_scroll = true;
        } else {
            QWidget *tlw = window();
// #ifndef QT_MAC_USE_COCOA           // 2018.10.18 - do not depend on internal headers
//             QPoint tlw_br = QPoint(tlw->width(), tlw->height()),
//                     my_br = qt_mac_posInWindow(this) + QPoint(w, h);
//             if(my_br.x() >= tlw_br.x() - 3 && my_br.y() >= tlw_br.y() - 3)
// #endif
                mac_need_scroll = true;
        }
        if(mac_need_scroll) {
// #ifndef QT_MAC_USE_COCOA           // 2018.10.18 - do not depend on internal headers
//             WindowAttributes attr;
//             GetWindowAttributes((WindowPtr)handle(), &attr);
//             mac_need_scroll = (attr & kWindowResizableAttribute);
// #endif
        }
        if(mac_need_scroll) {
            showc = true;
            if(d->vMode == Auto)
                showv = true;
            if(d->hMode == Auto)
                showh = true;
        }
#endif

        // Given other scroll bar will be shown, NOW do we need one?
        if (showh && h-vsbExt-tmarg-bmarg < contentsHeight()) {
            needv=true;
            if (d->vMode == Auto)
                showv=true;
        }
        if (showv && !d->inresize && w-hsbExt-lmarg-rmarg < d->contentsWidth()) {
            needh=true;
            if (d->hMode == Auto)
                showh=true;
        }
    } else {
        // Scrollbars not needed, only show scroll bar that are always on.
        needh = needv = false;
        showh = d->hMode == AlwaysOn;
        showv = d->vMode == AlwaysOn;
    }

    bool sc = d->signal_choke;
    d->signal_choke=true;

    // Hide unneeded scroll bar, calculate viewport size
    if (showh) {
        porth=h-hsbExt-tmarg-bmarg;
    } else {
        if (!needh)
            d->hbar->setValue(0);
        d->hbar->hide();
        porth=h-tmarg-bmarg;
    }
    if (showv) {
        portw=w-vsbExt-lmarg-rmarg;
    } else {
        if (!needv)
            d->vbar->setValue(0);
        d->vbar->hide();
        portw=w-lmarg-rmarg;
    }

    // Configure scroll bars that we will show
    if (needv) {
        d->vbar->setRange(0, contentsHeight()-porth);
        //d->vbar->setSteps(KtlQ3ScrollView::d->vbar->lineStep(), porth); // 2018.11.30
        d->vbar->setSingleStep(KtlQ3ScrollView::d->vbar->singleStep());
        d->vbar->setPageStep(porth);
    } else {
        d->vbar->setRange(0, 0);
    }
    if (needh) {
        d->hbar->setRange(0, QMAX(0, d->contentsWidth()-portw));
        //d->hbar->setSteps(KtlQ3ScrollView::d->hbar->lineStep(), portw); // 2018.11.30
        d->hbar->setSingleStep(KtlQ3ScrollView::d->hbar->singleStep());
        d->hbar->setPageStep(portw);
    } else {
        d->hbar->setRange(0, 0);
    }

    // Position the scroll bars, viewport and corner widget.
    int bottom;
    bool reverse = QApplication::layoutDirection() == Qt::RightToLeft;
    int xoffset = (reverse && (showv || cornerWidget())) ? vsbExt : 0;
    int xpos = reverse ? 0 : w - vsbExt;
    bool frameContentsOnly =
        style()->styleHint(QStyle::SH_ScrollView_FrameOnlyAroundContents);

    if(! frameContentsOnly) {
        if (reverse)
            xpos += fw;
        else
            xpos -= fw;
    }
    if (showh) {
        int right = (showc || showv || cornerWidget()) ? w-vsbExt : w;
        if (! frameContentsOnly)
            setHBarGeometry(*d->hbar, fw + xoffset, h-hsbExt-fw,
                             right-fw-fw, hsbExt);
        else
            setHBarGeometry(*d->hbar, 0 + xoffset, h-hsbExt, right,
                             hsbExt);
        bottom=h-hsbExt;
    } else {
        bottom=h;
    }
    if (showv) {
        clipper()->setGeometry(lmarg + xoffset, tmarg,
                                w-vsbExt-lmarg-rmarg,
                                bottom-tmarg-bmarg);
        d->viewportResized(w-vsbExt-lmarg-rmarg, bottom-tmarg-bmarg);
        if (! frameContentsOnly)
            changeFrameRect(QRect(0, 0, w, h));
        else
            changeFrameRect(QRect(xoffset, 0, w-vsbExt, bottom));
        if (showc || cornerWidget()) {
            if (! frameContentsOnly)
                setVBarGeometry(*d->vbar, xpos,
                                 fw, vsbExt,
                                 h-hsbExt-fw-fw);
            else
                setVBarGeometry(*d->vbar, xpos, 0,
                                 vsbExt,
                                 h-hsbExt);
        }
        else {
            if (! frameContentsOnly)
                setVBarGeometry(*d->vbar, xpos,
                                 fw, vsbExt,
                                 bottom-fw-fw);
            else
                setVBarGeometry(*d->vbar, xpos, 0,
                                 vsbExt, bottom);
        }
    } else {
        if (! frameContentsOnly)
            changeFrameRect(QRect(0, 0, w, h));
        else
            changeFrameRect(QRect(0, 0, w, bottom));
        clipper()->setGeometry(lmarg, tmarg,
                                w-lmarg-rmarg, bottom-tmarg-bmarg);
        d->viewportResized(w-lmarg-rmarg, bottom-tmarg-bmarg);
    }

    QWidget *corner = d->corner;
    if (d->corner) {
        if (! frameContentsOnly)
            corner->setGeometry(xpos,
                                h-hsbExt-fw,
                                vsbExt,
                                hsbExt);
        else
            corner->setGeometry(xpos,
                                h-hsbExt,
                                vsbExt,
                                hsbExt);
    }

    d->signal_choke=sc;

    if (d->contentsX()+visibleWidth() > d->contentsWidth()) {
        int x;
#if 0
        if (reverse)
            x =QMIN(0,d->contentsWidth()-visibleWidth());
        else
#endif
            x =QMAX(0,d->contentsWidth()-visibleWidth());
        d->hbar->setValue(x);
        // Do it even if it is recursive
        moveContents(-x, -d->contentsY());
    }
    if (d->contentsY()+visibleHeight() > contentsHeight()) {
        int y=QMAX(0,contentsHeight()-visibleHeight());
        d->vbar->setValue(y);
        // Do it even if it is recursive
        moveContents(-d->contentsX(), -y);
    }

    // Finally, show the scroll bars
    if (showh && (d->hbar->isHidden() || !d->hbar->isVisible()))
        d->hbar->show();
    if (showv && (d->vbar->isHidden() || !d->vbar->isVisible()))
        d->vbar->show();

    d->signal_choke=true;
    d->vbar->setValue(d->contentsY());
    d->hbar->setValue(d->contentsX());
    d->signal_choke=false;

    QSize newVisibleSize(visibleWidth(), visibleHeight());
    if (d->clipped_viewport && oldVisibleSize != newVisibleSize) {
        QResizeEvent e(newVisibleSize, oldVisibleSize);
        viewportResizeEvent(&e);
    }
}


/*!
    \reimp
*/
void KtlQ3ScrollView::setVisible(bool visible)
{
    if (visible && !isVisible()) {
        QWidget::setVisible(visible);
        updateScrollBars();
        d->hideOrShowAll(this);
    } else {
        QWidget::setVisible(visible);
    }
}

/*!
    \internal
 */
void KtlQ3ScrollView::resize(int w, int h)
{
    QWidget::resize(w, h);
}

/*!
    \internal
*/
void KtlQ3ScrollView::resize(const QSize& s)
{
    resize(s.width(), s.height());
}

/*!
    \reimp
*/
void KtlQ3ScrollView::resizeEvent(QResizeEvent* event)
{
    KtlQ3Frame::resizeEvent(event);

#if 0
    if (QApplication::reverseLayout()) {
        d->fake_scroll = true;
        scrollBy(-event->size().width() + event->oldSize().width(), 0);
        d->fake_scroll = false;
    }
#endif

    bool inresize = d->inresize;
    d->inresize = true;
    updateScrollBars();
    d->inresize = inresize;
    //d->scrollbar_timer.start(0, true); // 2018.11.30
    d->scrollbar_timer.setSingleShot(true);
    d->scrollbar_timer.start(0);

    d->hideOrShowAll(this);
}



/*!
    \reimp
*/
void  KtlQ3ScrollView::mousePressEvent(QMouseEvent * e)
{
    e->ignore();
}

/*!
    \reimp
*/
void  KtlQ3ScrollView::mouseReleaseEvent(QMouseEvent *e)
{
    e->ignore();
}


/*!
    \reimp
*/
void  KtlQ3ScrollView::mouseDoubleClickEvent(QMouseEvent *e)
{
    e->ignore();
}

/*!
    \reimp
*/
void  KtlQ3ScrollView::mouseMoveEvent(QMouseEvent *e)
{
    e->ignore();
}

/*!
    \reimp
*/
#ifndef QT_NO_WHEELEVENT
void KtlQ3ScrollView::wheelEvent(QWheelEvent *e)
{
    /* QWheelEvent ce(viewport()->mapFromGlobal(e->globalPos()),
                    e->globalPos(), e->delta(), e->state());  - 2018.11.30*/
    QWheelEvent ce(viewport()->mapFromGlobal(e->globalPos()),
                    e->globalPos(), e->delta(), e->buttons(), e->modifiers());
    viewportWheelEvent(&ce);
    if (!ce.isAccepted()) {
        if (e->orientation() == Qt::Horizontal && horizontalScrollBar())
            horizontalScrollBar()->event(e);
        else  if (e->orientation() == Qt::Vertical && verticalScrollBar())
            verticalScrollBar()->event(e);
    } else {
        e->accept();
    }
}
#endif

/*!
    \reimp
*/
void KtlQ3ScrollView::contextMenuEvent(QContextMenuEvent *e)
{
    if (e->reason() != QContextMenuEvent::Keyboard) {
        e->ignore();
        return;
    }

    /* QContextMenuEvent ce(e->reason(), viewport()->mapFromGlobal(e->globalPos()),
                          e->globalPos(), e->state()); - 2018.11.30 */
    QContextMenuEvent ce(e->reason(), viewport()->mapFromGlobal(e->globalPos()),
                          e->globalPos(), e->modifiers());
    viewportContextMenuEvent(&ce);
    if (ce.isAccepted())
        e->accept();
    else
        e->ignore();
}

KtlQ3ScrollView::ScrollBarMode KtlQ3ScrollView::vScrollBarMode() const
{
    return d->vMode;
}


/*!
    \enum KtlQ3ScrollView::ScrollBarMode

    This enum type describes the various modes of KtlQ3ScrollView's scroll
    bars.

    \value Auto  KtlQ3ScrollView shows a scroll bar when the content is
    too large to fit and not otherwise. This is the default.

    \value AlwaysOff  KtlQ3ScrollView never shows a scroll bar.

    \value AlwaysOn  KtlQ3ScrollView always shows a scroll bar.

    (The modes for the horizontal and vertical scroll bars are
    independent.)
*/


/*!
    \property KtlQ3ScrollView::vScrollBarMode
    \brief the mode for the vertical scroll bar

    The default mode is KtlQ3ScrollView::Auto.

    \sa hScrollBarMode
*/
void  KtlQ3ScrollView::setVScrollBarMode(ScrollBarMode mode)
{
    if (d->vMode != mode) {
        d->vMode = mode;
        updateScrollBars();
    }
}


/*!
    \property KtlQ3ScrollView::hScrollBarMode
    \brief the mode for the horizontal scroll bar

    The default mode is KtlQ3ScrollView::Auto.

    \sa vScrollBarMode
*/
KtlQ3ScrollView::ScrollBarMode KtlQ3ScrollView::hScrollBarMode() const
{
    return d->hMode;
}

void KtlQ3ScrollView::setHScrollBarMode(ScrollBarMode mode)
{
    if (d->hMode != mode) {
        d->hMode = mode;
        updateScrollBars();
    }
}


/*!
    Returns the widget in the corner between the two scroll bars.

    By default, no corner widget is present.
*/
QWidget* KtlQ3ScrollView::cornerWidget() const
{
    return d->corner;
}

/*!
    Sets the widget in the \a corner between the two scroll bars.

    You will probably also want to set at least one of the scroll bar
    modes to \c AlwaysOn.

    Passing 0 shows no widget in the corner.

    Any previous \a corner widget is hidden.

    You may call setCornerWidget() with the same widget at different
    times.

    All widgets set here will be deleted by the KtlQ3ScrollView when it is
    destroyed unless you separately reparent the widget after setting
    some other corner widget (or 0).

    Any \e newly set widget should have no current parent.

    By default, no corner widget is present.

    \sa setVScrollBarMode(), setHScrollBarMode()
*/
void KtlQ3ScrollView::setCornerWidget(QWidget* corner)
{
    QWidget* oldcorner = d->corner;
    if (oldcorner != corner) {
        if (oldcorner) oldcorner->hide();
        d->corner = corner;
        if (corner) corner->setParent(this);
        updateScrollBars();
        if (corner) corner->show();
    }
}


void KtlQ3ScrollView::setResizePolicy(ResizePolicy r)
{
    d->policy = r;
}

/*!
    \property KtlQ3ScrollView::resizePolicy
    \brief the resize policy

    The default is \c Default.

    \sa ResizePolicy
*/
KtlQ3ScrollView::ResizePolicy KtlQ3ScrollView::resizePolicy() const
{
    return d->policy;
}

/*!
    \internal
*/
void KtlQ3ScrollView::setEnabled(bool enable)
{
    KtlQ3Frame::setEnabled(enable);
}

/*!
    Removes the \a child widget from the scrolled area. Note that this
    happens automatically if the \a child is deleted.
*/
void KtlQ3ScrollView::removeChild(QWidget* child)
{
    if (!d || !child) // First check in case we are destructing
        return;

    QSVChildRec *r = d->rec(child);
    if (r) d->deleteChildRec(r);
}

/*!
    \internal
*/
void KtlQ3ScrollView::removeChild(QObject* child)
{
    // KtlQ3Frame::removeChild(child); // 2018.11.30
    if (child) {
        child->setParent(0);
    }
}

/*!
    Inserts the widget, \a child, into the scrolled area positioned at
    (\a x, \a y). The position defaults to (0, 0). If the child is
    already in the view, it is just moved.

    You may want to call enableClipper(true) if you add a large number
    of widgets.
*/
void KtlQ3ScrollView::addChild(QWidget* child, int x, int y)
{
    if (!child) {
#if defined(QT_CHECK_NULL)
        qWarning("KtlQ3ScrollView::addChild(): Cannot add null child");
#endif
        return;
    }
    child->ensurePolished();
    //child->setBackgroundOrigin(WidgetOrigin); // 2018.11.30 - does nothing in qt4

    if (child->parentWidget() == viewport()) {
        // May already be there
        QSVChildRec *r = d->rec(child);
        if (r) {
            r->moveTo(this,x,y,d->clipped_viewport);
            if (d->policy > Manual) {
                d->autoResizeHint(this);
                d->autoResize(this); // #### better to just deal with this one widget!
            }
            return;
        }
    }

    if (d->children.isEmpty() && d->policy != Manual) {
        if (d->policy == Default)
            setResizePolicy(AutoOne);
        child->installEventFilter(this);
    } else if (d->policy == AutoOne) {
        child->removeEventFilter(this); //#### ?????
        setResizePolicy(Manual);
    }
    if (child->parentWidget() != viewport()) {
            //child->reparent(viewport(), 0, QPoint(0,0), false); // 2018.11.30
            child->setParent(viewport());
            child->setGeometry(0, 0, child->width(), child->height());
    }
    d->addChildRec(child,x,y)->hideOrShow(this, d->clipped_viewport);

    if (d->policy > Manual) {
        d->autoResizeHint(this);
        d->autoResize(this); // #### better to just deal with this one widget!
    }
}

/*!
    Repositions the \a child widget to (\a x, \a y). This function is
    the same as addChild().
*/
void KtlQ3ScrollView::moveChild(QWidget* child, int x, int y)
{
    addChild(child,x,y);
}

/*!
    Returns the X position of the given \a child widget. Use this
    rather than QWidget::x() for widgets added to the view.

    This function returns 0 if \a child has not been added to the view.
*/
int KtlQ3ScrollView::childX(QWidget* child)
{
    QSVChildRec *r = d->rec(child);
    return r ? r->x : 0;
}

/*!
    Returns the Y position of the given \a child widget. Use this
    rather than QWidget::y() for widgets added to the view.

    This function returns 0 if \a child has not been added to the view.
*/
int KtlQ3ScrollView::childY(QWidget* child)
{
    QSVChildRec *r = d->rec(child);
    return r ? r->y : 0;
}

/*! \fn bool KtlQ3ScrollView::childIsVisible(QWidget*)
  \obsolete

  Returns true if \a child is visible. This is equivalent
  to child->isVisible().
*/

/*! \fn void KtlQ3ScrollView::showChild(QWidget* child, bool y)
  \obsolete

  Sets the visibility of \a child. Equivalent to
  QWidget::show() or QWidget::hide().
*/

/*!
    This event filter ensures the scroll bars are updated when a
    single contents widget is resized, shown, hidden or destroyed; it
    passes mouse events to the KtlQ3ScrollView. The event is in \a e and
    the object is in \a obj.
*/

bool KtlQ3ScrollView::eventFilter(QObject *obj, QEvent *e)
{
    bool disabled = !(qobject_cast<QWidget*>(obj)->isEnabled());
    if (!d)
        return false; // we are destructing
    if (obj == d->viewport || obj == d->clipped_viewport) {
        switch (e->type()) {
            /* Forward many events to viewport...() functions */
        case QEvent::Paint:
            viewportPaintEvent((QPaintEvent*)e);
            break;
        case QEvent::Resize:
            if (!d->clipped_viewport)
                viewportResizeEvent((QResizeEvent *)e);
            break;
        case QEvent::MouseButtonPress:
            if (disabled)
                return false;
            viewportMousePressEvent((QMouseEvent*)e);
            if (((QMouseEvent*)e)->isAccepted())
                return true;
            break;
        case QEvent::MouseButtonRelease:
            if (disabled)
                return false;
            viewportMouseReleaseEvent((QMouseEvent*)e);
            if (((QMouseEvent*)e)->isAccepted())
                return true;
            break;
        case QEvent::MouseButtonDblClick:
            if (disabled)
                return false;
            viewportMouseDoubleClickEvent((QMouseEvent*)e);
            if (((QMouseEvent*)e)->isAccepted())
                return true;
            break;
        case QEvent::MouseMove:
            if (disabled)
                return false;
            viewportMouseMoveEvent((QMouseEvent*)e);
            if (((QMouseEvent*)e)->isAccepted())
                return true;
            break;
#ifndef QT_NO_DRAGANDDROP
        case QEvent::DragEnter:
            if (disabled)
                return false;
            viewportDragEnterEvent((QDragEnterEvent*)e);
            break;
        case QEvent::DragMove: {
            if (disabled)
                return false;
            if (d->drag_autoscroll) {
                QPoint vp = ((QDragMoveEvent*) e)->pos();
                QRect inside_margin(autoscroll_margin, autoscroll_margin,
                                     visibleWidth() - autoscroll_margin * 2,
                                     visibleHeight() - autoscroll_margin * 2);
                if (!inside_margin.contains(vp)) {
                    startDragAutoScroll();
                    // Keep sending move events
                    ((QDragMoveEvent*)e)->accept(QRect(0,0,0,0));
                }
            }
            viewportDragMoveEvent((QDragMoveEvent*)e);
        } break;
        case QEvent::DragLeave:
            if (disabled)
                return false;
            stopDragAutoScroll();
            viewportDragLeaveEvent((QDragLeaveEvent*)e);
            break;
        case QEvent::Drop:
            if (disabled)
                return false;
            stopDragAutoScroll();
            viewportDropEvent((QDropEvent*)e);
            break;
#endif // QT_NO_DRAGANDDROP
#ifndef QT_NO_WHEELEVENT
        case QEvent::Wheel:
            if (disabled)
                return false;
            break;
#endif
        case QEvent::ContextMenu:
            if (disabled)
                return false;
            viewportContextMenuEvent((QContextMenuEvent*)e);
            if (((QContextMenuEvent*)e)->isAccepted())
                return true;
            break;
        case QEvent::ChildRemoved:
            removeChild((QWidget*)((QChildEvent*)e)->child());
            break;
        case QEvent::LayoutHint:
            d->autoResizeHint(this);
            break;
        default:
            break;
        }
    } else if (d && d->rec((QWidget*)obj)) {  // must be a child
        if (e->type() == QEvent::Resize)
            d->autoResize(this);
        else if (e->type() == QEvent::Move)
            d->autoMove(this);
    }
    return KtlQ3Frame::eventFilter(obj, e);  // always continue with standard event processing
}

/*!
    This event handler is called whenever the KtlQ3ScrollView receives a
    mousePressEvent(): the press position in \a e is translated to be a point
    on the contents.
*/
void KtlQ3ScrollView::contentsMousePressEvent(QMouseEvent* e)
{
    e->ignore();
}

/*!
    This event handler is called whenever the KtlQ3ScrollView receives a
    mouseReleaseEvent(): the release position in \a e is translated to be a
    point on the contents.
*/
void KtlQ3ScrollView::contentsMouseReleaseEvent(QMouseEvent* e)
{
    e->ignore();
}

/*!
    This event handler is called whenever the KtlQ3ScrollView receives a
    mouseDoubleClickEvent(): the click position in \a e is translated to be a
    point on the contents.

    The default implementation generates a normal mouse press event.
*/
void KtlQ3ScrollView::contentsMouseDoubleClickEvent(QMouseEvent* e)
{
    contentsMousePressEvent(e);             // try mouse press event
}

/*!
    This event handler is called whenever the KtlQ3ScrollView receives a
    mouseMoveEvent(): the mouse position in \a e is translated to be a point
    on the contents.
*/
void KtlQ3ScrollView::contentsMouseMoveEvent(QMouseEvent* e)
{
    e->ignore();
}

#ifndef QT_NO_DRAGANDDROP

/*!
    This event handler is called whenever the KtlQ3ScrollView receives a
    dragEnterEvent(): the drag position is translated to be a point
    on the contents.

    The default implementation does nothing. The \a event parameter is
    ignored.
*/
void KtlQ3ScrollView::contentsDragEnterEvent(QDragEnterEvent * /* event */)
{
}

/*!
    This event handler is called whenever the KtlQ3ScrollView receives a
    dragMoveEvent(): the drag position is translated to be a point on
    the contents.

    The default implementation does nothing. The \a event parameter is
    ignored.
*/
void KtlQ3ScrollView::contentsDragMoveEvent(QDragMoveEvent * /* event */)
{
}

/*!
    This event handler is called whenever the KtlQ3ScrollView receives a
    dragLeaveEvent(): the drag position is translated to be a point
    on the contents.

    The default implementation does nothing. The \a event parameter is
    ignored.
*/
void KtlQ3ScrollView::contentsDragLeaveEvent(QDragLeaveEvent * /* event */)
{
}

/*!
    This event handler is called whenever the KtlQ3ScrollView receives a
    dropEvent(): the drop position is translated to be a point on the
    contents.

    The default implementation does nothing. The \a event parameter is
    ignored.
*/

void KtlQ3ScrollView::contentsDropEvent(QDropEvent * /* event */)
{
}

#endif // QT_NO_DRAGANDDROP

/*!
    This event handler is called whenever the KtlQ3ScrollView receives a
    wheelEvent() in \a{e}: the mouse position is translated to be a
    point on the contents.
*/
#ifndef QT_NO_WHEELEVENT
void KtlQ3ScrollView::contentsWheelEvent(QWheelEvent * e)
{
    e->ignore();
}
#endif
/*!
    This event handler is called whenever the KtlQ3ScrollView receives a
    contextMenuEvent() in \a{e}: the mouse position is translated to
    be a point on the contents.
*/
void KtlQ3ScrollView::contentsContextMenuEvent(QContextMenuEvent *e)
{
    e->ignore();
}

/*!
    This is a low-level painting routine that draws the viewport
    contents. Reimplement this if drawContents() is too high-level
    (for example, if you don't want to open a QPainter on the
    viewport). The paint event is passed in \a pe.
*/
void KtlQ3ScrollView::viewportPaintEvent(QPaintEvent* pe)
{
    QWidget* vp = viewport();

    QPainter p(vp);
    QRect r = pe->rect();

    if (d->clipped_viewport) {
        QRect rr(
            -d->clipped_viewport->x(), -d->clipped_viewport->y(),
            d->viewport->width(), d->viewport->height()
           );
        r &= rr;
        if (r.isValid()) {
            int ex = r.x() + d->clipped_viewport->x() + d->contentsX();
            int ey = r.y() + d->clipped_viewport->y() + d->contentsY();
            int ew = r.width();
            int eh = r.height();
            drawContentsOffset(&p,
                d->contentsX()+d->clipped_viewport->x(),
                d->contentsY()+d->clipped_viewport->y(),
                ex, ey, ew, eh);
        }
    } else {
        r &= d->viewport->rect();
        int ex = r.x() + d->contentsX();
        int ey = r.y() + d->contentsY();
        int ew = r.width();
        int eh = r.height();
        drawContentsOffset(&p, d->contentsX(), d->contentsY(), ex, ey, ew, eh);
    }
}


/*!
    To provide simple processing of events on the contents, this
    function receives all resize events sent to the viewport.

    The default implementation does nothing. The \a event parameter is
    ignored.

    \sa QWidget::resizeEvent()
*/
void KtlQ3ScrollView::viewportResizeEvent(QResizeEvent * /* event */)
{
}

/*! \internal

  To provide simple processing of events on the contents, this
  function receives all mouse press events sent to the viewport,
  translates the event and calls contentsMousePressEvent().

  \sa contentsMousePressEvent(), QWidget::mousePressEvent()
*/
void KtlQ3ScrollView::viewportMousePressEvent(QMouseEvent* e)
{
    /* QMouseEvent ce(e->type(), viewportToContents(e->pos()),
        e->globalPos(), e->button(), e->state()); - 2018.11.30 */
    QMouseEvent ce(e->type(), viewportToContents(e->pos()),
        e->globalPos(), e->button(), e->buttons(), e->modifiers());
    contentsMousePressEvent(&ce);
    if (!ce.isAccepted())
        e->ignore();
}

/*!\internal

  To provide simple processing of events on the contents, this function
  receives all mouse release events sent to the viewport, translates
  the event and calls contentsMouseReleaseEvent().

  \sa QWidget::mouseReleaseEvent()
*/
void KtlQ3ScrollView::viewportMouseReleaseEvent(QMouseEvent* e)
{
    /* QMouseEvent ce(e->type(), viewportToContents(e->pos()),
        e->globalPos(), e->button(), e->state()); -- 2018.11.30 */
    QMouseEvent ce(e->type(), viewportToContents(e->pos()),
        e->globalPos(), e->button(), e->buttons(), e->modifiers());
    contentsMouseReleaseEvent(&ce);
    if (!ce.isAccepted())
        e->ignore();
}

/*!\internal

  To provide simple processing of events on the contents, this function
  receives all mouse double click events sent to the viewport,
  translates the event and calls contentsMouseDoubleClickEvent().

  \sa QWidget::mouseDoubleClickEvent()
*/
void KtlQ3ScrollView::viewportMouseDoubleClickEvent(QMouseEvent* e)
{
    /* QMouseEvent ce(e->type(), viewportToContents(e->pos()),
        e->globalPos(), e->button(), e->state());  - 2018.11.30 */
    QMouseEvent ce(e->type(), viewportToContents(e->pos()),
        e->globalPos(), e->button(), e->buttons(), e->modifiers());
    contentsMouseDoubleClickEvent(&ce);
    if (!ce.isAccepted())
        e->ignore();
}

/*!\internal

  To provide simple processing of events on the contents, this function
  receives all mouse move events sent to the viewport, translates the
  event and calls contentsMouseMoveEvent().

  \sa QWidget::mouseMoveEvent()
*/
void KtlQ3ScrollView::viewportMouseMoveEvent(QMouseEvent* e)
{
    QMouseEvent ce(e->type(), viewportToContents(e->pos()),
        e->globalPos(), e->button(), e->buttons(), e->modifiers());
    contentsMouseMoveEvent(&ce);
    if (!ce.isAccepted())
        e->ignore();
}

#ifndef QT_NO_DRAGANDDROP

/*!\internal

  To provide simple processing of events on the contents, this function
  receives all drag enter events sent to the viewport, translates the
  event and calls contentsDragEnterEvent().

  \sa QWidget::dragEnterEvent()
*/
void KtlQ3ScrollView::viewportDragEnterEvent(QDragEnterEvent* e)
{
    //e->setPoint(viewportToContents(e->pos())); // 2018.11.30
    QDragEnterEvent ev(viewportToContents(e->pos()), e->possibleActions(),
                       e->mimeData(), e->mouseButtons(), e->keyboardModifiers());
    contentsDragEnterEvent(&ev);
    //e->setPoint(contentsToViewport(e->pos())); // 2018.11.30
}

/*!\internal

  To provide simple processing of events on the contents, this function
  receives all drag move events sent to the viewport, translates the
  event and calls contentsDragMoveEvent().

  \sa QWidget::dragMoveEvent()
*/
void KtlQ3ScrollView::viewportDragMoveEvent(QDragMoveEvent* e)
{
    //e->setPoint(viewportToContents(e->pos())); // 2018.11.30
    QDragMoveEvent ev(viewportToContents(e->pos()), e->possibleActions(),
                      e->mimeData(), e->mouseButtons(), e->keyboardModifiers());
    contentsDragMoveEvent(&ev);
    //e->setPoint(contentsToViewport(e->pos())); // 2018.11.30
}

/*!\internal

  To provide simple processing of events on the contents, this function
  receives all drag leave events sent to the viewport and calls
  contentsDragLeaveEvent().

  \sa QWidget::dragLeaveEvent()
*/
void KtlQ3ScrollView::viewportDragLeaveEvent(QDragLeaveEvent* e)
{
    contentsDragLeaveEvent(e);
}

/*!\internal

  To provide simple processing of events on the contents, this function
  receives all drop events sent to the viewport, translates the event
  and calls contentsDropEvent().

  \sa QWidget::dropEvent()
*/
void KtlQ3ScrollView::viewportDropEvent(QDropEvent* e)
{
    //e->setPoint(viewportToContents(e->pos())); // 2018.11.30
    QDropEvent ev(viewportToContents(e->pos()), e->possibleActions(),
                    e->mimeData(), e->mouseButtons(), e->keyboardModifiers());
    contentsDropEvent(&ev);
    //e->setPoint(contentsToViewport(e->pos())); // 2018.11.30
}

#endif // QT_NO_DRAGANDDROP

/*!\internal

  To provide simple processing of events on the contents, this function
  receives all wheel events sent to the viewport, translates the
  event and calls contentsWheelEvent().

  \sa QWidget::wheelEvent()
*/
#ifndef QT_NO_WHEELEVENT
void KtlQ3ScrollView::viewportWheelEvent(QWheelEvent* e)
{
    /*
       Different than standard mouse events, because wheel events might
       be sent to the focus widget if the widget-under-mouse doesn't want
       the event itself.
    */
    /* QWheelEvent ce(viewportToContents(e->pos()),
        e->globalPos(), e->delta(), e->state()); */
    QWheelEvent ce(viewportToContents(e->pos()),
        e->globalPos(), e->delta(), e->buttons(), e->modifiers());
    contentsWheelEvent(&ce);
    if (ce.isAccepted())
        e->accept();
    else
        e->ignore();
}
#endif

/*! \internal

  To provide simple processing of events on the contents, this function
  receives all context menu events sent to the viewport, translates the
  event and calls contentsContextMenuEvent().
*/
void KtlQ3ScrollView::viewportContextMenuEvent(QContextMenuEvent *e)
{
    //QContextMenuEvent ce(e->reason(), viewportToContents(e->pos()), e->globalPos(), e->state());
    QContextMenuEvent ce(e->reason(), viewportToContents(e->pos()), e->globalPos(), e->modifiers());
    contentsContextMenuEvent(&ce);
    if (ce.isAccepted())
        e->accept();
    else
        e->ignore();
}

/*!
    Returns the component horizontal scroll bar. It is made available
    to allow accelerators, autoscrolling, etc.

    It should not be used for other purposes.

    This function never returns 0.
*/
QScrollBar* KtlQ3ScrollView::horizontalScrollBar() const
{
    return d->hbar;
}

/*!
    Returns the component vertical scroll bar. It is made available to
    allow accelerators, autoscrolling, etc.

    It should not be used for other purposes.

    This function never returns 0.
*/
QScrollBar* KtlQ3ScrollView::verticalScrollBar() const {
    return d->vbar;
}


/*!
    Scrolls the content so that the point (\a x, \a y) is visible with at
    least 50-pixel margins (if possible, otherwise centered).
*/
void KtlQ3ScrollView::ensureVisible(int x, int y)
{
    ensureVisible(x, y, 50, 50);
}

/*!
    \overload

    Scrolls the content so that the point (\a x, \a y) is visible with at
    least the \a xmargin and \a ymargin margins (if possible,
    otherwise centered).
*/
void KtlQ3ScrollView::ensureVisible(int x, int y, int xmargin, int ymargin)
{
    int pw=visibleWidth();
    int ph=visibleHeight();

    int cx=-d->contentsX();
    int cy=-d->contentsY();
    int cw=d->contentsWidth();
    int ch=contentsHeight();

    if (pw < xmargin*2)
        xmargin=pw/2;
    if (ph < ymargin*2)
        ymargin=ph/2;

    if (cw <= pw) {
        xmargin=0;
        cx=0;
    }
    if (ch <= ph) {
        ymargin=0;
        cy=0;
    }

    if (x < -cx+xmargin)
        cx = -x+xmargin;
    else if (x >= -cx+pw-xmargin)
        cx = -x+pw-xmargin;

    if (y < -cy+ymargin)
        cy = -y+ymargin;
    else if (y >= -cy+ph-ymargin)
        cy = -y+ph-ymargin;

    if (cx > 0)
        cx=0;
    else if (cx < pw-cw && cw>pw)
        cx=pw-cw;

    if (cy > 0)
        cy=0;
    else if (cy < ph-ch && ch>ph)
        cy=ph-ch;

    setContentsPos(-cx, -cy);
}

/*!
    Scrolls the content so that the point (\a x, \a y) is in the top-left
    corner.
*/
void KtlQ3ScrollView::setContentsPos(int x, int y)
{
#if 0
    // bounds checking...
    if (QApplication::reverseLayout())
        if (x > d->contentsWidth() - visibleWidth()) x = d->contentsWidth() - visibleWidth();
    else
#endif
        if (x < 0) x = 0;
    if (y < 0) y = 0;
    // Choke signal handling while we update BOTH sliders.
    d->signal_choke=true;
    moveContents(-x, -y);
    d->vbar->setValue(y);
    d->hbar->setValue(x);
    d->signal_choke=false;
}

/*!
    Scrolls the content by \a dx to the left and \a dy upwards.
*/
void KtlQ3ScrollView::scrollBy(int dx, int dy)
{
    setContentsPos(QMAX(d->contentsX()+dx, 0), QMAX(d->contentsY()+dy, 0));
}

/*!
    Scrolls the content so that the point (\a x, \a y) is in the center
    of visible area.
*/
void KtlQ3ScrollView::center(int x, int y)
{
    ensureVisible(x, y, 32000, 32000);
}

/*!
    \overload

    Scrolls the content so that the point (\a x, \a y) is visible with
    the \a xmargin and \a ymargin margins (as fractions of visible
    the area).

    For example:
    \list
    \i Margin 0.0 allows (x, y) to be on the edge of the visible area.
    \i Margin 0.5 ensures that (x, y) is in middle 50% of the visible area.
    \i Margin 1.0 ensures that (x, y) is in the center of the visible area.
    \endlist
*/
void KtlQ3ScrollView::center(int x, int y, float xmargin, float ymargin)
{
    int pw=visibleWidth();
    int ph=visibleHeight();
    ensureVisible(x, y, int(xmargin/2.0*pw+0.5), int(ymargin/2.0*ph+0.5));
}


/*!
    \fn void KtlQ3ScrollView::contentsMoving(int x, int y)

    This signal is emitted just before the contents are moved to
    position (\a x, \a y).

    \sa contentsX(), contentsY()
*/

/*!
    Moves the contents by (\a x, \a y).
*/
void KtlQ3ScrollView::moveContents(int x, int y)
{
    if (-x+visibleWidth() > d->contentsWidth())
#if 0
        if(QApplication::reverseLayout())
            x=QMAX(0,-d->contentsWidth()+visibleWidth());
        else
#endif
            x=QMIN(0,-d->contentsWidth()+visibleWidth());
    if (-y+visibleHeight() > contentsHeight())
        y=QMIN(0,-contentsHeight()+visibleHeight());

    int dx = x - d->vx;
    int dy = y - d->vy;

    if (!dx && !dy)
        return; // Nothing to do

    emit contentsMoving(-x, -y);

    d->vx = x;
    d->vy = y;

    if (d->clipped_viewport || d->static_bg) {
        // Cheap move (usually)
        d->moveAllBy(dx,dy);
    } else if (/*dx && dy ||*/
         (QABS(dy) * 5 > visibleHeight() * 4) ||
         (QABS(dx) * 5 > visibleWidth() * 4)
       )
    {
        // Big move
        if (viewport()->updatesEnabled())
            viewport()->update();
        d->moveAllBy(dx,dy);
    } else if (!d->fake_scroll || d->contentsWidth() > visibleWidth()) {
        // Small move
        clipper()->scroll(dx,dy);
    }
    d->hideOrShowAll(this, true);
}

/*!
    \property KtlQ3ScrollView::contentsX
    \brief the X coordinate of the contents that are at the left edge of
    the viewport.
*/
int KtlQ3ScrollView::contentsX() const
{
    return d->contentsX();
}

/*!
    \property KtlQ3ScrollView::contentsY
    \brief the Y coordinate of the contents that are at the top edge of
    the viewport.
*/
int KtlQ3ScrollView::contentsY() const
{
    return d->contentsY();
}

/*!
    \property KtlQ3ScrollView::contentsWidth
    \brief the width of the contents area
*/
int KtlQ3ScrollView::contentsWidth() const
{
    return d->contentsWidth();
}

/*!
    \property KtlQ3ScrollView::contentsHeight
    \brief the height of the contents area
*/
int KtlQ3ScrollView::contentsHeight() const
{
    return d->vheight;
}

/*!
    Sets the size of the contents area to \a w pixels wide and \a h
    pixels high and updates the viewport accordingly.
*/
void KtlQ3ScrollView::resizeContents(int w, int h)
{
    int ow = d->vwidth;
    int oh = d->vheight;
    d->vwidth = w;
    d->vheight = h;

    d->scrollbar_timer.setSingleShot(true);
    d->scrollbar_timer.start(0 /*, true */ );

    if (d->children.isEmpty() && d->policy == Default)
        setResizePolicy(Manual);

    if (ow > w) {
        // Swap
        int t=w;
        w=ow;
        ow=t;
    }
    // Refresh area ow..w
    if (ow < visibleWidth() && w >= 0) {
        if (ow < 0)
            ow = 0;
        if (w > visibleWidth())
            w = visibleWidth();
        clipper()->update(d->contentsX()+ow, 0, w-ow, visibleHeight());
    }

    if (oh > h) {
        // Swap
        int t=h;
        h=oh;
        oh=t;
    }
    // Refresh area oh..h
    if (oh < visibleHeight() && h >= 0) {
        if (oh < 0)
            oh = 0;
        if (h > visibleHeight())
            h = visibleHeight();
        clipper()->update(0, d->contentsY()+oh, visibleWidth(), h-oh);
    }
}

/*!
    Calls update() on a rectangle defined by \a x, \a y, \a w, \a h,
    translated appropriately. If the rectangle is not visible, nothing
    is repainted.

    \sa repaintContents()
*/
void KtlQ3ScrollView::updateContents(int x, int y, int w, int h)
{
    if (!isVisible() || !updatesEnabled())
        return;

    QWidget* vp = viewport();

    // Translate
    x -= d->contentsX();
    y -= d->contentsY();

    if (x < 0) {
        w += x;
        x = 0;
    }
    if (y < 0) {
        h += y;
        y = 0;
    }

    if (w < 0 || h < 0)
        return;
    if (x > visibleWidth() || y > visibleHeight())
        return;

    if (w > visibleWidth())
        w = visibleWidth();
    if (h > visibleHeight())
        h = visibleHeight();

    if (d->clipped_viewport) {
        // Translate clipper() to viewport()
        x -= d->clipped_viewport->x();
        y -= d->clipped_viewport->y();
    }

    vp->update(x, y, w, h);
}

/*!
    \overload

    Updates the contents in rectangle \a r
*/
void KtlQ3ScrollView::updateContents(const QRect& r)
{
    updateContents(r.x(), r.y(), r.width(), r.height());
}

/*!
    \overload
*/
void KtlQ3ScrollView::updateContents()
{
    updateContents(d->contentsX(), d->contentsY(), visibleWidth(), visibleHeight());
}

/*!
    \overload

    Repaints the contents of rectangle \a r. If \a erase is true the
    background is cleared using the background color.
*/
void KtlQ3ScrollView::repaintContents(const QRect& r, bool erase)
{
    repaintContents(r.x(), r.y(), r.width(), r.height(), erase);
}


/*!
    \overload

    Repaints the contents. If \a erase is true the background is
    cleared using the background color.
*/
void KtlQ3ScrollView::repaintContents(bool erase)
{
    repaintContents(d->contentsX(), d->contentsY(), visibleWidth(), visibleHeight(), erase);
}


/*!
    Calls repaint() on a rectangle defined by \a x, \a y, \a w, \a h,
    translated appropriately. If the rectangle is not visible, nothing
    is repainted. If \a erase is true the background is cleared using
    the background color.

    \sa updateContents()
*/
void KtlQ3ScrollView::repaintContents(int x, int y, int w, int h, bool /*erase*/)
{
    if (!isVisible() || !updatesEnabled())
        return;

    QWidget* vp = viewport();

    // Translate logical to clipper()
    x -= d->contentsX();
    y -= d->contentsY();

    if (x < 0) {
        w += x;
        x = 0;
    }
    if (y < 0) {
        h += y;
        y = 0;
    }

    if (w < 0 || h < 0)
        return;
    if (w > visibleWidth())
        w = visibleWidth();
    if (h > visibleHeight())
        h = visibleHeight();

    if (d->clipped_viewport) {
        // Translate clipper() to viewport()
        x -= d->clipped_viewport->x();
        y -= d->clipped_viewport->y();
    }

    vp->update(x, y, w, h);
}


/*!
    For backward-compatibility only. It is easier to use
    drawContents(QPainter*,int,int,int,int).

    The default implementation translates the painter appropriately
    and calls drawContents(QPainter*,int,int,int,int). See
    drawContents() for an explanation of the parameters \a p, \a
    offsetx, \a offsety, \a clipx, \a clipy, \a clipw and \a cliph.
*/
void KtlQ3ScrollView::drawContentsOffset(QPainter* p, int offsetx, int offsety, int clipx, int clipy, int clipw, int cliph)
{
    p->translate(-offsetx,-offsety);
    drawContents(p, clipx, clipy, clipw, cliph);
}

/*!
    \fn void KtlQ3ScrollView::drawContents(QPainter* p, int clipx, int clipy, int clipw, int cliph)

    Reimplement this function if you are viewing a drawing area rather
    than a widget.

    The function should draw the rectangle (\a clipx, \a clipy, \a
    clipw, \a cliph) of the contents using painter \a p. The clip
    rectangle is in the scrollview's coordinates.

    For example:
    \snippet doc/src/snippets/code/src_qt3support_widgets_q3scrollview.cpp 4

    The clip rectangle and translation of the painter \a p is already
    set appropriately.
*/
void KtlQ3ScrollView::drawContents(QPainter*, int, int, int, int)
{
}


/*!
    \reimp
*/
void KtlQ3ScrollView::frameChanged()
{
//     // slight ugle-hack - the listview header needs readjusting when
//     // changing the frame
//     if (Q3ListView *lv = qobject_cast<Q3ListView *>(this))
//         lv->triggerUpdate();
    KtlQ3Frame::frameChanged();
    updateScrollBars();
}


/*!
    Returns the viewport widget of the scrollview. This is the widget
    containing the contents widget or which is the drawing area.
*/
QWidget* KtlQ3ScrollView::viewport() const
{
    if (d->clipped_viewport)
        return  d->clipped_viewport;
    return d->viewport;
}

/*!
    Returns the clipper widget. Contents in the scrollview are
    ultimately clipped to be inside the clipper widget.

    You should not need to use this function.

    \sa visibleWidth(), visibleHeight()
*/
QWidget* KtlQ3ScrollView::clipper() const
{
    return d->viewport;
}

/*!
    \property KtlQ3ScrollView::visibleWidth
    \brief the horizontal amount of the content that is visible
*/
int KtlQ3ScrollView::visibleWidth() const
{
    return clipper()->width();
}

/*!
    \property KtlQ3ScrollView::visibleHeight
    \brief the vertical amount of the content that is visible
*/
int KtlQ3ScrollView::visibleHeight() const
{
    return clipper()->height();
}


void KtlQ3ScrollView::changeFrameRect(const QRect& r)
{
    QRect oldr = frameRect();
    if (oldr != r) {
        QRect cr = contentsRect();
        QRegion fr(frameRect());
        fr = fr.subtracted(contentsRect());
        setFrameRect(r);
        if (isVisible()) {
            cr = cr.intersected(contentsRect());
            fr = fr.united(frameRect());
            fr = fr.subtracted(cr);
            if (!fr.isEmpty())
                update(fr);
        }
    }
}


/*!
    Sets the margins around the scrolling area to \a left, \a top, \a
    right and \a bottom. This is useful for applications such as
    spreadsheets with "locked" rows and columns. The marginal space is
    \e inside the frameRect() and is left blank; reimplement
    drawFrame() or put widgets in the unused area.

    By default all margins are zero.

    \sa frameChanged()
*/
void KtlQ3ScrollView::setMargins(int left, int top, int right, int bottom)
{
    if (left == d->l_marg &&
         top == d->t_marg &&
         right == d->r_marg &&
         bottom == d->b_marg)
        return;

    d->l_marg = left;
    d->t_marg = top;
    d->r_marg = right;
    d->b_marg = bottom;
    updateScrollBars();
}


/*!
    Returns the left margin.

    \sa setMargins()
*/
int KtlQ3ScrollView::leftMargin() const
{
    return d->l_marg;
}


/*!
    Returns the top margin.

    \sa setMargins()
*/
int KtlQ3ScrollView::topMargin() const
{
    return d->t_marg;
}


/*!
    Returns the right margin.

    \sa setMargins()
*/
int KtlQ3ScrollView::rightMargin() const
{
    return d->r_marg;
}


/*!
    Returns the bottom margin.

    \sa setMargins()
*/
int KtlQ3ScrollView::bottomMargin() const
{
    return d->b_marg;
}

/*!
    \reimp
*/
bool KtlQ3ScrollView::focusNextPrevChild(bool next)
{
    //  Makes sure that the new focus widget is on-screen, if
    //  necessary by scrolling the scroll view.
    bool retval = KtlQ3Frame::focusNextPrevChild(next);
    if (retval) {
        QWidget *w = window()->focusWidget();
        if (isAncestorOf(w)) {
            QSVChildRec *r = d->ancestorRec(w);
           if (r && (r->child == w || w->isVisibleTo(r->child))) {
                QPoint cp = r->child->mapToGlobal(QPoint(0, 0));
                QPoint cr = w->mapToGlobal(QPoint(0, 0)) - cp;
                ensureVisible(r->x + cr.x() + w->width()/2, r->y + cr.y() + w->height()/2,
                              w->width()/2, w->height()/2);
            }
        }
    }
    return retval;
}



/*!
    When a large numbers of child widgets are in a scrollview,
    especially if they are close together, the scrolling performance
    can suffer greatly. If \a y is true the scrollview will use an
    extra widget to group child widgets.

    Note that you may only call enableClipper() prior to adding
    widgets.
*/
void KtlQ3ScrollView::enableClipper(bool y)        // note: this method as of 2018.11.30 is unused
{
    if (!d->clipped_viewport == !y)
        return;
    if (d->children.count())
        qFatal("May only call KtlQ3ScrollView::enableClipper() before adding widgets");
    if (y) {
        d->clipped_viewport = new KtlQClipperWidget(clipper(), "qt_clipped_viewport", QFlag(d->flags));
        d->clipped_viewport->setGeometry(-coord_limit/2,-coord_limit/2,
                                         coord_limit,coord_limit);
        //d->clipped_viewport->setBackgroundMode(d->viewport->backgroundMode());
        d->clipped_viewport->setBackgroundRole(d->viewport->backgroundRole());
        //d->viewport->setBackgroundMode(NoBackground); // no exposures for this // 2018.11.30
        d->viewport->setAttribute(Qt::WA_NoSystemBackground); // hope this is the correct replacement for above
        d->viewport->removeEventFilter(this);
        d->clipped_viewport->installEventFilter(this);
        d->clipped_viewport->show();
    } else {
        delete d->clipped_viewport;
        d->clipped_viewport = 0;
    }
}

/*!
    Sets the scrollview to have a static background if \a y is true,
    or a scrolling background if \a y is false. By default, the
    background is scrolling.

    Be aware that this mode is quite slow, as a full repaint of the
    visible area has to be triggered on every contents move.

    \sa hasStaticBackground()
*/
void  KtlQ3ScrollView::setStaticBackground(bool y)
{
    d->static_bg = y;
}

/*!
    Returns true if KtlQ3ScrollView uses a static background; otherwise
    returns false.

    \sa setStaticBackground()
*/
bool KtlQ3ScrollView::hasStaticBackground() const
{
    return d->static_bg;
}

/*!
    \overload

    Returns the point \a p translated to a point on the viewport()
    widget.
*/
QPoint KtlQ3ScrollView::contentsToViewport(const QPoint& p) const
{
    if (d->clipped_viewport) {
        return QPoint(p.x() - d->contentsX() - d->clipped_viewport->x(),
                       p.y() - d->contentsY() - d->clipped_viewport->y());
    } else {
        return QPoint(p.x() - d->contentsX(),
                       p.y() - d->contentsY());
    }
}

/*!
    \overload

    Returns the point on the viewport \a vp translated to a point in
    the contents.
*/
QPoint KtlQ3ScrollView::viewportToContents(const QPoint& vp) const
{
    if (d->clipped_viewport) {
        return QPoint(vp.x() + d->contentsX() + d->clipped_viewport->x(),
                       vp.y() + d->contentsY() + d->clipped_viewport->y());
    } else {
        return QPoint(vp.x() + d->contentsX(),
                       vp.y() + d->contentsY());
    }
}


/*!
    Translates a point (\a x, \a y) in the contents to a point (\a vx,
    \a vy) on the viewport() widget.
*/
void KtlQ3ScrollView::contentsToViewport(int x, int y, int& vx, int& vy) const
{
    const QPoint v = contentsToViewport(QPoint(x,y));
    vx = v.x();
    vy = v.y();
}

/*!
    Translates a point (\a vx, \a vy) on the viewport() widget to a
    point (\a x, \a y) in the contents.
*/
void KtlQ3ScrollView::viewportToContents(int vx, int vy, int& x, int& y) const
{
    const QPoint c = viewportToContents(QPoint(vx,vy));
    x = c.x();
    y = c.y();
}

/*!
    \reimp
*/
QSize KtlQ3ScrollView::sizeHint() const
{
    if (d->use_cached_size_hint && d->cachedSizeHint.isValid())
        return d->cachedSizeHint;

    //constPolish(); // 2018.11.30
    ensurePolished();
    int f = 2 * frameWidth();
    int h = fontMetrics().height();
    QSize sz(f, f);
    if (d->policy > Manual) {
        QSVChildRec *r = d->children.first();
        if (r) {
            QSize cs = r->child->sizeHint();
            if (cs.isValid())
                sz += cs.boundedTo(r->child->maximumSize());
            else
                sz += r->child->size();
        }
    } else {
        sz += QSize(d->contentsWidth(), contentsHeight());
    }
    if (d->vMode == AlwaysOn)
        sz.setWidth(sz.width() + d->vbar->sizeHint().width());
    if (d->hMode == AlwaysOn)
        sz.setHeight(sz.height() + d->hbar->sizeHint().height());
    return sz.expandedTo(QSize(12 * h, 8 * h))
             .boundedTo(QSize(36 * h, 24 * h));
}


/*!
    \reimp
*/
QSize KtlQ3ScrollView::minimumSizeHint() const
{
    int h = fontMetrics().height();
    if (h < 10)
        h = 10;
    int f = 2 * frameWidth();
    return QSize((6 * h) + f, (4 * h) + f);
}


/*!
    \reimp

    (Implemented to get rid of a compiler warning.)
*/
void KtlQ3ScrollView::drawContents(QPainter *)
{
}

#ifndef QT_NO_DRAGANDDROP

/*!
  \internal
*/
void KtlQ3ScrollView::startDragAutoScroll()
{
    if (!d->autoscroll_timer.isActive()) {
        d->autoscroll_time = initialScrollTime;
        d->autoscroll_accel = initialScrollAccel;
        d->autoscroll_timer.start(d->autoscroll_time);
    }
}


/*!
  \internal
*/
void KtlQ3ScrollView::stopDragAutoScroll()
{
    d->autoscroll_timer.stop();
}


/*!
  \internal
*/
void KtlQ3ScrollView::doDragAutoScroll()
{
    QPoint p = d->viewport->mapFromGlobal(QCursor::pos());

    if (d->autoscroll_accel-- <= 0 && d->autoscroll_time) {
        d->autoscroll_accel = initialScrollAccel;
        d->autoscroll_time--;
        d->autoscroll_timer.start(d->autoscroll_time);
    }
    int l = QMAX(1, (initialScrollTime- d->autoscroll_time));

    int dx = 0, dy = 0;
    if (p.y() < autoscroll_margin) {
        dy = -l;
    } else if (p.y() > visibleHeight() - autoscroll_margin) {
        dy = +l;
    }
    if (p.x() < autoscroll_margin) {
        dx = -l;
    } else if (p.x() > visibleWidth() - autoscroll_margin) {
        dx = +l;
    }
    if (dx || dy) {
        scrollBy(dx,dy);
    } else {
        stopDragAutoScroll();
    }
}


/*!
    \property KtlQ3ScrollView::dragAutoScroll
    \brief whether autoscrolling in drag move events is enabled

    If this property is set to true (the default), the KtlQ3ScrollView
    automatically scrolls the contents in drag move events if the user
    moves the cursor close to a border of the view. Of course this
    works only if the viewport accepts drops. Specifying false
    disables this autoscroll feature.
*/

void KtlQ3ScrollView::setDragAutoScroll(bool b)
{
    d->drag_autoscroll = b;
}

bool KtlQ3ScrollView::dragAutoScroll() const
{
    return d->drag_autoscroll;
}

#endif // QT_NO_DRAGANDDROP

/*!\internal
 */
void KtlQ3ScrollView::setCachedSizeHint(const QSize &sh) const
{
    if (isVisible() && !d->cachedSizeHint.isValid())
        d->cachedSizeHint = sh;
}

/*!\internal
 */
void KtlQ3ScrollView::disableSizeHintCaching()
{
    d->use_cached_size_hint = false;
}

/*!\internal
 */
QSize KtlQ3ScrollView::cachedSizeHint() const
{
    return d->use_cached_size_hint ? d->cachedSizeHint : QSize();
}

// QT_END_NAMESPACE

// #include "ktlq3scrollview.moc"
