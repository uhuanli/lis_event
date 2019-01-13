/*
 * Ugraph.h
 *
 *  Created on: 2015-2-8
 *      Author: liyouhuan
 */

#ifndef UGRAPH_H_
#define UGRAPH_H_
#include "../util/util.h"

class Ugraph{
public:
	Ugraph();

	bool add_edge(const string _edge);
	bool del_edge(const string _edge);
	bool update_D_I(map<string, double>* _s2d);
	bool getConnectedComponent(vector<string>& vcc_str);
	bool getConnectedComponent(vector<string>& _vec_edges, stringstream& _ss);

private:
	map<string, int> vertex2id;
	map<int, string> id2vertex;

	bool parseEdges(vector<string>& _vec_edges, vector<pair<int, int> >& pair_edges);
	bool getCC(vector<pair<int, int> >& pair_edges, vector<int> & _cc);
};


#endif /* UGRAPH_H_ */
