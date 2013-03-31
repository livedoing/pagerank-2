/*
 * Copyright (c) 2012 Chammika Mannakkara
 *
 *    Node.cpp - Simple Node class that represents a Web page/URL of network
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
#include "types.h"
#include "Node.h"

// write a Node to stream  
// write format : [ID] URL 
ostream& operator<<(ostream& os, const Node& n) {
  return os << '[' << n.id_ << ']' << n.url_ ;
}


// read Node from stream  
// Cannot directly create a node
// Reason: reader cannot uniquely assign an ID
istream& operator>>(istream& is, Node& n) {
  // don't call this !
  return is;
}

// write a Node to stream 
// STL map<> version
ostream& operator<<(ostream& os, const pair<string, Node>& p) {
  // Node n = p.second;
  return os << p.second ;
}

