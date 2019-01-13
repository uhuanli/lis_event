/*
 * util.cpp
 *
 *  Created on: 2015-2-2
 *      Author: liyouhuan
 */
#include "util.h"
#include "stopwords.h"
#include "windows.h"
#include "psapi.h"
int util::WIN_SIZE;
int util::SEQ_MEM_SIZE;
int util::INTERVAL;
int util::DENSE_TWEET_LIFE;//minute
RuntimeLis* util::rt_lis = NULL;
RuntimeDense* util::rt_dense = NULL;
ofstream util::flog;
ofstream util::run_track;
ofstream util::f_num_track;
set<string> util::stop_words;
double util::EPSILON = 1.05;
double util::THETA_PREC = 0.05;
char util::buf[util::buf_size];
int util::PARALLEL_NUM = 1;
double util::EDGE_NUM_NORM = 1;
double util::ACCURRACY;

long util::edge_num;
long util::freq_total = 0;

string util::strong_list_file;
set<string> util::all_event_edges;
map<string, int> util::first_appear;

bool util::weight_loglikelihood = true;
bool util::strong_edge_on_off = false;
bool util::use_stopword = false;
bool util::total_on_off = false;
bool util::get_strongedges = false;
bool util::cal_precision = false;
bool util::load_case = false;
bool util::acceler_prec = false;
bool util::frequency_prec = false;
string util::edge_str = "childhood forget";

/*  */
long long int util::get_time_cur()
{
	LARGE_INTEGER m_nTime;
	QueryPerformanceCounter(&m_nTime); //获取当前时间
	return m_nTime.QuadPart;
}
/*micro second*/
long util::cal_time(long long int _t1, long long int _t2){
	return (_t1-_t2)*1000.0*1000.0 / (util::ACCURRACY+0.0);
}

string util::space_str()
{
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);


	SIZE_T virtuallMemUsedByMe = pmc.WorkingSetSize;
	SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
	DWORDLONG total_vir = memInfo.ullTotalVirtual;
	DWORDLONG total_phy = memInfo.ullTotalPhys;

	stringstream _ss;
	_ss << "total_vir\t" << "total_phy\t" << "virtual\t" << "physical" << endl;
	_ss << total_vir << "\t";
	_ss << total_phy << "\t";
	_ss << virtuallMemUsedByMe << "\t";
	_ss << physMemUsedByMe;

	_ss << endl << endl << "freq_total\t" << util::freq_total << endl;

	return _ss.str();
}

bool util::add_firstappear(string _edge, int _ts){
	if(util::first_appear.find(_edge) == util::first_appear.end()){
		util::first_appear[_edge] = _ts;
		return true;
	}

	return false;
}

int util::get_firstappear(string _edge){
	map<string, int>::iterator itr = util::first_appear.find(_edge);
	if(itr == util::first_appear.end()){
		return -1;
	}

	return itr->second;
}

void util::load_caseedges(string _file, set<string>& _edges){
	ifstream _fin(_file.c_str());
	if(!_fin){
		cout << "load err " << _file << endl;
		exit(0);
	}

	char _buf[100];
	stringstream _ss;
	char k1[50], k2[50];
	int weight;
	int _count = 0;
	while(! _fin.eof())
	{
		_fin.getline(_buf, 99);
		if(strlen(_buf) < 3) break;
		_ss.clear();
		_ss.str("");
		_ss << _buf;
		_ss >> k1 >> k2;

		string _str = string(k1)+" "+string(k2);
		_edges.insert(_str);
		_count ++;
	}

	_fin.close();

	cout << "load case edges from " << _file << " " << _count << " edges" << endl;
}

void util::distinct_strongedges(string _from_file, string _to_file){
	ifstream _fin(_from_file.c_str());
	ofstream _fout(_to_file.c_str());
	if(!_fin || !_fout){
		cout << "fail to open distinct_strongedges" << endl;
		exit(0);
	}

	char _buf[100];
	stringstream _ss;
	char k1[50], k2[50];
	double ris;
	map<string, double> e2ris;
	while(! _fin.eof())
	{
		_fin.getline(_buf, 99);
		if(strlen(_buf) < 3) continue;
		_ss.clear();
		_ss.str("");

		_ss << _buf;
		_ss >> k1 >> k2 >> ris;
		string tmpstr = "" + string(k1) + " " + string(k2);
		if(e2ris[tmpstr] < ris){
			e2ris[tmpstr] = ris;
		}
	}

	multimap<double, string> ris2e;

	map<string, double>::iterator itr = e2ris.begin();
	while(itr != e2ris.end()){
		ris2e.insert(pair<double, string>(itr->second, itr->first));

		itr ++;
	}

	multimap<double, string>::reverse_iterator ritr = ris2e.rbegin();
	cout << "There are " << ris2e.size() << " edges" << endl;

	while(ritr != ris2e.rend()){
		_fout << ritr->second << " " << ritr->first << endl;
		_fout.flush();

		ritr ++;
	}

	_fin.close();
	_fout.close();
}

void util::rejust_strongedges(string _from_f, string _to_f, int _left_num){
	ifstream _fin(_from_f.c_str());
	ofstream _fout(_to_f.c_str());
	if(!_fin || !_fout){
		cout << "fail to open rejust_strongedges" << endl;
		exit(0);
	}

	char _buf[100];
	stringstream _ss;
	char k1[50], k2[50];
	double ris;
	vector<string> vstr;
	vector<double> vd;
	while(! _fin.eof())
	{
		_fin.getline(_buf, 99);
		if(strlen(_buf) < 3) continue;
		_ss.clear();
		_ss.str("");


		_ss << _buf;
		_ss >> k1 >> k2 >> ris;

		string tmpstr = "" + string(k1) + " " + string(k2);

		vstr.push_back(tmpstr);
		vd.push_back(ris);
	}

	int _total = vstr.size();
	double prob = (_left_num+0.0) / (_total+0.0);

	vector<string>::iterator sitr = vstr.begin();
	vector<double>::iterator ditr = vd.begin();
	while(sitr != vstr.end())
	{
		if(util::draw_lots(prob))
		{
			_fout << (*sitr) << " " << (*ditr) << endl;
			sitr ++;
			ditr ++;
		}
		else
		{
			sitr = vstr.erase(sitr);
			ditr = vd.erase(ditr);
		}
	}

	_fin.close();
	_fout.close();
}

void util::load_unlabel_edges(string _file, vector<string>& _edges, set<string>& _sedges){
	ifstream _fin(_file.c_str());
	if(!_fin){
		cout << "load err " << _file << endl;
		exit(0);
	}

	char _buf[100];
	stringstream _ss;
	char k1[50], k2[50];
	double ris;
	int weight;
	int _count = 0;
	while(! _fin.eof())
	{
		_fin.getline(_buf, 99);
		if(strlen(_buf) < 3) break;
		_ss.clear();
		_ss.str("");
		_ss << _buf;
		_ss >> k1 >> k2 >> ris;

		string _str = string(k1)+" "+string(k2);
		{
			if(_str == "harri potter"){
				cout << "harri potter" <<endl;
			}
		}
		_edges.push_back(_str);
		_sedges.insert(_str);
		{
			if(_str == "harri potter"){
				if(_sedges.find("harri potter") == _sedges.end()){
					cout << "not found" << endl;
				}else
				{
					cout << "found" << endl;
				}
			}
		}
		_count ++;
	}

	_fin.close();

	cout << "load from " << _file << " " << _count << " edges" << endl;
}

void util::label_strong(){
	set<string> edges_lis, edges_corr, edges_log;
	vector<string> v_lis, v_corr, v_log;
	string _f_lis, _f_corr, _f_log;
	string _folder = "D:/result/strong/init_strong/";
	_f_lis = _folder + "lis__parallel[1]_normalized[1]__theta[0.1].strong";
	_f_corr = _folder + "corr_thres[0.1].strong";
	_f_log = _folder + "log_thres[0.1].strong";

	edges_lis.clear(); edges_corr.clear(); edges_log.clear();
	v_lis.clear(); v_corr.clear(); v_log.clear();

	util::distinct_strongedges(_f_lis, _f_lis+"_distinct");
	util::distinct_strongedges(_f_corr, _f_corr+"_distinct");
	util::distinct_strongedges(_f_log, _f_log+"_distinct");


	util::load_unlabel_edges(_f_lis+"_distinct", v_lis, edges_lis);

	util::rejust_strongedges(_f_corr+"_distinct", _f_corr+"_distinct_rejust", v_lis.size());
	util::rejust_strongedges(_f_log+"_distinct", _f_log+"_distinct_rejust", v_lis.size());


	util::load_unlabel_edges(_f_corr+"_distinct_rejust", v_corr, edges_corr);
	util::load_unlabel_edges(_f_log+"_distinct_rejust", v_log, edges_log);

	string _lis_eventedges = "D:/result/strong/edge_lis";
//	ofstream _event_edges(_lis_eventedges.c_str());

	int weight = 0;
	set<string> _eventedges;
//	for(int i = 0; i < (int)v_lis.size(); i ++){
//		cout << v_lis[i] << " at " << i << "/" << v_lis.size() << " of lis ";
//		bool corr_has = (edges_corr.find(v_lis[i]) != edges_corr.end());
//		bool log_has = (edges_log.find(v_lis[i]) != edges_log.end());
//		cout << "corr[" << corr_has << "] " << "log[" << log_has << "] and Input your weight..." << endl;
//		cin >> weight;
//		_event_edges << v_lis[i] << " " << weight << endl;
//		_event_edges.flush();
//
//		if(weight >= util::load_threshold){
//			_eventedges.insert(v_lis[i]);
//		}
//		cout << endl;
//	}

	if(edges_lis.find("obama presid") == edges_lis.end()
		)
	{
		cout << "bug" << endl;
		exit(0);
	}
	else{
		cout << "not bug" << endl;
		exit(0);
	}

	/**/
	string _corr_eventedges = "D:/result/strong/edge_corr";
	ofstream _corr_edges(_corr_eventedges.c_str());

	for(int i = 0; i < v_corr.size(); i ++)
	{
		cout << "[" << v_corr[i] << "] at " << i << "/" << v_corr.size() << " of corr ";
		if(edges_lis.find(v_corr[i]) != edges_lis.end()){
			continue;
		}

		bool log_has = (edges_log.find(v_corr[i]) != edges_log.end());
		cout << "log[" << log_has << "] and Input your weight..." << endl;
		cin >> weight;
		_corr_edges << v_corr[i] << " " << weight << endl;
		_corr_edges.flush();

		if(weight >= util::load_threshold){
			_eventedges.insert(v_corr[i]);
		}
		cout << endl;
	}

	/**/
	string _log_eventedges = "D:/result/strong/edge_log";
	ofstream _log_edges(_log_eventedges.c_str());

	for(int i = 0; i < v_log.size(); i ++)
	{
		cout << v_log[i] << " at " << i << "/" << v_log.size() << " of log ";
		if(edges_lis.find(v_log[i]) != edges_lis.end()){
			continue;
		}
		if(edges_corr.find(v_log[i]) != edges_corr.end()){
			continue;
		}

		cout << "Input your weight..." << endl;
		cin >> weight;
		_log_edges << v_log[i] << " " << weight << endl;
		_log_edges.flush();

		if(weight >= util::load_threshold){
			_eventedges.insert(v_log[i]);
		}
		cout << endl;
	}


//	_event_edges.close();
	_corr_edges.close();
	_log_edges.close();
}

int util::load_threshold = 4;
void util::load_eventedges(string _file, set<string>& _edges){
	ifstream _fin(_file.c_str(), ios::in);
	if(!_fin){
		cout << "load err " << _file << endl;
		exit(0);
	}

	char _buf[100];
	stringstream _ss;
	char k1[50], k2[50];
	int weight;
	int _count = 0;
	while(! _fin.eof())
	{
		_fin.getline(_buf, 99);
		if(strlen(_buf) < 3) break;
		_ss.clear();
		_ss.str("");
		_ss << _buf;
		_ss >> k1 >> k2 >> weight;
		string sk1 = string(k1);
		string sk2 = string(k2);

		if(weight >= util::load_threshold)
		{
			_edges.insert(sk1+" "+sk2);
			_count ++;
		}
	}

	_fin.close();

	cout << "load from " << _file << " " << _count << " edges" << endl;
}

bool util::is_eventedge(string _edge)
{
	return util::all_event_edges.find(_edge) != util::all_event_edges.end();
}

bool util::is_stopword(string _word){
	return stopwords::is_stopword(_word);
}

bool util::draw_lots(double prob){
	int _n = rand() % 1000;
	return _n < (prob * 1000);
}


void util::scale_edges(double _prop, map<string, int>& e2freq){
	set<string> tobe_deleted;
	map<string, int>::iterator itr = e2freq.begin();
	while(itr != e2freq.end())
	{
		if(util::draw_lots(util::EDGE_NUM_NORM)){
			itr ++;
		}else{
			tobe_deleted.insert(itr->first);
			itr ++;
		}
	}

	set<string>::iterator sitr = tobe_deleted.begin();
	while(sitr != tobe_deleted.end())
	{
		e2freq.erase(*sitr);

		sitr ++;
	}
}

void util::get_rand(int _min, int _max, int _num, vector<int>& vrand)
{
	vrand.clear();

	int interval = _max - _min;
	if(interval < 1){
		cout << "err: max and min" << endl;
		exit(0);
	}
	for(int i = 0; i < _num; i ++){
		int _r = _min + (rand() % interval);
		vrand.push_back(_r);
	}
}

void util::log(const char* str, const char* lat)
{
	util::flog << str << lat;
	util::flog.flush();
}

void util::log(stringstream & _ss)
{
	util::flog << _ss.str();
	util::flog.flush();
}

void util::initial(){
	util::stop_words.insert("");
	stopwords::initial();

	srand((unsigned)time(NULL));

	LARGE_INTEGER m_nFreq;
	QueryPerformanceFrequency(&m_nFreq);
	util::ACCURRACY = (double)m_nFreq.QuadPart;

	util::flog.open("D:/result/flog", ios::out);
	if(! util::flog){
		cout << "log file err" << endl;
		exit(0);
	}

	util::run_track.open("D:/result/ftrack", ios::out);
	if(! util::run_track){
		cout << "track err" << endl;
		exit(0);
	}

	util::f_num_track.open("D:/result/num_track", ios::out);
	if(! util::f_num_track){
		cout << "num_track err" << endl;
		exit(0);
	}

	string _eventedges_lis = "D:/result/strong/edge_lis_done";
	util::load_eventedges(_eventedges_lis, util::all_event_edges);
}

void util::track(const char* str, const char* lat)
{
	if(! util::write_track) return;

	util::run_track << str << lat;
	util::run_track.flush();
}

void util::track(stringstream & _ss)
{
	if(! util::write_track) return;

	util::run_track << _ss.str();
	util::run_track.flush();
}

void util::num_track(const char* str, int _num)
{
	util::f_num_track << str << " = " << _num << endl;
	util::f_num_track.flush();
}

void util::num_track(stringstream & _ss)
{
	if(! util::on_num_track){
		return;
	}
	util::f_num_track << _ss.str();
	util::f_num_track.flush();
}

/* if ok, return date number, else return -1 */
int util::getNextMin(ifstream& _fin, map<string, int>& _s2i)
{
	if(_fin.eof()){
		return -1;
	}

	memset(util::buf, 0, sizeof(util::buf));
	memset(util::buf, 0, sizeof(util::buf));

	_fin.getline(util::buf, util::buf_size-1);
	if(strlen(util::buf) < 2){
		return -1;
	}

	int ts = 0;
	size_t _tab;
	size_t pre_tab;

	stringstream _ss(util::buf);
	string buf_str(util::buf);
	_ss >> ts;
	_tab = buf_str.find('\t');
	char k1[100], k2[100];
	string _tmp_str;
	int space_count = 0;
	int freq;
	while(! _ss.eof())
	{
		pre_tab = _tab;
		_tab = buf_str.find('\t', _tab+1);
		if(_tab == string::npos){
			_tab = buf_str.length();
		}

		freq = -1;
		space_count = 0;
		_ss >> k1;
		/* pre_tab+2 is in case that there is a space exactly after tab */
		for(size_t i = pre_tab+2; i < _tab; i ++)
		{
			if(buf_str.at(i) == ' ' && space_count == 0)
			{
				_ss >> k2;
				space_count ++;
			}
			else
			if(buf_str.at(i) == ' ' && space_count == 1)
			{
				_ss >> freq;
				space_count ++;
			}
		}
		if(space_count < 2 || k1[0] == '?' || k2[0] == '?'){
			continue;
		}

		util::freq_total += freq;

		if(util::use_stopword){
			if(util::is_stopword(string(k1)) || util::is_stopword(string(k2)))
				continue;
		}

		_tmp_str = string(k1)+" "+string(k2);

		_s2i[_tmp_str] += freq;

		if(util::load_case){
			util::add_firstappear(_tmp_str, ts);
		}
	}

	return ts;
}

int util::getNextInterval(ifstream& _fin, map<string, int>& _s2i, int _interval)
{
	_s2i.clear();
	int _tmp = 0;
	int _first_ts = -1;
	for(int i = 0; i < _interval; i ++)
	{
		_tmp = getNextMin(_fin, _s2i);

		if(_first_ts == -1)
		{
			_first_ts = _tmp;
		}

		if(_tmp == -1)
		{
			util::edge_num = _s2i.size();
			util::scale_edges(util::EDGE_NUM_NORM, _s2i);
			return _first_ts;
		}
	}

	util::edge_num = _s2i.size();
	util::scale_edges(util::EDGE_NUM_NORM, _s2i);

	return _first_ts;
}
