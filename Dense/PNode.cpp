/*
 * PNode.cpp
 *
 *  Created on: 2015-3-12
 *      Author: liyouhuan
 */
#include "PNode.h"
#include "Dense.h"

PNode::PNode(int _id){
	this->id = _id;
	this->density = PNode::NO_DENSE;
	this->depth = 0;
	this->parent_ptr = NULL;
	this->left_ptr = NULL;
	this->right_ptr = NULL;
}

void PNode::set_parent(PNode* _node){
	this->parent_ptr = _node;
}

PNode* PNode::get_parent(){
	return this->parent_ptr;
}

PNode* PNode::next(int _v){
	map<int, PNode*>::iterator itr = (this->childs).find(_v);
	if(itr != this->childs.end())
		return itr->second;

	return NULL;
}

void PNode::set_density(double _density){
	this->density = _density;
}

/* make sure that: is_dense_node() is true */
double PNode::get_density(){
	if(! this->is_dense_node()){
		cout << "err: get_density" << endl;
		exit(0);
	}

	return this->density;
}

void PNode::get_this_path(set<int>& _denseC){
	PNode* _parent = this->get_parent();
	/* if this is root */
	if(_parent == NULL){
		return;
	}

	_denseC.insert(this->get_id());

	_parent->get_this_path(_denseC);
}

void PNode::set_depth(int _d){
	this->depth = _d;
}

int PNode::get_depth(){
	util::track("@get_depth", "\n");

	return this->depth;
}

/* get all dense c that extend from this branches
 * (including this branch itself) */
void PNode::get_extendingC(set<PNode*> & _extending_c){
	if(this->is_dense_node()){
		_extending_c.insert(this);
		{
			stringstream _ss;
			_ss << "add dense extendC " << this->to_str() << endl;
			util::num_track(_ss);
		}
	}

	map<int, PNode*>::iterator itr = this->childs.begin();
	while(itr != this->childs.end())
	{
		PNode* _child = itr->second;
		_child->get_extendingC(_extending_c);

		itr ++;
	}

}

int PNode::child_num(){

	return this->childs.size();
}

void PNode::set_child(PNode* _node){
	this->childs[_node->get_id()] = _node;
}

PNode* PNode::get_child(int _v){
	map<int, PNode*>::iterator it_child = this->childs.find(_v);
	if(it_child == this->childs.end()){
		return NULL;
	}

	return it_child->second;
}

int PNode::get_id(){
	return this->id;
}

/* no recursive */
bool PNode::del_child(int _v){
	this->childs.erase(_v);
	return true;
}

PNode* PNode::get_left(){
	return this->left_ptr;
}

PNode* PNode::get_right(){
	return this->right_ptr;
}

bool PNode::set_left(PNode* _left){
	this->left_ptr = _left;
	return true;
}

bool PNode::set_right(PNode* _right){
	this->right_ptr = _right;
	return true;
}

bool PNode::is_tail(){
	return (this->right_ptr) == NULL;
}

bool PNode::is_head(){
	return (this->left_ptr) == NULL;
}

/* get density of C, matching path [_c_itr, _c_end)
 * excluding id of this node
 * OR: return NO_DENSE
 * */
double PNode::match_C_density(set<int>::iterator& _c_itr, set<int>::iterator& _c_end){
	{
		stringstream _ss;
		_ss << "::::matchC " << *_c_itr << endl;
		util::track(_ss);
	}

	if(_c_itr == _c_end)
	{
		util::track("\tBOUND1 target:child->matchC", "\n");
		if(this->is_dense_node())
			return this->density;
		else
			return PNode::NO_DENSE;
	}
	int _match_id = *_c_itr;
	map<int, PNode*>::iterator itr = this->childs.find(_match_id);
	if(itr == this->childs.end())
	{
		util::track("\tBOUND2 break:child->matchC", "\n");
		return PNode::NO_DENSE;
	}

	_c_itr ++;

	PNode* _child = (itr->second);

	util::track("RET:child->matchC", "\n");

	return _child->match_C_density(_c_itr, _c_end);
}

bool PNode::path_contain_v(int _v){
	if(this->get_id() == _v) return true;

	PNode* _parent = this->get_parent();
	if(_parent == NULL) return false;

	return _parent->path_contain_v(_v);
}

/* a label for a dense subgraph */
bool PNode::is_dense_node(){
	int n = this->depth;

	if(n < 2) return false;

	if( this->density > Dense::T(n) ){
		return true;
	}

	return false;
}

bool PNode::is_ouput_dense(){
	util::track("::::is_output_dense", "\n");

	int n = this->depth;
	{
		util::track("\t after depth", "\n");
	}
	if(n < 2){
		util::track("~~~~is_output_dense:false1", "\n");
		return false;
	}

	if( this->density > Dense::Tmax ){
		util::track("~~~~is_output_dense:true", "\n");
		return true;
	}

	util::track("~~~~is_output_dense:false2", "\n");

	return false;
}

string PNode::to_str(){
	stringstream _ss;
	_ss << "id=" << this->id << "\t" << "depth=" << this->depth
		<< "\tdensity=" << this->density;
	return _ss.str();
}

/* private */

/* including this node
 * ?? */
bool PNode::exist_extendingC(){

	if(this->is_dense_node()) return true;

	return this->child_num() > 0;
}



