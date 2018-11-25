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
inline GV_Line::GV_Line(){};

inline GV_Line::GV_Line(const GV_Vector3& linePoint,
						const GV_Vector3& lineVector)
{
	this->origin_p = linePoint;
	this->dir_v = lineVector;
};

inline BOOL GV_Line::operator==(const GV_Line& line) const
{
	return this->origin_p == line.origin_p && this->dir_v == line.dir_v;
};

inline BOOL GV_Line::operator!=(const GV_Line& line) const
{
	return !(*this == line);
};

inline GV_Line& GV_Line::Set(const GV_Vector3& linePoint,
							 const GV_Vector3& lineVector)
{
	this->origin_p = linePoint;
	this->dir_v = lineVector;
	return *this;
};

inline const GV_Vector3& GV_Line::GetPoint(void) const
{
	return this->origin_p;
};

inline const GV_Vector3& GV_Line::GetVector(void) const
{
	return this->dir_v;
};

inline void GV_Line::SetPoint(const GV_Vector3& point)
{
	this->origin_p = point;
};

inline void GV_Line::SetVector(const GV_Vector3& vector)
{
	this->dir_v = vector;
};

inline GV_Vector3 GV_Line::GetPointOnLine(float t) const
{
	return this->origin_p + t * this->dir_v;
};

inline GV_Vector3 GV_Line::Project(const GV_Vector3& point)
{
	GV_Vector3 d = point - this->origin_p;
	float dist = d.Dot(this->dir_v);
	return this->origin_p + dist * this->dir_v;
}