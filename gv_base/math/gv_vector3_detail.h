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

inline gv_float gv_vector3::Normalize()
{
	gv_float l = Length();
	if (TIsZero(l))
	{
		this->Set(1.0f, 0, 0);
		return 0.0f;
	} // GV_ASSERT(0);
	x /= l, y /= l, z /= l;
	return l;
};

inline gv_float gv_vector3::Length() const
{
	return GV_sqrt(x * x + y * y + z * z);
};

inline gv_float gv_vector3::Dot(const gv_vector3& b) const
{
	return (x * b.x + y * b.y + z * b.z);
}

inline gv_float gv_vector3::LengthSq() const
{
	return x * x + y * y + z * z;
};

inline BOOL gv_vector3::IsEqual(const gv_vector3& v, gv_float epsilon) const
{
	return (TAbs(x - v.x) + TAbs(y - v.y) + TAbs(z - v.z)) < epsilon;
};

inline BOOL gv_vector3::IsZero() const
{
	return TIsEqual(x, 0.0f) && TIsEqual(y, 0.0f) && TIsEqual(z, 0.0f);
};

inline void gv_vector3::Ortho()
{
	gv_vector3 bak(*this);
	x = -bak.y, y = bak.x, z = 0.0f;
}

inline void gv_vector3::Negtive()
{
	x = -x, y = -y, z = -z;
}

inline void gv_vector3::Zero()
{
	this->Set(0.0f, 0.0f, 0.0f);
};
inline void gv_vector3::Identity()
{
	this->Set(0.0f, 0.0f, 0.0f);
};

//==================================
/*
inline gv_vector3::operator		gv_float*()
{
        return af ;
};*/

inline gv_vector3 gv_vector3::operator-() const
{
	return gv_vector3(-x, -y, -z);
};

inline gv_vector3 gv_vector3::operator+() const
{
	return *this;
}

inline gv_vector3& gv_vector3::operator-=(const gv_vector3& a)
{
	x -= a.x, y -= a.y, z -= a.z;
	return *this;
}

inline gv_vector3& gv_vector3::operator-=(gv_float f)
{
	x -= f, y -= f, z -= f;
	return *this;
}
inline gv_vector3& gv_vector3::operator+=(const gv_vector3& a)
{
	x += a.x, y += a.y, z += a.z;
	return *this;
}

inline gv_vector3& gv_vector3::operator+=(gv_float f)
{
	x += f, y += f, z += f;
	return *this;
};

inline gv_vector3& gv_vector3::operator^=(const gv_vector3& a)
{
	gv_vector3 bak(*this);
	x = bak.y * a.z - bak.z * a.y, y = bak.z * a.x - bak.x * a.z,
	z = bak.x * a.y - bak.y * a.x;

	return *this;
}

inline gv_vector3& gv_vector3::operator*=(const gv_vector3& a)
{
	x *= a.x, y *= a.y, z *= a.z;
	return *this;
}

inline gv_vector3& gv_vector3::operator*=(gv_float f)
{
	x *= f, y *= f, z *= f;
	return *this;
}

inline gv_vector3& gv_vector3::operator/=(const gv_vector3& a)
{
	x /= a.x, y /= a.y, z /= a.z;
	return *this;
}

inline gv_vector3& gv_vector3::operator/=(gv_float f)
{
	x /= f, y /= f, z /= f;
	return *this;
}

inline int gv_vector3::operator==(const gv_vector3& a) const
{
	return TIsEqual(x, a.x) && TIsEqual(y, a.y) && TIsEqual(z, a.z);
};

inline int gv_vector3::operator!=(const gv_vector3& a) const
{
	return !(*this == a);
};

inline int gv_vector3::operator>=(const gv_vector3& a) const
{
	for (int i = 0; i < 3; i++)
	{
		if (this->af[i] > a.af[i])
			return TRUE;
		if (this->af[i] < a.af[i])
			return FALSE;
	}
	return TRUE;
};

inline int gv_vector3::operator<=(const gv_vector3& a) const
{
	for (int i = 0; i < 3; i++)
	{
		if (this->af[i] > a.af[i])
			return FALSE;
		if (this->af[i] < a.af[i])
			return TRUE;
	}
	return TRUE;
};

inline int gv_vector3::operator<(const gv_vector3& a) const
{
	for (int i = 0; i < 3; i++)
	{
		if (this->af[i] > a.af[i])
			return FALSE;
		if (this->af[i] < a.af[i])
			return TRUE;
	}
	return FALSE;
};

inline int gv_vector3::operator>(const gv_vector3& a) const
{
	for (int i = 0; i < 3; i++)
	{
		if (this->af[i] > a.af[i])
			return TRUE;
		if (this->af[i] < a.af[i])
			return FALSE;
	}
	return FALSE;
};

inline gv_vector3 gv_vector3::operator-(const gv_vector3& a) const
{
	return gv_vector3(x - a.x, y - a.y, z - a.z);
};

inline gv_vector3 gv_vector3::operator+(const gv_vector3& a) const
{
	return gv_vector3(x + a.x, y + a.y, z + a.z);
}

inline gv_vector3 gv_vector3::operator/(const gv_vector3& a) const
{
	return gv_vector3(x / a.x, y / a.y, z / a.z);
}

inline gv_vector3 gv_vector3::operator*(const gv_vector3& a) const
{
	return gv_vector3(x * a.x, y * a.y, z * a.z);
}

inline gv_vector3 gv_vector3::
operator^(const gv_vector3& a) const // scalar product
{
	return gv_vector3(y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x);
};

inline gv_vector3 gv_vector3::operator-(gv_float f) const
{
	return gv_vector3(x - f, y - f, z - f);
}
inline gv_vector3 gv_vector3::operator+(gv_float f) const
{
	return gv_vector3(x + f, y + f, z + f);
};

inline gv_vector3 gv_vector3::operator/(gv_float f) const
{
	return gv_vector3(x / f, y / f, z / f);
};
inline gv_vector3 gv_vector3::operator*(gv_float f) const
{
	return gv_vector3(x * f, y * f, z * f);
};

inline BOOL gv_vector3::IsUnit() const
{
	return TIsEqual(this->LengthSq(), 1.0f);
}

inline gv_vector3 gv_vector3::Cross(const gv_vector3& v) const
{
	return *this ^ v;
}

inline void gv_vector3::Cross(const gv_vector3& a, gv_vector3& result) const
{
	result.Set(y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x);
}

inline gv_float Dot(const gv_vector3& a, const gv_vector3& b)
{
	return a.Dot(b);
};

inline gv_vector3 Cross(const gv_vector3& a, const gv_vector3& b)
{
	return a ^ b;
};

inline void gv_vector3::Abs()
{
	x = TAbs(x);
	y = TAbs(y);
	z = TAbs(z);
}

inline void gv_vector3::Zero2Epsilon()
{
	if (TIsZero(x))
		x = GV_FLT_EPSILON + GV_FLT_EPSILON;
	if (TIsZero(y))
		y = GV_FLT_EPSILON + GV_FLT_EPSILON;
	if (TIsZero(z))
		z = GV_FLT_EPSILON + GV_FLT_EPSILON;
}

inline gv_vector3 operator-(gv_float f, const gv_vector3& b)
{
	return gv_vector3(b.x - f, b.y - f, b.z - f);
};
inline gv_vector3 operator+(gv_float f, const gv_vector3& b)
{
	return gv_vector3(b.x + f, b.y + f, b.z + f);
};
inline gv_vector3 operator/(gv_float f, const gv_vector3& b)
{
	return gv_vector3(f / b.x, f / b.y, f / b.z);
};
inline gv_vector3 operator*(gv_float f, const gv_vector3& b)
{
	return gv_vector3(b.x * f, b.y * f, b.z * f);
};

inline gv_float gv_vector3::operator|(const gv_vector3& v3) const
{
	return x * v3.x + y * v3.y + z * v3.z;
}

inline gv_float fDistance(const gv_vector3& v, const gv_vector3& v1)
{
	gv_float dx = (v.x - v1.x);
	gv_float dy = (v.y - v1.y);
	gv_float dz = (v.z - v1.z);
	return GV_sqrt(dx * dx + dy * dy + dz * dz);
}
inline int gv_vector3::PartitionCompare(const gv_vector3& p,
										BOOL use_greater_equal)
{
	int ret = 0;
	if (use_greater_equal)
	{
		if (x >= p.x)
			ret |= 4;
		if (y >= p.y)
			ret |= 2;
		if (z >= p.z)
			ret |= 1;
	}
	else
	{
		if (x > p.x)
			ret |= 4;
		if (y > p.y)
			ret |= 2;
		if (z > p.z)
			ret |= 1;
	}
	return ret;
};