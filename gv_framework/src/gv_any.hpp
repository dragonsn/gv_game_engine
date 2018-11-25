#pragma once
namespace gv
{
gv_any::gv_any()
{
	m_class = NULL;
	m_data = NULL;
};

gv_any::gv_any(gv_class_info* pcls)
{
	m_class = pcls;
	m_data = pcls->create_instance();
};

gv_any::~gv_any()
{
	clear();
};
void gv_any::clear()
{
	if (m_class && m_data)
	{
		this->m_class->delete_instance(m_data);
	}
	m_class = NULL;
	m_data = NULL;
}
gv_class_info* gv_any::get_class()
{
	return m_class;
};
void gv_any::set_class(gv_class_info* pcls)
{
	m_class = pcls;
};
bool gv_any::is_equal(const gv_any& a)
{
	if (!is_same_type(a))
		return false;
	if (!m_class)
		return false;
	return m_class->is_equal(m_data, a.m_data);
};
bool gv_any::is_less(const gv_any& a)
{
	if (!is_same_type(a))
		return false;
	if (!m_class)
		return false;
	return m_class->is_less(m_data, a.m_data);
}
bool gv_any::is_same_type(const gv_any& a)
{
	return m_class == a.m_class;
};
gv_byte* gv_any::get_data()
{
	return m_data;
};
}