#pragma once
namespace gv
{
class gv_network_mgr_imp;
class gv_network_mgr : public gv_event_processor
{

	friend class gv_com_net_host;

public:
	GVM_DCL_CLASS(gv_network_mgr, gv_event_processor)
	gv_network_mgr();
	~gv_network_mgr();
	//-------------------------events---------------------
	virtual void register_events();
	virtual void unregister_events();
	virtual bool tick(gv_float dt);
	virtual bool do_synchronization();
	//--
	virtual gv_int connect_host(gv_com_net_host* remote);
	virtual gv_int disconnect_host(gv_com_net_host* remote);
	virtual gv_com_net_host* create_remote_host();
	virtual void send_packet(gv_com_net_host* remote, gv_packet*,
							 bool need_reliable);
	virtual gv_packet* create_packet(bool need_reliable = true);

protected:
	virtual gv_int on_event_add_host(class gv_object_event_net_add_host* pevent);
	virtual gv_int
	on_event_remove_host(class gv_object_event_net_remove_host* pevent);
	virtual gv_int
	on_event_add_net_replica(class gv_object_event_net_add_net_replica* pevent);
	virtual gv_int on_event_remove_net_replica(
		class gv_object_event_net_remove_net_replica* pevent);
	virtual void create_connection(gvt_ref_ptr< gv_session_event >& event);
	virtual void cancel_connection(gvt_ref_ptr< gv_session_event >& event);
	virtual void remove_connection(gvt_ref_ptr< gv_session_event >& event);
	gv_network_mgr_imp* get_impl();

protected:
	gvt_ref_ptr< gv_network_mgr_imp > m_pimpl;
	gvt_ref_ptr< gv_com_net_host > m_local_host;
	gvt_array< gvt_ref_ptr< gv_com_net_host > > m_remote_hosts;
	gvt_hash_map< gv_socket_address, gvt_ptr< gv_com_net_host >, 1024 > m_address_map;
};
}