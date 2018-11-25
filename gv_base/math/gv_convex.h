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

#ifndef GV_CONVEX_H
#define GV_CONVEX_H

struct GV_ConvexTri
{
	int index[3];
	int normal;
};

struct GV_ConvexEdge
{
	GV_ConvexEdge(){};
	GV_ConvexEdge(int i0, int i1, int p0, int p1)
	{
		index[0] = i0;
		index[1] = i1;
		neighbourPlane[0] = p0;
		neighbourPlane[1] = p1;
	};
	int index[2];
	int neighbourPlane[2];
};

struct GV_PlaneFlags
{
	int cachedPushoutIndex;
};

struct GV_MATH_API GV_Convex
{
public:
	GV_Convex(){};
	GV_Convex(const GV_Convex& c)
	{
		m_BoundPlanes = c.m_BoundPlanes;
		m_PlaneFlags = c.m_PlaneFlags;
		m_Vertice = c.m_Vertice;
		m_Edges = c.m_Edges;
	};

	GVT_Array< GV_Plane > m_BoundPlanes;
	GVT_Array< GV_PlaneFlags > m_PlaneFlags;
	GVT_Array< GV_Vector3 > m_Vertice;
	GVT_Array< GV_ConvexEdge > m_Edges;

public:
	void MinkowskiAdd(const GV_Convex&, GV_Convex& ret);		 //
	void MinkowskiAddAccurate(const GV_Convex&, GV_Convex& ret); //

	void SetBox(const GV_Box& b);
	void SetFrustum(FLOAT fov, FLOAT aspect, FLOAT near_p, FLOAT far_p);
	void Transform(const GV_Matrix44& mat);
	void Reflect();

	BOOL Sweep(const GV_Vector3& start, const GV_Vector3& end, GV_Convex& c,
			   GV_SweepResult& ret);

	E_GEOM_RELATION X(const GV_Line&, GV_LineCheckResult ret[2]);
	E_GEOM_RELATION X(const GV_Matrix43& mata, const GV_Box& b,
					  const GV_Matrix43& matb,
					  GV_XResult& ret = *((GV_XResult*)NULL));
	E_GEOM_RELATION X(const GV_Vector3&, GV_XResult& ret);
};
GV_MATH_API GV_String& operator<<(GV_String& s, const GV_Convex& c);
#endif