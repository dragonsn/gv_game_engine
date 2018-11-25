#define DEBUG_REPLICA 1 && GV_DEBUG_VERSION
namespace gv
{
gv_net_replica::gv_net_replica()
{
	m_replica_id = gvc_max_replica_struct_id;
	m_is_dirty = false;
};
gv_net_replica::~gv_net_replica(){

};
gv_bool gv_net_replica::update_replica(gv_replica_update_result& result,
									   gv_int array_index)
{
	GV_ASSERT(0 && "should call overridden function!!");
	return true;
};
gv_bool gv_net_replica::update_one_var(gv_int var_idx, gv_byte& is_dirty,
									   gv_byte cu_is_dirty,
									   gv_replica_update_result& result,
									   gv_uint priority, gv_int array_index,
									   gv_byte* src, gv_byte* copy)
{
#pragma GV_REMINDER( \
	"[OPT] use macro to optimize the function , reduce virtual function calls")
	GV_ASSERT(array_index < 255);
	GV_ASSERT(var_idx < 255);
	gv_byte* pb_this = (gv_byte*)this;
	if (is_dirty && is_dirty < 255)
	{
		is_dirty++;
	}
	else
	{
		if (cu_is_dirty)
		{
			is_dirty = 1;
		}
	}
	if (is_dirty && is_dirty > priority)
	{
		gv_net_replica_update_info& info = *result.add_dummy();
		gv_var_info* pvar = m_class->get_var(var_idx);
		GV_ASSERT(pvar->is_need_replicate());
		info.m_index_in_array = (gv_byte)array_index;
		info.m_owner = this;
		info.m_priority = priority ? priority / (gv_int)is_dirty + 1 : priority;
		info.m_replica_id = this->m_replica_id;
		info.m_var = pvar;
		info.m_pdirty = &is_dirty;
		info.m_psrc = src;
		info.m_pcopy = copy;
	}
	return true;
};

bool gv_net_replica::on_send(gv_net_replica_update_info& info)
{
	info.m_var->get_type()->copy_to(info.m_psrc, info.m_pcopy);
	gv_byte& is_dirty = *info.m_pdirty;
	is_dirty = 0;
	return true;
};

bool gv_net_replica::recv_a_var(gv_packet& pkt, gv_byte idx)
{
	gv_var_info* pvar = this->m_class->get_var((gv_int)idx);
	if (!pvar)
		return false;
	gv_byte* pb_this = gvt_byte_ptr(*this);
	gv_byte* the_var = gvt_byte_ptr(*this) + pvar->get_offset();
	pvar->get_type()->read_data(the_var, &pkt);

	gv_var_info* pvar_sq_no = m_class->get_var(idx + 3);
	GV_ASSERT(pvar_sq_no->get_type()->get_type() == gve_data_type_uint);
	gv_uint& sq_no =
		gvt_from_byte_ptr< gv_uint >(pb_this + pvar_sq_no->get_offset());
	if (sq_no > m_cu_pkt_sq)
	{ // already synced , but udp break the order.
		return true;
	}
	sq_no = m_cu_pkt_sq;
#if DEBUG_REPLICA
	if (!pkt.is_size_counter())
	{
		gv_string_tmp s;
		pvar->get_type()->export_to_xml(the_var, s);
		GVM_DEBUG_OUT( "[replica]recv var " << pvar->get_location()
														<< " : " << s << gv_endl);
	}
#endif
	if (!pkt.eof())
		return true;
	return false;
};

bool gv_net_replica::send_a_var(gv_packet& pkt, gv_var_info* pvar)
{
	GV_ASSERT(pvar->get_index_in_cls() < 255);
	gv_byte* the_var = gvt_byte_ptr(*this) + pvar->get_offset();
	pvar->get_type()->write_data(the_var, &pkt);
#if DEBUG_REPLICA
	if (!pkt.is_size_counter())
	{
		gv_string_tmp s;
		pvar->get_type()->export_to_xml(the_var, s);
		GVM_DEBUG_OUT( "[replica]send var " << pvar->get_location()
														<< " : " << s << gv_endl);
	}
#endif
	if (!pkt.eof())
		return true;
	return false;
};

gv_net_replica* gv_net_replica::get_replica_in_array(gv_int idx)
{
	gv_byte* pb = ((gv_byte*)this) + m_class->get_aligned_size() * idx;
	return (gv_net_replica*)pb;
};
}