/*
 * graph.h
 *
 *  Created on: 2015-1-13
 *      Author: liyouhuan
 */

#ifndef TGRAPH_H_
#define TGRAPH_H_

#include<iostream>
#include<tr1/unordered_map>
#include "../Edge/kEdge.h"
using namespace std;
using namespace tr1;

typedef unordered_map<string, kEdge*> S2Eptr_map;

class Tgraph
{
public:
	Tgraph();

	int size();
	bool add(const string & _str, kEdge* _eptr);

	S2Eptr_map str2edge;
};


#endif /* TGRAPH_H_ */
