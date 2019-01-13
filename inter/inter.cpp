/*
 * inter.cpp
 *
 *  Created on: 2015-3-23
 *      Author: liyouhuan
 */
#include "inter.h"
#include "../Dense/prefixTree.h"

inter::inter(){
//	this->dense = new Dense();
//	this->rising = new Rising();

	this->f_inter_result.open("inter_result", ios::out);
}

inter::~inter(){
	this->f_inter_result.close();
}

void inter::inter_run(){
	this->init_inter();
	PrefixTree pre_tree;
	map<iiEdge, double>* edge2delta_w = NULL;

	while(edge2delta_w == NULL)
	{
		edge2delta_w = this->lis_next();
	}

	while(edge2delta_w != NULL)
	{
		this->dense->build_neighbors();

		pre_tree.update(*edge2delta_w);

		this->f_inter_result << pre_tree.tostr_ouputdense();
		this->f_inter_result.flush();

		delete edge2delta_w;
		edge2delta_w = this->lis_next();
	}
}

void inter::init_inter(){

}

map<iiEdge, double>* inter::lis_next(){

	return NULL;
}

void inter::dense_graph(){

}

