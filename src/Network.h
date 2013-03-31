/*
 * Copyright (c) 2012 Chammika Mannakkara
 *
 *    Network.h - Network class represents the Node interconnections
 *                in a directed graph 
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
#ifndef PAGERANK_NETWORK_CLASS
#define PAGERANK_NETWORK_CLASS

#include "Node.h"
#include "defaults.h"

// Network Class - represents a interconnections of Node objects in a directed
// graph. This graph is represented in an adjacency list
// (forward links from a node to neighbors ) and a back links 
// (backward link neighbors to a node). One or more attributes can be attached 
// each edge of the network. But for the purpose of this tool only one float/double
// attribute is kept.
// The class assigns a unique ID to each Node and also keep track of 
// ID -> Node and Node -> ID mappings.
class Network {

public:
  Network(unsigned int growth_rate=DEFAULT_GROWTH_RATE); // default ctor
  // ~Network(); // dtor - default is OK

  // Network Building :
  void add_edge(const string& src_url, const string& dst_url); // add an edge to network

  // I/O :
  friend ostream& operator<< (ostream &os, const Network &net);
  friend istream& operator>>(istream& is, Network& net);

  // Reference
  unsigned int num_nodes() const { return num_nodes_ ; }
  unsigned int num_edges() const { return num_edges_ ; }
  const map<string, Node>& get_url_2_node_map() const { return url_2_node_ ; }
  const map<node_id_type, Node>& get_id_2_node_map() const { return id_2_node_; }
  
protected:
  // Given a string representation of Node (here URL) this return the unique ID
  // used by Network to refer to the Node.
  node_id_type get_node_id( const string& src_url);

  unsigned int num_nodes_; // Number of nodes in the network
  unsigned int num_edges_; // Number of edges in the network

  // For each node on the network adjacency list keeps a map<> of 
  // outbound nodes and their attributes attached the edges
  vector<neighbor_set_type> adj_list_; 

  // This keeps a set<> of nodes pointing to each node. Since adj_list_
  // keep track of the attributes attached to the edge, this back_nodes
  // only keeps the set<> of nodes to avoid duplication of information
  vector<back_neighbor_set_type> back_node_set_; 

  // Growth rate defines at which rate the above two vector<>s are grown
  // to accomodate the network being read into memory. Need to be depend on
  // the order of the network.
  // Too low -> vector<>.resize() occur frequently, slow network read-in time
  // Too high -> vector<>s memory get allocated more than it needs for the network nodes
  unsigned int growth_rate_;

  // Network use generic unique IDs (issued incrementing num_nodes_) internally.
  // Following two mapping are used to map this ID from and to the Node class
  // which represents information on a Node.
  // In this case Node information is just URL data
  map<string, Node> url_2_node_; // URL -> ID (in) map
  map<node_id_type, Node> id_2_node_; // ID -> URL (out) map

private:
  Network( const Network&); // copy ctor -not allowed yet
  Network& operator=( const Network&); // assignment operator -not allowed yet

};

#endif
