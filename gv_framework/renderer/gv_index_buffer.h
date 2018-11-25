#pragma once
namespace gv
{
//============================================================================================
//								:
//============================================================================================
class gv_index_buffer : public gv_resource
{
public:
	GVM_DCL_CLASS_WITH_CONSTRUCT(gv_index_buffer, gv_resource);
	~gv_index_buffer(){};
	gv_int get_nb_index()
	{
		return m_raw_index_buffer.size();
	};
	gv_int get_index(int i)
	{
		return m_raw_index_buffer[i];
	};
	void set_nb_index(int nb)
	{
		m_raw_index_buffer.resize(nb);
	}
	void set_index(int i, gv_int idx)
	{
		m_raw_index_buffer[i] = idx;
	}

public:
	gvt_array< gv_int > m_raw_index_buffer;
};
};