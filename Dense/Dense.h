/*
 * Dense.h
 *
 *  Created on: 2015-3-6
 *      Author: liyouhuan
 */

#ifndef DENSE_H_
#define DENSE_H_
#include "../util/util.h"
#include "Dgraph.h"

class Dense
{
public:
	Dense(int _life, int _intl, double edge_norm, double _tmax, int _nmax, bool _measure);
	~Dense();
	void dense_run();

	static double weight(int _from, int _to);
	static double density(set<int>& _v_set);
	static int get_neighbors(int _v, set<int>& _neighbors);
	static string getStrByID(int _v);
	static string getDenseStr(set<int>& _neighbors);

	void build_neighbors();

	string to_presionstr();


	static double Tmax;
	static int Nmax;
	ofstream f_dense_result;
	ofstream dense_log;
	ofstream f_strong_edges;
	ofstream dense_space;
	ofstream dense_precision;


	static double S(int _n);
	/* g(n) = S(n)/( n*(n-1) ) */
	static double g(int _n);
	static double T(int _n);
	static double gmax();
	static double delta_max();
	static double delta_it();

	static map<iiEdge, double> cache_e2w;
	static int buffer_vset[1000];
	static iiEdge buffer_edge;
	static int time_stamp;

private:
	static Dgraph* d_graph;

	void initial_dense();
	void test_dense();
};


#endif /* DENSE_H_ */
