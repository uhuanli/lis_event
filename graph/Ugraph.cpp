/*
 * Ugraph.cpp
 *
 *  Created on: 2015-2-8
 *      Author: liyouhuan
 */
#include "Ugraph.h"
#include "../TimeWindow/Rgraph.h"
#include <boost/config.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/adjacency_list.hpp>

Ugraph::Ugraph()
{
	this->vertex2id.clear();
	this->id2vertex.clear();
}

bool Ugraph::add_edge(const string _edge)
{

	return false;
}
bool Ugraph::del_edge(const string _edge)
{

	return false;
}
bool Ugraph::update_D_I(map<string, double>* _s2d)
{

	return false;
}
bool Ugraph::getConnectedComponent(vector<string>& vcc_str)
{

	return true;
}
bool Ugraph::getConnectedComponent(vector<string>& _vec_edges, 	stringstream& _ss)
{
	if(_vec_edges.empty()){
		return false;
	}

	vector<pair<int, int> > pair_edges;
	/* parse <k1 k2> to <id1, id2> */
	this->parseEdges(_vec_edges, pair_edges);

	{
		if(util::debug_ugraph){
			cout << "finish parse" << endl;
		}
	}

	vector<int> _cc;
	/* build cc on all <id1, id2>s
	 * vertex i is in the _cc[i] Component;
	 * */
	this->getCC(pair_edges, _cc);
	/* map ccid to vids */
	multimap<int, int> cc2v;
	vector<int>::iterator vitr;
	int v_count = 0;
	set<int> cc_set;
	for(vitr = _cc.begin(); vitr != _cc.end(); vitr ++)
	{
		cc2v.insert(pair<int, int>(*vitr, v_count));
		cc_set.insert(*vitr);
		v_count ++;
	}

	_ss << "there are " << cc_set.size() << " CCs" << endl;

	{
		if(util::debug_ugraph){
			cout << "finish getCC" << endl;
		}
	}


	int cc_id = -1;
	multimap<int, int>::iterator cid2vid;

	bool _first = true;
	for(cid2vid = cc2v.begin(); cid2vid != cc2v.end(); cid2vid ++)
	{
		int new_cid = cid2vid->first;
		int vid = cid2vid->second;
		if(cc_id == -1){
			cc_id = new_cid;
		}

		if(cc_id != new_cid){
			cc_id = new_cid;
			_ss << "]\n\n";
			_first = true;
		}

		string keyword = this->id2vertex[vid];
		if(_first){
			_ss << "CC_" << cc_id << endl;
			_ss << "\t\t[" << keyword << "(v" << vid << ")";
			_first = false;
		}else{
			_ss << ", " << keyword << "(v" << vid << ")";
		}
	}
	_ss << "]\n";

	{
		if(util::debug_ugraph){
			cout << "finish string" << endl;
		}
	}

	return true;
}

bool Ugraph::parseEdges(vector<string>& _vec_edges, vector<pair<int, int> >& pair_edges)
{
	this->vertex2id.clear();
	pair_edges.clear();

	vector<string>::iterator itr;
	int next_id = 0;
	size_t tab;
	string k1;
	string k2;
	map<string, int>::iterator mitr;
	for(itr = _vec_edges.begin(); itr != _vec_edges.end(); itr ++){
		int id1 = -1, id2 = -1;
		tab = itr->find(' ');
		k1 = itr->substr(0, tab);
		k2 = itr->substr(tab+1);

		mitr = this->vertex2id.find(k1);
		if(mitr == this->vertex2id.end()){
			id1 = next_id;
			this->vertex2id[k1] = id1;
			this->id2vertex[id1] = k1;
			next_id ++;
		}else{
			id1 = mitr->second;
		}

		mitr = this->vertex2id.find(k2);
		if(mitr == this->vertex2id.end()){
			id2 = next_id;
			this->vertex2id[k2] = id2;
			this->id2vertex[id2] = k2;
			next_id ++;
		}else{
			id2 = mitr->second;
		}

		pair_edges.push_back(pair<int, int>(id1, id2));
	}
	return true;
}
bool Ugraph::getCC(vector<pair<int, int> >& pair_edges, vector<int> & _cc)
{
	  using namespace boost;
	  typedef adjacency_list < vecS, vecS, undirectedS > Graph;
	  typedef graph_traits < Graph >::vertex_descriptor Vertex;
	  const int numV = this->vertex2id.size();
		{
			if(util::debug_ugraph){
				cout << "numV = " << numV << endl;
			}
		}
	  Graph uG(numV);
	  typedef vector<pair<int, int> > VPII;
	  VPII::iterator vitr = pair_edges.begin();
	  for(; vitr != pair_edges.end(); vitr ++){
		  boost::add_edge(vitr->first, vitr->second, uG);
	  }
		{
			if(util::debug_ugraph){
				cout << "finish add_edge" << endl;
			}
		}
	  _cc.clear();
	  _cc.resize(numV);
	  int CCnum = boost::connected_components(
			  uG,
			  make_iterator_property_map(_cc.begin(), boost::get(boost::vertex_index, uG), _cc[0])
			  );
		{
			if(util::debug_ugraph){
				cout << "numCC" << CCnum << endl;
			}
		}
	  return true;
}

