/*
 * Dgraph.h
 *
 *  Created on: 2015-3-6
 *      Author: liyouhuan
 */

#ifndef DGRAPH_H_
#define DGRAPH_H_

#include "iiEdge.h"

class Dgraph
{
public:
	Dgraph();
	~Dgraph();

	void initial_dgraph();

	/*ret: delta weight of edge */
	map<iiEdge, double>* next_delta();

	static int get_neighbors(int _v, set<int>& _neighbors);

	static double weight(int _from, int _to);

	static double log_likelihood_ratio(int _f1, int _f2, int _f_both, int _f_total);
	static double correlation(int _f1, int _f2, int _f_both, int _f_total);

	string d_getStrByID(int _v);

	string strong_tostr(double _threshold);
	string to_precisionstr();

	const static double NO_WEIGHT = -1000.0;
	const static double CHI_THRESHOLD = 3.84;

	void build_neighbors();

private:
	/*  */
	map<iiEdge, double>* coming_weight(map<string, int>& _edge2freq);

	map<iiEdge, double>* log_likelihood	(map<iiEdge, int>& _edge2freq, map<int, int> _v2freq, int _total_freq);

	map<iiEdge, double>* correlation_coefficient(map<iiEdge, int>& _edge2freq, map<int, int> _v2freq, int _total_freq);


	/*  */
	map<iiEdge, double>* delta_weight(map<iiEdge, double>* _coming_w);


	/*  */
	map<string, int>* get_total_edge2freq();
	string to_str(map<iiEdge, double>* _e2w);

	int getID_add(string _keyword);

	static double chi_square(int _f1, int _f2, int _f_both, int _f_total);

	vector<map<string, int>* > slice_edge2freq;

	int slice_max;

	ifstream edgestream;
	map<int, string> id2str;
	map<string, int> str2id;

	static map<int, set<int> > vertex2neighbors;

	static map<iiEdge, double> edge2weight;

	static ofstream ratio_computation;
	static ofstream f_coming_w;
	static ofstream f_delta_w;
	static ofstream f_e2weight;

};

#endif /* DGRAPH_H_ */
