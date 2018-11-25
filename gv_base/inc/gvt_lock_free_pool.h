#pragma once
#define GV_ENABLE_LOCKFREE_POOL 1

namespace gv
{
template < class T, int chunk_size = 256 >
class gvt_lock_free_pool
{
public:
	typedef gvt_lock_free_list< T, gvp_memory_null > type_of_free_list;
	typedef typename type_of_free_list::type_of_node type_of_node;
	struct chunk
	{
		type_of_node pad[chunk_size];
	};

public:
	gvt_lock_free_pool()
	{
	}
#if GV_ENABLE_LOCKFREE_POOL

	gvt_lock_free_pool(int nb_chunk_init)
	{
		for (int i = 0; i < nb_chunk_init; i++)
		{
			add_chunk();
		}
	}
	~gvt_lock_free_pool()
	{
		purge_memory();
	}
	T* allocate()
	{
		typename type_of_free_list::node* pnode = m_free_list.unlink_front();
		while (!pnode)
		{
			add_chunk();
			pnode = m_free_list.unlink_front();
		}
		m_used_count++;
		return &pnode->data;
	}
	void free(void* data)
	{
		m_used_count--;
		if (m_used_count.get() < 0)
		{
			GV_DEBUG_BREAK;
		}
		m_free_list.push((typename type_of_free_list::node*)data);
	};
	void purge_memory()
	{
		if (m_used_count.get() != 0)
		{
			gv_string_tmp s;
			s << " [WARNING!!!] still :  " << m_used_count.get()
			  << " in lock free pool not freed , when try to destroy the pool , "
				 "potential problems!!! \r\n";
			gv_debug_output(*s);
			// GV_DEBUG_BREAK;
		}
		m_free_list.unlink_all_reverse();
		m_chunk_list.clear();
		m_used_count.set(0);
	}

protected:
	void add_chunk()
	{
		chunk* chunk = m_chunk_list.push();
		for (int i = 0; i < chunk_size - 1; i++)
		{
			chunk->pad[i].next = &chunk->pad[i + 1];
		}
		chunk->pad[chunk_size - 1].next = NULL;
		m_free_list.push_list(chunk->pad, chunk->pad + chunk_size - 1);
	}

	gvt_lock_free_list< T, gvp_memory_null > m_free_list;
	gvt_lock_free_list< chunk, gv_lock_free > m_chunk_list;
	gv_atomic_count m_used_count;
#else

	gvt_lock_free_pool(int nb_chunk_init)
	{
	}
	~gvt_lock_free_pool()
	{
	}
	T* allocate()
	{
		return (T*)new char[sizeof(T)];
	}
	void free(void* data)
	{
		delete data;
	};
	void purge_memory()
	{
	}
#endif
};
}