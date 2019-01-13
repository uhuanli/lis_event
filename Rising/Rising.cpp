/*
 * Rising.cpp
 *
 *  Created on: 2015-3-11
 *      Author: liyouhuan
 */

#include "Rising.h"
#include "RuntimeLIS.h"
#include "windows.h"

multimap<string, int> Rising::strong_ts;
set<string> Rising::rcase_edges;

Rising::Rising(int _winsz, int _intl, double _epsilon, int _parallel, double _e_norm, double _theta)
{
	cout << "win=" << _winsz << endl <<
			"intl=" << _intl << endl <<
			"epsilon=" << _epsilon << endl <<
			"parallel=" << _parallel << endl <<
			"norm=" << _e_norm << endl <<
			"theta=" << _theta << endl;
	util::WIN_SIZE = _winsz;
	util::INTERVAL = _intl;
	util::EPSILON = _epsilon;
	util::PARALLEL_NUM = _parallel;
	util::EDGE_NUM_NORM = _e_norm;
	util::THETA_PREC = _theta;
	this->case_str = "";
}

Rising::~Rising(){
	this->f_rising_result.close();
	this->f_space.close();
	this->f_strong_list.close();
	this->f_edgeprec.close();
	this->f_accprec.close();
	this->f_freqprec.close();
}

void Rising::initial_cc()
{
	util::SEQ_MEM_SIZE = util::WIN_SIZE + 2;
	util::use_stopword = true;
	RuntimeLis::parallel_insert = new long[util::PARALLEL_NUM+1];
	RuntimeLis::parallel_delete = new long[util::PARALLEL_NUM+1];
	for(int i = 0; i < util::PARALLEL_NUM+1; i ++){
	RuntimeLis::parallel_insert[i] = 0;
	RuntimeLis::parallel_delete[i] = 0;
	}
	util::rt_lis = new RuntimeLis();
	util::initial();

	string _folder = "D:/result/";
	string _param_str;
	string _result_file;
	string _strong_list_file;
	{
		stringstream _ss;
//		long _time = util::get_time_cur() / (60*60);
//		_time %= 1000;
//		_ss << _result_file << _time;
		_ss << "_winsz[" << util::WIN_SIZE << "]";
		_ss << "_parallel[" << util::PARALLEL_NUM << "]";
		_ss << "_normalized[" << util::EDGE_NUM_NORM << "]";
		_ss << "__theta[" << util::THETA_PREC << "]";
		_param_str = _ss.str();
	}
	_result_file = _folder + "lis/lisresult_" + _param_str;
	_strong_list_file = _folder + "strong/init_strong/lis_" + _param_str;
	string _space = _folder + "lis/space_" + _param_str;
	string _precision = _folder + "lis/precision_" + _param_str;

	this->f_edgeprec.open(_precision.c_str(), ios::out);

	this->f_space.open(_space.c_str(), ios::out);

	this->f_rising_result.open(_result_file.c_str(), ios::out);


	this->f_strong_list.open(_strong_list_file.c_str(), ios::out);

	if(! this->f_rising_result || !this->f_strong_list
		|| !this->f_space || !this->f_edgeprec){
		cout << "rising_result or strong_list or space or precision open fail" << endl;
		system("pause");
	}

	string _acc = _folder +"strong/init_strong/acceleration";
	string _freq = _folder +"strong/init_strong/frequency";

	this->f_accprec.open(_acc.c_str(), ios::out);

	this->f_freqprec.open(_freq.c_str(), ios::out);

	if(! this->f_accprec || ! this->f_freqprec){
		cout << "fail acc or freq" << endl;
		system("pause");
	}


	util::track("finish initial", "\n");
}

void Rising::rising_run()
{
	cout << "run!" << endl;

	if(util::load_case){
		this->load_case_edges();
	}

	this->initial_cc();
	this->test_cc();

	this->f_space << util::space_str() << endl;

	if(util::load_case){
		this->write_case_edges_ts();
	}

	cout << "finish run" << endl;
}


void Rising::load_case_edges(){
	this->rcase_edges.clear();
	if(this->case_str == ""){
		cout << "no case" << endl;
		system("pause");
	}

	string _file = "D:/result/lis/case/"+this->case_str;
	util::load_caseedges(_file, this->rcase_edges);
}

void Rising::write_case_edges_ts(){
	ofstream _fout;
	string _file = "D:/result/lis/case/"+this->case_str+".e2ts";
	_fout.open(_file.c_str(), ios::out);
	if(!_fout){
		cout << "fail open " << _file << endl;
		system("pause");
		exit(0);
	}
	_fout << "edge\ttime\tfirst" << endl;

	multimap<string, int>::iterator itr = this->strong_ts.begin();
	while(itr != this->strong_ts.end()){
		int _first = util::get_firstappear(itr->first);
		if(_first == -1){
			cout << "first err" << endl;
			system("pause");
		}

		_fout << itr->first << "\t" << itr->second << "\t" << _first << endl;
		_fout.flush();

		itr ++;
	}

	_fout.close();
}

void Rising::test_cc()
{
	cout << "run cc!" << endl;
	util::track("IN test_cc", "\n");

	string edge_file = "D:/things/Data/stem03_7-9_edge";

	Rgraph _rg(edge_file, util::INTERVAL);

	map<string, double>* s2d = NULL;
	int null_next = 0;
	while(s2d == NULL)
	{
		util::rt_lis->global_begin();

		util::rt_lis->lis_begin();
		s2d = _rg.next_cc();

		if(s2d != NULL)
		{
			util::rt_lis->lis_end();
			if(util::frequency_prec){
				this->f_freqprec << _rg.get_freq_precisionstr() << endl;
				this->f_freqprec.flush();
			}
			if(util::acceler_prec){
				this->f_accprec << _rg.get_acc_precisionstr() << endl;
				this->f_accprec.flush();
			}
		}

		null_next ++;
		if(null_next > 10000)
		{
			cout << "null_next bug " << endl;
		}
	}

	vector<string> strong_edges;
	Ugraph uG;
	while(s2d != NULL)
	{
		int edge_num = _rg.getStrongEdges(strong_edges);
		stringstream _ss;

		{/* log */
			_ss << _rg.strongedge_str() << endl;
			_ss << "+++++\nThere are [" << edge_num << "] strong edges " << endl;
			util::log(_ss);
			this->f_rising_result << _ss.str() << endl;
			_ss.str("");
			_ss.clear();

			this->f_strong_list << _rg.strong_list_str() << endl;
			this->f_strong_list.flush();

			this->f_edgeprec << _rg.precisionedge_str() << endl;
			this->f_edgeprec.flush();
		}//log

		util::rt_lis->subgraph_begin();
		uG.getConnectedComponent(strong_edges, _ss);
		util::rt_lis->subgraph_end();

		{/* log */
			_ss << "\n\n****************************\n\n" << endl;
			this->f_rising_result << _ss.str() << endl;
			util::log(_ss);
		}//log

		delete s2d;

		util::rt_lis->global_end();


		util::rt_lis->global_begin();

		util::rt_lis->lis_begin();
		s2d = _rg.next_cc();
		util::rt_lis->lis_end();
		if(util::frequency_prec){
			this->f_freqprec << _rg.get_freq_precisionstr() << endl;
			this->f_freqprec.flush();
		}
		if(util::acceler_prec){
			this->f_accprec << _rg.get_acc_precisionstr() << endl;
			this->f_accprec.flush();
		}
	}

	cout << "finish run" << endl;
	util::track("OUT test_cc", "\n\n");
}


