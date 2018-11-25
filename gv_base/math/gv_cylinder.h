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

#ifndef GV_CYLINDER_H
#define GV_CYLINDER_H

struct GV_MATH_API GV_Cylinder
{
	//
	union {
		struct
		{
			GV_Line axis;
		};
		struct
		{
			GV_Vector3 o;
			GV_Vector3 dir;
		};
	};
	FLOAT r;
	//
	inline GV_Cylinder(){};
	inline GV_Cylinder(const GV_Vector3& _o, const GV_Vector3& _dir, FLOAT _r)
	{
		o = _o;
		dir = _dir;
		r = _r;
	};
	inline GV_Cylinder(const GV_Cylinder& c)
	{
		o = c.o;
		r = c.r, dir = c.dir;
	};

	// collision test ;
	E_GEOM_RELATION X(const GV_Vector3&);
	E_GEOM_RELATION X(const GV_Box&);
	E_GEOM_RELATION X(const GV_Sphere&);
	E_GEOM_RELATION X(const GV_Cylinder&);
	E_GEOM_RELATION X(const GV_Line&, FLOAT& s, FLOAT& t);
};
#endif