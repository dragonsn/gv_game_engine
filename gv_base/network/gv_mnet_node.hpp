
#if 0	
		class gv_mnet_imp
		{
		public:
			class channel
			{
			public:
				gv_mnet_sid		  m_sid;			//session id 
				gv_socket_address m_source_address; //user id ; 
				gv_socket_address m_target_address; //user id ; 
				gve_mnet_state	  m_state;
				gv_atomic_count	  m_incoming_unreliable_sequence_number;
				gv_atomic_count	  m_incoming_reliable_sequence_number;
				gv_atomic_count	  m_outgoing_unreliable_sequence_number;
				gv_atomic_count	  m_outgoing_reliable_sequence_number;

				gvt_lock_free_list < gv_packet * > m_incoming_unreliabe;
				gvt_lock_free_list < gv_packet * > m_incoming_reliabe;
				gvt_lock_free_list < gv_packet * > m_incoming_unreliabe;

			};
		public:
			gv_mnet_init_info m_init_info;
			gvt_array< channel> m_channels;
		};

		gv_mnet_node::gv_mnet_node()
		{

		};

		gv_mnet_node::~gv_mnet_node()
		{
		};

		bool			gv_mnet_node::create(const gv_mnet_init_param &)
		{
			return true; 
		};

		gv_uint			gv_mnet_node::connect(const gv_socket_address & peer_address)
		{
			return 0; 
		};

		gv_uint         gv_mnet_node::send	(const gv_mnet_sid &, gv_packet * packet, bool reliable)
		{
		};
		gv_uint         gv_mnet_node::ping(const gv_mnet_sid &)
		{
		};
		gv_uint         gv_mnet_node::disconnect(const gv_mnet_sid &)
		{
		};
		gv_packet *		gv_mnet_node::recieve()
		{
		};
		gv_bool			gv_mnet_node::is_valid_sid(const gv_mnet_sid & sid)
		{
		};
		void			gv_mnet_node::set_encrypt_callback(encrypt_callback callback)
		{
		};
		void			gv_mnet_node::set_authentication_callback(authentication_callback callback)
		{
		};
#endif
