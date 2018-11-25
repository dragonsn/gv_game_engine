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

inline GV_LineSeg::GV_LineSeg(){};
inline GV_LineSeg::GV_LineSeg(const GV_Vector3& endpoint1,
							  const GV_Vector3& endpoint2)
{
	this->start_p = endpoint1;
	this->end_p = endpoint2;
};

// Setting, Getting functions
inline void GV_LineSeg::Set(const GV_Vector3& endpoint1,
							const GV_Vector3& endpoint2)
{
	this->start_p = endpoint1;
	this->end_p = endpoint2;
};
inline void GV_LineSeg::Get(GV_Vector3& endpoint1,
							GV_Vector3& endpoint2) const
{
	endpoint1 = this->start_p;
	endpoint2 = this->end_p;
};
inline const GV_Vector3& GV_LineSeg::GetEndpoint1(void) const
{
	return start_p;
}

inline const GV_Vector3& GV_LineSeg::GetEndpoint2(void) const
{
	return end_p;
}

// Returns the distance from the line segment to the point

inline float GV_LineSeg::DistanceSquaredToPoint(const GV_Vector3& point,
												GV_Vector3& otherPoint) const
{
	float f = this->DistanceToPoint(point, otherPoint);
	return f * f;
}

// Returns the distance from the line segment to the line segment

inline bool GV_LineSeg::operator==(const GV_LineSeg& segment) const
{
	return this->start_p == segment.start_p && this->end_p == segment.end_p;
}

inline bool GV_LineSeg::operator!=(const GV_LineSeg& segment) const
{
	return !(*this == segment);
};

inline GV_Vector3 GV_LineSeg::GetMidPoint(void)
{
	return (start_p + start_p) / 2.0f;
}

inline GV_Vector3 GV_LineSeg::GetDir() const
{
	GV_Vector3 v = this->end_p - this->start_p;
	v.Normalize();
	return v;
}
// ASCII GV_LineSeg input/output
