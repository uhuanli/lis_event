/*
 * Tgraph.cpp
 *
 *  Created on: 2015-1-13
 *      Author: liyouhuan
 */
#include "Tgraph.h"

Tgraph::Tgraph()
{

}

int Tgraph::size()
{
	return this->str2edge.size();
}

bool Tgraph::add(const string & _str, kEdge* _eptr)
{
	this->str2edge.insert(pair<string, kEdge*>(_str, _eptr));
	return true;
}

