#
# Copyright (c) 2012 Chammika Mannakkara
#
#    Makefile - Makefile for building PageRank tool
#
#    This is a part of simple tool calculate the PageRank
#
#    This source code is free software; you can redistribute it
#    and/or modify it in source code form under the terms of the GNU
#    General Public License as published by the Free Software
#    Foundation; either version 2 of the License, or (at your option)
#    any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA

CPP = g++
LD  = g++
CPPFLAGS = -Wall
LDFLAGS = 
CPP_DEBUG_FLAGS = -g
CPP_RELEASE_FLAGS = -O3 -DNDEBUG 
CPP_PROFILE_FLAGS = -O3 -g -DNDEBUG 

EXE =   pagerank
SRCDIR = src
OBJDIR = obj
BINDIR = bin
LIBDIR =

SRCS = $(wildcard $(SRCDIR)/*.cpp)
HDRS = $(wildcard $(SRCDIR)/*.h)
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

.PHONY : all debug

all:: CPPFLAGS+= $(CPP_RELEASE_FLAGS)
all:: $(BINDIR)/$(EXE)

debug:: CPPFLAGS+=$(CPP_DEBUG_FLAGS) 
debug::$(BINDIR)/$(EXE)

profile:: CPPFLAGS+=$(CPP_PROFILE_FLAGS) 
profile::$(BINDIR)/$(EXE)

$(BINDIR)/$(EXE): $(OBJS) 
	$(LD) $(LDFLAGS) $(OBJS) -o $@ $(LIBDIR)

$(OBJS): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HDRS)
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJDIR)/*.o $(BINDIR)/$(EXE) $(BINDIR)/core*

