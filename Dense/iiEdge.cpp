/*
 * iiEdge.cpp
 *
 *  Created on: 2015-3-6
 *      Author: liyouhuan
 */

#include "iiEdge.h"

iiEdge::iiEdge(){

}

iiEdge::iiEdge(int _f, int _to){
	this->from = _f;
	this->to = _to;
}

bool iiEdge::operator > (const iiEdge &  _e)const{
	if(this->from != _e.from){
		return this->from > _e.from;
	}

	return this->to > _e.to;
}

bool iiEdge::operator < (const iiEdge &  _e)const{
	if(this->from != _e.from){
		return this->from < _e.from;
	}

	return this->to < _e.to;
}

bool iiEdge::operator ==(const iiEdge &  _e)const{
	return (this->from == _e.from)  &&  (this->to == _e.to);
}

iiEdge& iiEdge::operator = (const iiEdge &  _e){
	this->from = _e.from;
	this->to = _e.to;
	return *this;
}

string iiEdge::to_str()const{
	stringstream _ss;
	_ss << this->from << " " << this->to;
	return _ss.str();
}
