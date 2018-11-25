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

inline GV_Quat::GV_Quat()
{
}

inline GV_Quat::GV_Quat(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
};

inline GV_Quat::GV_Quat(const FLOAT* pf)
{
	x = pf[0];
	y = pf[1];
	z = pf[2];
	w = pf[3];
}

inline GV_Quat::GV_Quat(const GV_Quat& q)
{
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;
}

inline void GV_Quat::Set(FLOAT a, FLOAT b, FLOAT c, FLOAT d)
{
	x = a, y = b, z = c, w = d;
}
inline void GV_Quat::Get(FLOAT& a, FLOAT& b, FLOAT& c, FLOAT& d)
{
	a = x, b = y, c = z, d = w;
}

inline GV_Quat GV_Quat::operator+(const GV_Quat& q) const
{
	return GV_Quat(x + q.x, y + q.y, z + q.z, w + q.w);
}
inline GV_Quat GV_Quat::operator-(const GV_Quat& q) const
{
	return GV_Quat(x + q.x, y + q.y, z + q.z, w + q.w);
}

inline GV_Quat GV_Quat::operator*(const GV_Quat& q) const
{
	FLOAT Dx = x * q.w + y * q.z - z * q.y + w * q.x;
	FLOAT Dy = -x * q.z + y * q.w + z * q.x + w * q.y;
	FLOAT Dz = x * q.y - y * q.x + z * q.w + w * q.z;
	FLOAT Dw = -x * q.x - y * q.y - z * q.z + w * q.w;
	return (GV_Quat(Dx, Dy, Dz, Dw));
};

inline GV_Quat GV_Quat::operator/(const GV_Quat& q) const
{
	GV_Quat bak(q);
	bak.Inverse();
	return (*this) * bak;
}

inline GV_Quat& GV_Quat::operator+=(const GV_Quat& q)
{
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;
	return *this;
}

inline GV_Quat& GV_Quat::operator-=(const GV_Quat& q)
{
	x -= q.x;
	y -= q.y;
	z -= q.z;
	w -= q.w;
	return *this;
}

inline GV_Quat& GV_Quat::operator*=(const GV_Quat& q)
{
	FLOAT Dx = x * q.w + y * q.z - z * q.y + w * q.x;
	FLOAT Dy = -x * q.z + y * q.w + z * q.x + w * q.y;
	FLOAT Dz = x * q.y - y * q.x + z * q.w + w * q.z;
	FLOAT Dw = -x * q.x - y * q.y - z * q.z + w * q.w;
	this->Set(Dx, Dy, Dz, Dw);
	return *this;
}
inline GV_Quat& GV_Quat::operator/=(const GV_Quat& q)
{
	GV_Quat bak(q);
	bak.Inverse();
	(*this) *= bak;
	return *this;
}

inline int GV_Quat::operator==(const GV_Quat& p) const
{
	return TIsEqual(x, p.x) && TIsEqual(y, p.y) && TIsEqual(z, p.z) &&
		   TIsEqual(w, p.w);
}

inline void GV_Quat::Negate()
{
	x = -x, y = -y, z = -z, w = -w;
}

inline void GV_Quat::Normalize()
{
	float f = GV_sqrt(x * x + y * y + z * z + w * w);
	this->x /= f;
	this->y /= f;
	this->z /= f;
	this->w /= f;
};

inline void GV_Quat::Conjugate()
{
	this->x = -this->x;
	this->y = -this->y;
	this->z = -this->z;
};