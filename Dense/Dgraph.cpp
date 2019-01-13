/*
 * Dgraph.cpp
 *
 *  Created on: 2015-3-6
 *      Author: liyouhuan
 */

#include "Dgraph.h"
#include "Dense.h"

map<iiEdge, double> Dgraph::edge2weight;
map<int, set<int> > Dgraph::vertex2neighbors;

ofstream Dgraph::ratio_computation;
ofstream Dgraph::f_coming_w;
ofstream Dgraph::f_delta_w;
ofstream Dgraph::f_e2weight;


Dgraph::Dgraph(){

	string _edge_path = "D:/things/Data/stem03_7-9_edge";
	this->edgestream.open(_edge_path.c_str(), ios::in);
	if(! this->edgestream){
		cout << "Can not open " << _edge_path << endl;
		exit(0);
	}

	this->slice_max = util::DENSE_TWEET_LIFE / util::INTERVAL;

	this->ratio_computation.open("ratio_computation");
	this->f_coming_w.open("coming_w");
	this->f_delta_w.open("delta_w");
	this->f_e2weight.open("e2weight");

}

Dgraph::~Dgraph(){
	this->edgestream.close();
	this->ratio_computation.close();
	this->f_coming_w.close();
	this->f_delta_w.close();
	this->f_e2weight.close();
}

void Dgraph::initial_dgraph(){

}

/*ret: delta weight of edge */
map<iiEdge, double>* Dgraph::next_delta(){
	map<string, int>* _e2freq = new map<string, int>();
	int ts = util::getNextInterval(this->edgestream, *_e2freq, util::INTERVAL);

	Dense::time_stamp = ts;

	util::track("\t----first nextIntl", "\n");

	if(_e2freq->empty() || ts > 80000){ // || ts < 71730
		delete _e2freq;
		return NULL;
	}

//	cout << " slice_num: " << this->slice_edge2freq.size() << endl;

	if((int)this->slice_edge2freq.size() >= this->slice_max)
	{
		vector<map<string, int>* >::iterator _oldest = this->slice_edge2freq.begin();
		delete (*_oldest);
		this->slice_edge2freq.erase(_oldest);
		this->slice_edge2freq.push_back(_e2freq);
	}
	else
	{
		this->slice_edge2freq.push_back(_e2freq);
	}

	util::track("\t----slice", "\n");

	map<string, int> _visible;
	for(int i = 0; i < (int)this->slice_edge2freq.size(); i ++){
		map<string, int>* _tmp_e2freq = this->slice_edge2freq[i];
		map<string, int>::iterator _tmp_itr = _tmp_e2freq->begin();
		while(_tmp_itr != _tmp_e2freq->end())
		{
			_visible[_tmp_itr->first] += _tmp_itr->second;

			_tmp_itr ++;
		}
	}

	util::track("\t-----visible", "\n");

	map<iiEdge, double>* _coming_w = this->coming_weight(_visible);
	{
		this->f_coming_w << ts << endl;
		this->f_coming_w << this->to_str(_coming_w);
		this->f_coming_w << endl << endl;
	}

	if(util::debug_check_coming_w){
		map<iiEdge, double>::iterator tmp_itr = _coming_w->begin();
		int _count = 10;
		int _i = 0;
		stringstream _ss;
		while(tmp_itr != _coming_w->end() && _i < _count){
			_ss << tmp_itr->first.to_str() << " " << tmp_itr->second << endl;
			_i ++;
			tmp_itr ++;
		}
		_ss << "*\n";
		map<iiEdge, double>::reverse_iterator rtmp_itr = _coming_w->rbegin();
		_i = 0;
		while(rtmp_itr != _coming_w->rend() && _i < _count){
			_ss << rtmp_itr->first.to_str() << " " << rtmp_itr->second << endl;
			_i ++;
			rtmp_itr ++;
		}

		util::track(_ss);
	}

	util::track("\t----coming_weight", "\n");

	map<iiEdge, double>* _delta_w = this->delta_weight(_coming_w);



	util::track("\t----delta_weight", "\n");

	delete _coming_w;
	{
		this->f_delta_w << ts << endl;
		this->f_delta_w << this->to_str(_delta_w);
		this->f_delta_w << endl << endl;

		this->f_e2weight << ts << endl;
		this->f_e2weight << this->to_str(& this->edge2weight);
		this->f_e2weight << endl << endl;
	}

	return _delta_w;
}

int Dgraph::get_neighbors(int _v, set<int>& _neighbors){
	map<int, set<int> >::iterator itr = Dgraph::vertex2neighbors.find(_v);
	if(itr == Dgraph::vertex2neighbors.end()){
		return 0;
	}

	_neighbors.insert((itr->second).begin(), (itr->second).end());

	return (itr->second).size();
}

double Dgraph::weight(int _from, int _to){
	iiEdge _e(_from, _to);
	map<iiEdge, double>::iterator itr = Dgraph::edge2weight.find(_e);

	if(itr == Dgraph::edge2weight.end()){
		return 0.0;
	}

	return itr->second;
}

double Dgraph::log_likelihood_ratio(int _f1, int _f2, int _f_both, int _f_total){
	int N[2][2];
	double E[2][2];
	N[1][1] = _f_both;
	N[1][0] = _f1 - _f_both;
	N[0][1] = _f2 - _f_both;
	N[0][0] = _f_total - _f1 - _f2 + _f_both;

	E[1][1] = _f1 * (_f2 / (_f_total+0.0));
	E[1][0] = _f1 * ((_f_total - _f2) / (_f_total+0.0));
	E[0][1] = _f2 * ((_f_total - _f1) / (_f_total+0.0));
	E[0][0] = (_f_total - _f1) * ((_f_total - _f2) / (_f_total+0.0));

	double _ret = 0.0;
	for(int i = 0; i < 2; i ++)
	{
		for(int j = 0; j < 2; j ++)
		{
			_ret += 2 * N[i][j] * log(N[i][j] / (E[i][j]+0.0));
		}
	}

	Dgraph::ratio_computation << _f1 << "\t" << _f2 << " both" << _f_both
		   << " total" << _f_total << " ratio[" << _ret << "]" << endl;

	Dgraph::ratio_computation.flush();

	return _ret;
}

double Dgraph::correlation(int _f1, int _f2, int _f_both, int _f_total){
	double _ret = 0.0;
	double _tmp1 = (_f_total*_f_both) - (_f1 * _f2);

	double _tmp2_1 = sqrt( (_f_total-_f1)*_f1 + 0.0);
	double _tmp2_2 = sqrt( (_f_total-_f2)*_f2 + 0.0);
	double _tmp2 = (_tmp2_1 * _tmp2_2);

	_ret = _tmp1 / _tmp2;

	Dgraph::ratio_computation << "f1[" <<  _f1 << "]\tf2[" << _f2 << "] both" << _f_both
		   << " total" << _f_total << " ratio[" << _ret << "]" << endl;

	Dgraph::ratio_computation.flush();

	return _ret;
}

double Dgraph::chi_square(int _f1, int _f2, int _f_both, int _f_total){
	int N[2][2];
	double E[2][2];
	N[1][1] = _f_both;
	N[1][0] = _f1 - _f_both;
	N[0][1] = _f2 - _f_both;
	N[0][0] = _f_total - _f1 - _f2 + _f_both;

	E[1][1] = _f1 * (_f2 / (_f_total+0.0));
	E[1][0] = _f1 * ((_f_total - _f2) / (_f_total+0.0));
	E[0][1] = _f2 * ((_f_total - _f1) / (_f_total+0.0));
	E[0][0] = (_f_total - _f1) * ((_f_total - _f2) / (_f_total+0.0));

	double _ret = 0.0;
	for(int i = 0; i < 2; i ++)
	{
		for(int j = 0; j < 2; j ++)
		{
			_ret += (N[i][j] - E[i][j])*(N[i][j] - E[i][j]) / (E[i][j]+0.0);
		}
	}

//	Dgraph::ratio_computation << "\t\tX " << _f1 << "\t" << _f2 << " both" << _f_both
//		   << " total" << _f_total << " ratio[" << _ret << "]" << endl;

//	Dgraph::ratio_computation.flush();

	return _ret;
}

string Dgraph::d_getStrByID(int _v){
	map<int, string>::iterator itr = this->id2str.find(_v);
	if(itr == this->id2str.end()){
		return "NULL";
	}

	return itr->second;
}

string Dgraph::strong_tostr(double _threshold){

	if(_threshold < 0.0 || _threshold > 1.0){
		cout << "strong_tostr err" << endl;
		exit(0);
	}

	multimap<double, string> w2edge;
	map<iiEdge, double>::iterator itr = this->edge2weight.begin();
	string _sfrom;
	string _sto;
	while(itr != this->edge2weight.end())
	{
		_sfrom =  Dgraph::d_getStrByID(itr->first.from);
		_sto = Dgraph::d_getStrByID(itr->first.to);
		string _edge = _sfrom + " " + _sto;
		w2edge.insert(pair<double, string>(itr->second, _edge));

		itr ++;
	}

	int k = w2edge.size() * _threshold + 1;

	stringstream _ss;
	multimap<double, string>::reverse_iterator ritr = w2edge.rbegin();
	while(ritr != w2edge.rend() && k > 0){
		_ss << ritr->second << " " << ritr->first << endl;

		ritr ++;
		k --;
	}

	return _ss.str();
}

string Dgraph::to_precisionstr()
{
	multimap<double, string> w2edge;
	map<iiEdge, double>::iterator itr = this->edge2weight.begin();
	string _sfrom;
	string _sto;
	while(itr != this->edge2weight.end())
	{
		_sfrom =  Dgraph::d_getStrByID(itr->first.from);
		_sto = Dgraph::d_getStrByID(itr->first.to);
		string _edge = _sfrom + " " + _sto;
		w2edge.insert(pair<double, string>(itr->second, _edge));

		itr ++;
	}

	multimap<double, string>::reverse_iterator ritr;
	int tmp_i = 0;
	int _positive = 0;
	//{0.04, 0.08, 0.12, 0.16, 0.20}
	double tao[5] = {0.03, 0.06, 0.09, 0.12,0.15};
	int top[5] = {20, 40, 60, 80, 100};
	for(int i = 0; i < 5; i ++){
		top[i] = w2edge.size() * tao[i];
	}

	double prec[5] = {0, 0, 0, 0, 0};
	for(ritr = w2edge.rbegin(); ritr != w2edge.rend(); ritr ++)
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

/* private */

map<iiEdge, double>* Dgraph::coming_weight(map<string, int>& _edge2freq){
	map<iiEdge, int> iiedge2freq;
	map<string, int>::iterator itr = _edge2freq.begin();
	stringstream decode_edge;
	char k1[100], k2[100];
	while(itr != _edge2freq.end()){
		decode_edge.str("");
		decode_edge.clear();
		memset(k1, 0, sizeof(k1));
		memset(k2, 0, sizeof(k2));

		decode_edge << itr->first;
		decode_edge >> k1 >> k2;

		int id1 = this->getID_add(k1);
		int id2 = this->getID_add(k2);
		if(id1 < id2){
			iiEdge _e(id1, id2);
			iiedge2freq[_e] = itr->second;
		}else{
			iiEdge _e(id2, id1);
			iiedge2freq[_e] = itr->second;
		}

		itr ++;
	}

	map<int, int> v2freq;
	int _total_freq = 0;
	{
		map<iiEdge, int>::iterator tmp_itr = iiedge2freq.begin();
		while(tmp_itr != iiedge2freq.end())
		{
			int id1 = (tmp_itr->first).from;
			int id2 = (tmp_itr->first).to;
			v2freq[id1] += tmp_itr->second;
			v2freq[id2] += tmp_itr->second;
			_total_freq += tmp_itr->second;

			tmp_itr ++;
		}
	}

	if(util::weight_loglikelihood){
		return this->log_likelihood(iiedge2freq, v2freq, _total_freq);
	}else{
		return this->correlation_coefficient(iiedge2freq, v2freq, _total_freq);
	}
}

map<iiEdge, double>* Dgraph::log_likelihood
(map<iiEdge, int>& _edge2freq, map<int, int> _v2freq, int _total_freq)
{
	map<iiEdge, double>* edge2weight = new map<iiEdge, double>();

	map<iiEdge, int>::iterator itr = _edge2freq.begin();
	while(itr != _edge2freq.end())
	{
		int v1 = (itr->first).from;
		int v2 = (itr->first).to;
		int f1 = _v2freq[v1];
		int f2 = _v2freq[v2];
		int f_both = (itr->second);

		double _w = Dgraph::log_likelihood_ratio(f1, f2, f_both, _total_freq);
		(*edge2weight)[itr->first] = _w;

		itr ++;
	}

	return edge2weight;
}

map<iiEdge, double>* Dgraph::correlation_coefficient
(map<iiEdge, int>& _edge2freq, map<int, int> _v2freq, int _total_freq)
{
	map<iiEdge, double>* edge2weight = new map<iiEdge, double>();

	map<iiEdge, int>::iterator itr = _edge2freq.begin();
	while(itr != _edge2freq.end())
	{
		int v1 = (itr->first).from;
		int v2 = (itr->first).to;
		int f1 = _v2freq[v1];
		int f2 = _v2freq[v2];
		int f_both = (itr->second);

		double _w;
		double _chi = Dgraph::chi_square(f1, f2, f_both, _total_freq);
		{
			string _s1 = this->d_getStrByID(v1);
			string _s2 = this->d_getStrByID(v2);
			this->ratio_computation << "\n" << _s1 << "[" << v1 << "]\t" << _s2 << "[" << v2 << "]" << endl;
		}
		if(_chi <= Dgraph::CHI_THRESHOLD){
			this->ratio_computation << "X: " << v1 << " " << v2 << endl;
			_w = 0;
		}else{
			_w = Dgraph::correlation(f1, f2, f_both, _total_freq);
		}

		(*edge2weight)[itr->first] = _w;

		itr ++;
	}

	return edge2weight;
}

/* cal delta
 * update edge2weight */
map<iiEdge, double>* Dgraph::delta_weight(map<iiEdge, double>* _coming_w){
	stringstream _l_ss;
	int n_pre = 0, n_coming = 0, n_both = 0;

	map<iiEdge, double>* _delta_w = new map<iiEdge, double>();
	map<iiEdge, double>::iterator itr_coming = _coming_w->begin();
	map<iiEdge, double>::iterator itr_pre = Dgraph::edge2weight.begin();
	while(itr_coming != _coming_w->end() &&
		  itr_pre != Dgraph::edge2weight.end())
	{
		if((itr_coming->first) > (itr_pre->first))
		{
			(*_delta_w)[itr_pre->first] = - itr_pre->second;
			itr_pre ++;
			n_pre ++;

		}
		else
		if((itr_coming->first) < (itr_pre->first))
		{
			(*_delta_w)[itr_coming->first] = itr_coming->second;
			itr_coming ++;
			n_coming ++;

		}
		else
		{

			n_both ++;
			(*_delta_w)[itr_coming->first] = itr_coming->second - itr_pre->second;
			itr_coming ++;
			itr_pre ++;
		}
	}

	while(itr_coming != _coming_w->end())
	{
		(*_delta_w)[itr_coming->first] = itr_coming->second;
		itr_coming ++;
		n_coming ++;
	}

	while(itr_pre != Dgraph::edge2weight.end())
	{
		(*_delta_w)[itr_pre->first] = - itr_pre->second;
		itr_pre ++;
		n_pre ++;
	}

	{
		_l_ss << "pre=" << n_pre << " coming=" << n_coming << " both=" << n_both << endl;
		util::track(_l_ss);
	}

	Dgraph::edge2weight.clear();

	Dgraph::edge2weight.insert(_coming_w->begin(), _coming_w->end());

	return _delta_w;
}

void Dgraph::build_neighbors(){
	map<iiEdge, double>::iterator itr = this->edge2weight.begin();
	this->vertex2neighbors.clear();
	int v1, v2;
	while(itr != this->edge2weight.end())
	{
		v1 = itr->first.from;
		v2 = itr->first.to;

		this->vertex2neighbors[v1].insert(v2);
		this->vertex2neighbors[v2].insert(v1);

		itr ++;
	}
}

/*  */
map<string, int>* Dgraph::get_total_edge2freq(){

	return NULL;
}

string Dgraph::to_str(map<iiEdge, double>* _e2w){
	stringstream _ss;
	map<iiEdge, double>::iterator itr = _e2w->begin();
	while(itr != _e2w->end())
	{
		_ss << "[" << itr->first.from << ", " << itr->first.to << " = " << itr->second << "] ";

		itr ++;
	}

	return _ss.str();
}

int Dgraph::getID_add(string _keyword){
	map<string, int>::iterator itr = this->str2id.find(_keyword);
	if(itr == this->str2id.end()){
		int id = this->str2id.size();
		this->str2id[_keyword] = id;
		this->id2str[id] = _keyword;
		return id;
	}

	return itr->second;
}
