/*
 * Copyright (c) 2012 Chammika Mannakkara
 *
 *    Log.h - Simple loging/debugging mechanism with #define macros
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
#ifndef PAGERANK_LOG
#define PAGERANK_LOG

#include <iostream>

using std::cout;
using std::cerr;

struct Log { // class Log public:
  static unsigned char level_;
};

// Simple macros loging : DEBUG, PRINT and ERROR

// #define NDEBUG

#define ERROR(str) std::cerr << str ;
 
#ifndef NDEBUG
#define DEBUG(str) std::cout << str ;
#else
#define DEBUG(str) 
#endif

#define PRINT(lvl, str) if (Log::level_ >= lvl) std::cout << str ;

#define LOG_LVL_1       1
#define LOG_LVL_2       2
#define LOG_LVL_3       3

#endif
