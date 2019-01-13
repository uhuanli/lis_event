/*
 * inter.h
 *
 *  Created on: 2015-3-23
 *      Author: liyouhuan
 */
#ifndef INTER_H_
#define INTER_H_

#include "../Dense/Dense.h"
#include "../Rising/Rising.h"

class inter
{
public:
	inter();
	~inter();

	void inter_run();
private:
	Dense* dense;
	Rising* rising;
	ofstream f_inter_result;

	void init_inter();
	map<iiEdge, double>* lis_next();
	void dense_graph();
};



#endif /* INTER_H_ */
