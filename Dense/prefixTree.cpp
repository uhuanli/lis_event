/*
 * prefixTree.cpp
 *
 *  Created on: 2015-3-6
 *      Author: liyouhuan
 */
#include "prefixTree.h"
#include "Dense.h"

//int PrefixTree::buf_vset[1000];

/* public */
PrefixTree::PrefixTree(){
	this->f_dense_c.open("D:/result/dense_c");
	this->f_output_c.open("D:/result/output_c");
	this->f_bound.open("D:/result/bound_track");
	this->prefix_log.open("D:/result/prefix_log", ios::out);
	this->root = new PNode(-1);
	this->node_num = 1;

}

PrefixTree::~PrefixTree(){
	this->f_dense_c.close();
	this->f_output_c.close();
	this->f_bound.close();
	this->prefix_log.close();
}

/* we must update negative edge first
 *  */
bool PrefixTree::update(map<iiEdge, double>& _edge2delta_w){

	if(util::get_strongedges || util::cal_precision){
		return false;
	}

	{
		stringstream _ss;
		_ss << "\n::::PrefixTree::update " << _edge2delta_w.size() << " edges" << endl;
		util::track(_ss);
//		cout << _ss.str() << endl;

	}

	{//update negative edges
		map<iiEdge, double>::iterator itr1;
		itr1 = _edge2delta_w.begin();
		int _neg_i = 0;
		while(itr1 != _edge2delta_w.end())
		{
			if(itr1->second >= 0){
				itr1 ++;
				continue;
			}

			Dense::cache_e2w.clear();

			this->update_edge((itr1->first).from, (itr1->first).to, itr1->second);

			_neg_i ++;
			itr1 ++;
			int group = 500;
			if(_neg_i % group == 0){
//				cout << "\t neg " << _neg_i << " node=" << this->node_num << "  ... ";
//				cout.flush();
			}
//			if(_neg_i % (4*group) == 0) cout << endl;
		}
	}

	util::track("\n\n\t***************finish negative edges", "\n");
//	cout << "\t***************finish negative edges" << endl;

	{//update positive edges
		map<iiEdge, double>::iterator itr2;
		itr2 = _edge2delta_w.begin();
		int _pos_i = 0;
		while(itr2 != _edge2delta_w.end())
		{
			if(itr2->second < 0){
				itr2 ++;
				continue;
			}

			Dense::cache_e2w.clear();

			this->update_edge((itr2->first).from, (itr2->first).to, itr2->second);

			_pos_i ++;
			itr2 ++;
			int group = 500;
			if(_pos_i % group == 0){
//				cout << "\t pos " << _pos_i << " node=" << this->node_num << "  ... ";
//				cout << flush;
//				cout.flush();
			}


//			if(_pos_i % (4*group) == 0) cout << endl;
		}
	}

	util::track("\n\n\t**********finish positive edges", "\n\n");

	{
		stringstream _ss;
		_ss << "~~~~PrefixTree::update " << _edge2delta_w.size() << " edges" << endl;
		util::track(_ss);
	}

	return true;
}

string PrefixTree::tostr_ouputdense(){
	stringstream _ss;
	set<PNode*>::iterator itr = this->output_denseC.begin();
	set<int> v_set;
	_ss << "There are " << this->output_denseC.size() <<
			" output dense at " << Dense::time_stamp << endl;
	int _i = 0;
	while(itr != this->output_denseC.end())
	{
		v_set.clear();
		(*itr)->get_this_path(v_set);
		_ss << " " << _i << ":\t" << Dense::getDenseStr(v_set) << endl;

		itr ++;
		_i ++;
	}
	_ss << endl;
	return _ss.str();
}

/* private */

bool PrefixTree::update_edge(int _from, int _to, double _delta_w){
	util::track("\n********************update_edge", "\n");
	{
		stringstream _ss;
		_ss << "::::update " << _from << ", " << _to << " = "
			<< _delta_w << " node_num: " << this->node_num << endl;
		util::num_track(_ss);
	}

	if(_from == _to){
		string _s1 = Dense::getStrByID(_from);
		string _s2 = Dense::getStrByID(_to);
		cout << "err from: " << _s1 << "[" << _from << "] "
			 << "to: " << _s2 << "[" << _to << "]" << endl;

		exit(0);
	}

	{
		stringstream _ss;
		string _s1 = Dense::getStrByID(_from);
		string _s2 = Dense::getStrByID(_to);
		_ss << "\t\tupdate " << _s1 << "[" << _from << "] "
				 << "to: " << _s2 << "[" << _to << "] = " << _delta_w << endl;
		util::track(_ss);
	}

	if(_delta_w < 0){
		this->update_negative(_from, _to, _delta_w);
	}else{
		this->update_positive(_from, _to, _delta_w);
	}

	util::track("~~~~~~~~~~~~~~~~~~~update_edge", "\n");

	return true;
}

bool PrefixTree::update_positive(int _from, int _to, double _delta_w){

	/* check <_from, _to> whether dense */
	set<int> v_set;
	v_set.insert(_from);
	v_set.insert(_to);

	if(! this->exist_dense(v_set))
	{
		util::track("\tbefore add if", "\n");
		PNode* _dense = this->add_if_dense(v_set);
	}

	{
		util::track("\tafter add if", "\n");
	}

	set<PNode*>* _contain_f = this->denseC_containV(_from);
	set<PNode*>* _contain_t = this->denseC_containV(_to);
	{
		util::track("\tafter contain", "\n");
		util::num_track("\tcontain from", _contain_f->size());
		util::num_track("\tcontain to", _contain_t->size());
	}
	set<PNode*>::iterator itr_f = _contain_f->begin();
	set<PNode*>::iterator itr_t = _contain_t->begin();

	util::track("\t-----finish two containV", "\n");

	/*  */
	while(itr_f != _contain_f->end() &&
		  itr_t != _contain_t->end())
	{
		if((*itr_f) == (*itr_t))
		{
			this->update_positive_both((*itr_f));
			itr_f ++;
			itr_t ++;
		}
		else
		if((*itr_f) > (*itr_t))
		{
			this->update_positive_either((*itr_t), _from, _delta_w);
			itr_t ++;
		}
		else
		if((*itr_f) < (*itr_t))
		{
			this->update_positive_either((*itr_f), _to, _delta_w);
			itr_f ++;
		}
	}
	/*  */
	while(itr_f != _contain_f->end())
	{
		this->update_positive_either((*itr_f), _to, _delta_w);
		itr_f ++;
	}
	/*  */
	while(itr_t != _contain_t->end())
	{
		this->update_positive_either((*itr_t), _from, _delta_w);
		itr_t ++;
	}

	delete _contain_f;
	delete _contain_t;

	{
		stringstream _ss;
		_ss << "::::update_positive done" << endl;
		util::track(_ss);
	}

	return true;
}

bool PrefixTree::update_negative(int _from, int _to, double _delta_w){
	{

	}

	set<PNode*>* _contain_both = this->denseC_containBoth(_from, _to);
	if(_contain_both == NULL){
		return false;
	}

	set<PNode*>::iterator itr = _contain_both->begin();

	bool _pre_is_output = false;
	while(itr != _contain_both->end())
	{
		_pre_is_output = (*itr)->is_ouput_dense();
		this->update_density(*itr);
		if(! (*itr)->is_ouput_dense())
		{
			this->removefrom_output((*itr));
		}

		if(! (*itr)->is_dense_node())
		{
			this->cancel_dense((*itr));
			this->removefrom_dense((*itr));
		}

		itr ++;
	}

	{
		stringstream _ss;
		_ss << "::::update_negative done" << endl;
		util::track(_ss);
	}

	delete _contain_both;

	return true;
}

bool PrefixTree::update_positive_both(PNode* _dense_both){
	bool _was_output = false;
	bool _is_output = false;
	_was_output = (_dense_both)->is_ouput_dense();
	this->update_density(_dense_both);
	_is_output = (_dense_both)->is_ouput_dense();
	if(!_was_output && _is_output)
	{
		this->addin_output(_dense_both);
	}

	if((_dense_both)->is_dense_node())
	{
		this->addin_dense(_dense_both);
	}
	return true;
}

bool PrefixTree::update_positive_either(PNode* _dense_either, int _other_v, double _delta_w){
	{
		stringstream _ss;
		_ss << "::::update pos either other[" << _other_v << "] = " << _delta_w << endl;
		util::track(_ss);
		util::track(_ss);
		util::num_track(_ss);
	}

	set<int> _c_f;
	(_dense_either)->get_this_path(_c_f);

	_c_f.insert(_other_v);
	bool _exist = this->exist_dense(_c_f);
	if(_exist){
		util::track("~~~~update either exist", "\n");
		util::num_track("~~~~update either exist", -1);
		return false;
	}

	PNode* _dense = this->add_if_dense(_c_f);

	if(_dense != NULL)
	{
		set<int> _neighbors;
		set<int>::iterator tmp_itr = _c_f.begin();
		while(tmp_itr != _c_f.end())
		{
			Dense::get_neighbors((*tmp_itr), _neighbors);

			tmp_itr ++;
		}
		int _imax = (_delta_w / Dense::delta_it()) + 1;
		this->explore(_c_f, _neighbors, 2, _imax);

		util::track("~~~~update either true", "\n");

		return true;
	}

	util::track("~~~~update either false", "\n");
	return false;
}

void PrefixTree::explore(set<int>& _v_set, set<int>& _neighbors, int n_it, int _imax){
	util::track("::::explore", "\n");
	{
		stringstream _ss;
		string tabs = "";
		int tmp_i = 0;
		while(tmp_i < n_it){
			tabs += "\t";
			tmp_i ++;
		}
		_ss << tabs << " node_num: " << this->node_num << endl;
		util::num_track(_ss);
		util::track(_ss);
	}

	int _n = _v_set.size();
	if(_n >= Dense::Nmax || n_it > _imax)
	{
		util::track("~~~~explore bound", "\n");
		return;
	}

	util::track("\t before nei erase", "\n");

	{/* erase */
		set<int>::iterator _tmp_itr = _v_set.begin();
		while(_tmp_itr != _v_set.end())
		{
			_neighbors.erase((*_tmp_itr));

			_tmp_itr ++;
		}
	}

	util::track("\tafter nei erase", "\n");
	int _nei_sz = _neighbors.size();
	int* _nei_list = new int[_nei_sz];
	{
		int _i_tmp = 0;
		set<int>::iterator s_itr = _neighbors.begin();
		while(s_itr != _neighbors.end())
		{
			_nei_list[_i_tmp] = (*s_itr);
			_i_tmp ++;
			s_itr ++;
		}
	}

	for(int i = 0; i < _nei_sz; i ++)
	{
		util::track("\tIN for neighbors", "\n");
		int _nei_id = (_nei_list[i]);

		_v_set.insert(_nei_id);
		/* we have made sure that no overlap between _v_set and _neighbors
		 * if exist_dense, _v_set either was just updated or will be updated
		 * since C(_v_set) contain <_from, _to> */
		if(this->exist_dense(_v_set)){
			_v_set.erase(_nei_id);
			continue;
		}

		util::track("\tin while@explore", "\n");

		PNode* _dense = this->add_if_dense(_v_set);
		if(_dense != NULL)
		{
			Dense::get_neighbors(_nei_id, _neighbors);
			this->explore(_v_set, _neighbors, n_it+1, _imax);
		}

		_v_set.erase(_nei_id);
	}

	delete[] _nei_list;

	util::track("~~~~explore", "\n");
}
/* ?? */
double PrefixTree::get_weight(int _from, int _to){

	return Dense::weight(_from, _to);
}

double PrefixTree::update_density(PNode* _dense){
	set<int> v_set;
	_dense->get_this_path(v_set);
	return this->cal_density(v_set);
}

double PrefixTree::cal_density(set<int>& _v_set){
	return Dense::density(_v_set);
}

PNode* PrefixTree::add_if_dense(set<int>& _v_set){
	util::track("::::add if dense", "\n");
	double _density = this->cal_density(_v_set);
	int _n = _v_set.size();
	if(_density >= Dense::T(_n))
	{
		PNode* _dense = this->add_dense2index(_v_set, _density);
		{
			util::track("\t before cur_denseC", "\n");
		}
		this->cur_denseC.insert(_dense);
		{
			util::track("\t after cur_denseC", "\n");
			if(_dense == NULL){
				cout << "add_if_dense::_dense NULL" << endl;
				exit(0);
			}
		}
		if(_dense->is_ouput_dense())
		{
			util::track("\tis_out_put", "\n");
			this->addin_output(_dense);
		}

		util::track("~~~~add if dense _dense", "\n");
		return _dense;
	}

	{
		stringstream _ss;
		_ss << "~~~~add if dense NULL density:" << _density << endl;
		util::track(_ss);
	}

	return NULL;
}


/* report output if needed */
PNode* PrefixTree::add_dense2index(set<int>& _v_set, double _density){
	util::track("::::add_dense2index", "\n");

	PNode* _cur = this->root;

	util::num_track("root size: ", this->root->child_num());

	set<int>::iterator itr = _v_set.begin();
	while(itr != _v_set.end())
	{
		PNode* _child = _cur->get_child(*itr);
		if(_child == NULL)
		{
			_child = new PNode(*itr);
			this->node_num ++;
			_child->set_parent(_cur);
			_child->set_depth(_cur->get_depth() + 1);
			this->addin_list(_child);
			_cur->set_child(_child);

			_cur = _child;
		}
		else
		{
			_cur = _child;
		}

		itr ++;
	}

	_cur->set_density(_density);

	util::track("~~~~add_dense2index", "\n");
	return _cur;
}

set<PNode*>* PrefixTree::denseC_containV(int _v){
	map<int, PNode*>::iterator itr = this->id2nodelist.find(_v);
	if(itr == this->id2nodelist.end()){
		return new set<PNode*>();
	}

	set<PNode*>* _node_set = new set<PNode*>();
	PNode* _cursor = itr->second;
	while(_cursor != NULL)
	{
		_cursor->get_extendingC(*_node_set);
		{
			stringstream _ss;
			_ss << "cursor: " << _cursor->to_str() << endl;
			_ss << "_node_set size: " << _node_set->size() << endl;
			util::num_track(_ss);
		}
		_cursor = _cursor->get_right();
	}

	return _node_set;
}

set<PNode*>* PrefixTree::denseC_containBoth(int _v1, int _v2){
	set<PNode*>*  _contain_v1 = this->denseC_containV(_v1);
	if(_contain_v1->empty()){
		delete _contain_v1;
		return NULL;
	}

	set<PNode*>*  _contain_v2 = this->denseC_containV(_v2);
	if(_contain_v2->empty()){
		delete _contain_v2;
		return NULL;
	}

	set<PNode*>* _ret = this->denseC_containBoth(_contain_v1, _contain_v2);

	delete _contain_v1;
	delete _contain_v2;

	return _ret;
}

set<PNode*>* PrefixTree::denseC_containBoth(set<PNode*>* _contain_v1, set<PNode*>* _contain_v2){
	set<PNode*>::iterator itr1 = _contain_v1->begin();
	set<PNode*>::iterator itr2 = _contain_v2->begin();
	set<PNode*>* _intersection = new set<PNode*>();
	while(itr1 != _contain_v1->end() &&
		  itr2 != _contain_v2->end())
	{
		if((*itr1) < (*itr2))
		{
			itr1 ++;
		}
		else
		if((*itr1) > (*itr2))
		{
			itr2 ++;
		}
		else
		{
			_intersection->insert(*itr1);
			itr1 ++;
			itr2 ++;
		}
	}

	if(_intersection->size() != 0)
	{
		return _intersection;
	}
	else
	{
		delete _intersection;
		return NULL;
	}
}

bool PrefixTree::addin_list(PNode* _node){
	util::track("::::addINlist", "\n");

	int _id = _node->get_id();
	map<int, PNode*>::iterator itr = this->id2nodelist.find(_id);
	{
		stringstream _ss;
		_ss << "add node: " << _node->to_str() << endl;
		util::num_track(_ss);
	}
	if(itr == this->id2nodelist.end()){
		_node->set_left(NULL);
		_node->set_right(NULL);
		this->id2nodelist[_id] = _node;

		util::track("~~~addINlist-create", "\n");

		util::num_track("id2node", this->id2nodelist.size());

		return true;
	}

	PNode* _old_head = itr->second;
	_node->set_left(NULL);
	_node->set_right(_old_head);
	_old_head->set_left(_node);
	(this->id2nodelist)[_id] = _node;

	util::num_track("id2node", this->id2nodelist.size());

	util::track("~~~addINlist-add", "\n");
	return true;
}

bool PrefixTree::removefrom_list(PNode* _node){
	int _id = _node->get_id();
	map<int, PNode*>::iterator itr = this->id2nodelist.find(_id);
	if(itr == this->id2nodelist.end()){
		return false;
	}

	if(_node->is_head())
	{
		if(_node->is_tail())
		{
			this->id2nodelist.erase(_id);
		}
		else
		{
			PNode* _right = _node->get_right();
			_right->set_left(NULL);
			this->id2nodelist[_id] = _right;
			_node->set_right(NULL);
		}
	}
	else
	if(_node->is_tail())
	{
		PNode* _left = _node->get_left();
		_left->set_right(NULL);
		_node->set_left(NULL);
	}
	else
	{
		PNode* _right = _node->get_right();
		PNode* _left = _node->get_left();
		_right->set_left(_left);
		_left->set_right(_right);
		_node->set_left(NULL);
		_node->set_right(NULL);
	}

	return false;
}

bool PrefixTree::addin_output(PNode* _output){
	this->output_denseC.insert(_output);
	return true;
}

bool PrefixTree::removefrom_output(PNode* _node){
	this->output_denseC.erase(_node);
	set<int> tmp;
	_node->get_this_path(tmp);
	this->prefix_log << Dense::getDenseStr(tmp) << endl;
	this->prefix_log.flush();
	return true;
}

bool PrefixTree::addin_dense(PNode* _dense){
	this->cur_denseC.insert(_dense);
	return true;
}

bool PrefixTree::removefrom_dense(PNode* _node){
	this->cur_denseC.erase(_node);
	return true;
}

bool PrefixTree::exist_dense(set<int>& _v_set){
	util::track("::::exist_dense set", "\n");

	set<int>::iterator itr1 = _v_set.begin();
	set<int>::iterator itr2 = _v_set.end();
	{
		stringstream _ss;
		set<int>::iterator itr = itr1;
		while(itr != itr2){
			_ss << (*itr) << " ";
			itr ++;
		}
		util::track(_ss);
	}
	double _density = this->root->match_C_density(itr1, itr2);

	util::track("~~~~exist_dense set", "\n");
	return _density != PNode::NO_DENSE;
}

/* note this->cur_denseC */
bool PrefixTree::cancel_dense(PNode* _dense_c){
	_dense_c->set_density(PNode::NO_DENSE);
	if(_dense_c->child_num() != 0){
		return true;
	}

	PNode* _cur = _dense_c;
	PNode* _parent = _dense_c->get_parent();
	while(_parent != NULL)
	{
		_parent->del_child(_cur->get_id());
		delete _cur;
		this->node_num --;

		if(_parent->child_num() != 0){
			return true;
		}

		_cur = _parent;
		_parent = _cur->get_parent();
	}
	return true;
}
