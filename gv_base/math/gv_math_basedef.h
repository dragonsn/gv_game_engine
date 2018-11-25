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

#ifndef GV_MATH_BASEDEF_H
#define GV_MATH_BASEDEF_H

#undef GV_MATH_API
#ifdef GV_MATH_EXPORTS
#define GV_MATH_API GV_API_EXPORT
#else
#define GV_MATH_API GV_API_IMPORT
#endif

typedef enum {
	E_GR_SEPERATE,
	E_GR_INTERSECT,
	E_GR_INCLUDE,
	E_GR_OCCLUDE,
	E_GR_EQUAL,
} E_GEOM_RELATION;

template < class T >
BOOL TIsZero(T f)
{
	return TAbs(f) < GV_FLT_EPSILON;
}

template < class T >
BOOL TIsEqual(T a, T b)
{
	return TAbs(a - b) < GV_FLT_EPSILON;
}

template < class T >
BOOL TIsLess(T a, T b)
{
	return a < b - GV_FLT_EPSILON;
}

template < class T >
BOOL TIsGreater(T a, T b)
{
	return a > b + GV_FLT_EPSILON;
}

template < class T >
T TMirror(T o, T x)
{
	return 2 * o - x;
}

///////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////

inline FLOAT GV_Interpolate_Cos(FLOAT _f1, FLOAT _f2, FLOAT _f)
{
	FLOAT f = (1.0f - (FLOAT)GV_cos(_f * GV_PI)) * 0.5f;
	return _f1 * (1.0f - f) + _f2 * f;
}

///////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////

inline FLOAT GV_Interpolate_Cubic(FLOAT v0, FLOAT v1, FLOAT v2, FLOAT v3,
								  FLOAT x)
{

	FLOAT P = (v3 - v2) - (v0 - v1);
	FLOAT Q = (v0 - v1) - P;
	FLOAT R = v2 - v0;
	FLOAT S = v1;
	return P * (x * x * x) + Q * (x * x) + R * x + S;
}

///////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////
inline FLOAT GV_Interpolate_Linear(FLOAT a, FLOAT b, FLOAT Alpha)
{
	return a * (1 - Alpha) + b * Alpha;
}
//

inline float GV_fRandom(float min, float max)
{
	float a = (float)rand();
	return ((a) / (RAND_MAX)) * (max - min) + min;
}

#endif
