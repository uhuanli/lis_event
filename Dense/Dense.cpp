/*
 * Dense.cpp
 *
 *  Created on: 2015-3-6
 *      Author: liyouhuan
 */
#include "Dense.h"
#include "prefixTree.h"
#include "RuntimeDense.h"

double Dense::Tmax;
int Dense::Nmax;
Dgraph* Dense::d_graph;
int Dense::time_stamp;
map<iiEdge, double> Dense::cache_e2w;
int Dense::buffer_vset[1000];
iiEdge Dense::buffer_edge;

Dense::Dense(int _life, int _intl, double edge_norm, double _tmax, int _nmax, bool _measure){
	cout << "life=" << _life << endl <<
			"intl=" << _intl << endl <<
			"norm=" << edge_norm << endl <<
			"tmax=" << _tmax << endl <<
			"nmax=" << _nmax << endl;
	util::DENSE_TWEET_LIFE = _life;//min
	util::INTERVAL = _intl;
	util::EDGE_NUM_NORM = edge_norm;
	Dense::Tmax = _tmax;
	Dense::Nmax = _nmax;
	util::weight_loglikelihood = _measure;
}

Dense::~Dense(){
	this->f_dense_result.close();
	this->dense_log.close();
	this->f_strong_edges.close();
	this->dense_space.close();
	this->dense_precision.close();
}

double Dense::weight(int _from, int _to){
	iiEdge _e(_from, _to);
	map<iiEdge, double>::iterator itr = Dense::cache_e2w.find(_e);
	if(itr == Dense::cache_e2w.end())
	{
		return Dgraph::weight(_from, _to);
	}
	else
	{
		return itr->second;
	}
}

double Dense::density(set<int>& _v_set){
	double _sum_w = 0;
	int _n = _v_set.size();
	set<int>::iterator itr1, itr2;

	int _i = 0;

	for(itr1 = _v_set.begin(); itr1 != _v_set.end(); itr1 ++)
	{
		Dense::buffer_vset[_i] = *itr1;
		_i ++;
	}

	for(int i = 0; i < _n; i ++)
	{
		for(int j = i + 1; j < _n; j ++)
		{
			_sum_w += Dense::weight(Dense::buffer_vset[i], Dense::buffer_vset[j]);
		}
	}

	_sum_w /= Dense::S(_n);

	return _sum_w;
}

int Dense::get_neighbors(int _v, set<int>& _neighbors){
	int add_num = Dense::d_graph->get_neighbors(_v, _neighbors);
	return add_num;
}

string Dense::getStrByID(int _v){
	return Dense::d_graph->d_getStrByID(_v);
}

string Dense::getDenseStr(set<int>& _neighbors){
	if(_neighbors.size() == 0){
		cout << "error dense str" << endl;
		exit(0);
	}

	stringstream _ss;
	_ss << "[";
	set<int>::iterator itr = _neighbors.begin();
	_ss << Dense::getStrByID(*itr) << "(" << *itr << ")";
	itr ++;
	for(; itr != _neighbors.end(); itr ++)
	{
		_ss << ", " << Dense::getStrByID(*itr) << "(" << *itr << ")";
	}
	_ss << "]" << endl;

	return _ss.str();
}

void Dense::build_neighbors(){
	this->d_graph->build_neighbors();
}

string Dense::to_presionstr(){
	stringstream _ss;
	_ss << "hello" << endl;
	return _ss.str();
}

void Dense::dense_run(){
	this->initial_dense();
	this->test_dense();

	this->dense_space << util::space_str() << endl;
	this->dense_space.flush();
}

double Dense::S(int _n){
	if(_n <= 1){
		cout << "@S: err n " << _n << endl;
		exit(0);
	}

	return (_n*(_n-1)) / 2.0;
}

/* g(n) = S(n)/( n*(n-1) ) */
double Dense::g(int _n){
	if(_n <= 1){
		cout << "@g: err n " << _n << endl;
		exit(0);
	}

	return Dense::S(_n) / (_n*(_n-1.0));
}

double Dense::T(int _n){
	if(_n >= Dense::Nmax){
		return Dense::Tmax;
	}

	double tmp_0 = (1.0 / Dense::g(_n));

	double tmp_1 = (Dense::gmax() * Dense::Tmax);

	double tmp_2_1 = ((_n-2.0) / (_n-1.0));
	double tmp_2_2 = ((Dense::Nmax - 2.0) / (Dense::Nmax - 1.0));
	double tmp_2 = Dense::delta_it() * (tmp_2_1 - tmp_2_2);

	return tmp_0 * (tmp_1 + tmp_2);
}

double Dense::gmax(){
	return g(Dense::Nmax);
}

double Dense::delta_max(){
	return Dense::S(Dense::Nmax)*Dense::Tmax / (Dense::Nmax * (Dense::Nmax - 2));
}

double Dense::delta_it(){

	return 0.4 * Dense::delta_max();
}

/* private */

void Dense::initial_dense(){
	/*  */
// int _life, int _intl, double edge_norm, double _tmax, int _nmax
	util::use_stopword = false;
	Dense::d_graph = new Dgraph();
	util::rt_dense = new RuntimeDense(Dense::Nmax, Dense::Tmax, Dense::delta_it());

	string _result_file;
	string _param;
	string _space_param = "";
	{
		stringstream _ss;
		_ss << "_intl[" << util::INTERVAL << "]";
		_ss << "_Nmax[" << Dense::Nmax << "]";
		_ss << "_Tmax[" << Dense::Tmax << "]";
		_ss << "_deltaIT[" << Dense::delta_it() << "]";
		_param = _ss.str();
		_space_param = "space+"+_ss.str();
		_ss.clear(); _ss.str("");

		_ss << "D:/result/dense/";
		_ss << "life[" << util::DENSE_TWEET_LIFE << "]";
		_ss << _param;
		_result_file = _ss.str();

		_ss << endl;
		for(int i = 2; i <= Dense::Nmax; i ++)
		{
			_ss << "T" << i << " = " << Dense::T(i) << " ";
		}
		_ss << endl;

		if(util::weight_loglikelihood){
			_ss << "USE likelihood" << endl;
		}else{
			_ss << "USE correlation" << endl;
		}

		cout << _ss.str() << endl;
	}

	string _strong_path = "D:/result/strong/init_strong/";
	{
		stringstream _tmp_ss;
		_tmp_ss << "thres[" << util::THETA_PREC << "]";
		_param = _tmp_ss.str();
	}


	this->f_dense_result.open(_result_file.c_str(), ios::out);
	if(!this->f_dense_result){
		cout << "dense_result open err" << endl;
		exit(0);
	}

	this->dense_log.open("D:/result/dense/dense_log", ios::out);
	if(!this->dense_log){
		cout << "dense_log open err" << endl;
		exit(0);
	}


	if(util::weight_loglikelihood){
		_param = "log_" + _param;
	}else{
		_param = "corr_" + _param;
	}
	_strong_path += _param;

	this->f_strong_edges.open(_strong_path.c_str());
	if(!this->f_strong_edges){
		cout << "f_strong open err: " << _strong_path << endl;
		exit(0);
	}

	{
		stringstream _tmp_ss;
		_tmp_ss << "_norm[" << util::EDGE_NUM_NORM << "]";
		_space_param += _tmp_ss.str();
	}

	string _f_space = "D:/result/dense/space/" + _space_param;
	this->dense_space.open(_f_space.c_str(), ios::out);

	string _f_precision = "D:/result/dense/precision/" + _param;
	this->dense_precision.open(_f_precision.c_str(), ios::out);

	util::initial();

	util::track("~~~~initial_dense", "\n");
}

void Dense::test_dense(){
	util::track("::::test_dense", "\n");

	PrefixTree pre_tree;

	map<iiEdge, double>* edge2delta_w = NULL;

	int null_next = 0;
	while(edge2delta_w == NULL)
	{
		util::rt_dense->global_begin();
		util::rt_dense->measure_begin();

		edge2delta_w = this->d_graph->next_delta();

		if(util::get_strongedges){
			this->f_strong_edges << this->d_graph->strong_tostr(util::THETA_PREC) << endl;
			this->f_strong_edges.flush();
		}

		if(util::cal_precision){
			this->dense_precision << this->d_graph->to_precisionstr() << endl;
			this->dense_precision.flush();
		}

		if(edge2delta_w != NULL){
			util::rt_dense->measure_end();
		}

		null_next ++;
		if(null_next > 10000)
		{
			cout << "null_next bug " << endl;
		}
	}

	util::track("----first next", "\n");

	int _count = 0;
	long begin_t = clock();
	while(edge2delta_w != NULL)
	{
		long before_update = clock();

		util::rt_dense->subgraph_begin();

		this->d_graph->build_neighbors();

		pre_tree.update(*edge2delta_w);

		this->f_dense_result << pre_tree.tostr_ouputdense();
		this->f_dense_result.flush();

		{
			long after_update = clock();
			_count ++;
			{
				stringstream _ss;
				_ss << "test_dense->while-> count " << _count << endl <<
					" cost[" << (after_update - before_update) / 1000.0 << "] "
					<< "sum[" << (after_update - begin_t)/ 1000.0 << "]" << endl;
				if(_count % 20 == 0){
					cout << _ss.str() << endl;
				}
//				if(_count > 50) break;

				util::track(_ss);
			}
		}

		util::rt_dense->subgraph_end();
		util::rt_dense->global_end();

		util::rt_dense->global_begin();

		delete edge2delta_w;

		util::rt_dense->measure_begin();

		edge2delta_w = (this->d_graph)->next_delta();
		if(util::get_strongedges){
			this->f_strong_edges << this->d_graph->strong_tostr(util::THETA_PREC) << endl;
			this->f_strong_edges.flush();
		}

		if(util::cal_precision){
			this->dense_precision << this->d_graph->to_precisionstr() << endl;
			this->dense_precision.flush();
		}

		util::rt_dense->measure_end();
	}

	util::track("~~~~test_dense", "\n");
}
