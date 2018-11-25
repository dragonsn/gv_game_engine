namespace gv
{
static const gv_byte gvc_invalid_replica_array_idx = 255;
static const gv_byte gvc_invalid_replica_var_idx = 255;
class gv_net_replica;
class gv_com_net_replica : public gv_component
{
	friend class gv_network_mgr;
	friend class gv_com_net_host;

public:
	GVM_DCL_CLASS(gv_com_net_replica, gv_component);
	gv_com_net_replica();
	~gv_com_net_replica();
	//============================================================================================
	//							:the component methods
	//============================================================================================
	gv_bool tick(gv_float dt);
	gv_bool set_resource(gv_resource*);
	gv_bool is_tickable();
	gv_bool sync_to_entity(gv_entity*);
	void on_attach();
	void on_detach();
	//============================================================================================
	void set_in_coming_host(gv_com_net_host*);
	void set_out_going_host(gv_com_net_host*);

	virtual void update_all_replica(gv_replica_update_result& result,
									gv_int max_var);
	virtual bool append_replica_array(gv_class_info* replica_cls,
									  gv_int max_array_index);
	gv_net_replica* get_replica_array(gv_ushort replica_id, gv_int& array_size);
	void remove_all_replica_array();
	gv_bool remove_replica_array(gv_class_info* replica_cls);

	template < class type_of_replica >
	type_of_replica* get_replica_array(gv_int& array_size)
	{
		return gvt_cast< type_of_replica >(
			get_replica_array(type_of_replica::static_id(), array_size));
	};

protected:
	gvt_array< gvt_ptr< gv_net_replica > > m_replica_arrays;
	gvt_array< gv_int > m_replica_array_sizes;
	gv_bool m_is_outgoing;
	gvt_ref_ptr< gv_com_net_host > m_bound_host;
};
}