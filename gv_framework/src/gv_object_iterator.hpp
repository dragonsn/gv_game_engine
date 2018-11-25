namespace gv
{

gv_object_iterator::gv_object_iterator()
{
	m_sandbox = NULL;
}
gv_object_iterator::gv_object_iterator(gv_sandbox* sandbox)
{
	m_sandbox = sandbox;
	for (gv_int i = 0; i < gvc_max_object_hash; i++)
	{
		gv_object* pobj = m_sandbox->m_pimpl->m_object_hash_table[i];
		m_i_hash_value = i;
		if (pobj)
		{
			m_p_object = pobj;
			return;
		}
	}
	m_p_object = NULL;
}
gv_object_iterator& gv_object_iterator::operator++()
{
	if (!m_p_object)
		return *this;
	gv_object* pobj = m_p_object->m_next;
	if (pobj)
	{
		m_p_object = pobj;
		return *this;
	}
	for (gv_int i = m_i_hash_value + 1; i < gvc_max_object_hash; i++)
	{
		pobj = m_sandbox->m_pimpl->m_object_hash_table[i];
		if (pobj)
		{
			m_i_hash_value = i;
			m_p_object = pobj;
			return (*this);
		}
	}
	m_p_object = NULL;
	return *this;
}
//
}