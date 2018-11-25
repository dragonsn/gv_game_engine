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

inline void GV_Rect::Set(FLOAT x, FLOAT y, FLOAT x1, FLOAT y1)
{
	min_p.x = x, min_p.y = y, max_p.x = x1, max_p.y = y1;
}

inline void GV_Rect::Enlarge(FLOAT x, FLOAT y)
{
	max_p.x += x;
	max_p.y += y;
}

inline void GV_Rect::GetPos(FLOAT& x, FLOAT& y) const
{
	x = min_p.x;
	y = min_p.y;
}

inline void GV_Rect::GetSize(FLOAT& x, FLOAT& y) const
{
	x = this->max_p.x - this->min_p.x;
	y = this->max_p.y - this->min_p.y;
}

inline void GV_Rect::Move(FLOAT x, FLOAT y)
{
	this->min_p.x += x;
	this->max_p.x += x;
	this->max_p.y += y;
	this->min_p.y += y;
}

inline void GV_Rect::MoveTo(FLOAT x, FLOAT y)
{
	FLOAT cx;
	FLOAT cy;
	this->GetSize(cx, cy);
	this->min_p.x = x;
	this->min_p.y = y;
	this->SetSize(cx, cy);
}

inline void GV_Rect::SetSize(FLOAT x, FLOAT y)
{
	this->max_p.x = this->min_p.x + x;
	this->max_p.y = this->min_p.y + y;
}

inline BOOL GV_Rect::bInRect(const GV_Vector2& p)
{
	if (p.x > (this->max_p.x + GV_FLT_EPSILON) ||
		p.x < (this->min_p.x - GV_FLT_EPSILON) ||
		p.y > (this->max_p.y + GV_FLT_EPSILON) ||
		p.y < (this->min_p.y - GV_FLT_EPSILON))
	{
		return FALSE;
	}
	return TRUE;
}
