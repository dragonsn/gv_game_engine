inline char* gv_int_to_string(gv_int value, char* string, int radix = 10)
{
	return itoa(value, string, radix);
};
inline char* gv_uint_to_string(gv_uint value, char* string, int radix = 10)
{
	return _ui64toa((gv_ulong)value, string, radix);
};
inline char* gv_long_to_string(gv_long value, char* string, int radix = 10)
{
	return _i64toa(value, string, radix);
}
inline char* gv_ulong_to_string(gv_ulong value, char* string, int radix = 10)
{
	return _ui64toa(value, string, radix);
}
inline char* gv_double_to_string(double value, char* buffer,
								 int number_of_significant_digits = 20)
{
	sprintf(buffer, "%10.10g", value);
	return buffer;
};
inline wchar_t* gv_int_to_string(gv_int value, wchar_t* string,
								 int radix = 10)
{
	return _itow(value, string, radix);
};
inline wchar_t* gv_uint_to_string(gv_int value, wchar_t* string,
								  int radix = 10)
{
	return _itow(value, string, radix);
};
inline wchar_t* gv_long_to_string(gv_long value, wchar_t* string,
								  int radix = 10)
{
	return _i64tow(value, string, radix);
};
inline wchar_t* gv_ulong_to_string(gv_ulong value, wchar_t* string,
								   int radix = 10)
{
	return _ui64tow(value, string, radix);
};
inline gv_int gv_string_to_int(const char* s)
{
	return ::atoi(s);
};
inline gv_int gv_string_to_int(const wchar_t* s)
{
	return ::_wtoi(s);
};
inline gv_int gv_string_to_int(const char* s, int radix)
{
	return (gv_int)_strtoi64(s, NULL, radix);
};
inline gv_uint gv_string_to_uint(const char* s, int radix = 10)
{
	return (gv_uint)_strtoui64(s, NULL, radix);
};
inline gv_uint gv_string_to_hex(const char* s)
{
	return (gv_uint)_strtoui64(s, NULL, 16);
};
inline gv_long gv_string_to_long(const char* string, int radix = 10)
{
	return _strtoi64(string, NULL, radix);
}
inline gv_ulong gv_string_to_ulong(const char* string, int radix = 10)
{
	return _strtoui64(string, NULL, radix);
}
inline double gv_string_to_double(const char* s)
{
	return ::atof(s);
};

inline float gv_string_to_float(const char* s)
{
	float f;
	sscanf(s, "%f", &f);
	return f;
};

inline char* gv_float_to_string(gv_float f, char* buffer)
{
	sprintf(buffer, "%10.10g", f);
	return buffer;
};
