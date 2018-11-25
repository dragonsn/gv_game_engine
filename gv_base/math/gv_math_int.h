#pragma once

inline unsigned int gv_next_power_of_two(unsigned int v)
{
	if (!((v & (v - 1)) == 0))
	{
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		return v + 1;
	}
	else
	{
		return v;
	}
}