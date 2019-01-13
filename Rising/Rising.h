/*
 * Rising.h
 *
 *  Created on: 2015-3-11
 *      Author: liyouhuan
 */

#ifndef RISING_H_
#define RISING_H_

#include "../TimeWindow/TimeWindow.h"
#include "../util/util.h"
#include "../TimeWindow/Rgraph.h"
#include "../graph/Ugraph.h"

class Rising
{
public:
	Rising(int _winsz, int _intl, double _epsilon, int _parallel, double _e_norm, double _theta);
	~Rising();
	void rising_run();

	string case_str;
	void load_case_edges();
	void write_case_edges_ts();
	static multimap<string, int> strong_ts;
	static set<string> rcase_edges;

private:
	ofstream f_rising_result;
	ofstream f_strong_list;
	ofstream f_space;
	ofstream f_edgeprec;
	ofstream f_accprec;
	ofstream f_freqprec;

	void initial_cc();
	void test_cc();

};

#endif /* RISING_H_ */
