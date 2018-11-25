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

#ifndef GV_DONUT_H
#define GV_DONUT_H

#if _MSC_VER > 1000
#pragma once
#endif

struct GV_Box;
struct GV_Sphere;
struct GV_Cylinder;
struct GV_Donut;
struct GV_Frustum;

struct GV_Donut
{
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// data member
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	FLOAT r1, r2;
	GV_Vector3 pos;

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// functions
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	GV_Donut();
	GV_Donut(FLOAT, FLOAT, const GV_Vector3&);
	BOOL IsEmpty();
	void vCreateMinOutside(const GV_LineSeg&);
	void vCreateMaxInside(const GV_Sphere&);
	void vCreateMinOutside(const GV_Sphere&);
	void vCreateMaxInside(const GV_Cylinder&);
	void vCreateMinOutside(const GV_Cylinder&);
	void Create(int, const GV_Vector3*);

	E_GEOM_RELATION eX(const GV_Vector3&);
	E_GEOM_RELATION eX(const GV_Box&);
	E_GEOM_RELATION eX(const GV_Sphere&);
	E_GEOM_RELATION eX(const GV_Cylinder&);
	E_GEOM_RELATION eX(int, const GV_Vector3*);

	// BOOL Operation!
	GV_Donut& operator+=(const GV_LineSeg&);
	GV_Donut& operator+=(const GV_Vector3&);
	GV_Donut& operator+=(const GV_Box&);
	GV_Donut& operator+=(const GV_Sphere&);
	GV_Donut& operator+=(const GV_Cylinder&);

	GV_Donut& operator-=(const GV_LineSeg&);
	GV_Donut& operator-=(const GV_Vector3&);
	GV_Donut& operator-=(const GV_Box&);
	GV_Donut& operator-=(const GV_Sphere&);
	GV_Donut& operator-=(const GV_Cylinder&);

	GV_Donut& operator&=(const GV_LineSeg&);
	GV_Donut& operator&=(const GV_Vector3&);
	GV_Donut& operator&=(const GV_Box&);
	GV_Donut& operator&=(const GV_Sphere&);
	GV_Donut& operator&=(const GV_Cylinder&);

	//=================================================
	void Rotate(const GV_Matrix43&, GV_Vector3[8]);
	void Rotate(const GV_Matrix44&, GV_Vector3[8]);
	void Rotate(const GV_Quat&, GV_Vector3[8]);
	void Scale(const GV_Vector3&);
	void Trans(const GV_Vector3&);

	//=================================================

	void GetMinP(GV_Vector3&);
	void GetMaxP(GV_Vector3&);
	void GetCenter(GV_Vector3&);
	void GetVertex(GV_Vector3[8]);
	void GetScale(GV_Vector3);
	FLOAT GetWidth();
	FLOAT GetHeight();
	FLOAT GetLength();
	void SetWidth(FLOAT);
	void SetHeight(FLOAT);
	void vsetLength(FLOAT);

	void SetMinP(const GV_Vector3&);
	void SetMaxP(const GV_Vector3&);
	void SetCenter(const GV_Vector3&);
	void SetScale(const GV_Vector3&);

	void vMoveCenterTo(const GV_Vector3&);
	void vMoveMaxTo(const GV_Vector3&);
	void vMoveMinTo(const GV_Vector3&);
};

inline GVI_Stream& operator>>(GVI_Stream& s, GV_Donut& c)
{
	s >> c.pos >> c.r1 >> c.r2;
	return s;
}

inline GVI_Stream& operator<<(GVI_Stream& s, const GV_Donut& c)
{
	s << c.pos << c.r1 << c.r2;
	return s;
}
#endif
