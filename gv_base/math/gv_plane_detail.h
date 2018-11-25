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

inline GV_Plane::GV_Plane(){};

inline GV_Plane::GV_Plane(float x, float y, float z, float d)
{
	this->normal_v.Set(x, y, z);
	this->offset = d;
};

inline GV_Plane::GV_Plane(const GV_Vector3& normal, float d)
{
	this->normal_v = normal;
	this->offset = d;
};

inline GV_Plane::GV_Plane(const GV_Vector3& normal, const GV_Vector3& point)
{
	this->normal_v = normal;
	this->offset = -normal.Dot(point);
};

inline GV_Plane::GV_Plane(const GV_Vector3& v1, const GV_Vector3& v2,
						  const GV_Vector3& v3)
{
	this->normal_v = (v2 - v1) ^ (v3 - v1);
	this->normal_v.Normalize();
	this->offset = -this->normal_v.Dot(v1);
};

inline bool GV_Plane::operator==(const GV_Plane& plane) const
{
	return this->normal_v == plane.normal_v &&
		   TIsEqual(this->offset, plane.offset);
};

inline bool GV_Plane::operator!=(const GV_Plane& plane) const
{
	return !(*this == plane);
};

inline GV_Plane GV_Plane::operator-() const
{
	GV_Plane plane(-this->normal_v, -this->offset);
	return plane;
};

inline void GV_Plane::Set(const GV_Vector3& normal, float d)
{
	this->normal_v = normal;
	this->offset = d;
};

inline bool GV_Plane::Set(const GV_Vector3& v1, const GV_Vector3& v2,
						  const GV_Vector3& v3)
{
	GV_Plane a(v1, v2, v3);
	(*this) = a;
	return TRUE;
};
inline void GV_Plane::Set(const GV_Vector3& normal, const GV_Vector3& point)
{
	GV_Plane p(normal, point);
	*this = p;
};

inline float GV_Plane::TestPoint(const GV_Vector3& point) const
{
	return point.Dot(this->normal_v) + this->offset;
};

inline void GV_Plane::GetNormal(GV_Vector3& n) const
{
	n = this->normal_v;
}

inline const GV_Vector3& GV_Plane::GetNormal() const
{
	return normal_v;
};

inline void GV_Plane::GetNormal(float& x, float& y, float& z) const
{
	x = normal_v.x;
	y = normal_v.y;
	z = normal_v.z;
}

inline void GV_Plane::GetOffset(float& d)
{
	d = offset;
}
inline float GV_Plane::GetOffset()
{
	return offset;
};

inline void GV_Plane::SetNormal(const GV_Vector3& vector)
{
	normal_v = vector;
}

inline void GV_Plane::SetOffset(float f)
{
	offset = f;
}

inline GV_Vector3 GV_Plane::Project(const GV_Vector3& point) const
{
	return point - this->normal_v * (this->normal_v.Dot(point) + this->offset);
} // End of RSPlane::Project()
