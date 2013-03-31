/*
 * Copyright (c) 2012 Chammika Mannakkara
 *
 *    Network.cpp - Implementation of abstract Network class which represents 
 *                  Node connections in a directed graph
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
#include <cassert>
#include "Network.h"
#include "Log.h"


// ctor
// growth_rate is the only parameter for constructor if not provided
// DEFAULT_GROWTH_RATE is used. 
Network::Network(unsigned int growth_rate) : growth_rate_(growth_rate) {

  assert(growth_rate && "Growth Rate of the network cannot be zero!");

  num_nodes_ = 0; 
  num_edges_ = 0;
  // adj_list_ and back_node_set_ need to be properly initialized
  // (map<>s need to be created) to insert elements
  adj_list_.resize( growth_rate_) ;
  back_node_set_.resize( growth_rate_) ;
}

// get the network internal ID of a Node corresponding to string (url in this case) 
// representation of the Node.
// If URL is new : issue a new ID, update the mappings ID -> URL and URL -> ID
// If URL is already in the network : returns corresponding ID 
// Returns the unique ID for a string URL
node_id_type Network::get_node_id( const string& url) {
  node_id_type id;
  in_citer iter = url_2_node_.find( url);
  if (iter == url_2_node_.end() ) { // new url -> issue a new ID and add it to in/out maps
    id = num_nodes_++ ;
    // insert in url -> ID map
    Node node = Node(id, url) ;
    url_2_node_.insert( url_node_pair( url, node ));
    // insert in ID -> url map
    id_2_node_.insert( id_node_pair( id, node ));
    DEBUG("New node added to network : " << node << endl);
  }
  else { // url already in the network -> return it's ID
    id = iter->second.id();
  }
  return id;
}

// Add an edge src_url ---> dst_url in the Network
// This method is called while reading the network file of the form:
// <src_url> <dst_url>
//
// This method converts the URL to unique IDs and store  the ID in 
// forward (adj_list_) and backward (back_node_set_) sets
// For the adj_list_ it stores the attribute = 1.0 <==> adjacency matrix
// For back_node_set_ no attributes were store (to avoid duplication of information)
// Special cases are handled as follows
// 1. self-loops (src_url == dst_url) ==> skip
// 2. duplicate edges                 ==> skip
void Network::add_edge(const string& src_url, const string& dst_url) {
  // get unique IDs corresponding to url strings
  node_id_type src_id = get_node_id( src_url);
  node_id_type dst_id = get_node_id( dst_url);

  if (src_id == dst_id ) { // self-loop
    PRINT(LOG_LVL_3, "Ignoring self-loop for node " << (url_2_node_.find( src_url))->second << endl);
    return;
  }

  // insert edge to adj_list_
  if (adj_list_.size() <= src_id) { // range check
    DEBUG("Resizing adjacency list vector<> size = " << src_id + growth_rate_ << endl);
    adj_list_.resize(src_id + growth_rate_) ; // accomodate src_id position + additional
  }

  assert((adj_list_.size() > src_id) && "Adjacency list is NOT properly resized");
  attr_type& edge = adj_list_[src_id][dst_id] ; // O(ln(n))
  if (edge == 0.0 ) { // if the edge is not inserted before
    edge = 1.0 ;
    ++num_edges_ ;
  }
  else { // duplicate edge - no need to add to back_node_set_, it's already added
    PRINT(LOG_LVL_3, "Ignoring duplicate edge for nodes " 
	           << (url_2_node_.find( src_url))->second << " -> " 
	           << (url_2_node_.find( dst_url))->second << endl);
    return;
  }

  // insert edge to back_node_set_ (ie. add 'src_id' to the the back_node_set_ of 'dst_id')
  if (back_node_set_.size() <= dst_id) {
    DEBUG("Resizing back node set vector<> size = " << dst_id + growth_rate_ << endl);
    back_node_set_.resize(dst_id + growth_rate_) ; // accomodate dst_id position + additional
  }

  assert((back_node_set_.size() > dst_id) && "Back node set is NOT properly resized");
  back_node_set_[dst_id].insert( src_id); // O(ln(n))
  
}

// Read the network from an input stream of format:
// <src_url> <dst_url>
// <src_url> <dst_url>
// ..
istream& operator>>(istream& is, Network& net) {
  string src, dst;
  int edges = 0;

  while (is >> src >> dst) {
    net.add_edge( src, dst) ; 
    ++edges;
    if (!(edges% PROGRESS_REPORT_STEP)) {
      PRINT(LOG_LVL_2, edges << " Edges processed." << endl);
    } 
  }
  return is;
}

// Output the network information to an output stream
ostream& operator<< (ostream &os, const Network &net) {
  os << "Network" << endl;
  os << "Nodes : [ID]URL" << endl;
  for(in_citer iter = net.url_2_node_.begin(); iter != net.url_2_node_.end(); ++iter) {
    cout << iter->second << endl;
  }
  
  os << "Edges : " << endl;
  for (unsigned int i=0; i < net.num_nodes_; ++i) { // list edges for each node
    os << i << "\t: " ;
    const neighbor_set_type& neighbors = net.adj_list_[i];
    for(attr_citer iter = neighbors.begin(); iter != neighbors.end(); ++iter) {
      os << iter->first << " ";
    }
    os << endl;
  }
  os << "Number of Nodes = " << net.num_nodes_  << endl
     << "Number of Edges = " << net.num_edges_  << endl;
  return os;
}

