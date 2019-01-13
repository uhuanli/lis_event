/*
 * main.cpp
 *
 *  Created on: 2015-1-13
 *      Author: liyouhuan
 */
#include "util/util.h"
#include "Rising/Rising.h"
#include "Dense/Dense.h"

/*
 *
 */
void main_inter(){

}



int main(int argc, char* args[])
{
	cout << "::::main" << endl;
	stringstream _ss;
	for(int i = 1; i < argc; i ++){
		_ss << args[i] << " ";
	}

	int _exp;

	if(argc >= 2)
	{
		_ss >> _exp;
		if(_exp == 0)// dense
		{
			double _e_norm, _tmax;
			int _nmax;
			_ss >> _e_norm >> _tmax >> _nmax;
			Dense _dense(60, 3, _e_norm, _tmax, _nmax, false);//(60, 3, 1, 0.4, 6);
			cout << "::::dense_run" << endl;
			_dense.dense_run();
			cout << "::::dense_end" << endl;
		}
		else
		if(_exp == 1)// rising
		{
			double _e_norm, _theta;
			int _parallel;
			_ss >> _e_norm >> _theta >> _parallel;
			Rising _rising(10, 3, 1.01, _parallel, _e_norm, _theta);
			cout << "::::rising_run" << endl;
			_rising.rising_run();
			cout << "::::rising_run" << endl;
		}
		else
		if(_exp == 2)
		{
			Rising _rising(10, 3, 1.01, 1, 1.0, 0.10);
			cout << "::::rising_run" << endl;
			_rising.rising_run();
			cout << "::::rising_run" << endl;
		}
		else
		if(_exp == 3)/* get_strongedges from log */
		{
			util::get_strongedges = true;
			util::THETA_PREC = 0.1;
			Dense _dense(60, 3, 1.0, 0.4, 6, true);//(60, 3, 1, 0.4, 6);
			cout << "::::dense_run" << endl;
			_dense.dense_run();
			cout << "::::dense_end" << endl;
		}
		else
		if(_exp == 4)/* get_strongedges from corr */
		{
			util::get_strongedges = true;
			util::THETA_PREC = 0.1;
			Dense _dense(60, 3, 1.0, 0.4, 6, false);//(60, 3, 1, 0.4, 6);
			cout << "::::dense_run" << endl;
			_dense.dense_run();
			cout << "::::dense_end" << endl;
		}
		else
		if(_exp == 5)/* measure precision */
		{
			int _measure = 0; // 1 for log, 2 for corr

			_ss >> _measure;
			if(_measure != 1 && _measure != 2){
				cout << "measure bug" << endl;
				system("pasue");
				exit(0);
			}

			bool is_log = (_measure == 1);

			util::cal_precision = true;
			{
				Dense _dense(60, 3, 1.0, 0.4, 6, is_log);//(60, 3, 1, 0.4, 6);
				_dense.dense_run();
			}
		}
		else
		if(_exp == 6)/* varying window size */
		{
			cout << "varying win size ";
			double _e_norm;
			int  winsize;
			_ss >> _e_norm >> winsize;
			cout << " norm=" << _e_norm << "\twinsz=" << winsize << endl;
			Rising _rising(winsize, 3, 1.01, 1, _e_norm, 0.05);
			cout << "::::rising_run" << endl;
			_rising.rising_run();
			cout << "::::rising_run" << endl;
		}
		else
		if(_exp == 7)
		{
			char _case[100];
			_ss >> _case;
			Rising _rising(10, 3, 1.01, 1, 1.0, 0.05);
			_rising.case_str = string(_case);
			util::load_case = true;
			cout << "case for " << _rising.case_str << endl;

			cout << "::::rising_run" << endl;
			_rising.rising_run();
			cout << "::::rising_run" << endl;
		}
		else
		if(_exp == 8)/* strong_edges of acceleration */
		{
			util::acceler_prec = true;
			util::THETA_PREC = 0.1;
			Rising _rising(10, 3, 1.01, 1, 1.0, 0.1);
			cout << "acceleration" << endl;
			cout << "::::rising_run" << endl;
			_rising.rising_run();
			cout << "::::rising_run" << endl;
		}
		else
		if(_exp == 9)/* strong_edges of frequency based */
		{
			util::frequency_prec = true;
			util::THETA_PREC = 0.1;
			Rising _rising(10, 3, 1.01, 1, 1.0, 0.1);

			cout << "frequency" << endl;
			cout << "::::rising_run" << endl;
			_rising.rising_run();
			cout << "::::rising_run" << endl;
		}
	}
	else
	{
		cout << "eclispe" << endl;
		util::label_strong();
	}
}


