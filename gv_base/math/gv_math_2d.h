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

#ifndef GV_MATH_2D_H
#define GV_MATH_2D_H

#if _MSC_VER > 1000
#pragma once
#endif

struct GV_MATH_API GV_Math2D
{

	// Triangle utility functions

	// Is the specified point inside the triangle?
	static BOOL bPointInTriangle(const GV_Vector3& pt, const GV_Vector3& t0,
								 const GV_Vector3& t1, const GV_Vector3& t2);

	static BOOL bPointInTriangle(const GV_Vector3& pt, GV_Vector3 triangle[3]);

	// Sweep two rects
	static BOOL bRectSweep(const GV_Rect& ra, const GV_Vector2& a0,
						   const GV_Vector2& a1, const GV_Rect& rb,
						   const GV_Vector2& b0, const GV_Vector2& b1, FLOAT& u0,
						   FLOAT& u1, GV_Vector2& hitNormal,
						   GV_Vector2& exitNormal);
};

#endif