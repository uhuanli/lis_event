/*
 * RuntimeDense.cpp
 *
 *  Created on: 2015-4-15
 *      Author: liyouhuan
 */
#include "RuntimeDense.h"
#include "../util/util.h"

RuntimeDense::RuntimeDense(int _nmax, double _tmax, double _delta_it){
	string _folder = "D:/result/runtime/";
	string file_dense_runtime;

	stringstream _ss;
	{
		_ss << "_Nmax[" << _nmax << "]";
		_ss << "_Tmax[" << _tmax << "]";
		_ss << "_deltaIT[" << _delta_it << "]";
		_ss << "_life[" << util::DENSE_TWEET_LIFE << "]";
		_ss << "_intl[" << util::INTERVAL << "]";
		_ss << "_norm[" << util::EDGE_NUM_NORM << "]";
	}
	file_dense_runtime = _folder + "dense_runtime_" + _ss.str();

	this->dense_runtime.open(file_dense_runtime.c_str());
	if(! this->dense_runtime){
		cout << "dense_runtime open fail" << endl;
		exit(0);
	}

	this->align_dense_runtime.open(("D:/result/align/dense_"+_ss.str()).c_str());
	if(! this->align_dense_runtime){
		cout << "dense_align open fail" << endl;
		exit(0);
	}
	this->align_dense_runtime << "global_num\t" << "time_num\t" <<
			"global_avg\t" << "measure_avg\t" << "subgraphs_avg\t" <<
			"global\t" << "measure\t" << "subgraph\t" << "edge_num" << endl;

	this->global_num = 0;
	this->global_sum = 0;
	this->measure_sum = 0;
	this->subgraph_sum = 0;

	this->globals.clear();
	this->space_global.clear();
	this->measures.clear();
	this->subgraphs.clear();
}

string RuntimeDense::to_str(){
	stringstream _ss;
	_ss << "global_num: " << this->global_num;
	_ss << "\t global_avg: " << (long long int)(this->global_sum / this->global_num);
	_ss << "\t lis_avg: " << (long long int)(this->measure_sum / this->global_num);
	_ss << "\t subgraph_avg: " << (long long int)(this->subgraph_sum / this->global_num);
	_ss << endl;

	long _tmp_g = *(this->globals.rbegin());
	_ss << "global: " << _tmp_g << endl;
	long _tmp_m = *(this->measures.rbegin());
	_ss << "measure: " << _tmp_m << endl;
	long _tmp_s = *(this->subgraphs.rbegin());
	_ss << "subgraphs: " << _tmp_s << endl;

	string _ret = _ss.str();

	{
		_ss.str("");
		_ss.clear();
		_ss << this->global_num << "\t" << (this->global_num*util::INTERVAL);
		_ss << "\t" << (long long int)(this->global_sum / this->global_num);
		_ss << "\t" << (long long int)(this->measure_sum / this->global_num);
		_ss << "\t" << (long long int)(this->subgraph_sum / this->global_num);
		_ss << "\t" << _tmp_g;
		_ss << "\t" << _tmp_m;
		_ss << "\t" << _tmp_s;
		_ss << "\t" << util::edge_num;
		this->align_dense_runtime << _ss.str() << endl;
		this->align_dense_runtime.flush();
	}

	return _ret;
}

void RuntimeDense::measure_begin(){
	this->tmp_measure = util::get_time_cur();
}
void RuntimeDense::measure_end(){
	long long int _time = util::get_time_cur();
	long long int _cost = util::cal_time(_time, this->tmp_measure);

	this->measure_sum += _cost;
	this->measures.push_back(_cost);
}

void RuntimeDense::subgraph_begin(){
	this->tmp_subgraph = util::get_time_cur();
}
void RuntimeDense::subgraph_end(){
	long long int _time = util::get_time_cur();
	long long int _cost =util::cal_time(_time, this->tmp_subgraph);

	this->subgraph_sum += _cost;
	this->subgraphs.push_back(_cost);
}

void RuntimeDense::global_begin(){
	this->tmp_global = util::get_time_cur();
}
void RuntimeDense::global_end(){
	long long int _time = util::get_time_cur();
	long long int _cost = util::cal_time(_time,this->tmp_global);

	this->global_num ++;
	this->global_sum += _cost;
	this->globals.push_back(_cost);
	this->space_global.push_back(0);

	this->dense_runtime << this->to_str() << endl << endl;
	this->dense_runtime.flush();
}



