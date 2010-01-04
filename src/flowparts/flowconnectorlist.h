//
// C++ Interface: flowconnectorlist
//
// Description: 
//
//
// Author: David Saxton, Alan Grimes, Zoltan Padrah <zoltan.padrah@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FLOWCONNECTORLIST_H
#define FLOWCONNECTORLIST_H

class Connector;
class FlowConnector;

// #include "flowconnector.h"

// these typedef's shoud go in a separate header one day
typedef QValueList< QGuardedPtr<Connector> > ConnectorList;


/**
 * @short a list of connector between FlowNodes
 * @author David Saxton, Alan Grimes, Zoltan Padrah <zoltan.padrah@gmail.com>
 *
 * This class implements a list of FlowConnector objects; this class fulfills two
 * requirements:
 * 1. it provides type safety for classes related to flowparts
 * 2. can be cast to a generic ConnectorList, to be used in other contexts
 * 
 * For QValueList interface see http://doc.trolltech.com/3.3/qvaluelist.html
 */

/*
	Notations used in the source code:
	 O(n) : the method has this complexitiy
	 assert ? : in that method some assertions could be made, considering 
			that the two list should have the same contents
 */
 
class FlowConnectorList {
	
public :
	
	// cast operator, for casting it to ConnectorList
	operator ConnectorList() {
		return list;
	}
	
	// QValueList's interface
	typedef QGuardedPtr<FlowConnector> T;
	
#define CAST_POINTER 	(QGuardedPtr<Connector>)(Connector *)(FlowConnector *)
	
	typedef QValueListIterator<T> iterator;
	typedef QValueListConstIterator<T> const_iterator;
	typedef T value_type;
	typedef value_type * pointer;
	typedef const value_type * const_pointer;
	typedef value_type & reference;
	typedef const value_type & const_reference;
	typedef size_t size_type;
	
	FlowConnectorList () : list(), flowList() { }

	FlowConnectorList ( const QValueList<T> & l ) : flowList(l) { // O(n)
		FlowConnectorList::iterator it, end = flowList.end();
		for( it = flowList.begin(); it != end; it++)
			list.append( CAST_POINTER *it);
	}

	FlowConnectorList ( const std::list<T> & l ) : flowList(l) {
		FlowConnectorList::iterator it, end = flowList.end();
		for( it = flowList.begin(); it != end; it++)
			list.append( CAST_POINTER *it);
	}

	~FlowConnectorList () { }	// leak check ?

	QValueList<T> & operator= ( const QValueList<T> & l ) { // -> O(n)
		flowList = l;
		list.clear();
		FlowConnectorList::iterator it, end = flowList.end();
		for( it = flowList.begin(); it != end; it++)
			list.append( CAST_POINTER  *it);
		return flowList;
	}

	QValueList<T> & operator= ( const std::list<T> & l ) {	// O(n)
		flowList = l;
		list.clear();
		FlowConnectorList::iterator it, end = flowList.end();
		for( it = flowList.begin(); it != end; it++)
			list.append( CAST_POINTER *it);
	
		return flowList;
	}

	bool operator== ( const std::list<T> & l ) const {
		return flowList == l;
	}

	bool operator== ( const QValueList<T> & l ) const {
		return flowList == l;
	}

	bool operator!= ( const QValueList<T> & l ) const {
		return flowList != l;
	}

	iterator begin () {
		return flowList.begin();
	}

	const_iterator begin () const {
		return flowList.begin(); // ? 
	}

	const_iterator constBegin () const {
		return flowList.constBegin();
	}

	iterator end () {
		return flowList.end();
	}

	const_iterator end () const {
		return flowList.end(); 	// ?
	}

	const_iterator constEnd () const {
		return flowList.constEnd();
	}

	iterator insert ( iterator it, const T & x ) {	// O(n)	
		list.insert(  convertIterator( it ), CAST_POINTER x);
		return flowList.insert(it,x);
	}

	uint remove ( const T & x ) {
		list.remove( CAST_POINTER  x);
		return flowList.remove(x);
	}

	void clear () {
		flowList.clear();
		list.clear();
	}

	QValueList<T> & operator<< ( const T & x ) {
		list << CAST_POINTER  x;
		return flowList << x;
	}

	size_type size () const {	// assert ?
		return flowList.size();
	}

	bool empty () const {		// assert ?
		return flowList.empty();
	}

	void push_front ( const T & x ) {
		list.push_front(CAST_POINTER x);
		flowList.push_front(x);
	}

	void push_back ( const T & x ) {
		list.push_back(CAST_POINTER x);
		flowList.push_back(x);
	}

	iterator erase ( iterator it ){		// O(n)
		list.erase( convertIterator(it) );
		return flowList.erase(it);
	}

	iterator erase ( iterator first, iterator last ){	// O(n)
		list.erase( convertIterator(first), convertIterator(last) );
		return flowList.erase(first, last);
	}

	reference front () {
		return flowList.front();
	}

	const_reference front () const {
		return flowList.front();
	}

	reference back () {
		return flowList.back();
	}

	const_reference back () const {
		return flowList.back();
	}

	void pop_front () {
		flowList.pop_front();
		list.pop_front();
	}

	void pop_back () {
		flowList.pop_back();
		list.pop_back();
	}

	void insert ( iterator pos, size_type n, const T & x ) { 	// O(n)
		list.insert( convertIterator(pos) ,n, CAST_POINTER x); 
		flowList.insert(pos,n,x);
	}

	QValueList<T> operator+ ( const QValueList<T> & l ) const {
		return flowList + l;
	}

	QValueList<T> & operator+= ( const QValueList<T> & l ) {	// O(n)
		const_iterator end = l.end();
		for(const_iterator it = l.begin(); it != end; it++)
			list.append( CAST_POINTER  *it );
		return flowList += l;
	}

	iterator fromLast () {
		return flowList.fromLast();
	}

	const_iterator fromLast () const {
		return flowList.fromLast();
	}

	bool isEmpty () const {
		return flowList.isEmpty();
	}

	iterator append ( const T & x ){
		list.append(CAST_POINTER x);
		return flowList.append(x);
	}

	iterator prepend ( const T & x ){
		list.prepend(CAST_POINTER x);
		return flowList.prepend(x);
	}

	iterator remove ( iterator it ){
	// -> O(n)
		list.remove( convertIterator( it ) );
		return flowList.remove(it);
	}

	T & first () {	// assert ?
		return flowList.first();
	}

	const T & first () const {			// assert ?
		return flowList.first();
	}

	T & last () {					// assert ?
		return flowList.last();
	}

	const T & last () const {			// assert ?
		return flowList.last();
	}

	T & operator[] ( size_type i ) {		// assert ?
		return flowList[i];
	}

	const T & operator[] ( size_type i ) const {	// assert ?
		return flowList[i];
	}

	iterator at ( size_type i ) {			// assert ?
		return flowList.at(i);
	}

	const_iterator at ( size_type i ) const {	// assert ?
		return flowList.at(i);
	}

	iterator find ( const T & x ) {			// assert ?
		return flowList.find(x);
	}

	const_iterator find ( const T & x ) const {	// assert ?
		return flowList.find(x);
	}

	iterator find ( iterator it, const T & x ) {	// assert ?
		return flowList.find(it, x);
	}
 
	const_iterator find ( const_iterator it, const T & x ) const {	// assert ?	
		return flowList.find(it, x);
	}

	int findIndex ( const T & x ) const {		// assert ?
		return flowList.findIndex(x);
	}

	size_type contains ( const T & x ) const {
		return flowList.contains(x);
	}

	size_type count () const {			// assert ?
		return flowList.count();
	}

	QValueList<T> & operator+= ( const T & x ) {
		list += CAST_POINTER x;
		return flowList += x;
	}

private: 
	ConnectorList list;
	QValueList< T > flowList;
	
	/**
	 *    Converts an iterator from FlowConnector list to Connector list. Complexity: O(n) !
	 * @param orig original iterator from FlowConnector list
	 * @return iterator converted to Connector list
	 */
	ConnectorList::iterator convertIterator(QValueList< T >::iterator orig){
		ConnectorList::iterator it2 = list.begin();
		for(QValueList< T >::iterator it = flowList.begin(); it != orig; it++)
			it2++;
		return it2;
	}
	
};

#endif
