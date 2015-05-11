//
// C++ Implementation: flowconnectorlist
//
// Description: 
//
//
// Author: David Saxton, Alan Grimes, Zoltan Padrah <zoltan.padrah@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <Qt/qpointer.h>
#include <Qt/qlist.h>

#include "flowconnectorlist.h"

FlowConnectorList::FlowConnectorList ( const std::list<T> & l ) : flowList(l) {
    FlowConnectorList::iterator it, end = flowList.end();
    for( it = flowList.begin(); it != end; it++)
        list.append( CAST_POINTER *it);
}

QList<T> & FlowConnectorList::operator= ( const std::list<T> & l ) {   // O(n)
    flowList = l;
    list.clear();
    FlowConnectorList::iterator it, end = flowList.end();
    for( it = flowList.begin(); it != end; it++)
        list.append( CAST_POINTER *it);

    return flowList;
}

bool FlowConnectorList::operator== ( const std::list<T> & l ) const {
    return flowList == l;
}

void FlowConnectorList::insert ( iterator pos, size_type n, const T & x ) {    // O(n)
    list.insert( convertIterator(pos) ,n, CAST_POINTER x);
    flowList.insert(pos,n,x);
}

FlowConnectorList::iterator FlowConnectorList::fromLast () {
    return flowList.fromLast();
}

FlowConnectorList::const_iterator FlowConnectorList::fromLast () const {
    return flowList.fromLast();
}
