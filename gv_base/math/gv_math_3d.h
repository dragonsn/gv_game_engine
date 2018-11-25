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

#ifndef GV_MATH_3D_H
#define GV_MATH_3D_H

#if _MSC_VER > 1000
#pragma once
#endif

struct GV_MATH_API GV_Math3D
{
	//----------------------------------------------------------------------------
	//-- Function Declarations, Macros -------------------------------------------
	//----------------------------------------------------------------------------

	// Finds the nearest point on a plane from a point
	static GV_Vector3 NearestPointOnPlane(const GV_Vector3& point,
										  const GV_Vector3& planeNormal,
										  FLOAT planeOffset);

	// Finds the nearest point on a line from a point.
	static GV_Vector3 NearestPointOnLine(const GV_Vector3& point,
										 const GV_Vector3& linePoint,
										 const GV_Vector3& lineVector,
										 FLOAT& lineParameter);

	// Finds the intersection between a line and a plane.  The lineParameter
	//   is the value that represents the resulting point in the equation
	//   linePoint + (lineParameter * lineVector).
	static GV_Vector3 IntersectLineWithPlane(const GV_Vector3& linePoint,
											 const GV_Vector3& lineVector,
											 const GV_Vector3& planeNormal,
											 FLOAT planeOffset,
											 FLOAT& lineParameter);

	// Intersects a 3D line with a plane.  Returns TRUE if there is an
	//   intersection, otherwise FALSE.  If there is an intersection, the
	//   intersection point and the parameter scaling of the line is returned.
	static BOOL bIntersectLineWithPlane(const GV_Line& line,
										const GV_Plane& plane,
										GV_Vector3& intersectionPoint,
										FLOAT& intersectionLineParameter);

	// Return the determinant of a 3x3 matrix composed of 3 RSVector3s
	static FLOAT fDeterminantOfVectors(const GV_Vector3& a, const GV_Vector3& b,
									   const GV_Vector3& c);

	static BOOL bIntersectLineWithSphere(const GV_Vector3& linePoint,
										 const GV_Vector3& lineVector,
										 const GV_Vector3& sphereCenter,
										 FLOAT sphereRadius, FLOAT& s, FLOAT& t);

	// If three planes intersect, this returns true and puts the resulting
	//   point in the last parameter.
	static BOOL bIntersectionOfThreePlanes(const GV_Plane& plane0,
										   const GV_Plane& plane1,
										   const GV_Plane& plane2,
										   GV_Vector3& result);

	// Returns TRUE if the point is inside the triangle (it is assumed that
	//   the point has been projected onto the surface already).
	static BOOL IsPointInsideTriangle(const GV_Vector3& point,
									  const GV_Vector3& v0, const GV_Vector3& v1,
									  const GV_Vector3& v2,
									  const GV_Vector3& triangleNormal);
	static BOOL IsPointInsideTriangle(const GV_Vector3& point,
									  const GV_Vector3* v0, const GV_Vector3* v1,
									  const GV_Vector3* v2,
									  const GV_Vector3& triangleNormal);

	// Test to see at what time the sphere first intersects the plane.
	static BOOL bSpherePlaneSweep(const GV_Vector3& C0, const GV_Vector3& C1,
								  const FLOAT radius, const GV_Plane& plane,
								  GV_Vector3& Ci, FLOAT& t);

	static BOOL IsColinear(const GV_Vector3& a, const GV_Vector3& b,
						   const GV_Vector3& c);

	//----------------------------------------------------------------------------
	//-- Typedefs, Structs, Classes ----------------------------------------------
	//----------------------------------------------------------------------------

	//----------------------------------------------------------------------------
	//-- Inline Functions --------------------------------------------------------
	//----------------------------------------------------------------------------

	//----------------------------------------------------------------------------
	// @ QuadraticFormula()
	// ---------------------------------------------------------------------------
	// Return true if r1 and r2 are real
	//----------------------------------------------------------------------------

	static BOOL bQuadraticFormula(const FLOAT a, const FLOAT b, const FLOAT c,
								  FLOAT& r1,
								  /*first*/ FLOAT& r2 /*and second roots*/);

	//----------------------------------------------------------------------------
	// @ IsPointInsideTriangle()
	// ---------------------------------------------------------------------------
	// Returns true if the point is inside the triangle (on the plane).  It is
	//   assumed that the point has already been projected onto the plane.
	//
	// This inline calculates the triangle normal before sending it down to the
	// faster version. NOTE: We may not need to normalize the normal, it may be
	// enough just to get the direction.
	//----------------------------------------------------------------------------
	static BOOL IsPointInsideTriangle(const GV_Vector3& point,
									  const GV_Vector3& v0, const GV_Vector3& v1,
									  const GV_Vector3& v2);

	static BOOL bIntersectRayTriangle(const GV_Line& l, const GV_Vector3& v0,
									  const GV_Vector3& v1, const GV_Vector3& v2,
									  FLOAT& t, FLOAT& u, FLOAT& v,
									  BOOL& hitback);

	static BOOL IsPointInTri(GV_Vector3& point, GV_Vector3& p1, GV_Vector3& p2,
							 GV_Vector3& p3);

	static BOOL bIntersectTwoPlane(const GV_Plane& plane1, const GV_Plane& plane2,
								   GV_Line& result);
};
//

#endif