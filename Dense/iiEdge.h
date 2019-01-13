/*
 * iiEdge.h
 *
 *  Created on: 2015-3-6
 *      Author: liyouhuan
 */

#ifndef IIEDGE_H_
#define IIEDGE_H_

#include "../util/util.h"

class iiEdge
{
public:
	int from;
	int to;

	iiEdge();
	iiEdge(int _f, int _to);
	bool operator > (const iiEdge &  _e)const;
	bool operator < (const iiEdge &  _e)const;
	bool operator ==(const iiEdge &  _e)const;
	iiEdge& operator = (const iiEdge &  _e);

	string to_str()const;
};


#endif /* IIEDGE_H_ */
