/*
 * RuntimeLIS.cpp
 *
 *  Created on: 2015-4-16
 *      Author: liyouhuan
 */

#include "RuntimeLIS.h"
#include "../util/util.h"

long* RuntimeLis::parallel_insert = NULL;
long* RuntimeLis::parallel_delete = NULL;

RuntimeLis::RuntimeLis(){
	stringstream _ss;
	string _folder = "D:/result/runtime/";
	string file_lis_runtime;
	{
		_ss << "_winsz[" << util::WIN_SIZE << "]";
		_ss << "_para[" << util::PARALLEL_NUM << "]";
		_ss << "_norm[" << util::EDGE_NUM_NORM << "]";
		_ss << "_theta[" << util::THETA_PREC << "]";
	}
	file_lis_runtime = _folder + "lis_rt_" + _ss.str();

	this->lis_runtime.open(file_lis_runtime.c_str());
	if(! this->lis_runtime){
		cout << "lis_runtime open fail" << endl;
		exit(0);
	}

	this->align_lis_runtime.open(("D:/result/align/lis_"+_ss.str()).c_str());
	if(! this->align_lis_runtime){
		cout << "align_lis_runtime open fail" << endl;
		exit(0);
	}
	this->align_lis_runtime << "global_num\t" << "time_num\t" <<
			"global_avg\t" << "lis_avg\t" << "subgraphs_avg\t" <<
			"lis_update_avg\t" <<
			"global\t" << "lis\t" << "subgraph\t" <<
			"update_sum\t" << "update_max\t" << "edge_num" << endl;


	this->global_num = 0;
	this->global_sum = 0;
	this->lis_sum = 0;
	this->subgraph_sum = 0;
	this->lis_udpate_sum = 0;

	this->globals.clear();
	this->space_global.clear();
	this->liss.clear();
	this->lis_updates.clear();
	this->subgraphs.clear();

	this->update_parallel_max.clear();
	this->parallel_updates.clear();
	this->update_parallel_sum.clear();
}

string RuntimeLis::to_str(){
	stringstream _ss;
	long _tmp;
	_ss << "global_num: " << this->global_num;
	_ss << "\t global_avg: " << (long long int)(this->global_sum / this->global_num);
	_ss << "\t lis_avg: " << (long long int)(this->lis_sum / this->global_num);
	_ss << "\t subgraph_avg: " << (long long int)(this->subgraph_sum / this->global_num);
	_ss << "\t lis_update_avg: " << (long long int)(this->lis_udpate_sum / this->global_num);
	_ss << "\t parallel_avg: " << util::PARALLEL_NUM;
	_ss << endl;

	long _tmp_g = *(this->globals.rbegin());
	_ss << "global: " << _tmp_g << endl;
	long _tmp_l = *(this->liss.rbegin());
	_ss << "lis: " << _tmp_l << endl;
	long _tmp_s = *(this->subgraphs.rbegin());
	_ss << "subgraphs: " << _tmp_s << endl;

	long _tmp_us = *(this->update_parallel_sum.rbegin());
	_ss << "update_sum: " << _tmp;
	long _tmp_um = *(this->update_parallel_max.rbegin());
	_ss << "\t update_max: " << _tmp << endl;

	string _ret = _ss.str();
	{
		_ss.str("");
		_ss.clear();
		_ss << this->global_num << "\t" << (this->global_num*util::INTERVAL);
		_ss << "\t" << (long long int)(this->global_sum / this->global_num);
		_ss << "\t" << (long long int)(this->lis_sum / this->global_num);
		_ss << "\t" << (long long int)(this->subgraph_sum / this->global_num);
		_ss << "\t" << (long long int)(this->lis_udpate_sum / this->global_num);
		_ss << "\t" << _tmp_g;
		_ss << "\t" << _tmp_l;
		_ss << "\t" << _tmp_s;
		_ss << "\t" << _tmp_us;
		_ss << "\t" << _tmp_um;
		_ss << "\t" << util::edge_num;
		this->align_lis_runtime << _ss.str() << endl;
	}
	return _ret;
}

void RuntimeLis::lis_begin(){
	this->tmp_lis = util::get_time_cur();
}
void RuntimeLis::lis_end(){
	long long int _time = util::get_time_cur();
	long long int _cost = util::cal_time(_time, this->tmp_lis);
//	this->lis_runtime << "**lis: " << _cost << endl;

	this->lis_sum += _cost;
	this->liss.push_back(_cost);
}

void RuntimeLis::lis_update_begin(){
	this->tmp_lis_update = util::get_time_cur();
}
void RuntimeLis::lis_update_end(long* updates){
	long long int _time = util::get_time_cur();
	long long int _cost = util::cal_time(_time, this->tmp_lis_update);

//	this->lis_runtime << "**lis_update: " << _cost << endl;

	long _max = -1;
	long  _paralles_sum = 0;
	for(int i = 0; i < util::PARALLEL_NUM; i ++){
		_paralles_sum += updates[i];
		if (_max < updates[i]){
			_max = updates[i];
		}
	}

	this->lis_udpate_sum += _cost;
	this->lis_updates.push_back(_cost);

	this->update_parallel_sum.push_back(_paralles_sum);
	this->update_parallel_max.push_back(_max);
	this->parallel_updates.push_back(updates);
}

void RuntimeLis::subgraph_begin(){
	this->tmp_subgraph = util::get_time_cur();
}
void RuntimeLis::subgraph_end(){
	long long int _time = util::get_time_cur();
	long long int _cost =util::cal_time(_time, this->tmp_subgraph);

	this->subgraph_sum += _cost;
	this->subgraphs.push_back(_cost);
}

void RuntimeLis::global_begin(){
	this->tmp_global = util::get_time_cur();
}
void RuntimeLis::global_end(){
	long long int _time = util::get_time_cur();
	long long int _cost = util::cal_time(_time,this->tmp_global);
//	this->lis_runtime << "**global: " << _cost << endl;

	this->global_num ++;
	this->global_sum += _cost;
	this->globals.push_back(_cost);
	this->space_global.push_back(0);

	this->lis_runtime << this->to_str() << endl << endl;
	this->lis_runtime.flush();
}



