/*
 * TimeWindow.h
 *
 *  Created on: 2015-1-13
 *      Author: liyouhuan
 */

#ifndef TIMEWINDOW_H_
#define TIMEWINDOW_H_
#include "../util/util.h"
#include "Tgraph.h"
using namespace std;

class TimeWindow
{
public:
	TimeWindow();
	void initial();
	/* push edge freq in new interval and return updated edges of G_w */
	map<string, double>* update(map<string, int> & _edge2freq, int _t_cur, set<string>& removed_edges);

	int gc();
private:
	/* created new Tgraph and update for insertion */
	Tgraph* new_Tgraph_insert(map<string, int> & _edge2freq, int _t_cur);

	Tgraph* del_Tgraph(int _t_cur);

	kEdge* get_active_edge(const string&  _str);

	Tgraph** graphs;
	S2Eptr_map active_edges;

	int head;
	int size;
};


#endif /* TIMEWINDOW_H_ */
