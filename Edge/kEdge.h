/*
 * edge.h
 *
 *  Created on: 2015-1-13
 *      Author: liyouhuan
 */

#ifndef EDGE_H_
#define EDGE_H_

#include<iostream>
#include<sstream>
#include<stdlib.h>
using namespace std;

class kEdge
{
public:
	kEdge();
	~kEdge();
	static const int MAX_FREQ = 255;
	static const bool e_debug = false;

	bool initial();

	int update_edge(int _freq, int _t_cur);

	int insert_tail(int _freq, int _t_cur);

	/* return left size */
	int drop_head(int _t_cur);

	/* get rising strength */
	double rising();

	double get_delta();

	bool isEmpty();

	int length();

	string to_str(string _prefix = "");

	int time_stamp;
private:
	double ris;
	double ris_delta;
	int lis_len;
	int head;
	int size;
	int* rlen;
	int* sval_index;
	int* freq_seq;
	int* bs_tail_index;

	void drop_one();
	void update_rising();
	static double one_rising(int _rlen, int _sval, int _freq);
	int rising_length(int _i);
	string one_str(int _i);
	string bs_str();
	string freq_str();
};


#endif /* EDGE_H_ */
