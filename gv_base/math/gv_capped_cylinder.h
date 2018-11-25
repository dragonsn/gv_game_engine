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

#ifndef GV_CAPPED_CYLINDER_H
#define GV_CAPPED_CYLINDER_H

struct GV_Box;
struct GV_Sphere;
struct GV_CappedCylinder;
struct GV_Donut;
struct GV_Frustum;

struct GV_CappedCylinder
{

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// data member
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	FLOAT r;
	FLOAT h;
	GV_Vector3 o;

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// data member
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	GV_CappedCylinder();
	GV_CappedCylinder(const GV_Vector3& origin, float r, float h);
	~GV_CappedCylinder();

	BOOL IsEmpty();

	//=================================================
	void SetCenter(const GV_Vector3&);
	void GetCenter(const GV_Vector3&);
	void SetRadius(FLOAT r);
	FLOAT GetRadius();
};

inline GVI_Stream& operator>>(GVI_Stream& s, GV_CappedCylinder& c)
{
	s >> c.o >> c.h >> c.r;
	return s;
}

inline GVI_Stream& operator<<(GVI_Stream& s, const GV_CappedCylinder& c)
{
	s << c.o << c.h << c.r;
	return s;
}

#endif