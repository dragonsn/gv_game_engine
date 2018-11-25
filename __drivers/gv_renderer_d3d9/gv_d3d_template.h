#pragma once

#define GVM_VERIFY_D3D(x) \
	if (FAILED(x))        \
		GV_ASSERT(0);
#define GVM_SAFE_RELEASE_D3D(x) \
	if (x)                      \
	{                           \
		x->Release();           \
		x = NULL;               \
	}
#define DEBUG_VS 0

namespace gv
{
#define GV_PROFILE_EVENT_PIX(name, index) \
	gv_profiler_marker_pix GV_MAKE_UNIQUE_ID(_marker_)(#name, L#name);

class gv_profiler_marker_pix : public gv_profiler_marker
{
public:
	gv_profiler_marker_pix(const char* name, LPCWSTR lname)
		: gv_profiler_marker(name, 0)
	{
		D3DPERF_BeginEvent(D3DCOLOR_XRGB(200, 100, 100), lname);
	};

	~gv_profiler_marker_pix()
	{
		D3DPERF_EndEvent();
	}
};
}
namespace gv
{
//============================================================================================
//								:
//============================================================================================
template < class type_of_data >
class gvt_ptr_d3d
{
public:
	gvt_ptr_d3d()
		: m_ptr(NULL)
	{
	}
	gvt_ptr_d3d(type_of_data* in_ptr)
	{
		m_ptr = in_ptr;
		if (in_ptr)
			m_ptr->Release();
	}
	gvt_ptr_d3d(const gvt_ptr_d3d& in_ptr)
	{
		m_ptr = in_ptr.m_ptr;
		if (m_ptr)
		{
			m_ptr->AddRef();
		}
	}
	~gvt_ptr_d3d()
	{
		if (m_ptr)
		{
			m_ptr->Release();
		}
	}

	inline gvt_ptr_d3d& operator=(type_of_data* in_ptr)
	{
		type_of_data* older_ptr = m_ptr;
		m_ptr = in_ptr;
		if (m_ptr)
		{
			m_ptr->AddRef();
		}
		if (older_ptr)
		{
			older_ptr->Release();
		}
		return *this;
	}

	inline gvt_ptr_d3d& operator=(const gvt_ptr_d3d& p)
	{
		return * this = p.m_ptr;
	}

	inline bool operator==(const gvt_ptr_d3d& Other) const
	{
		return m_ptr == Other.m_ptr;
	}

	inline type_of_data* operator->() const
	{
		return m_ptr;
	}

	inline operator type_of_data*() const
	{
		return m_ptr;
	}

	inline type_of_data*& ptr()
	{
		return m_ptr;
	}

private:
	type_of_data* m_ptr;
};
//============================================================================================
//								:
//============================================================================================
template < class type_of_vertex, gv_uint fvf_vertex >
class gvt_dynamic_vb_d3d
{
public:
	gvt_dynamic_vb_d3d()
	{
		m_current_vb = 0;
		m_max_vertex_nb = 0;
		m_vertex_nb[0] = 0;
		m_vertex_nb[1] = 0;
	}
	~gvt_dynamic_vb_d3d()
	{
		on_device_lost();
	}
	gv_uint get_fvf()
	{
		return fvf_vertex;
	}
	void init_vb(gv_int max_vertex)
	{
		for (int buf = 0; buf < 2; buf++)
		{
			GVM_VERIFY_D3D((get_device_d3d9()->CreateVertexBuffer(
				(max_vertex + 1) * sizeof(type_of_vertex), D3DUSAGE_WRITEONLY,
				fvf_vertex, D3DPOOL_DEFAULT, &this->m_vb[buf].ptr(), NULL)))
		}
		m_max_vertex_nb = max_vertex;
	};
	void on_device_lost()
	{
		m_current_vb = 0;
		m_vertex_nb[0] = 0;
		m_vertex_nb[1] = 0;
		m_vb[0] = m_vb[1] = NULL;
		m_cu_vertex = NULL;
	}
	void on_device_reset()
	{
		init_vb(m_max_vertex_nb);
	}
	void add_vertex(const type_of_vertex& v)
	{
		if (cu_vb_size() < m_max_vertex_nb)
		{
			*m_cu_vertex = v;
			m_cu_vertex = m_cu_vertex + 1;
			cu_vb_size()++;
		}
	};
	void swap_buffer()
	{
		m_current_vb = !m_current_vb;
		cu_vb_size() = 0;
	};
	gv_int& cu_vb_size()
	{
		return m_vertex_nb[m_current_vb];
	};
	gv_int& last_vb_size()
	{
		return m_vertex_nb[!m_current_vb];
	};
	IDirect3DVertexBuffer9* cu_vb()
	{
		return m_vb[m_current_vb];
	}
	IDirect3DVertexBuffer9* last_vb()
	{
		return m_vb[!m_current_vb];
	}
	void set_stream(gv_uint offset_in_bytes = 0)
	{
		get_device_d3d9()->SetStreamSource(0, last_vb(), offset_in_bytes,
										   sizeof(type_of_vertex));
		get_device_d3d9()->SetFVF(get_fvf());
	}

	void start_vb()
	{
		VOID* pVertices;
		if (cu_vb())
		{
			GVM_VERIFY_D3D(cu_vb()->Lock(0, m_max_vertex_nb * sizeof(type_of_vertex),
										 (void**)&pVertices, 0));
			/// GVM_DEBUG_CONSOLE_OUT(main,"lock "<< cu_vb());
			m_cu_vertex = (type_of_vertex*)pVertices;
		}
	}
	void end_vb()
	{
		if (cu_vb())
		{
			GVM_VERIFY_D3D(cu_vb()->Unlock());
			// GVM_DEBUG_CONSOLE_OUT(main,"unlock "<< cu_vb());
		}
	}
	gv_uint get_offet()
	{
		return cu_vb_size() * sizeof(type_of_vertex);
	}

protected:
	gvt_ptr_d3d< IDirect3DVertexBuffer9 > m_vb[2];
	gv_int m_vertex_nb[2];
	gv_int m_current_vb;
	gv_int m_max_vertex_nb;
	gvt_ptr< type_of_vertex > m_cu_vertex;
};
//============================================================================================
//								:
//============================================================================================
template < class type_of_vertex, gv_uint fvf_vertex >
class gvt_dynamic_vb_d3d_mt
	: public gvt_dynamic_vb_d3d< type_of_vertex, fvf_vertex >
{
public:
	typedef gvt_dynamic_vb_d3d< type_of_vertex, fvf_vertex > super;
	gvt_dynamic_vb_d3d_mt()
	{
	}
	~gvt_dynamic_vb_d3d_mt()
	{
	}
	void init_vb(gv_int max_vertex)
	{
		super::init_vb(max_vertex);
		m_vb_mem[0].reserve(max_vertex);
		m_vb_mem[1].reserve(max_vertex);
	}
	void fill_vb()
	{
		if (cu_vb_mem().size() && cu_vb())
		{
			start_vb();
			for (int i = 0; i < cu_vb_mem().size(); i++)
			{
				super::add_vertex(cu_vb_mem()[i]);
			}
			end_vb();
		}
	}
	void add_vertex(const type_of_vertex& v)
	{
		if (cu_vb_mem().size() < m_max_vertex_nb)
		{
			cu_vb_mem().push_back(v);
		}
	}
	gv_int cu_vb_size()
	{
		return cu_vb_mem().size();
	};
	void swap_buffer()
	{
		fill_vb();
		super::swap_buffer();
		cu_vb_mem().clear_and_reserve();
	};

protected:
	gvt_array< type_of_vertex >& cu_vb_mem()
	{
		return m_vb_mem[m_current_vb];
	}
	gvt_array< type_of_vertex >& last_vb_mem()
	{
		return m_vb_mem[!m_current_vb];
	}

protected:
	gvt_array< type_of_vertex > m_vb_mem[2];
};
//============================================================================================
//								:
//============================================================================================
template < class type_of_index >
class gvt_dynamic_ib_d3d
{
public:
	gvt_dynamic_ib_d3d()
	{
		m_current_ib = 0;
		m_max_index_nb = 0;
		m_index_nb[0] = 0;
		m_index_nb[1] = 0;
	}
	~gvt_dynamic_ib_d3d()
	{
		on_device_lost();
	}
	void init_ib(gv_int max_index)
	{
		for (int buf = 0; buf < 2; buf++)
		{
			GVM_VERIFY_D3D(get_device_d3d9()->CreateIndexBuffer(
				(max_index + 1) * sizeof(type_of_index), D3DUSAGE_WRITEONLY,
				D3DPOOL_DEFAULT, &this->m_ib[buf], NULL););
		}
		m_max_index_nb = max_index;
		start_ib();
	};
	void on_device_lost()
	{
		m_current_ib = 0;
		m_index_nb[0] = 0;
		m_index_nb[1] = 0;
		m_ib[0] = m_ib[1] = NULL;
	}
	void on_device_reset()
	{
		init_ib(m_max_index_nb);
	}
	void add_index(const type_of_index& v)
	{
		if (cu_ib_size() < m_max_index_nb)
		{
			*m_cu_index = v;
			m_cu_index = m_cu_index + 1;
			cu_ib_size()++;
		}
	};
	void swap_buffer()
	{
		m_current_ib = !m_current_ib;
	};
	gv_int& cu_ib_size()
	{
		return m_index_nb[m_current_ib];
	};
	gv_int& last_ib_size()
	{
		return m_index_nb[!m_current_ib];
	};
	IDirect3DIndexBuffer9* cu_ib()
	{
		return m_ib[m_current_ib];
	}
	IDirect3DIndexBuffer9* last_ib()
	{
		return m_ib[!m_current_ib];
	}
	void start_ib()
	{
		VOID* p = NULL;
		if (cu_ib())
		{
			GVM_VERIFY_D3D(
				cu_ib()->Lock(0, last_ib_triangle().size() * sizeof(type_of_index),
							  (void**)&p, 0));
			m_cu_index = (type_of_index*)p;
			cu_ib_size() = 0;
		}
	}
	void end_ib()
	{
		if (cu_ib())
			cu_ib()->Unlock();
	}

protected:
	gvt_ptr_d3d< IDirect3DIndexBuffer9 > m_ib[2];
	gv_int m_index_nb[2];
	gv_int m_current_ib;
	gv_int m_max_index_nb;
	gvt_ptr< type_of_index > m_cu_index;
};

//============================================================================================
//								:
//============================================================================================
template < class render_state, class dynamic_vb, class draw_policy >
class gvt_dynamic_renderer_d3d : public dynamic_vb
{
public:
	typedef dynamic_vb super;
	struct render_state_block
	{
		render_state_block()
		{
			vb_offset = vertex_start = vertex_end = 0;
		}
		render_state_block(const render_state_block& b)
		{
			*this = b;
		}
		render_state_block& operator=(const render_state_block& b)
		{
			state = b.state;
			vb_offset = b.vb_offset;
			vertex_start = b.vertex_start;
			vertex_end = b.vertex_end;
			return *this;
		}
		render_state state;
		gv_uint vb_offset;
		gv_int vertex_start;
		gv_int vertex_end;
	};
	gvt_dynamic_renderer_d3d()
	{
		m_states[0].reserve(100);
		m_states[1].reserve(100);
	}
	~gvt_dynamic_renderer_d3d()
	{
	}
	void swap_buffer()
	{
		super::swap_buffer();
		cu_states().clear_and_reserve();
	};
	void draw()
	{
		super::set_stream();
		for (int i = 0; i < last_states().size(); i++)
		{
			render_state_block& block = last_states()[i];
			if (i == last_states().size() - 1)
			{
				block.vertex_end = last_vb_size() - 1;
			}
			if (block.vertex_end <= block.vertex_start)
				continue;
			block.state.set();
			draw_policy d;
			d.draw(block.vertex_start, block.vertex_end);
		}
	}
	void begin_new_state(const render_state& state)
	{
		if (!cu_states().size() || !(cu_states().last()->state == state))
		{
			if (cu_states().size())
			{
				cu_states().last()->vertex_end = cu_vb_size() - 1;
			}
			render_state_block block;
			block.state = state;
			block.vb_offset = get_offet();
			block.vertex_start = cu_vb_size();
			cu_states().push_back(block);
		}
	}
	gvt_array< render_state_block >& cu_states()
	{
		return m_states[m_current_vb];
	};
	gvt_array< render_state_block >& last_states()
	{
		return m_states[!m_current_vb];
	};

protected:
	gvt_array< render_state_block > m_states[2];
};
//============================================================================================
//								:
//============================================================================================
}