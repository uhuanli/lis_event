/*
 * edge.cpp
 *
 *  Created on: 2015-1-13
 *      Author: liyouhuan
 */

#include "kEdge.h"
#include "../util/util.h"
#include<math.h>

kEdge::kEdge()
{
	this->head = 0;
	this->size = 0;
	this->lis_len = -1;
	this->time_stamp = -1;
	this->freq_seq = NULL;
	this->rlen = NULL;
	this->sval_index = NULL;
	this->bs_tail_index = NULL;
}
kEdge::~kEdge()
{
	delete[] this->freq_seq;
	delete[] this->rlen;
	delete[] this->sval_index;
}

bool kEdge::initial()
{
	this->size = 0;
	this->head = 0;

	int _size = util::SEQ_MEM_SIZE;

	if(this->freq_seq == NULL)
	{
		this->freq_seq = new int[_size];
	}

	if(this->rlen == NULL)
	{
		this->rlen = new int[_size];
	}

	if(this->sval_index == NULL)
	{
		this->sval_index = new int[_size];
	}

	if(this->bs_tail_index == NULL)
	{
		this->bs_tail_index = new int[_size];
	}
	for(int i = 0; i < _size; i ++)
	{
		this->bs_tail_index[i] = -1;
	}

	this->ris_delta = 0.0;

	return true;
}

int kEdge::update_edge(int _freq, int _t_cur)
{
	return -1;
}

/*
 * 1. move tail forward some step
 * ("some" : the previous insertion could be more than one time points ago
 * we need add some zeros between previous insertion and the current one
 * Since several consecutive zeros could be regarded as only one zero, then
 * "some" is in fact only two, namely, zero and the current one)
 * 2. update index when insert (zero and) the current frequency
 * compute rlen of the new tail
 * compute sval of the new tail
 * update ris directly
 * 3. update time_stamp as the current time point and size++ (or size+=2);
 * return size
 *  */
int kEdge::insert_tail(int _freq, int _t_cur)
{
	if(_freq > kEdge::MAX_FREQ){
		cout << "At " << _t_cur << "freq " << _freq <<   " is too large" << endl;
		exit(0);
	}
//
//	if(e_debug){
//		cout <<"insert " << _freq << " at time_" << _t_cur << endl;
//	}

	int _t_prev = this->time_stamp;
	this->time_stamp = _t_cur;
	bool insert_zero = (_t_prev + 1 <_t_cur) && (_t_prev != -1);

	int insert_pos = -1;
	if(insert_zero){
		insert_pos = (this->head + this->size) % (util::SEQ_MEM_SIZE);
		this->freq_seq[insert_pos] = 0;
		this->rlen[insert_pos] = 1;
		this->sval_index[insert_pos] = insert_pos;
		this->bs_tail_index[0] = insert_pos;
		this->size ++;
//		if(e_debug){
//			cout <<"insert " << 0 << " pos_ " << insert_pos << endl;
//		}
	}

	{
//		if(e_debug){
//			cout <<"head " << this->head << " and size " << this->size << endl;
//		}
	}

	insert_pos = (this->head + this->size) % (util::SEQ_MEM_SIZE);
	this->freq_seq[insert_pos] = _freq;
	{
//		if(e_debug){
//			cout <<"insert " << _freq << " pos_ " << insert_pos << endl;
//		}
	}
	this->size ++;
	for(int i = 0; i < (util::SEQ_MEM_SIZE); i ++)
	{
		if(this->bs_tail_index[i] == -1)/* bs_i is empty*/
		{
			this->bs_tail_index[i] = insert_pos;
			this->rlen[insert_pos] = i+1;
			break;
		}

		int _bs_tail_freq = this->freq_seq[ (this->bs_tail_index[i]) ];
		if(_freq <= _bs_tail_freq)
		{
			this->bs_tail_index[i] = insert_pos;
			this->rlen[insert_pos] = i+1;
			break;
		}
	}

	int _rlen = this->rlen[insert_pos];
	int _sval_index = -1;
	if(_rlen == 1){
		_sval_index = insert_pos;
	}else{
		int _upper_bs_tail = this->bs_tail_index[_rlen-1 - 1];
		_sval_index = this->sval_index[_upper_bs_tail];
	}
	this->sval_index[insert_pos] = _sval_index;

	/* is lis */
	if(this->lis_len == _rlen)
	{
		int _sval = this->freq_seq[_sval_index];
		double _ris = kEdge::one_rising(_rlen, _sval, _freq);

		double _pre_ris = this->ris;
		if(this->ris < _ris){
			this->ris = _ris;
		}

		/* insertion is always conducted before deletion  */
		this->ris_delta = this->ris - _pre_ris;
	}
	else /* new lis_len */
	if(this->lis_len < _rlen)
	{
		int _sval = this->freq_seq[_sval_index];

		double _pre_ris = this->ris;
		this->ris = kEdge::one_rising(_rlen, _sval, _freq);
		this->lis_len = _rlen;
		this->ris_delta = this->ris - _pre_ris;
	}


//	if(util::debug_edge_str){
//		cout << "insert " << _freq << " at " << _t_cur << endl;
//		cout << this->to_str("\t") << endl;
//	}
	return this->size;
}

/*
 * move head forward a step(if head value is zero, forward two steps)
 * recompute rlen of all elements based on their previous value
 * recompute sval of all elements and update ris
 * size --;(if head value is zero, size -= 2;)
 * return left size
 *  */
int kEdge::drop_head(int _t_cur)
{
	{
		util::track("IN drop_head", "\n");
	}

	int _head_val = this->freq_seq[ this->head ];
	{
//		if(util::total_on_off){
//		stringstream _ss;
//		_ss << "just want to drop " << _head_val << endl;
//		_ss << this->to_str() << "\n-----" << endl;
//		util::log(_ss);
//		}

		stringstream _ss;
		_ss << "just drop " << _head_val << endl;
		_ss << this->to_str() << "\n------\n" << endl;
		util::track(_ss);
	}

	if(_head_val == 0)
	{
		if(this->size == 2){
			this->size = 0;
			return 0;
		}

//		if(e_debug)
//		{
//			cout << "drop zero" << endl;
//		}
		this->drop_one();
		/* update rising will be done later */
	}

	if(this->size == 1){
		this->size = 0;
		return 0;
	}
//	if(e_debug){
//		cout << "drop " << _head_val << endl;
//	}
	this->drop_one();

	double _pre_ris = this->rising();
	this->update_rising();

	/* insertion has be done if equal */
	if(this->time_stamp == _t_cur){
		this->ris_delta += this->ris - _pre_ris;
	}else{
		this->ris_delta = this->ris - _pre_ris;
	}

//	{
//		if(util::total_on_off){
//		stringstream _ss;
//		_ss << this->to_str() << endl;
//		util::log(_ss);
//		}
//	}

	return this->size;
}

/* get rising strength */
double kEdge::rising()
{
	if(this->size == 0){
		return -1.0;
	}
	return this->ris;
}

double kEdge::get_delta()
{
	if(this->ris_delta < 0.001 && this->ris_delta > -0.001){
		this->ris_delta = 0.0;
	}
	return this->ris_delta;
}

bool kEdge::isEmpty()
{
	return this->size == 0;
}

int kEdge::length(){
	return this->size;
}

string kEdge::to_str(string _prefix)
{
	stringstream _ret;

	_ret << "size[" << (int)this->size << "] ";
	_ret << "lis_len[" << (int)this->lis_len << "]";

	_ret << endl;

	_ret << this->freq_str() << endl;

	for(int i = 0; i < (int)this->size; i ++)
	{
		_ret << "\t" << this->one_str(i) << "\n";
	}

	_ret  << this->bs_str() << endl;
	_ret << "ris: " << this->rising() << endl;

	_ret << endl;

	return _ret.str();
}

void kEdge::drop_one()
{
	int _lis_len = -1;

	for(int i = 0; i < util::SEQ_MEM_SIZE; i ++){
		this->bs_tail_index[i] = -1;
	}


	for(int i = this->head + 1; i < (this->head + this->size); i ++)
	{
		int _i = i % (util::SEQ_MEM_SIZE);
		if(this->sval_index[_i] == this->head)
		{
			this->rlen[_i] --;
		}
		int _rlen = this->rlen[_i];
		this->bs_tail_index[_rlen-1] = _i;

		if(_lis_len < _rlen){
			_lis_len = _rlen;
		}

		int _sval_index = -1;
		if(_rlen == 1){
			_sval_index = _i;
		}else{
			int _upper_bs_tail = this->bs_tail_index[_rlen-1 - 1];
			_sval_index = this->sval_index[_upper_bs_tail];
		}
		this->sval_index[_i] = _sval_index;
	}

	this->lis_len = _lis_len;
	this->size --;
	this->head = (this->head + 1) % (util::SEQ_MEM_SIZE);

}

void kEdge::update_rising()
{
	this->ris = -1.0;

	for(int i = this->head; i < (this->head + this->size); i ++)
	{
		int _i = i % (util::SEQ_MEM_SIZE);
		int _rlen = this->rlen[_i];
		if(_rlen == this->lis_len)
		{
			int _sval = this->freq_seq[ (this->sval_index[_i]) ];
			int _freq = this->freq_seq[_i];
			double _ris = kEdge::one_rising(_rlen, _sval, _freq);
			if(this->ris < _ris){
				this->ris = _ris;
			}
		}
	}
}

double kEdge::one_rising(int _rlen, int _sval, int _freq)
{
	if(_rlen == 1){
		_sval = 0;
	}
	if(util::debug_airlin_contact){
		return _rlen*(_freq - _sval);
	}
	double l = (_rlen+0.0)/util::WIN_SIZE;
	double a = (_freq - _sval + 0.0) / (pow(util::EPSILON, _sval));
	return l * a;
}

int kEdge::rising_length(int _i)
{
	return this->rlen[_i];
}

string kEdge::one_str(int _i)
{
	stringstream _ss;
	_ss << "[" << _i << ": ";
	int id = (this->head + _i) % (util::SEQ_MEM_SIZE);
	_ss << "(fr=" << (int)this->freq_seq[id] << ") ";
	_ss << "(rl=" << (int)this->rlen[id] << ") ";
	int i_sval = this->sval_index[id];
	_ss << "(si=" << (int)this->freq_seq[i_sval] << ")] ";
	return _ss.str();
}

string kEdge::bs_str()
{
	stringstream _ret;
	_ret << "bs_tail: ";
	for(int i = 0; i < (int)this->size; i ++){
		int bs_id = this->bs_tail_index[i];
		if(bs_id != -1){
			_ret << "[(" << i << "):" << (int)this->freq_seq[bs_id] << "] ";
		}
	}
	return _ret.str();
}

string kEdge::freq_str()
{
	stringstream _ret;
	_ret << "freq: [" << this->freq_seq[this->head];
	for(int i = 1; i < (int)this->size; i ++){
		int id = (this->head+i)%(util::SEQ_MEM_SIZE);
		_ret << ", " << this->freq_seq[id];
	}
	_ret << "]";
	return _ret.str();
}

