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

#ifndef GV_POLYHEDRON_H
#define GV_POLYHEDRON_H

struct GV_MATH_API GV_Polyhedron
{

	GV_Polyhedron();
	GV_Polyhedron(const GV_Box&);
	GV_Polyhedron(const GV_Polyhedron&);

	GVT_Array< GV_Vector3 > m_Vertice;
	GVT_Array< INT > m_Edges;
	GVT_Array< INT > m_Triangles;

	inline int NumTri() const
	{
		return m_Triangles.Size() / 3;
	}
	inline int NumEdge() const
	{
		return m_Edges.Size() / 2;
	}
	inline int NumVertex() const
	{
		return m_Vertice.Size();
	};
	void Move(const GV_Vector3& pos);
	void Transform(const GV_Matrix44& mat);
	GV_Box GetAABB()
	{
		GV_Box b;
		for (int i = 0; i < this->m_Vertice.Size(); i++)
		{
			b.Add(this->m_Vertice[i]);
		}
		return b;
	};
	void SetBox(const GV_Box& b);
	GV_Vector3 GetTriNormal(int) const;
	//
	E_GEOM_RELATION X(const GV_Line&, GV_LineCheckResult ret[2]) const;
	E_GEOM_RELATION X(const GV_Vector3&) const;

	BOOL Sweep(const GV_Vector3& speed, const GV_Polyhedron& poly,
			   GV_SweepResult& ret);
};

#endif