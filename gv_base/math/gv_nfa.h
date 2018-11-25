/*
Copyright (C) shennnan(shennan@gv4game.com) 2002-2004

This file is part of GreenValley, the free game engine.
GreenValley is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

GreenValley is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GreenValley; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef GV_NFA_H
#define GV_NFA_H
#include "GV_BitSet.h"
class GV_Nfa
{

	CNfa();
	~CNfa();
	Clear();
	inline const CNfa& operator=(const CNfa& n);
	SetAccept(char* acceptstring);

public:
	GV_BitSet m_edgeSet;
	int m_Edge;
	int m_isAcceptable;
	GV_String m_acceptString;
	int m_NextIndex[0];
};

/*
class CNfaMgr
{
public:
        GV_Nfa *  Allocate	(		);
        void	Delete		(CNfa  *);
        int		QueryIndex	(CNfa * );
        GV_Nfa *	GetByIndex	(int index);
        CSet *  e_Closure(CSet  * pstart, char  ** pAccept,  int  * anchor ); //
get all the reachable state from this nfa
                DumpAll (FILE * fp );
        CSet * move(CSet & inp_set ,unsigned char  c );
};
*/

#endif