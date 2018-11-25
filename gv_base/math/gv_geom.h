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

#ifndef GV_GEOM_H
#define GV_GEOM_H

struct GV_SweepResult
{
	GV_SweepResult()
	{
		t[0] = GV_FLT_HUGE, t[1] = -GV_FLT_HUGE;
	}
	FLOAT t[2];		   // time for enter and exit.
	GV_Vector3 pt[2];  // the contact point when enter and exit
	GV_Vector3 nml[2]; // the normal of the contact point.
	UINT user_data;

	inline void Add(const GV_SweepResult& r)
	{
		if (r.t[0] < t[0])
		{
			t[0] = r.t[0];
			pt[0] = r.pt[0];
			nml[0] = r.nml[0];
		}
		if (r.t[1] > t[1])
		{
			t[1] = r.t[1];
			pt[1] = r.pt[1];
			nml[1] = r.nml[1];
		}
	};
};

struct GV_LineCheckResult
{
	FLOAT line_param;
	GV_Vector3 pos;
	GV_Vector3 normal;
	INT flag;
	DWORD user_data;
};

struct GV_XResult
{
	GV_Vector3 pos;
	GV_Vector3 normal;
	FLOAT depth;
	DWORD user_data;
};

#endif
