/*
 * TimeWindow.cpp
 *
 *  Created on: 2015-1-13
 *      Author: liyouhuan
 */

#include "TimeWindow.h"
#include "../util/util.h"
#include "../Rising/RuntimeLis.h"
TimeWindow::TimeWindow()
{
	this->graphs = NULL;
	this->head = 0;
	this->size = 0;
}

void TimeWindow::initial()
{
	if(this->graphs == NULL){
		this->graphs = new Tgraph*[util::SEQ_MEM_SIZE];
	}

	this->active_edges.clear();

	this->head = 0;
	this->size = 0;
}

/* push edge freq in new interval and return updated edges of G_w */
map<string, double>* TimeWindow::update(map<string, int> & _edge2freq, int _t_cur, set<string>& removed_edges)
{
	{
		stringstream _ss;
		_ss << "IN TimeWindow::update with edge2freq size " << _edge2freq.size();
		_ss << " at time: " << _t_cur << endl;
		util::track(_ss);
	}

	/* Insert new Tgraph into window*/
	Tgraph* _new_graph = this->new_Tgraph_insert(_edge2freq, _t_cur);
	int _new_pos = (this->head + this->size) % (util::SEQ_MEM_SIZE);
	this->size ++;
	this->graphs[_new_pos] = _new_graph;

	{
		if(util::debug_total){
			util::log("finish insertion", "\n");
		}

		{
			stringstream _ss;
			_ss << "finish insertion at pos " << _new_pos << endl;
			util::track("finish insertion", "\n");
		}
	}

	/* Remove oldest Tgraph from window */
	Tgraph* _del_graph = NULL;
	if(this->size > util::WIN_SIZE){
		_del_graph = this->del_Tgraph(_t_cur);
		this->size --;
		this->head = (this->head + 1) % (util::SEQ_MEM_SIZE);
	}

	{
		if(util::debug_total){
			util::log("finish deletion", "\n");
		}

		util::track("finish deletion", "\n");
	}

	map<string, double>* update_ris = new map<string, double>();
	S2Eptr_map::iterator itr;

	/* get updated edges from Inserted edges */
	S2Eptr_map * _s2e = & (_new_graph->str2edge);
	kEdge* eptr = NULL;
	for(itr = _s2e->begin(); itr != _s2e->end(); itr ++)
	{
		eptr = itr->second;
		if(eptr->get_delta() != 0.0){
			update_ris->insert(pair<string, double>(itr->first, eptr->rising()));
		}
	}

	{
		if(util::debug_total){
			util::log("finish updated from edge inserted", "\n");
		}

		util::track("finish updated from inserted", "\n");
	}

	if(_del_graph == NULL) return update_ris;

	/* get updated edges from Deleted edges */
	_s2e = & (_del_graph->str2edge);
	bool del_edge = false;
	removed_edges.clear();
	for(itr = _s2e->begin(); itr != _s2e->end(); itr ++)
	{
		eptr = itr->second;
		del_edge = (eptr->isEmpty());
		if(del_edge){
			this->active_edges.erase(itr->first);
			removed_edges.insert(itr->first);
			{
				stringstream _ss;
				_ss << "erase " << itr->first << "from active_edges with size "
						<< this->active_edges.size() << endl;
				util::track(_ss);
			}
			delete eptr;
			continue;
		}

		if(eptr->get_delta() != 0.0){
			update_ris->insert(pair<string, double>(itr->first, eptr->rising()));
		}
	}
	delete _del_graph;

	{
		if(util::debug_total){
			util::log("finish updated from edge deleted", "\n");
		}

		util::track("finish udpated from edge deleted", "\n");
	}

	util::track("OUT TimeWindow::update", "\n");
	{
		stringstream _ss;
		_ss << "return update_ris of size " << update_ris->size() << endl << endl << endl;
		util::track(_ss);
	}

	return update_ris;
}

int TimeWindow::gc()
{
	return 0;
}


/* created new Tgraph and update for insertion */
Tgraph* TimeWindow::new_Tgraph_insert(map<string, int> & _edge2freq, int _t_cur)
{
	util::track("IN TimeWindow::new_Tgraph_insert", "\n");
	Tgraph* _ret = new Tgraph();
	map<string, int>::iterator itr;
	kEdge * eptr = NULL;

	long long int tmp_begin = util::get_time_cur();
	int _count = 0;
	int _per_num = _edge2freq.size() / util::PARALLEL_NUM;
	for(itr = _edge2freq.begin(); itr != _edge2freq.end(); itr ++)
	{
		eptr = this->get_active_edge(itr->first);
		eptr->insert_tail(itr->second, _t_cur);
		_ret->add(itr->first, eptr);

		_count ++;
		if(_count % _per_num == 0){
			long long int _tmp_end = util::get_time_cur();
			int _iparallel = (_count / _per_num) - 1;
			RuntimeLis::parallel_insert[_iparallel] =
					util::cal_time(_tmp_end, tmp_begin);
			tmp_begin = util::get_time_cur();
		}
	}
	util::track("OUT TimeWindow::new_Tgraph_insert", "\n");
	{
		stringstream _ss;
		_ss << "return new_Tgraph of size " << _ret->size() << endl << endl;
		util::track(_ss);
	}
	return _ret;
}

/* call the drop_head() of each edge in this oldest interval */
Tgraph* TimeWindow::del_Tgraph(int _t_cur)
{
	{
		stringstream _ss;
		_ss << "IN TimeWindow::del_Tgraph with head " << this->head << endl;
		util::track(_ss);
	}

	Tgraph* _del_graph = this->graphs[this->head];

	{
		if(_del_graph){
			stringstream _ss;
			_ss << "NULL del_Tgraph at head " << this->head << endl;
			util::track(_ss);
		}
	}
	S2Eptr_map* _s2e = & (_del_graph->str2edge);
	S2Eptr_map::iterator itr;

	long long int tmp_begin = util::get_time_cur();
	int _count = 0;
	int _per_num = _s2e->size() / util::PARALLEL_NUM;
	for(itr = _s2e->begin(); itr != _s2e->end(); itr ++)
	{

		{
			stringstream _ss;
			_ss << "\tdrop ";
			util::track(_ss);
			_ss.str("");
			_ss.clear();
			 _ss << itr->first << endl;
			 util::track(_ss);
		}


		(itr->second)->drop_head(_t_cur);

		_count ++;
		if(_count % _per_num == 0){
			long long int _tmp_end = util::get_time_cur();
			int _iparallel = (_count / _per_num) - 1;
			RuntimeLis::parallel_delete[_iparallel] =
					util::cal_time(_tmp_end, tmp_begin);
			tmp_begin = util::get_time_cur();
		}

		{
			if(util::total_on_off){
			stringstream _ss;
			_ss << "\t finish drop <" << itr->first << ">" << endl;
			util::log(_ss);
			}

			{
				stringstream _ss;
				_ss << "\tfinish drop " << itr->first << endl;
				util::track(_ss);
			}
		}
	}

	util::track("OUT TimeWindow::del_Tgraph", "\n");
	{
		stringstream _ss;
		_ss << "return _del_graph of size " << _del_graph->size() << endl << endl;
		util::track(_ss);
	}

	return _del_graph;
}

/* if no relative edge, new one and add it into active_edges */
kEdge* TimeWindow::get_active_edge(const string&  _str)
{
	S2Eptr_map::iterator s2eitr;
	kEdge * eptr = NULL;
	s2eitr = this->active_edges.find(_str);
	if(s2eitr != this->active_edges.end()){
		eptr = s2eitr->second;
	}else{
		eptr = new kEdge();
		eptr->initial();
		this->active_edges.insert(pair<string, kEdge*>(_str, eptr));
	}

	return eptr;
}
