/***************************************************************************
 *   Copyright (C) 1999-2005 Trolltech AS                                  *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This file may be distributed and/or modified under the terms of the   *
 *   GNU General Public License version 2 as published by the Free         *
 *   Software Foundation                                                   *
 ***************************************************************************/

#include "canvasitemlist.h"

//#include <q3tl.h>
#include <QtAlgorithms>

void KtlQCanvasItemList::sort()
{
    // qHeapSort(*  /* ((QList<KtlQCanvasItemPtr>*)) */ this); // 2018.12.07
    qSort(*this);
}

KtlQCanvasItemList KtlQCanvasItemList::operator+(const KtlQCanvasItemList &l) const
{
    KtlQCanvasItemList l2(*this);
    for (const_iterator it = l.begin(); it != l.end(); ++it)
        l2.append(*it);
    return l2;
}
