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

#include "flowconnectorlist.h"

#include "flowconnector.h"

#include <QPointer>
#include <QList>

FlowConnectorList::FlowConnectorList ( const QList<T> & l ) : flowList(l) { // O(n)
    FlowConnectorList::iterator it, end = flowList.end();
    for( it = flowList.begin(); it != end; it++)
        list.append( CAST_POINTER *it);
}

// FlowConnectorList::FlowConnectorList ( const std::list<FlowConnectorList::T> & l ) : flowList(l) {
//     FlowConnectorList::iterator it, end = flowList.end();
//     for( it = flowList.begin(); it != end; it++)
//         list.append( CAST_POINTER *it);
// }

QList<FlowConnectorList::T> & FlowConnectorList::operator= ( const QList<FlowConnectorList::T> & l ) { // -> O(n)
    flowList = l;
    list.clear();
    FlowConnectorList::iterator it, end = flowList.end();
    for( it = flowList.begin(); it != end; it++)
        list.append( CAST_POINTER  *it);
    return flowList;
}

// QList<FlowConnectorList::T> & FlowConnectorList::operator= ( const std::list<FlowConnectorList::T> & l ) {   // O(n)
//     flowList = l;
//     list.clear();
//     FlowConnectorList::iterator it, end = flowList.end();
//     for( it = flowList.begin(); it != end; it++)
//         list.append( CAST_POINTER *it);
//
//     return flowList;
// }

// bool FlowConnectorList::operator== ( const std::list<FlowConnectorList::T> & l ) const {
//     return flowList == l;
// }

FlowConnectorList::iterator FlowConnectorList::insert ( FlowConnectorList::iterator it, const FlowConnectorList::T & x ) {  // O(n)
    list.insert(  convertIterator( it ), CAST_POINTER x);
    return flowList.insert(it,x);
}

uint FlowConnectorList::remove ( const FlowConnectorList::T & x ) {
    list.removeAll( CAST_POINTER  x);
    return flowList.removeAll(x);
}

QList<FlowConnectorList::T> & FlowConnectorList::operator<< ( const FlowConnectorList::T & x ) {
    list << CAST_POINTER  x;
    return flowList << x;
}

void FlowConnectorList::push_front ( const FlowConnectorList::T & x ) {
    list.push_front(CAST_POINTER x);
    flowList.push_front(x);
}

void FlowConnectorList::push_back ( const FlowConnectorList::T & x ) {
    list.push_back(CAST_POINTER x);
    flowList.push_back(x);
}

// void FlowConnectorList::insert ( FlowConnectorList::iterator pos, size_type n, const FlowConnectorList::T & x ) {    // O(n)
//     list.insert( convertIterator(pos) ,n, CAST_POINTER x);
//     flowList.insert(pos,n,x);
// }

QList<FlowConnectorList::T> & FlowConnectorList::operator+= ( const QList<FlowConnectorList::T> & l ) {  // O(n)
    const_iterator end = l.end();
    for(const_iterator it = l.begin(); it != end; it++)
        list.append( CAST_POINTER  *it );
    return flowList += l;
}

// FlowConnectorList::iterator FlowConnectorList::fromLast () {
//     return flowList.fromLast();
// }

FlowConnectorList::iterator FlowConnectorList::append ( const FlowConnectorList::T & x ){
    list.append(CAST_POINTER x);
    // return flowList.append(x);
    flowList.append(x);
    iterator ret = flowList.end();
    --ret;
    return ret;
}

FlowConnectorList::iterator FlowConnectorList::prepend ( const FlowConnectorList::T & x ){
    list.prepend(CAST_POINTER x);
    //return flowList.prepend(x);
    flowList.prepend(x);
    return flowList.begin();
}

QList<FlowConnectorList::T> & FlowConnectorList::operator+= ( const FlowConnectorList::T & x ) {
    list += CAST_POINTER x;
    return flowList += x;
}

// FlowConnectorList::const_iterator FlowConnectorList::fromLast () const {
//     return flowList.fromLast();
// }
