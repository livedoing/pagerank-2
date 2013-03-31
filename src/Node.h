/*
 * Copyright (c) 2012 Chammika Mannakkara
 *
 *    Node.h - Simple Node class that represents a Web page/URL of network
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
#ifndef PAGERANK_NODE_CLASS
#define PAGERANK_NODE_CLASS

#include "types.h"

// Node Class - represents a Web page of the Network. As for this tool it
// only contains the URL of the page. This Node class serves as a mapping between
// the abstract respresentation of the Network class, which assigns a unique
// ID for each Node it encounter.
class Node {
public:
   Node() { } // default ctor - to be used by STL container initializations only
   Node( node_id_type id, const string& url): id_(id) , url_(url) { } // trivial ctor
  // Node( const Node&); // copy ctor - default OK
  // Node& operator=(const Node&); // assignment operator -default OK

  node_id_type id() const { return id_; }
  const string& url() const { return url_; }

  // operator less() for sorting STL Node containers based on ID
  bool operator<(const Node& rhs) const { return id_ < rhs.id_; } 

  // I/O :
  friend ostream& operator<<(ostream& os, const Node& n); // write a Node to stream   
  friend istream& operator>>(istream& is, Node& n); // read Node from stream

private:
  node_id_type id_; // unique ID assigned to Node
  string       url_; // URL string of node
  // Other Node data (if any) goes here
};

#endif
