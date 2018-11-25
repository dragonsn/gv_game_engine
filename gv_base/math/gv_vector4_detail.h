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

inline GV_Vector4::GV_Vector4(){

};

inline GV_Vector4::GV_Vector4(FLOAT _x, FLOAT _y, FLOAT _z)
{
	x = _x, y = _y, z = _z, w = 1.0f;
}
inline GV_Vector4::GV_Vector4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w)
{
	x = _x, y = _y, z = _z, w = _w;
}

inline GV_Vector4::GV_Vector4(const GV_Vector3& a)
{
	x = a.x, y = a.y, z = a.z, w = 1.0f;
}

inline GV_Vector4::GV_Vector4(const GV_Vector4& a)
{
	x = a.x, y = a.y, z = a.z, w = a.w;
}

inline GV_Vector4::GV_Vector4(FLOAT* pf)
{
	if (!pf)
		this->Zero();
	else
		x = pf[0], y = pf[1], z = pf[2], w = pf[3];
}

//==================================
// set		& get
inline void GV_Vector4::Set(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w)
{
	x = _x, y = _y, z = _z, w = _w;
}

inline void GV_Vector4::Get(FLOAT& _x, FLOAT& _y, FLOAT& _z, FLOAT& _w)
{
	_x = x, _y = y, _z = z, _w = w;
}

inline void GV_Vector4::CopyTo(FLOAT* p) const
{
	p[0] = x, p[1] = y, p[2] = z, p[3] = w;
}

inline void GV_Vector4::CopyFrom(const FLOAT* pf)
{
	x = pf[0], y = pf[1], z = pf[2], w = pf[3];
}

//==================================
inline FLOAT GV_Vector4::Normalize()
{
	float l = Length();
	if (TIsZero(l))
		GV_ASSERT(0);
	x /= l, y /= l, z /= l, w = 1.0f;
	return l;
};

inline FLOAT GV_Vector4::Length() const
{
	float l = GV_sqrt(x * x + y * y + z * z);
	return l;
};

inline FLOAT GV_Vector4::Dot(const GV_Vector4& b) const
{
	return (x * b.x + y * b.y + z * b.z);
}

inline FLOAT GV_Vector4::LengthSq() const
{
	float l = x * x + y * y + z * z;
	return l;
};

inline BOOL GV_Vector4::IsEqual(const GV_Vector4& v, FLOAT epsilon) const
{
	return (TAbs(x - v.x) + TAbs(y - v.y) + TAbs(z - v.z) + TAbs(w - v.w)) <
		   epsilon;
};

inline BOOL GV_Vector4::IsZero() const
{
	return TIsEqual(x, 0.0f) && TIsEqual(y, 0.0f) && TIsEqual(z, 0.0f) &&
		   TIsEqual(w, 0.0f);
};

inline void GV_Vector4::Ortho()
{
	GV_Vector4 bak(*this);
	x = -bak.y, y = bak.x, z = 0.0f, w = 1.0f;
}

inline void GV_Vector4::Homogenize()
{
	x /= w, y /= w, z /= w;
	w = 1.0f;
}

inline void GV_Vector4::Negtive()
{
	x = -x, y = -y, z = -z, w = -w;
}

inline void GV_Vector4::Zero()
{
	this->Set(0.0f, 0.0f, 0.0f, 0.0f);
};
inline void GV_Vector4::Identity()
{
	this->Set(0.0f, 0.0f, 0.0f, 1.0f);
};

//==================================
inline GV_Vector4::operator FLOAT*()
{
	return af;
};

inline GV_Vector4 GV_Vector4::operator-() const
{
	return GV_Vector4(-x, -y, -z, -w);
};

inline GV_Vector4 GV_Vector4::operator+() const
{
	return *this;
}

inline GV_Vector4& GV_Vector4::operator-=(const GV_Vector4& a)
{
	x -= a.x, y -= a.y, z -= a.z, w -= a.w;
	return *this;
}

inline GV_Vector4& GV_Vector4::operator-=(FLOAT f)
{
	x -= f, y -= f, z -= f, w -= f;
	return *this;
}
inline GV_Vector4& GV_Vector4::operator+=(const GV_Vector4& a)
{
	x += a.x, y += a.y, z += a.z, w += a.w;
	return *this;
}

inline GV_Vector4& GV_Vector4::operator+=(FLOAT f)
{
	x += f, y += f, z += f, w += f;
	return *this;
};

inline GV_Vector4& GV_Vector4::operator^=(const GV_Vector4& a)
{
	GV_Vector4 bak(*this);
	x = bak.y * a.z - bak.z * a.y, y = bak.z * a.x - bak.x * a.z,
	z = bak.x * a.y - bak.y * a.x, w = 1.0f;
	return *this;
}

inline GV_Vector4& GV_Vector4::operator*=(const GV_Vector4& a)
{
	x *= a.x, y *= a.y, z *= a.z, w *= a.w;
	return *this;
}

inline GV_Vector4& GV_Vector4::operator*=(FLOAT f)
{
	x *= f, y *= f, z *= f, w *= f;
	return *this;
}

inline GV_Vector4& GV_Vector4::operator/=(const GV_Vector4& a)
{
	x *= a.x, y *= a.y, z *= a.z, w *= a.w;
	return *this;
}

inline GV_Vector4& GV_Vector4::operator/=(FLOAT f)
{
	x /= f, y /= f, z /= f, w /= f;
	return *this;
}

inline int GV_Vector4::operator==(const GV_Vector4& a) const
{
	return TIsEqual(x, a.x) && TIsEqual(x, a.x) && TIsEqual(x, a.x) &&
		   TIsEqual(w, a.w);
};

inline GV_Vector4 GV_Vector4::operator-(const GV_Vector4& a) const
{
	return GV_Vector4(x - a.x, y - a.y, z - a.z, w - a.w);
};

inline GV_Vector4 GV_Vector4::operator+(const GV_Vector4& a) const
{
	return GV_Vector4(x + a.x, y + a.y, z + a.z, w + a.w);
}

inline GV_Vector4 GV_Vector4::operator/(const GV_Vector4& a) const
{
	return GV_Vector4(x / a.x, y / a.y, z / a.z, w / a.w);
}

inline GV_Vector4 GV_Vector4::operator*(const GV_Vector4& a) const
{
	return GV_Vector4(x * a.x, y * a.y, z * a.z, w * a.w);
}

inline GV_Vector4 GV_Vector4::
operator^(const GV_Vector4& a) const // scalar product
{
	return GV_Vector4(y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x,
					  1.0f);
};

inline GV_Vector4 GV_Vector4::operator-(FLOAT f) const
{
	return GV_Vector4(x - f, y - f, z - f, w - f);
}
inline GV_Vector4 GV_Vector4::operator+(FLOAT f) const
{
	return GV_Vector4(x + f, y + f, z + f, w + f);
};

inline GV_Vector4 GV_Vector4::operator/(FLOAT f) const
{
	return GV_Vector4(x / f, y / f, z / f, w / f);
};
inline GV_Vector4 GV_Vector4::operator*(FLOAT f) const
{
	return GV_Vector4(x * f, y * f, z * f, w * f);
};

inline BOOL GV_Vector4::IsUnit() const
{
	return TIsEqual(this->LengthSq(), 1.0f);
}
