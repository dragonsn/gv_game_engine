#pragma once

namespace gv
{
template < int max_size = 1024 >
class gvt_bool_array
{
public:
	static const int bytes_size = (max_size - 1) / 8 + 1;

	gvt_bool_array()
	{
		memset(bytes, 0, sizeof(bytes));
	};

	bool get(int index)
	{
		GV_ASSERT(index < max_size);
		int byte_index = index / 8;
		int shift_index = index % 8;
		gv_byte i = bytes[byte_index];
		gv_byte mask = 1 << shift_index;
		return (i & mask) != 0;
	}

	void set(int index, bool b)
	{
		GV_ASSERT(index < max_size);
		int byte_index = index / 8;
		int shift_index = index % 8;
		gv_byte& i = bytes[byte_index];
		gv_byte mask = ((1 << shift_index));
		if (b)
			i |= mask;
		else
			i &= ~mask;
	}

protected:
	gv_byte bytes[bytes_size];
};
}