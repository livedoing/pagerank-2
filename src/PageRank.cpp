/*
 * Copyright (c) 2012 Chammika Mannakkara
 *
 *    PageRank.cpp - Implementation of PageRank class which provides methods to  
 *                   calculate PageRank and inspect network for leaks/sinks
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
#include <cmath> // for fabs() -convergence check

#include "PageRank.h"
#include "Log.h"

// Constructor receives the PageRank calculation parameters in adition to base class Network
// constructor parameter.
PageRank::PageRank(rank_type decay, unsigned int iterations, rank_type epsilon, 
		   unsigned int growth_rate)
  : Network( growth_rate), decay_factor_(decay), iterations_(iterations), epsilon_(epsilon) {
  
}

// Find rank leaks in the network and return a vector of leak Nodes by 
// reference. Uses the internal function is_rank_leak() to find out whether each node
// is a rank leak
// Returns true if any rank leak nodes were found
// Complexity : O(N)
// where 
//    N - Number of nodes in the network
bool PageRank::find_rank_leaks(vector<Node>& leaks) {
  leaks.clear();
  for (node_id_type i=0; i < num_nodes_; ++i) { // O(N) - for each node
    if (is_rank_leak( i)) { 
      out_citer iter = id_2_node_.find( i);
      leaks.push_back( iter->second);
    }
  }
  return !leaks.empty();
}

// Find rank sinks in the network and return a vector of Nodes vectors by 
// reference. 
// Returns true if any rank one or more rank sink(s) were found
// Complexity (worst case ) : O(N x (N+E) )
// where 
//    N - Number of nodes in the network
//    E - Number of edges in the network
// Algorithm : 
// Attempts to classify each node from UNKNOWN state to -> either SINK or NOT_SINK
// using Breadth-First-Search (BFS).
//  NOT_SINK - ALL nodes of the network can be reached from this node 
//  SINK     - ALL nodes of the network can NOT be reached from this node
// 
// for node 'n'in all the nodes except (leak nodes or already marked SINK nodes)
//   while BFS from 'n' using visited[]
//       cur_node <= next node in BFS search
//       if (cur_node is NOT_SINK) 
//              'n' <= NOT_SINK    // becasue any node can be reached from cur_node now
//
//   if (BFS terminated but not ALL nodes reached) 
//         all visited[] nodes in BFS <= SINK
//
//   sink group <= { all visited[] nodes in BFS }
//   merge sink group (if possible) or create new sink group
//           
bool PageRank::find_rank_sinks(vector<vector<Node> >& sinks) {

  sinks.clear();
  vector<eNode_Status> status; // status of the node in this sink search
  status.resize( num_nodes_); // all are UNKNOWN

  for (node_id_type i=0; i < num_nodes_; ++i) { // O(N) - for each node find groups of sinks

    // nodes except (leak or already marked SINK nodes)
    if (is_rank_leak(i)) { // rank leak node
      PRINT(LOG_LVL_3, "node " << i << " is a rank leak, skipping... " << endl);
      status[i]= SINK ; // leak is a special sink
      continue; // skip rank leaks, collect only rank sinks
    }
    else if (status[i]== SINK) { // starting node already marked SINK -> skip
      PRINT(LOG_LVL_3, "node " << i << " is already in a sink, skipping... " << endl);
      continue;
    }

    vector<node_id_type> connected; // set of connected nodes so far
    queue<node_id_type> bfs; // queue for BFS from the starting node
    std::vector<bool> visited; // std::vector<bool> specilization used
    connected.clear();
    bfs = queue<node_id_type>(); // clear() is not provided for the adaptor !
    visited.resize( num_nodes_) ;

    bfs.push( i); // start with i-th node
    unsigned int num_visited = 0;
    bool arrives_at_no_sink = false; // BFS exit condition
    node_id_type cur_node;

    PRINT(LOG_LVL_3,"starting node " << i << endl);

    // Breadth-First-Search is performed to reach all the Nodes from starting node
    // Complexity : O(N+ E)
    // BFS terminating conditions :
    //      search arrives at node labeled NOT_SINK
    while (!bfs.empty()) { // Do Breadth-First-Search from starting node 'i'

      cur_node = bfs.front();
      bfs.pop();
      DEBUG("visiting node " << cur_node << endl);
      if (!visited[cur_node] ) {
	connected.push_back( cur_node);
	visited[cur_node]= true;
	++num_visited;
      }
      else {
	DEBUG("already visited node " << cur_node << endl);
	continue;
      }

      // check whether cur_node is NOT_SINK : BFS terminating condition
      if (status[cur_node]== NOT_SINK) {
	// BFS arrive at a NOT_SINK node => starting node is also NOT_SINK
	// becasue any node can be reached from here
	status[i] = NOT_SINK;
	arrives_at_no_sink= true;	
	DEBUG("node " << cur_node << " is NOT sink -> node " << i << " is NOT a sink too." << endl);
	break; 
      }    
      // get all outbound neighbors from the adj_list_
      DEBUG("finding neighbours..." << endl);
      neighbor_set_type& neighbors = adj_list_[cur_node];
      for(attr_iter iter = neighbors.begin(); iter != neighbors.end(); ++iter) {
	node_id_type next_node = iter->first;
	if ( !visited[ next_node]) {
	  bfs.push( next_node);
	  DEBUG("neighbour " << iter->first << " added to BFS" << endl);
	}
	else {
	  DEBUG("neighbour " << iter->first << " already visited skipping..." << endl);
	}
      }

    } // BFS loop

    if (arrives_at_no_sink) { // continue to next node
      PRINT(LOG_LVL_3, "Arrives at a node " << cur_node << " which had been marked NOT a sink" << endl); 
      PRINT(LOG_LVL_3, "All nodes can be reached from here. Node " << i << " is not a sink" << endl);
      continue;
    }

    //   if (BFS terminated but not ALL nodes reached) 
    //         all visited[] nodes in BFS <= SINK
    if (num_visited != num_nodes_) { // could NOT reach all nodes from i-th node
      PRINT(LOG_LVL_3, "Rank sink found." << endl);
      PRINT(LOG_LVL_3, "Only " << num_visited << " nodes could be visited out of " 
	             << num_nodes_ << " ->  rank_sink" << endl);
      PRINT(LOG_LVL_3, "Following nodes are in the sink " << endl);
      for (node_id_type j=0; j < connected.size(); ++j ) {
	PRINT(LOG_LVL_3, connected[j] << " ");
      }
      PRINT(LOG_LVL_3, endl);      

      vector<Node> rank_sink; // make copies of Nodes !
      rank_sink.reserve( connected.size());
      sort(connected.begin(), connected.end()) ; // sorted arrays needed for merging phase
      for (unsigned int j = 0; j < connected.size(); ++j) {
	node_id_type sink_node = connected[j];
	status[sink_node]= SINK; // mark as a SINK node
	out_citer iter = id_2_node_.find( sink_node);
	rank_sink.push_back( iter->second);
      }
      
      // check whether the created rank_sink can be  merged with an already created rank sink
      // if possible merge, otherwise create an new rank sink set
      merge_rank_sinks(sinks, rank_sink);

    }
    else { // num_visited == num_nodes_ -> all nodes reached -> NOT a sink
      status[i]= NOT_SINK; // mark as a NO_SINK node
      PRINT(LOG_LVL_3, "All nodes can be reached from node " << i << endl 
	            << "This node does not belong to a rank sink" << endl);
    }
  }
    
  return !sinks.empty();

}

// Merge a given rank sink (new_sink) (ie. a set of Nodes) with one of exsisting
// rank sink(s).
// Returns ture if it merges, otherwise insert it to sink(s) group as a new rank sink
// merge condition : if new_sink n sinks_i => merged( new_sink u sinks_i) for all 'i' in sinks
// where
//    n, u - set intersect and union operators
//    sink_i - 'i'th sink set in sinks
bool PageRank::merge_rank_sinks(vector<vector<Node> >& sinks, vector<Node>& new_sink) const {

  // check whether the created rank_sink can be  merged with an already created rank sink
  for (unsigned int rs=0; rs< sinks.size(); ++rs) { // check all rank sink created so far
    bool shared_nodes= has_intersection(sinks[rs].begin(), sinks[rs].end(), 
					new_sink.begin(), new_sink.end());
    if (shared_nodes) { // merge new rank_sink with sinks[rs] set
      vector<Node> merged_sink;
      merged_sink.resize( new_sink.size() + sinks[rs].size()); // size() on the safe side
      vector<Node>::iterator nsize;
      nsize = set_union(new_sink.begin(), new_sink.end(),
			sinks[rs].begin(), sinks[rs].end(), merged_sink.begin());
      merged_sink.resize(nsize - merged_sink.begin()); // resize
      sinks[rs] = merged_sink;
      PRINT(LOG_LVL_3, "Rank sink is merged with a previously found sink" << endl);
      return true;
    }
  }
  
  // couldn't merge create a new sink group
  sinks.push_back( new_sink);
  PRINT(LOG_LVL_3, "New rank sink created" << endl);
  return false;
  
}


// computes PageRanks on the network
// Returns a vector<> of PageRanks indexed in unique ID of the Newtork Nodes
// Complexity (of core alogrithm) : O (N x n_map_avg x ln(n_map_avg) )
// where 
//    N -         Number of nodes in the network
//    n_map_avg - Average number of nodes in neighbor ( map<> data structure)
//
//    n_map_avg.ln(n_map_avg) = [ n_1.ln(n_1) + n_2.ln(n_2) .... + n_N.ln(n_N)] / N
//    where
//        n_1, n_2, ... n_N are number of (outbound) neighbors for each node
// Algorithm :
// 1. Fix leak nodes - by adding edges to ALL the nodes pointing to a leak node
// 2. Normalize the adjacency matrix - by transforming 1 => d* 1/L
// 3. Transpose the normalized adjacency matrix
// 4. Initialize ranks of t0, and compute the constant term of PageRank ie. (1-d)/N
// 5. Perform power iteration (core algorithm)
// 6. Check for convergence (if given)
const vector<rank_type>& PageRank::calculate_PageRanks() {

  PRINT(LOG_LVL_2, "Calculation Parameters : " << endl
	<< "decay factor = " << decay_factor_ << endl 
	<< "iterations   = " << iterations_ << endl
  	<< "epsilon      = " << epsilon_ 
	<< ((epsilon_ == NO_CONVERGENCE_CHECK)? " <no_converevence_check>" : "") << endl );
  // 1. Fix leak nodes : rank leaks are fixed by adding edges to ALL the nodes pointing to it
  //    Ref. Arvind A. et al. Searching the Web, pp 33 : footnote 8 (Alternative solution)
  PRINT(LOG_LVL_2, "Fixing rank leak nodes..." << endl);
  int edges_added = 0;
  for (node_id_type i=0; i < num_nodes_; ++i) { // for each node
    if (is_rank_leak( i)) { 
      PRINT(LOG_LVL_3, "For node " <<  (id_2_node_.find( i))->second << endl);
      back_neighbor_set_type& back_nodes = back_node_set_[i];
      for(back_neighbor_citer bn= back_nodes.begin(); bn != back_nodes.end(); ++bn) {
	adj_list_[i][*bn] = 1.0 ; // create link to back node
	PRINT(LOG_LVL_3, "Creating link " << (id_2_node_.find( i))->second << " -> " 
	      << (id_2_node_.find( *bn))->second << endl);
	++edges_added;
      }
    }
  }
  if (edges_added) {
    PRINT(LOG_LVL_2, edges_added << " Edges were aded to fix leak nodes." << endl);
  }
  else {
    PRINT(LOG_LVL_2, "There were no rank leaks to fix" << endl);    
  }
  // Free up space used by back_node_set_ vector< map<>> as for the this PageRank tool
  // the only use of the the back_node_set_ is to perform above step 1. Fix rank sinks
  // This may be helpful for building transpose of the adj_list_ in step 3
  // Caution: This invalidate the Network class member back_node_set_ and any operations
  // depend on it. Hence, not advisable unless really needed to free-up memory
  back_node_set_.clear();

  // Swap with an empty vector is the only way to gurantee that capacity is reduced and
  // momeory is released  
  vector<back_neighbor_set_type>().swap(back_node_set_);

  /*
  // 2. Adjacency list now have 1.0 in attribute field for each neigbor (outbound)
  //    Transform this to a normalized matrix with each 
  //           attribute = d* (1/L) 
  //    where L - number of neighbors
  //          d - decay factor
  // Complexity : O(N x n_map_avg x ln(n_map_avg) )
  PRINT(LOG_LVL_2, "Normalizing the adjacency matrix..." << endl);
  for (unsigned int i=0; i < num_nodes_; ++i) { // O(N) x
    neighbor_set_type& neighbors = adj_list_[i];
    for(attr_iter iter = neighbors.begin(); iter != neighbors.end(); ++iter) { // O(n_map_avg) 
      iter->second =  decay_factor_ / neighbors.size(); // d * (1/L) 
    }
  }

  // 3. Transpose the normalized adjacency matrix
  // Complexity : O(N x n_map_avg x ln(n_map_avg))
  // where 
  //    N -         Number of nodes in the network
  //    n_map_avg - Average number of nodes in neighbor ( map<> data structure)
  PRINT(LOG_LVL_2, "Transposing the normalized adjacency matrix..." << endl);
  vector<neighbor_set_type> adj_list_transposed_ ;
  adj_list_transposed_.resize( num_nodes_);

  for (unsigned int i= 0; i < num_nodes_; ++i) { // O(N) 
    neighbor_set_type& neighbors = adj_list_[i];
    for(attr_citer iter = neighbors.begin(); iter != neighbors.end(); ++iter) { // O(n_map_avg)
      // transposed entry
      adj_list_transposed_[iter->first][i] = iter->second; // ln(n_map_avg) 
    }
  }
  */

  // 2 + 3 : Normalize AND Transporse the Adjacency matrix (above Step #2 & #3 ) in one go !
  //         Normalize = d* (1/L)
  //         where L - number of neighbors
  //               d - decay factor
  //         Transpose adj_list_transpose_[i][j] <==> adj_list_[j][i]
  //
  // Complexity : O(N x n_map_avg x ln(n_map_avg))
  // where 
  //    N -         Number of nodes in the network
  //    n_map_avg - Average number of nodes in neighbor ( map<> data structure)
  PRINT(LOG_LVL_2, "Normalizing and Transposing the adjacency matrix..." << endl);
  vector<neighbor_set_type> adj_list_transposed_ ;
  adj_list_transposed_.resize( num_nodes_);

  for (unsigned int i= 0; i < num_nodes_; ++i) { // O(N) 
    neighbor_set_type& neighbors = adj_list_[i];
    for(attr_citer iter = neighbors.begin(); iter != neighbors.end(); ++iter) { // O(n_map_avg)
      // transposed entry <= normalized value
      adj_list_transposed_[iter->first][i] = decay_factor_ / neighbors.size() ; // ln(n_map_avg) 
    }
  }
  

  
  // 4. PR(t+1) = d*[A]T * PR(t) + (1-d)/ N
  //    Calculate the second constant term which need to be added to page_rank
  //    No need to calculate this repeatedly
  rank_type rank_const = (1- decay_factor_) / num_nodes_ ; //  (1-d)/ N
  
  // Create page_ranks_ vector with initial (t0) values : init_val = 1/N, equal ranks to all nodes
  page_ranks_.resize( num_nodes_);
  fill(page_ranks_.begin(), page_ranks_.end(), 1.0/num_nodes_); // initial rank = 1/N

  // New ranks (t+1) are calculated to a new array to check for convergence
  vector<rank_type> new_ranks;
  new_ranks.resize( num_nodes_);

  // 5. Calculate : PR(k+1) = (d/L) * PR(k) + rank_const
  //    Using adj_list_transposed_ instead of adj_list_ (with swapped col/row access
  //    to mimic the transposition) has a significant performace gain due the
  //    underlaying vector<map<>> structure:
  //    Using adj_list_            : O(N^2 x ln(n_map_avg) )
  //          adj_list_transposed_ : O(N x n_map_avg x ln(n_map_avg) )
  //    where N >> n_map_avg
  PRINT(LOG_LVL_2, "Performing power iteration..." << endl);
  for (unsigned int k=0; k < iterations_; ++k) {
    PRINT(LOG_LVL_2, "Iteration #" << k+1 << endl);
    
    // PageRank core computation loop
    // complexity : O (N x n_map_avg x ln(n_map_avg) )
    // where 
    //    N -         Number of nodes in the network
    //    n_map_avg - Average number of nodes in neighbor ( map<> data structure)
    //
    //    n_map_avg.ln(n_map_avg) = [ n_1.ln(n_1) + n_2.ln(n_2) .... + n_N.ln(n_N)] / N
    //    where
    //        n_1, n_2, ... n_N are number of (outbound) neighbors for each node
    //
    for (unsigned int i= 0; i < num_nodes_; ++i) { // O(N)
      new_ranks[i]= rank_const; 
      neighbor_set_type& neighbors = adj_list_transposed_[i]; // O(const)
      for(attr_citer iter = neighbors.begin(); iter != neighbors.end(); ++iter) { // O(n_map_avg) 
	new_ranks[i] += iter->second * page_ranks_[iter->first] ;
      }
    }

#ifndef NDEBUG
    DEBUG("PageRanks calculated at Iteration : " << k+1 << endl);
    ostream_iterator<double> out (cout,"\n");
    copy (new_ranks.begin(), new_ranks.end(), out );
#endif

    // 6. Check for convergence between new_ranks and page_ranks_
    if ((epsilon_!= NO_CONVERGENCE_CHECK) &&  //bad double/float equivalnce check, but OK for const -1.0
	is_converged(new_ranks, page_ranks_, epsilon_) ) { 
      PRINT(LOG_LVL_2, "PageRanks converged within the given accuracy." << endl 
	            << "Terminating power iteration" << endl);
      break;
    }

    // copy new ranks(t+1 ) -> page_ranks to start a new iteration
    copy(new_ranks.begin(), new_ranks.end(), page_ranks_.begin());    
  }

  return page_ranks_;
}

// small predicate for convergence test : |new_rank - old_rank| < e
struct CompEpsilon {
  CompEpsilon(rank_type e) : e_(e) { }
  bool operator()(rank_type x, rank_type y) { return fabs(x-y) < e_; }

  const rank_type e_ ;
};

// indicate whether the PageRanks have converged within the given accuracy, epsilon
bool PageRank::is_converged(vector<rank_type>& new_ranks, vector<rank_type>& old_ranks, 
			    rank_type epsilon) const {
  return equal(new_ranks.begin(), new_ranks.end(), old_ranks.begin(), CompEpsilon( epsilon));
}

// rank leak : no outbound edges 
inline bool PageRank::is_rank_leak(node_id_type id) const { 
  return adj_list_[id].empty() ; 
}  
