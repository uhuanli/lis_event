/*
 * Rgraph.h
 *
 *  Created on: 2015-2-6
 *      Author: liyouhuan
 */

#ifndef RGRAPH_H_
#define RGRAPH_H_
#include "TimeWindow.h"
#include "../util/util.h"

class Rgraph
{
public:
	Rgraph(string _e_file, int _intl);
	~Rgraph();

	map<string, double>* next_cc();
	string get_freq_strongstr();
	string get_acce_strongstr();
	string strongedge_str();
	string precisionedge_str();
	string get_acc_precisionstr();
	string get_freq_precisionstr();
	string strong_list_str();
	int getStrongEdges(vector<string> & _vec_edges);
private:

	string freq_strongstr(map<string, int>& _e2freq);
	string acce_strongstr(map<string, int>& _e2freq);
	string acce_strongs;
	string freq_strongs;
	string acce_precs;
	string freq_precs;
	double get_k_th_ris(map<string, double>* _s2d, double _pre);

	map<string, double>* get_delta();

	bool update_edge2ris(map<string, double>* _s2d);

	/* double = -1.0, delete
	 * double = 1.0, insert
	 */
	map<string, double>* get_I_D_edges(map<string, double>* _s2d, double _theta);

	int time_stamp;
	int interval;
	string edge_file;
	ifstream estream;
	TimeWindow t_win;
	double pre_theta;
	map<string, double> edge2ris;
	multimap<double, string> acc2edge;
	map<string, int> tmpedge2freq;
	char buf[100*1000];
	static const int buf_size = 100*1000;
	static const bool debug_get_next = false;
};


#endif /* RGRAPH_H_ */
