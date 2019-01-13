/*
 * prefixTree.h
 *
 *  Created on: 2015-3-6
 *      Author: liyouhuan
 */

#ifndef PREFIXTREE_H_
#define PREFIXTREE_H_

#include "../util/util.h"
#include "../Dense/iiEdge.h"
#include "PNode.h"

class PrefixTree
{
public:
	PrefixTree();
	~PrefixTree();
	bool update(map<iiEdge, double>& _edge2delta_w);

	string tostr_ouputdense();

private:
	PNode* root;
	map<int, PNode*> id2nodelist;
	set<PNode*> cur_denseC;
	set<PNode*> output_denseC;
	ofstream f_dense_c;
	ofstream f_output_c;
	ofstream f_bound;
	ofstream prefix_log;
	int node_num;

//	static int buf_vset[1000];

	bool update_edge(int _from, int _to, double _delta_w);
	bool update_positive(int _from, int _to, double _delta_w);
	bool update_negative(int _from, int _to, double _delta_w);
	bool update_positive_both(PNode* _dense_both);
	bool update_positive_either(PNode* _dense_either, int _other_v, double _delta_w);

	double get_weight(int _from, int _to);
	double update_density(PNode* _dense);
	double cal_density(set<int>& _v_set);
	/* true for dense
	 * false for not */
	PNode* add_if_dense(set<int>& _v_set);
	PNode* add_dense2index(set<int>& _v_set, double _density);
	void explore(set<int>& _v_set, set<int>& _neighbors, int _i, int _imax);

	set<PNode*>* denseC_containV(int _v);
	set<PNode*>* denseC_containBoth(int _v1, int _v2);
	set<PNode*>* denseC_containBoth(set<PNode*>* _contain_v1, set<PNode*>* _contain_v2);

	bool addin_list(PNode* _node);
	bool removefrom_list(PNode* _node);

	bool addin_output(PNode* _output);
	bool removefrom_output(PNode* _node);

	bool addin_dense(PNode* _dense);
	bool removefrom_dense(PNode* _node);

	bool exist_dense(set<int>& _v_set);
	bool cancel_dense(PNode* _dense_c);
};



#endif /* PREFIXTREE_H_ */
