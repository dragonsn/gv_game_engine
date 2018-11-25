#pragma once

#ifndef GV_INT_h
#define GV_INT_h

namespace GV_Int32
{

inline unsigned int Ones(unsigned int x)
{
	/* 32-bit recursive reduction using SWAR...
  but first step is mapping 2-bit values
  into sum of 2 1-bit values in sneaky way
  */
	x -= ((x >> 1) & 0x55555555);
	x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
	x = (((x >> 4) + x) & 0x0f0f0f0f);
	x += (x >> 8);
	x += (x >> 16);
	return (x & 0x0000003f);
}

inline unsigned int Log2(unsigned int x)
{
	int y = (x & (x - 1));

	y |= -y;
	y >>= (32 - 1);
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return (TOnes32(x >> 1) - y);
}
};

#endif