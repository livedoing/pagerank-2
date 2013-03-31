/*
 * Copyright (c) 2012 Chammika Mannakkara
 *
 *    PageRank.h - Derived class of a generic Network class which provides 
 *                 facilities to calculate PageRank of the Network
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
#ifndef PAGERANK_PAGERANK_CLASS
#define PAGERANK_PAGERANK_CLASS

#include "Network.h"

// PageRank class - Derived class of a generic Network class which provides 
// facilities to calculate PageRank of the Network. This class also provides
// methods to analyze a Network for rank leaks and sinks.
class PageRank : public Network {

public:
  PageRank(rank_type decay, unsigned int iterations, rank_type epsilon, 
	   unsigned int growth_rate=DEFAULT_GROWTH_RATE); // ctor
  // ~PageRank(); // dtor - default is OK

  // Diagonastic :
  bool find_rank_leaks(vector<Node>& leaks);
  bool find_rank_sinks(vector<vector<Node> >& sinks );

  // Computation :
  const vector<rank_type>& calculate_PageRanks();

private:
  // PageRank scores calculated for each node
  vector<rank_type>  page_ranks_; 
  // PageRank calculation parameters
  rank_type decay_factor_ ;
  unsigned int iterations_;
  rank_type epsilon_;

  // Helpers :
  // indicate whether a given node is rank leak
  bool is_rank_leak(node_id_type id) const ;  

  // merge a new_rank sink with a one of the exsisting rank sinks if possible
  bool merge_rank_sinks(vector<vector<Node> >& sinks, vector<Node>& new_sink) const;

  // indicate whether the PageRanks have converged to given accuracy, epsilon
  bool is_converged(vector<rank_type>& new_ranks, vector<rank_type>& old_ranks, 
		    rank_type epsilon) const ;

  // Node type for rank sink classification in the find_rank_sinks()
  // UNKNOWN -  initial state of all nodes
  // NOT_SINK - ALL nodes of the network can be reached from this node 
  // SINK     - ALL nodes of the network can NOT be reached from this node
  typedef enum { UNKNOWN, NOT_SINK, SINK } eNode_Status; 

};

#endif
