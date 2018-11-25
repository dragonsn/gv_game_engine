#pragma once
namespace gv
{
#define GVM_REGISTER_NET_REPLICA_ID(name, num)             \
	static const gv_ushort gv_net_replica_id_##name = num; \
	static const gv_ushort GV_MACRO_CONCATE(gv_net_replica_, num) = num;

struct gv_net_replica_update_info;
typedef gvt_array_cached< gv_net_replica_update_info, 1024 >
	gv_replica_update_result;
//============================================================================================
// replica of game objects for networking ,updated by gv_com_replica, send to
// other host if changes.
//============================================================================================
class gv_net_replica
{
public:
	gv_net_replica();
	virtual ~gv_net_replica();

public:
	virtual gv_bool update_replica(gv_replica_update_result& result,
								   gv_int array_index);
	// current_index , name##_is_dirty,
	// name##_is_dirty?name##_is_dirty:(name==name#_sent),
	// result,current_priority, array_index
	gv_bool update_one_var(gv_int var_index, gv_byte& dirty_flag,
						   gv_byte cu_is_dirty, gv_replica_update_result& result,
						   gv_uint priority, gv_int array_index, gv_byte* src,
						   gv_byte* copy);
	bool on_send(gv_net_replica_update_info& info);
	bool recv_a_var(gv_packet& pkt, gv_byte idx);
	bool send_a_var(gv_packet& pkt, gv_var_info* pvar);
	gv_net_replica* get_replica_in_array(gv_int idx);
	inline gv_class_info* get_class()
	{
		return m_class;
	};
	inline gv_ushort get_id()
	{
		return m_replica_id;
	}
	inline void set_dirty(bool dirty)
	{
		m_is_dirty = dirty;
	}
	inline bool is_dirty()
	{
		return m_is_dirty;
	}
	inline void set_sq_no(gv_uint sq_no)
	{
		m_cu_pkt_sq = sq_no;
	}

protected:
	gv_uint m_cu_pkt_sq;
	gv_ushort m_replica_id;
	gv_bool m_is_dirty;
	gvt_ptr< gv_class_info > m_class;
};

template < class type_of_replica >
inline type_of_replica* gvt_cast(gv_net_replica* ptype)
{
	if (ptype && ptype->get_id() == type_of_replica::static_id())
		return (type_of_replica*)ptype;
	return NULL;
}

//============================================================================================
//								:struct used to prepare
//replica to send
//============================================================================================
static gv_int gvc_net_replic_max_level = 16;
struct gv_net_replica_update_info
{
	gv_net_replica_update_info(){};
	gv_ushort m_replica_id;
	gv_byte m_index_in_array;
	gv_int m_priority;
	gv_var_info* m_var;
	gv_net_replica* m_owner;

	gv_byte* m_pdirty;
	gv_byte* m_psrc;
	gv_byte* m_pcopy;

	inline bool operator==(const gv_net_replica_update_info& info)
	{
		return m_replica_id == info.m_replica_id &&
			   m_index_in_array == info.m_index_in_array &&
			   m_priority == info.m_priority && m_var == info.m_var &&
			   m_owner == info.m_owner;
	}
	inline bool operator<(const gv_net_replica_update_info& info)
	{
		if (m_priority < info.m_priority)
			return true;
		if (m_priority > info.m_priority)
			return false;
		if (m_replica_id < info.m_replica_id)
			return true;
		if (m_replica_id > info.m_replica_id)
			return false;
		if (m_var->get_index_in_cls() < info.m_var->get_index_in_cls())
			return true;
		if (m_var->get_index_in_cls() > info.m_var->get_index_in_cls())
			return false;
		return false;
	}
};
};