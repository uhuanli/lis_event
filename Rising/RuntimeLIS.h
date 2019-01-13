/*
 * RuntimeLIS.h
 *
 *  Created on: 2015-4-16
 *      Author: liyouhuan
 */

#ifndef RUNTIMELIS_H_
#define RUNTIMELIS_H_

#include <vector>
#include <fstream>
using namespace std;

class RuntimeLis{
public:

	RuntimeLis();

	string to_str();

	void lis_begin();
	void lis_end();

	void lis_update_begin();
	void lis_update_end(long* updates);

	void subgraph_begin();
	void subgraph_end();

	void global_begin();
	void global_end();

	int global_num;
	long global_sum;
	vector<long> globals;
	vector<long> space_global;

	long lis_sum;
	vector<long> liss;

	long subgraph_sum;
	vector<long> subgraphs;

	long lis_udpate_sum;
	vector<long> lis_updates;

	vector<long> update_parallel_sum;
	vector<long> update_parallel_max;
	vector<long*> parallel_updates;
	static long* parallel_insert;
	static long* parallel_delete;

	int parallel_num;
	double edge_num_norm;

private:
	long long int tmp_lis;
	long long int tmp_lis_update;
	long long int tmp_subgraph;
	long long int tmp_global;

	ofstream lis_runtime;
	ofstream align_lis_runtime;

};

#endif /* RUNTIMELIS_H_ */
