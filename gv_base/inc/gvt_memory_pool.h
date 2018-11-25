#pragma once

namespace gv
{
const unsigned int const_64k = 65536;
const unsigned int const_64k_mask = ~(const_64k - 1);
const unsigned int const_64k_shift = 16;

class memory_system_page_64K
{
public:
	void* alloc(size_t size)
	{
		GV_ASSERT(size == const_64k);
		void* pret = gv_alloc_page64k();
		;
		return pret;
	}
	void free(void* p)
	{
		gv_free_page64k(p);
	};
	void* realloc(void* p, size_t size)
	{
		GV_ASSERT(0);
		return NULL;
	};
};

class gv_fixed_memory_base
{
public:
	friend class gvt_memory_pool_manager;
	gv_fixed_memory_base()
	{
		if (!pages_owned)
			pages_owned = new ((void*)pages_owned_shadow) gvt_bool_array< const_64k >;
	};
	virtual ~gv_fixed_memory_base(){};

public:
	virtual void* alloc(size_t size) = 0;
	virtual void free(void* p) = 0;
	virtual void* realloc(void* p, size_t size) = 0;
	virtual int get_fixed_size() = 0;
	static bool own(void* p)
	{
		return get_head(p) != 0;
	};

protected:
	struct memory_page_head
	{
		int nb_alloc;
		int nb_freed;
		int total_alloced;
		gv_fixed_memory_base* owner;
		char* begin()
		{
			return (char*)this;
		}
		char* end()
		{
			return begin() + const_64k;
		}
		bool own(void* p)
		{
			return p >= begin() && p < end();
		}
	};
	static gvt_bool_array< const_64k >* pages_owned;
	static int fa_nb_page;
	static char pages_owned_shadow[sizeof(gvt_bool_array< const_64k >)];
	static memory_page_head* get_head(void* p)
	{
		unsigned int page_head = ((size_t)p) & const_64k_mask;
		unsigned int index = page_head;
		index >>= const_64k_shift;
		if ((*pages_owned).get(index))
		{
			return (memory_page_head*)page_head;
		}
		return NULL;
	}
	static int add_page(memory_page_head* phead)
	{
		gv_int_ptr index = (gv_int_ptr)phead;
		index >>= const_64k_shift;
		GV_ASSERT(!(*pages_owned).get(index));
		(*pages_owned).set(index, true);
		++fa_nb_page;
		return index;
	};

	static int remove_page(memory_page_head* phead)
	{
		unsigned int index = (size_t)phead;
		index >>= const_64k_shift;
		GV_ASSERT((*pages_owned).get(index));
		(*pages_owned).set(index, false);
		--fa_nb_page;
		return index;
	}
};

template < int node_size >
class gvt_memory_pool : public gv_fixed_memory_base
{
public:
	static const unsigned int block_size = const_64k;
	static const unsigned int page_mask = ~(block_size - 1);
	typedef memory_system_page_64K policy_memory_block;
	typedef gvt_memory_pool< node_size > type_of_pool;

	struct memory_pool_node
	{
		memory_pool_node()
		{
		}
		unsigned char pad[node_size - 4];
	};
	typedef gvt_slist_no_alloc< memory_pool_node > type_of_node_list;
	typedef typename type_of_node_list::type_of_node type_of_node;

	static const int node_num_in_block = (block_size - sizeof(memory_page_head) -
										  4 /*link node*/ - 2 /*min pad size*/) /
										 node_size;
	static const int byte_pad_size =
		block_size - sizeof(memory_page_head) - node_num_in_block * node_size - 4;
	static const int nb_min_free = node_num_in_block * 1;

	struct memory_pool_block
	{
		memory_page_head head;
		type_of_node node_pool[node_num_in_block];
		unsigned char pad[byte_pad_size];

		memory_pool_block()
		{
			head.nb_freed = node_num_in_block;
			head.nb_alloc = 0;
			head.total_alloced = node_num_in_block;
			gv_fixed_memory_base::add_page(&head);
		}
		~memory_pool_block()
		{
			GV_ASSERT(head.nb_freed == node_num_in_block);
			gv_fixed_memory_base::remove_page(&head);
		};
	};
	typedef gvt_slist< memory_pool_block, policy_memory_block > type_of_block_list;

public:
	inline gvt_memory_pool()
	{
		// no static check,just add here ,should use boost_static_assert
		GV_ASSERT(sizeof(memory_pool_block) == const_64k - 4);
	}
	~gvt_memory_pool()
	{
		free_list.clear();
	}
	void* alloc(size_t size)
	{
		GV_ASSERT(size <= node_size);
		if (free_list.begin() == free_list.end())
		{
			memory_pool_block* pb = &(*block_list.add_dummy());
			GV_ASSERT((((size_t)pb) & 0xffff) == 0);
			free_list.add(pb->node_pool, gvt_array_length(pb->node_pool));
			pb->head.owner = this;
		}
		typename type_of_node_list::iterator it = free_list.begin();
		free_list.erase(it.get_node(), NULL);
		void* preturn = it.get_node();
		memory_page_head* phead = get_head(preturn);
		GV_ASSERT(phead);
		phead->nb_freed--;
		phead->nb_alloc++;
		return it.get_node();
	};
	void free(void* p)
	{
		free_list.add((typename type_of_node_list::type_of_node*)p);
		memory_page_head* phead = get_head(p);
		GV_ASSERT(phead);
		phead->nb_freed++;
		if (phead->nb_freed == phead->total_alloced &&
			phead->nb_alloc > nb_min_free)
		// free the block
		// better solution ?
		{
			typename type_of_node_list::iterator it = free_list.begin();
			typename type_of_node_list::iterator it_pre;
			while (it != free_list.end())
			{
				typename type_of_node_list::iterator it_next = it;
				it_next++;
				if (phead->own(it.get_node()))
				{
					free_list.erase(it, it_pre);
				}
				else
					it_pre = it;
				it = it_next;
			}
			block_list.erase((typename type_of_block_list::type_of_node*)phead);
		}
	};
	void* realloc(void* p, size_t size)
	{
		GV_ASSERT(size <= node_size);
		if (!p)
			return alloc(size);
		return p;
	};
	int get_fixed_size()
	{
		return node_size;
	};

protected:
	type_of_node_list free_list;
	type_of_block_list block_list;

private:
	gvt_memory_pool(const type_of_pool&)
	{
	}
	gvt_memory_pool& operator=(const gvt_memory_pool&){};
};

class gvt_memory_pool_manager
{
public:
	static const int nb_block_size = 256;
	static const int nb_fa = 255;
	gvt_memory_pool_manager()
	{
		GV_ASSERT(sizeof(void*) ==
				  4); // this memory pool only can be used in 32 bit system!!!
		cu_fa = 0;
		for (int i = 0; i < nb_block_size + 1; i++)
			used_fa_table[i] = nb_fa;
		memset(fas, 0, sizeof(fas));
		register_allocator();
	}
	~gvt_memory_pool_manager()
	{
		for (int i = 0; i < cu_fa; i++)
		{
			delete fas[i];
		}
	}
	template < int block_size >
	void use_fa(int s_min, int s_max)
	{
		GV_ASSERT(cu_fa < nb_fa);
		GV_ASSERT(s_min <= s_max);
		GV_ASSERT(s_max <= block_size);
		GV_ASSERT(s_min < nb_block_size);
		GV_ASSERT(s_max <= nb_block_size);
		fas[cu_fa] = new gvt_memory_pool< block_size >;
		for (int i = s_min; i <= s_max; i++)
		{
			used_fa_table[i] = cu_fa;
		}
		++cu_fa;
	}

	void register_allocator()
	{
		use_fa< 8 >(1, 8);
		use_fa< 16 >(9, 16);
		use_fa< 24 >(17, 24);
		use_fa< 32 >(25, 32);
	}

	void* try_alloc(size_t size)
	{
		if (size < nb_block_size && used_fa_table[size] != nb_fa)
		{
			gv_fixed_memory_base* pa = fas[used_fa_table[size]];
			GV_ASSERT(pa);
			return pa->alloc(size);
		}
		return 0;
	}

	bool try_free(void* p)
	{
		gv_fixed_memory_base::memory_page_head* phead =
			gv_fixed_memory_base::get_head(p);
		if (phead)
		{
			phead->owner->free(p);
			return true;
		}
		return false;
	}

	bool try_realloc(void* p_old, void*& p_new, size_t size, size_t& old_size)
	{
		p_new = NULL;
		gv_fixed_memory_base::memory_page_head* phead =
			gv_fixed_memory_base::get_head(p_old);
		old_size = 0;
		if (phead)
		{
			old_size = phead->owner->get_fixed_size();
		}
		if (size < nb_block_size && used_fa_table[size] != nb_fa && phead)
		{
			gv_fixed_memory_base* pa = fas[used_fa_table[size]];
			if (pa != phead->owner)
			{
				p_new = pa->alloc(size);
				memcpy(p_new, p_old, gvt_min(size, old_size));
				phead->owner->free(p_old);
			}
			else
			{
				p_new = pa->realloc(p_old, size);
			}
			return true;
		}
		return false;
	}

private:
	int cu_fa;
	gv_fixed_memory_base* fas[nb_fa];
	int used_fa_table[nb_block_size + 1];
};

} // namespace gv