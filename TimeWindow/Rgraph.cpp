/*
 * Rgraph.cpp
 *
 *  Created on: 2015-2-6
 *      Author: liyouhuan
 */
#include "Rgraph.h"
#include "../Rising/RuntimeLis.h"
#include "../Rising/Rising.h"

Rgraph::Rgraph(string _e_file, int _intl)
{
	this->interval = _intl;
	this->edge_file = _e_file;
	this->time_stamp = 0;
	this->t_win.initial();
	this->pre_theta = -1.0;

	this->estream.open(_e_file.c_str(), ios::in);
	if(! this->estream){
		cout << "err open " << _e_file << endl;
		exit(0);
	}
}

Rgraph::~Rgraph()
{
	this->estream.close();
}

map<string, double>* Rgraph::next_cc()
{
	util::track("IN Rgraph::next_cc", "\n");

	int flag = 0;
	map<string, int> _s2i;
	flag = util::getNextInterval(this->estream, _s2i, this->interval);
	if(flag == -1 || flag > 80000){  // || flag < 71730
		return NULL;
	}

	{/* acc or freq */
		/*  */
		this->acce_strongs = this->acce_strongstr(_s2i);
		this->freq_strongs = this->freq_strongstr(_s2i);
		{
			this->tmpedge2freq.clear();
			this->tmpedge2freq.insert(_s2i.begin(), _s2i.end());
		}
	}

	{/* debug */

		{
			stringstream _ss;
			_ss << "flag @@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << flag << endl;
			util::track(_ss);
		}

		{
			stringstream _ss;
			_ss << " flag[" << flag << "] ";
			util::log(_ss.str().c_str(), "\n");
		}

		if(flag % 100 == 0){
			cout << "=> " << flag << endl;
		}
	}

	map<string, double>* _s2d;
	set<string> _removed_edges;

	util::rt_lis->lis_update_begin();
	_s2d = this->t_win.update(_s2i, this->time_stamp, _removed_edges);
	long* tmp_longs = new long[10];
	for(int i = 0; i < util::PARALLEL_NUM; i ++){
		tmp_longs[i] = RuntimeLis::parallel_delete[i];
		tmp_longs[i] += RuntimeLis::parallel_insert[i];
	}
	util::rt_lis->lis_update_end(tmp_longs);


	{/* debug */
		stringstream _ss;
		_ss << "finish update at " << this->time_stamp << " with _s2d size " << _s2d->size() << endl;
		util::log(_ss.str().c_str(), "\n");

		util::track(_ss);
	}

	set<string>::iterator s_itr;
	for(s_itr = _removed_edges.begin(); s_itr != _removed_edges.end(); s_itr ++){
		this->edge2ris.erase(*s_itr);
	}

	double _theta = this->get_k_th_ris(_s2d, util::THETA_PREC);

	{
		stringstream _ss;
		_ss << "theta[" << _theta << "] at " << this->time_stamp << endl;
		util::log(_ss);

		util::track(_ss);
	}

	map<string, double>* d_and_i = this->get_I_D_edges(_s2d, _theta);

	this->update_edge2ris(_s2d);

	if(util::load_case){
		set<string>::iterator itr_case = Rising::rcase_edges.begin();
		while(itr_case != Rising::rcase_edges.end()){
			map<string, double>::iterator e2ris_itr = this->edge2ris.find(*itr_case);

			if(e2ris_itr != this->edge2ris.end())
			{
				if(e2ris_itr->second > _theta)
				{
					Rising::strong_ts.insert(pair<string, int>(*itr_case, flag));
				}
			}

			itr_case ++;
		}
	}

	delete _s2d;

	this->pre_theta = _theta;

	this->time_stamp ++;

	util::track("OUT Rgraph::next_cc", "\n");
	{
		stringstream _ss;
		_ss << "finish next_cc at " << this->time_stamp << endl;
		_ss << "return d_and_i of size " << d_and_i ->size() << endl;
		util::track(_ss);
	}

	return d_and_i;
}

string Rgraph::get_freq_strongstr(){
	return this->freq_strongs;
}
string Rgraph::get_acce_strongstr(){
	return this->acce_strongs;
}

string Rgraph::freq_strongstr(map<string, int>& _e2freq){
	stringstream _ss;

	return _ss.str();
}
string Rgraph::acce_strongstr(map<string, int>& _e2freq){
	stringstream _ss;

	map<string, int>::iterator pre_itr = this->tmpedge2freq.begin();
	map<string, int>::iterator cur_itr = _e2freq.begin();
	this->acc2edge.clear();
	double _delta = 0;
	while(pre_itr != this->tmpedge2freq.end() && cur_itr != _e2freq.end())
	{
		if(pre_itr->first == cur_itr->first)
		{
			_delta = cur_itr->second - pre_itr->second;
			this->acc2edge.insert(pair<double, string>(_delta, pre_itr->first));

			pre_itr ++; cur_itr ++;
		}
		else
		if(pre_itr->first > cur_itr->first)
		{
			_delta = cur_itr->second;
			this->acc2edge.insert(pair<double, string>(_delta, cur_itr->first));

			cur_itr ++;
		}
		else
		if(pre_itr->first < cur_itr->first)
		{
			_delta = 0 - pre_itr->second;
			this->acc2edge.insert(pair<double, string>(_delta, pre_itr->first));

			pre_itr ++;
		}
	}

	while(pre_itr != this->tmpedge2freq.end())
	{
		_delta = 0 - pre_itr->second;
		this->acc2edge.insert(pair<double, string>(_delta, pre_itr->first));

		pre_itr ++;
	}

	while(cur_itr != _e2freq.end())
	{
		_delta = cur_itr->second;
		this->acc2edge.insert(pair<double, string>(_delta, cur_itr->first));

		cur_itr ++;
	}

	int k = util::THETA_PREC * this->acc2edge.size();

	multimap<double, string>::reverse_iterator ritr = this->acc2edge.rbegin();
	while(ritr != this->acc2edge.rend() && k > 0)
	{
		if(ritr->first <= 0) break;
		_ss << ritr->second << " " << ritr->first << endl;
		k --;
	}

	return _ss.str();
}

string Rgraph::strongedge_str()
{
	stringstream _ss;
	_ss << "At: " << this->time_stamp << endl;
	multimap<double, string> md2s;
	map<string, double>::iterator itr;
	for(itr = this->edge2ris.begin(); itr != this->edge2ris.end(); itr ++)
	{
		if(itr->second > this->pre_theta){
			md2s.insert(pair<double, string>(itr->second, itr->first));
		}
	}

	multimap<double, string>::reverse_iterator ritr;
	char tmp[100];
	int tmp_i = 0;
	for(ritr = md2s.rbegin(); ritr != md2s.rend(); ritr ++){
		sprintf(tmp, "%.3lf", ritr->first);
		_ss << "[" << ritr->second << " " << tmp << "]\t";
		tmp_i ++;
	}
	_ss << endl << endl;

	return _ss.str();
}

string Rgraph::precisionedge_str()
{
	multimap<double, string> md2s;
	map<string, double>::iterator itr;
	for(itr = this->edge2ris.begin(); itr != this->edge2ris.end(); itr ++)
	{
		md2s.insert(pair<double, string>(itr->second, itr->first));
	}

	multimap<double, string>::reverse_iterator ritr;
	int tmp_i = 0;
	int _positive = 0;
	//{0.04, 0.08, 0.12, 0.16, 0.20}
	double tao[5] = {0.03, 0.06, 0.09, 0.12,0.15};
	int top[5] = {20, 40, 60, 80, 100};
//	for(int i = 0; i < 5; i ++){
//		top[i] = md2s.size() * tao[i];
//	}

	double prec[5] = {0, 0, 0, 0, 0};
	for(ritr = md2s.rbegin(); ritr != md2s.rend(); ritr ++)
	{
		if(util::is_eventedge(ritr->second))
		{
			_positive ++;
		}

		tmp_i ++;
		for(int i = 0; i < 5; i ++)
		{
			if(tmp_i == top[i])
			{
				prec[i] = _positive/(top[i]+0.0);
			}
		}
		if(tmp_i == top[4]){
			break;
		}
	}

	if(prec[0] == 0){
		prec[0] = _positive / (top[0]+0.0);
	}

	stringstream _ss;
	_ss << prec[0];
	for(int i = 1; i < 5; i ++)
	{
		if(prec[i] == 0){
			prec[i] = (prec[i-1] * top[i-1])/(top[i]+0.0);
		}
		_ss << "\t" << prec[i];
	}

	return _ss.str();
}

string Rgraph::get_acc_precisionstr()
{
	multimap<double, string>::reverse_iterator ritr;
	int tmp_i = 0;
	int _positive = 0;
	//{0.04, 0.08, 0.12, 0.16, 0.20}
	double tao[5] = {0.03, 0.06, 0.09, 0.12,0.15};
	int top[5] = {20, 40, 60, 80, 100};
//	for(int i = 0; i < 5; i ++){
//		top[i] = md2s.size() * tao[i];
//	}

	double prec[5] = {0, 0, 0, 0, 0};
	for(ritr = this->acc2edge.rbegin(); ritr != this->acc2edge.rend(); ritr ++)
	{
		if(util::is_eventedge(ritr->second))
		{
			_positive ++;
		}

		tmp_i ++;
		for(int i = 0; i < 5; i ++)
		{
			if(tmp_i == top[i])
			{
				prec[i] = _positive/(top[i]+0.0);
			}
		}
		if(tmp_i == top[4]){
			break;
		}
	}

	if(prec[0] == 0){
		prec[0] = _positive / (top[0]+0.0);
	}

	stringstream _ss;
	_ss << prec[0];
	for(int i = 1; i < 5; i ++)
	{
		if(prec[i] == 0){
			prec[i] = (prec[i-1] * top[i-1])/(top[i]+0.0);
		}
		_ss << "\t" << prec[i];
	}

	return _ss.str();
}
string Rgraph::get_freq_precisionstr()
{
	multimap<double, string> md2s;
	map<string, int>::iterator itr;
	for(itr = this->tmpedge2freq.begin(); itr != this->tmpedge2freq.end(); itr ++)
	{
		md2s.insert(pair<double, string>(itr->second+0.0, itr->first));
	}

	multimap<double, string>::reverse_iterator ritr;
	int tmp_i = 0;
	int _positive = 0;
	//{0.04, 0.08, 0.12, 0.16, 0.20}
	double tao[5] = {0.03, 0.06, 0.09, 0.12,0.15};
	int top[5] = {20, 40, 60, 80, 100};
//	for(int i = 0; i < 5; i ++){
//		top[i] = md2s.size() * tao[i];
//	}

	double prec[5] = {0, 0, 0, 0, 0};
	for(ritr = md2s.rbegin(); ritr != md2s.rend(); ritr ++)
	{
		if(util::is_eventedge(ritr->second))
		{
			_positive ++;
		}

		tmp_i ++;
		for(int i = 0; i < 5; i ++)
		{
			if(tmp_i == top[i])
			{
				prec[i] = _positive/(top[i]+0.0);
			}
		}
		if(tmp_i == top[4]){
			break;
		}
	}

	if(prec[0] == 0){
		prec[0] = _positive / (top[0]+0.0);
	}

	stringstream _ss;
	_ss << prec[0];
	for(int i = 1; i < 5; i ++)
	{
		if(prec[i] == 0){
			prec[i] = (prec[i-1] * top[i-1])/(top[i]+0.0);
		}
		_ss << "\t" << prec[i];
	}

	return _ss.str();
}

string Rgraph::strong_list_str(){
	stringstream _ss;
	multimap<double, string> md2s;
	map<string, double>::iterator itr;
	for(itr = this->edge2ris.begin(); itr != this->edge2ris.end(); itr ++)
	{
		if(itr->second > this->pre_theta){
			md2s.insert(pair<double, string>(itr->second, itr->first));
		}
	}

	multimap<double, string>::reverse_iterator ritr;
	char tmp[100];
	int tmp_i = 0;


	ritr = md2s.rbegin();
	sprintf(tmp, "%.3lf", ritr->first);
	_ss << ritr->second << " " << tmp;
	ritr ++;

	for( ; ritr != md2s.rend(); ritr ++){
		sprintf(tmp, "%.3lf", ritr->first);
		_ss << "\n" << ritr->second << " " << tmp;
		tmp_i ++;
	}

	return _ss.str();
}

int Rgraph::getStrongEdges(vector<string> & _vec_edges)
{
	_vec_edges.clear();
	map<string, double>::iterator itr;
	for(itr = this->edge2ris.begin(); itr != this->edge2ris.end(); itr ++)
	{
		if(itr->second > this->pre_theta){
			_vec_edges.push_back(itr->first);
		}
	}

	return _vec_edges.size();
}



double Rgraph::get_k_th_ris(map<string, double>* _s2d, double _k_pre)
{
	multiset<double> ris_set;
	map<string, double>::iterator itr, _new_itr;
	{
		stringstream _ss;
		_ss << "current edge number: " << this->edge2ris.size() << endl;
		_ss << "current updated edge num: " << _s2d->size() << endl;
		util::log(_ss);

		util::track(_ss);
	}
	itr = this->edge2ris.begin();
	_new_itr = _s2d->begin();
	while(itr != this->edge2ris.end() && _new_itr != _s2d->end())
	{
		int _cmp = strcmp(itr->first.c_str(), _new_itr->first.c_str());
		if(_cmp == 0){
			ris_set.insert(itr->second);
			itr ++;
			_new_itr ++;
		}else
		if(_cmp > 0){
			/* mind here */
			ris_set.insert(_new_itr->second);
			_new_itr ++;
		}else
		if(_cmp < 0){
			ris_set.insert(itr->second);
			itr ++;
		}
	}

	while(itr != this->edge2ris.end()){
		ris_set.insert(itr->second);

		itr ++;
	}

	while(_new_itr != _s2d->end()){
		ris_set.insert(_new_itr->second);

		_new_itr ++;
	}

	if(_k_pre <= 0 || _k_pre >= 1){
		cout << "err _k_pre " << _k_pre << endl;
		exit(0);
	}

	multiset<double>::iterator ms_itr = ris_set.upper_bound(-0.5);
	{
		stringstream _ss;
		_ss << "remove" << " from ris_set[" << ris_set.size()
			<< "] with threshold " << *ms_itr << endl;
		util::log(_ss);
	}
	ris_set.erase(ris_set.begin(), ms_itr);
	{
		stringstream _ss;
		_ss << "left" << " ris_set[" << ris_set.size() << "]" << endl;
		util::log(_ss);
	}

	int k = (int)(ris_set.size() * _k_pre);
	multiset<double>::reverse_iterator ms_ritr = ris_set.rbegin();
	{
		stringstream _ss;
		_ss << "The first " << k << " ris from " << ris_set.size()  << endl;
		util::log(_ss);

		util::track(_ss);
	}
	while(k > 0 && ms_ritr != ris_set.rend()){
		ms_ritr ++;
		k --;
	}
	if(ms_ritr == ris_set.rend()){
		stringstream _ss;
		_ss << "err ms_ritr@Rgraph.cpp@get_k_th_ris" << endl;
		util::log(_ss);
		exit(0);
	}
	else
	{
		return *ms_ritr;
	}

	return -1.0;
}

bool Rgraph::update_edge2ris(map<string, double>* _s2d)
{
	util::track("IN udpate_edge2ris", "\n");

	map<string, double>::iterator itr;
	for(itr = _s2d->begin(); itr != _s2d->end(); itr ++){
		this->edge2ris[itr->first] = itr->second;
	}

	util::track("OUT udpate_edge2ris", "\n");
	return true;
}

/* double = -1.0, delete
 * double = 1.0, insert
 *  */
map<string, double>* Rgraph::get_I_D_edges(map<string, double>* _s2d, double _theta)
{

	map<string, double>* _ret = new map<string, double>();

	if(util::nouse_s2d) return _ret;

	map<string, double>::iterator itr, _new_itr;
	itr = this->edge2ris.begin();
	_new_itr = _s2d->begin();
	bool itr_was_edge = false;
	bool itr_be_edge = false;
	bool new_be_edge = false;
	while(itr != this->edge2ris.end() && _new_itr != _s2d->end())
	{
		int _cmp = strcmp(itr->first.c_str(), _new_itr->first.c_str());
		itr_was_edge = (itr->second) > (this->pre_theta);
		itr_be_edge = (itr->second) > (_theta);
		new_be_edge = (_new_itr->second) > (_theta);
		if(_cmp > 0)
		{
			/* insert new_itr edge */
			if(new_be_edge){
				_ret->insert(pair<string, double>(_new_itr->first, 1.0));
			}

			_new_itr ++;
		}
		else
		if(_cmp < 0)
		{
			/* delete itr edge */
			if(itr_was_edge && !itr_be_edge){
				_ret->insert(pair<string, double>(itr->first, -1.0));
			}
			else
			/* insert itr edge */
			if(!itr_was_edge && itr_be_edge){
				_ret->insert(pair<string, double>(itr->first, 1.0));;
			}

			itr ++;
		}
		else
		if(_cmp == 0)
		{
			/* delete edge */
			if(itr_was_edge && ! new_be_edge){
				_ret->insert(pair<string, double>(itr->first, -1.0));
			}
			else
			/* insert edge */
			if(!itr_was_edge && new_be_edge){
				_ret->insert(pair<string, double>(itr->first, 1.0));;
			}

			itr ++;
			_new_itr ++;
		}

	}/* end while */

	while(itr != this->edge2ris.end()){
		itr_was_edge = (itr->second) > (this->pre_theta);
		itr_be_edge = (itr->second) > (_theta);

		/* delete itr edge */
		if(itr_was_edge && !itr_be_edge){
			_ret->insert(pair<string, double>(itr->first, -1.0));
		}
		else
		/* insert itr edge */
		if(!itr_was_edge && itr_be_edge){
			_ret->insert(pair<string, double>(itr->first, 1.0));;
		}

		itr ++;
	}

	while(_new_itr != _s2d->end()){
		new_be_edge = (_new_itr->second) > (_theta);

		/* insert new_itr edge */
		if(new_be_edge){
			_ret->insert(pair<string, double>(_new_itr->first, 1.0));;
		}

		_new_itr ++;
	}

	return _ret;
}


