namespace gv
{

gv_com_net_replica::gv_com_net_replica()
{
	GVM_SET_CLASS(gv_com_net_replica);
};
gv_com_net_replica::~gv_com_net_replica()
{
	this->remove_all_replica_array();
};
//============================================================================================
//							:the component methods
//============================================================================================
gv_bool gv_com_net_replica::tick(gv_float dt)
{
	return true;
};
gv_bool gv_com_net_replica::set_resource(gv_resource*)
{
	return true;
};
gv_bool gv_com_net_replica::is_tickable()
{
	return false;
};
gv_bool gv_com_net_replica::sync_to_entity(gv_entity*)
{
	return true;
};
void gv_com_net_replica::on_attach()
{
	GVM_POST_EVENT(net_add_net_replica, net, pe->replica = this);
};
void gv_com_net_replica::on_detach()
{
	GVM_POST_EVENT(net_remove_net_replica, net, pe->replica = this);
};
//============================================================================================
void gv_com_net_replica::set_in_coming_host(gv_com_net_host* host)
{
	GV_ASSERT(host->is_remote());
	m_is_outgoing = false;
	m_bound_host = host;
};

void gv_com_net_replica::set_out_going_host(gv_com_net_host* host)
{
	GV_ASSERT(host->is_remote());
	m_is_outgoing = true;
	m_bound_host = host;
};

void gv_com_net_replica::update_all_replica(gv_replica_update_result& result,
											gv_int max_var)
{
	for (int i = 0; i < this->m_replica_arrays.size(); i++)
	{
		gv_int array_size = this->m_replica_array_sizes[i];
		gv_net_replica* array_ptr = this->m_replica_arrays[i];
		gv_byte* array_byte_ptr = (gv_byte*)array_ptr;
		if (!array_ptr)
			continue;
		gv_class_info* pcls = array_ptr->get_class();
		for (int j = 0; j < array_size; j++)
		{
			array_ptr =
				(gv_net_replica*)(array_byte_ptr + j * pcls->get_aligned_size());
			array_ptr->update_replica(result, j);
		}
	}
	return;
};

gv_bool gv_com_net_replica::append_replica_array(gv_class_info* replica_cls,
												 gv_int max_array_index)
{
	if (max_array_index >= gvc_invalid_replica_array_idx)
		return false;
	for (int i = 0; i < this->m_replica_arrays.size(); i++)
	{
		gv_net_replica* array_ptr = this->m_replica_arrays[i];
		if (array_ptr->get_class() == replica_cls)
			return false;
	}
	gv_byte* parray_byte = replica_cls->create_array(max_array_index);
	this->m_replica_arrays.add((gv_net_replica*)parray_byte);
	this->m_replica_array_sizes.add(max_array_index);
	return true;
};

gv_bool gv_com_net_replica::remove_replica_array(gv_class_info* replica_cls)
{
	for (int i = 0; i < this->m_replica_arrays.size(); i++)
	{
		gv_net_replica* array_ptr = this->m_replica_arrays[i];
		if (array_ptr->get_class() == replica_cls)
		{
			gv_byte* parray_byte = (gv_byte*)array_ptr;
			array_ptr->get_class()->delete_array(parray_byte,
												 this->m_replica_array_sizes[i]);
			this->m_replica_array_sizes.erase(i);
			this->m_replica_arrays.erase(i);
			return true;
		}
	}
	return false;
};

void gv_com_net_replica::remove_all_replica_array()
{
	for (int i = 0; i < this->m_replica_arrays.size(); i++)
	{
		gv_net_replica* array_ptr = this->m_replica_arrays[i];
		gv_byte* parray_byte = (gv_byte*)array_ptr;
		array_ptr->get_class()->delete_array(parray_byte,
											 this->m_replica_array_sizes[i]);
	}
	this->m_replica_array_sizes.clear();
	this->m_replica_arrays.clear();
};

gv_net_replica* gv_com_net_replica::get_replica_array(gv_ushort replica_id,
													  gv_int& array_size)
{
	for (int i = 0; i < this->m_replica_arrays.size(); i++)
	{
		gv_net_replica* array_ptr = this->m_replica_arrays[i];
		if (array_ptr->get_id() == replica_id)
		{
			array_size = m_replica_array_sizes[i];
			return array_ptr;
		}
	}
	return NULL;
};

GVM_IMP_CLASS(gv_com_net_replica, gv_component);
GVM_VAR(gv_bool, m_is_outgoing)
GVM_VAR(gvt_ref_ptr< gv_com_net_host >, m_bound_host)
GVM_END_CLASS
};

namespace gv
{
#define GVM_NET_REPLICA_IMP_STRUCT
#define GVM_NET_REPLICA_IMP_REPLICA
#define GV_NET_REPLICA_INC_FILE "..\network\gv_net_replica_basic.h"
#include "gv_net_replica_maker.h"

#define GVM_REMOTE_CALL_IMP_STRUCT
#define GV_REMOTE_CALL_INC_FILE "..\network\gv_remote_call_basic.h"
#include "gv_remote_call_maker.h"
}