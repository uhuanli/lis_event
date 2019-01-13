/*
 * RuntimeDense.h
 *
 *  Created on: 2015-4-15
 *      Author: liyouhuan
 */

#ifndef RUNTIMEDENSE_H_
#define RUNTIMEDENSE_H_

#include <vector>
#include <fstream>
using namespace std;

class RuntimeDense{
public:

	RuntimeDense(int _nmax, double _tmax, double _delta_it);

	string to_str();

	void measure_begin();
	void measure_end();

	void subgraph_begin();
	void subgraph_end();

	void global_begin();
	void global_end();

	int global_num;
	double global_sum;
	vector<long> globals;
	vector<long> space_global;

	double measure_sum;
	vector<long> measures;

	double subgraph_sum;
	vector<long> subgraphs;

private:
	long long int tmp_measure;
	long long int tmp_lis_update;
	long long int tmp_subgraph;
	long long int tmp_global;

	ofstream dense_runtime;
	ofstream align_dense_runtime;

};


#endif /* RUNTIMEDENSE_H_ */
