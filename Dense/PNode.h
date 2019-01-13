/*
 * PNode.h
 *
 *  Created on: 2015-3-12
 *      Author: liyouhuan
 */

#ifndef PNODE_H_
#define PNODE_H_

#include "../util/util.h"
#include "../Dense/iiEdge.h"

class PNode
{
public:
	static const double NO_DENSE = -1000;

	PNode(int _id);
	void set_parent(PNode* _node);
	PNode* get_parent();
	PNode* next(int _v);


	/* make sure that: is_dense_node() is true */
	void set_density(double _density);
	double get_density();
	void get_this_path(set<int>& _denseC);

	void set_depth(int _d);
	int get_depth();

	/* get all dense c that extend from this branches
	 * (including this branch itself) */
	void get_extendingC(set<PNode*> & _extending_c);


	int child_num();
	void set_child(PNode* _node);
	PNode* get_child(int _v);
	int get_id();
	bool del_child(int _v);/* no recursive */

	PNode* get_left();
	PNode* get_right();
	bool set_left(PNode* _left);
	bool set_right(PNode* _right);
	bool is_tail();
	bool is_head();

	/* get density of C, matching path [_c_itr, _c_end)
	 * excluding id of this node
	 * OR: return NO_DENSE
	 * */
	double match_C_density(set<int>::iterator& _c_itr, set<int>::iterator& _c_end);

	bool path_contain_v(int _v);/*  */

	/* a label for a dense subgraph */
	bool is_dense_node();
	bool is_ouput_dense();

	string to_str();

private:
	int id;
	int depth;
	double density;
	map<int, PNode*> childs;
	PNode* parent_ptr;

	/* form the link of Nodes with the same id */
	PNode* left_ptr;
	PNode* right_ptr;


	/* including this node */
	bool exist_extendingC();
};


#endif /* PNODE_H_ */
