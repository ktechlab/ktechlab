/***************************************************************************
 *   Copyright (C) 1999-2005 Trolltech AS                                  *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This file may be distributed and/or modified under the terms of the   *
 *   GNU General Public License version 2 as published by the Free         *
 *   Software Foundation                                                   *
 ***************************************************************************/

#ifndef KTL_CANVASITEMLIST_H_
#define KTL_CANVASITEMLIST_H_

#include <QList>

#include <map>

class KtlQCanvasItem;

typedef std::multimap< double, KtlQCanvasItem* > SortedCanvasItems;


class KtlQCanvasItemList : public QList<KtlQCanvasItem*>
{
    public:
        void sort();
        KtlQCanvasItemList operator+(const KtlQCanvasItemList &l) const;
};


#endif // KTL_CANVASITEMLIST_H_
