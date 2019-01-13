/*
 * util.h
 *
 *  Created on: 2015-2-2
 *      Author: liyouhuan
 */

#ifndef UTIL_H_
#define UTIL_H_

#include<iostream>
#include<sstream>
#include<fstream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<vector>
#include<map>
#include<set>
#include<time.h>
#include<math.h>
using namespace std;

class RuntimeLis;
class RuntimeDense;

class util
{
public:
	static double EPSILON;
	static double THETA_PREC;
	static int WIN_SIZE;
	static int SEQ_MEM_SIZE;
	static int INTERVAL;
	static int DENSE_TWEET_LIFE;//minute
	static const int buf_size = 1000*1000;
	static char buf[buf_size];
	static int PARALLEL_NUM;
	static double EDGE_NUM_NORM;
	static long edge_num;
	static long freq_total;
	static string strong_list_file;

	static set<string> all_event_edges;
	static map<string, int> first_appear;
	static bool add_firstappear(string _edge, int _ts);
	static int get_firstappear(string _edge);

	static double ACCURRACY;

	static RuntimeLis* rt_lis;
	static RuntimeDense* rt_dense;

	static ofstream flog;
	static ofstream run_track;
	const static bool on_num_track = false;
	static ofstream f_num_track;
	static set<string> stop_words;
	static bool weight_loglikelihood;

	static const bool debug_check_coming_w = true;
	static const bool debug_airlin_contact = false;
	static const bool nouse_s2d = true;
	static bool strong_edge_on_off;
	static bool use_stopword;
	static string edge_str;
	static const bool debug_ugraph = false;
	static const bool debug_total = false;
	static bool total_on_off;
	static const bool flog_on = false;
	static const bool write_track = false;
	static bool get_strongedges;
	static bool cal_precision;
	static bool load_case;
	static bool acceler_prec;
	static bool frequency_prec;

	static long long int get_time_cur();
	static long cal_time(long long int _t1, long long int _t2);
	static string space_str();

	static void load_caseedges(string _file, set<string>& _edges);

	static void distinct_strongedges(string _from_file, string _to_file);
	static void rejust_strongedges(string _from_f, string _to_f, int _left_num);
	static void load_unlabel_edges(string _file, vector<string>& _edges, set<string>& _sedges);
	static void label_strong();
	static int load_threshold;
	static void load_eventedges(string _file, set<string>& _edges);
	static bool is_eventedge(string _edge);
	static bool is_stopword(string _word);
	static bool draw_lots(double prob);
	static void scale_edges(double _prop, map<string, int>& e2freq);
	static void get_rand(int _min, int _max, int _num, vector<int>& vrand);
	static void log(const char* str, const char* lat);
	static void log(stringstream & _ss);
	static void initial();
	static void track(const char* str, const char* lat);
	static void track(stringstream & _ss);
	static void num_track(const char* str, int _num);
	static void num_track(stringstream & _ss);

	static int getNextMin(ifstream& _fin, map<string, int>& _s2i);
	static int getNextInterval(ifstream&, map<string, int>& _s2i, int _interval);
};


#endif /* UTIL_H_ */
