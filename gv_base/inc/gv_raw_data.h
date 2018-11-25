#pragma once
namespace gv
{
class gv_raw_data
{
public:
	gv_raw_data(){};
	gv_raw_data(const gv_raw_data& data)
	{
		m_data = data.m_data;
	}
	gv_raw_data(void* p, gv_int size)
	{
		m_data.init((gv_byte*)p, size);
	};
	~gv_raw_data()
	{
	}
	inline gv_raw_data operator=(const gv_raw_data& data)
	{
		m_data = data.m_data;
		return *this;
	}
	inline bool operator==(const gv_raw_data& data) const
	{
		return m_data == data.m_data;
	}
	inline bool operator<(const gv_raw_data& data) const
	{
		return m_data < data.m_data;
	}
	inline bool operator!=(const gv_raw_data& a) const
	{
		return !(*this == a);
	}
	gvt_array< gv_byte > m_data;
};

template < class type_of_stream >
inline type_of_stream& operator<<(type_of_stream& s, const gv_raw_data& d)
{
	s << d.m_data.size();
	for (int i = 0; i < d.m_data.size(); i++)
	{
		s << d.m_data[i];
	}
	return s;
}

template < class type_of_stream >
inline const type_of_stream& operator>>(type_of_stream& s, gv_raw_data& d)
{
	gv_int size;
	s >> size;
	d.m_data.resize(size);
	for (int i = 0; i < d.m_data.size(); i++)
	{
		s >> d.m_data[i];
	}
	return s;
}
}