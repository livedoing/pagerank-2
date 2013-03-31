/*
 * Copyright (c) 2012 Chammika Mannakkara
 *
 *    defaults.h - default values used in various parts of the tool
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

#ifndef PAGERANK_DEFAULTS
#define PAGERANK_DEFAULTS

// don't check for convergenece by default
#define NO_CONVERGENCE_CHECK  -1.0

// adj_list are grown at this rate
#define DEFAULT_GROWTH_RATE  20000

// output message control defaults
#define DEFAULT_LOG_LEVEL    2
#define PROGRESS_REPORT_STEP 10000

#endif
