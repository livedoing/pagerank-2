/*
 * Copyright (c) 2012 Chammika Mannakkara
 *
 *    main.cpp - Entry point to PageRank tool. Handle command-line and execute
 *               the tool in [check | run] modes
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
#include <cmath>

#include "PageRank.h"
#include "Log.h"
#include "defaults.h"

// forward declarations
void usage(void);
bool parse_cmdline(int argc, char *argv[], 
		   ifstream &net_file, bool &run_mode, double &decay_factor, 
		   int &iterations, double &epsilon, unsigned char &log_lvl,
		   unsigned int &growth_rate);
void read_network(PageRank& n, ifstream& net_file);
void exec_run_mode( PageRank& n);
void exec_check_mode(PageRank& n);

// Program Globals
unsigned char Log::level_ = DEFAULT_LOG_LEVEL;


int main(int argc, char *argv[]) {

  ifstream net_file;
  bool run_mode = false;
  double decay_factor = 0.0 ;
  int iterations = 0;
  double epsilon = NO_CONVERGENCE_CHECK;
  unsigned int growth_rate = DEFAULT_GROWTH_RATE;

  bool parsed = parse_cmdline( argc, argv, 
			       net_file, run_mode, decay_factor, iterations, epsilon, Log::level_,
			       growth_rate);
  if (!parsed) {
    usage();
    exit(0);
  }

  // create PageRank object
  PageRank n(decay_factor, iterations, epsilon, growth_rate);


  read_network(n, net_file);
  if (run_mode) {
    exec_run_mode( n);
  }
  else { // check mode
    exec_check_mode( n);
  }

}

// Reads a Network from input stream
// using the stream extraction operator for Network class
void read_network(PageRank& n, ifstream& net_file) {
  PRINT(LOG_LVL_1, "Reading Network..."<< endl);
  net_file >> n; // read in network
  PRINT(LOG_LVL_1, "Network Reading complete." << endl);

  PRINT(LOG_LVL_1, "Number of Nodes = " << n.num_nodes() << endl);
  PRINT(LOG_LVL_1, "Number of Edges = " << n.num_edges() << endl);
  
  PRINT(LOG_LVL_3, n) ; // cout << n; // check network
}

// Run mode of the PageRank calculation tool
// Computes the PageRanks and output them using the ID -> Node mapping
void exec_run_mode( PageRank& n) {
  PRINT(LOG_LVL_1, "Finding PageRanks... " << endl); 
  const vector<rank_type>& page_ranks = n.calculate_PageRanks();
  PRINT(LOG_LVL_1, "PageRank computation complete." << endl);

  const map<node_id_type, Node>& mapping = n.get_id_2_node_map();
  PRINT(LOG_LVL_1, "PageRanks :" << endl);
  for (unsigned int node=0; node< page_ranks.size(); ++node) {
    out_citer node_iter = mapping.find( node);
    PRINT(LOG_LVL_1, page_ranks[node] << '\t' << node_iter->second.url() << endl);
  }
#ifndef NDEBUG
  // Check summation of PageRanks
  rank_type sum_PageRanks = 0.0;
  sum_PageRanks = accumulate( page_ranks.begin(), page_ranks.end(), sum_PageRanks );
  DEBUG("Sigma PageRanks = " << sum_PageRanks << endl);
#endif

}

// Check mode of the PageRank calculation tool
// Find rank leaks and rank sinks and output the groups
void exec_check_mode(PageRank& n) {
  vector<Node> ln;
  ostream_iterator<Node> output (cout,"\n");
    
  PRINT(LOG_LVL_1, "Finding rank leaks... " << endl);
  bool leaks_found = n.find_rank_leaks(ln);
  if (leaks_found) {
    PRINT(LOG_LVL_1, "Leaks : there are " << ln.size() << " leak node(s) " << endl);
    copy ( ln.begin(), ln.end(), output );
  }
  else {
    PRINT(LOG_LVL_1, "No PageRank leaks were found in the network" << endl);
  }
  
  vector<vector<Node> > ls;
  
  PRINT(LOG_LVL_1, "Finding rank sinks... " << endl);
  bool sinks_found = n.find_rank_sinks(ls);
  if (sinks_found) {
    PRINT(LOG_LVL_1, "Sinks : there are " << ls.size() << " sink group(s) " << endl);
    for (unsigned int i = 0; i < ls.size(); ++i) {
      PRINT(LOG_LVL_1, "Sink Group #" << i << endl); 
      copy ( ls[i].begin(), ls[i].end(), output );
    }
  }
  else {
    PRINT(LOG_LVL_1, "No PageRank sinks were found in the network" << endl);
  }
}

// Usage description
void usage(void) {
  PRINT(LOG_LVL_1, "Usage:" << endl);
  PRINT(LOG_LVL_1, "pagerank <network_file> check [-l <log_level>] [-g growth_rate]" << endl);
  PRINT(LOG_LVL_1, "OR" << endl);
  PRINT(LOG_LVL_1, "pagerank <network_file> run <decay_factor> <iterations>\n" 
                   "         [-e <epsilon>] [-l <log_level>] [-g growth_rate]" << endl) ;
}

// simple commandline parser - not much checking
// user is expected to give sane cmdline arguments
// returns -true if cmdline is successfully parsed 
bool parse_cmdline(int argc, char *argv[], 
		   ifstream &net_file, bool &run_mode, double &decay_factor, 
		   int &iterations, double &epsilon, unsigned char &log_lvl, 
		   unsigned int &growth_rate) {
  if (argc < 3 || argc > 11)
    return false;

  net_file.open(argv[1],  ifstream::in);
  if (!net_file.is_open()) {
    ERROR("Couldn't open file : " << argv[1] << endl);
    return false;    
  }

  int opt_args = 0;
  if (string(argv[2]) == "run") { // run mode
    run_mode= true;
    if (argc < 5) // not enough arguments for run mode
      return false;
    // get mandetory parameters run <decay_factor> <iterations>
    decay_factor = atof(argv[3]);
    iterations = atoi(argv[4]);
    opt_args= 5; // from 5
  }
  else if (string(argv[2]) == "check") { // check mode
    run_mode= false;
    opt_args= 3; // from 3
  }
  else { // unknown mode
    return false;    
  }
  // iterate over optional parameters
  for (int i=opt_args; i < argc; i+= 2) { 
    if (argc < i+2) // missing parameters
      return false; 
    switch(argv[i][1]) {
    case 'e' :
      epsilon = atof(argv[i+1]); break;
    case 'l' :
      log_lvl = atoi(argv[i+1]); break;
    case 'g' :
      growth_rate = atoi(argv[i+1]); break;
    default:
      return false;
    }
  }
  return true;
}
