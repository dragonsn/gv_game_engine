inline char* gv_int_to_string(gv_int value, char* string, int radix = 10)
{
	sprintf(string, "%d", value);
	return string;
};
inline char* gv_uint_to_string(gv_uint value, char* string, int radix = 10)
{
	sprintf(string, "%u", value);
	return string;
};
inline char* gv_long_to_string(gv_long value, char* string, int radix = 10)
{
	sprintf(string, "%lld", value);
	return string;
}
inline char* gv_ulong_to_string(gv_ulong value, char* string, int radix = 10)
{
	sprintf(string, "%llu", value);
	return string;
}
inline char* gv_double_to_string(gv_double value, char* string,
								 int number_of_significant_digits = 20)
{
	sprintf(string, "%g", value);
	return string;
};

inline wchar_t* gv_int_to_string(gv_int value, wchar_t* string,
								 int radix = 10)
{
	swprintf(string, GV_ATOI_BUF_SIZE, L"%d", value);
	return string;
};
inline wchar_t* gv_uint_to_string(gv_uint value, wchar_t* string,
								  int radix = 10)
{
	swprintf(string, GV_ATOI_BUF_SIZE, L"%u", value);
	return string;
};
inline wchar_t* gv_long_to_string(gv_long value, wchar_t* string,
								  int radix = 10)
{
	swprintf(string, GV_ATOI_BUF_SIZE, L"%lld", value);
	return string;
};
inline wchar_t* gv_ulong_to_string(gv_ulong value, wchar_t* string,
								   int radix = 10)
{
	swprintf(string, GV_ATOI_BUF_SIZE, L"%llu", value);
	return string;
};
//====================================================================
inline gv_int gv_string_to_int(const char* s)
{
	gv_int value;
	sscanf(s, "%d", &value);
	return value;
};

inline gv_int gv_string_to_int(const wchar_t* s)
{
	gv_int value;
	swscanf(s, L"%d", &value);
	return value;
};

inline gv_uint gv_string_to_uint(const char* s)
{
	gv_uint value;
	sscanf(s, "%u", &value);
	return value;
};

inline gv_uint gv_string_to_uint(const wchar_t* s)
{
	gv_uint value;
	swscanf(s, L"%u", &value);
	return value;
};

inline gv_int gv_string_to_int(const char* s, int radix)
{
	return gv_string_to_int(s);
};

inline gv_uint gv_string_to_hex(const char* s)
{
	gv_uint value;
	sscanf(s, "%x", &value);
	return value;
};

inline gv_long gv_string_to_long(const char* s, int radix = 10)
{
	gv_long value;
	sscanf(s, "%lld", &value);
	return value;
}

inline gv_long gv_string_to_long(const wchar_t* s, int radix = 10)
{
	gv_long value;
	swscanf(s, L"%lld", &value);
	return value;
}

inline gv_ulong gv_string_to_ulong(const char* s, int radix = 10)
{
	gv_ulong value;
	sscanf(s, "%llu", &value);
	return value;
}
inline gv_ulong gv_string_to_ulong(const wchar_t* s, int radix = 10)
{
	gv_ulong value;
	swscanf(s, L"%llu", &value);
	return value;
}

inline gv_double gv_string_to_double(const char* s)
{
	gv_double value;
	sscanf(s, "%lf", &value);
	return value;
};

inline gv_double gv_string_to_double(const wchar_t* s)
{
	gv_double value;
	swscanf(s, L"%lf", &value);
	return value;
};

inline float gv_string_to_float(const char* s)
{
	float f;
	sscanf(s, "%f", &f);
	return f;
};

inline char* gv_float_to_string(gv_float f, char* buffer)
{
	sprintf(buffer, "%f", f);
	return buffer;
};
