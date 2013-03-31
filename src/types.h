/*
 * Copyright (c) 20012 Chammika Mannakkara
 *
 *    types.h - all the types used in the simple PageRank tool + all the 
 *              STL inclusions goes here
 *
 *    This is a part of simple tool calculate the PageRank
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */
#ifndef PAGERANK_TYPES
#define PAGERANK_TYPES

#include <vector>
#include <map>
#include <list>
#include <set>
#include <queue>
#include <string>
#include <algorithm>
#include <iterator>
#include <functional>
#include <numeric>
#include <iostream>
#include <istream>
#include <ostream>
#include <fstream>

// used STL containers algorithms and std stream classes/iterators
using std::vector;
using std::map;
// using std::list;
using std::pair;
using std::set;
using std::queue;
using std::string;
using std::istream;
using std::ostream;
using std::ifstream;
using std::ofstream;
using std::ostream_iterator;

using std::cout;
using std::cin;
using std::endl;

using std::sort;
using std::accumulate;
using std::fill;
// using std::set_intersection;
using std::set_union;
using std::equal;

// Node ID type
typedef unsigned int node_id_type; 

// Attributes attached to edge of Network
// This can be of any class(object) containing more infomation
// for the purpose of this tool it's just float or double
// to hold 
// A). 1 - in case of adjacency matrix 
// B). 1/B(j) - normalized PageRank transfer factor in case
//              of PageRank calculations 
typedef double       attr_type;

// Type for PageRank calculation in case the float type is adequate
// for the computations this definition can be switched to use
// required type
typedef double       rank_type;
// typedef float       rank_type;

// This type represents a set of neighbors which a given Node points to.
// For each Neighbor, this also keeps the attributes of the 
// Node ---[attrib]----> Neighbor edge
typedef map<node_id_type, attr_type> neighbor_set_type;
typedef map<node_id_type, attr_type>::const_iterator attr_citer;
typedef map<node_id_type, attr_type>::iterator attr_iter;

// This type represents a set of neighbors pointing to a given Node.
// Neighbor -------> Node
// This does NOT contain the edge attributes as they are kept in the
// Node's forward links (ie. above neighbor_set_type)
typedef set<node_id_type> back_neighbor_set_type;
typedef set<node_id_type>::const_iterator back_neighbor_citer;

class Node;

// Iterator for URL -> ID (in) map
typedef map<string, Node>::const_iterator in_citer;
typedef pair <string, Node> url_node_pair;

// Iterator for ID -> URL (out) map
typedef map<node_id_type, Node>::const_iterator out_citer;
typedef pair <node_id_type, Node> id_node_pair;

// template method to check whether there is an intersection two vectors<>
// (or any container that provides linear access)
// This function is better than std::set_intersection() becasue we want to 
// just test whether there is intersection. std::set_intersection() returns
// the intersecting range as well
template<typename InputIterator1, typename InputIterator2>
bool 
  has_intersection(InputIterator1 first1, InputIterator1 last1,
		   InputIterator2 first2, InputIterator2 last2) {
     while (first1 != last1 && first2 != last2)  {
          if (*first1 < *first2)
             ++first1;
          else if (*first2 < *first1)
             ++first2;
          else
             return true;
       }
       return false;
}

#endif
