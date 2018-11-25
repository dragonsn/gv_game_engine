namespace gv
{
template < int size_of_block = 4096 >
class gvt_memory_stack : public gvp_memory_default
{
	const static gv_uint MAGIC_NUMBER_ALLOCATED = 0xBACECAFE;
	const static gv_uint MAGIC_NUMBER_FREED = 0xDEADDEAD;
	struct block
	{
		block()
		{
			used_size = 0;
		}
		char data[size_of_block];
		gv_int used_size;
	};
	struct memory_info
	{
		gv_int size;
		gv_int tag;
	};

	memory_info* get_mem_info(void* p)
	{
		return (memory_info*)p - 1;
	}

public:
	gvt_memory_stack()
	{
	}
	~gvt_memory_stack()
	{
	}
	inline void* alloc(size_t size)
	{
		size_t actural_size = size + sizeof(memory_info);
		GV_ASSERT(actural_size <= size_of_block);
		typename gvt_slist< block >::iterator it = m_list.begin();
		if (!it.get_node() || it->used_size + actural_size >= size_of_block)
		{
			m_list.add_dummy();
			it = m_list.begin();
		}
		memory_info* pinfo = (memory_info*)(it->data + it->used_size);
		pinfo->size = (gv_int)size;
		pinfo->tag = MAGIC_NUMBER_ALLOCATED;
		it->used_size += (gv_int)(actural_size);
		return pinfo + 1;
	};
	inline void free(void* p)
	{
		if (!p)
			return;
		memory_info* pinfo = get_mem_info(p);
		GV_ASSERT(pinfo->tag = MAGIC_NUMBER_ALLOCATED);
		pinfo->tag = MAGIC_NUMBER_FREED;
	};
	inline void* realloc(void* p, size_t size)
	{
		if (!p)
			return alloc(size);
		memory_info* pinfo = get_mem_info(p);
		if (size <= (size_t)pinfo->size)
		{
			return p;
		}
		void* pnew;
		pnew = alloc(size);
		memcpy(pnew, p, gvt_min(size, (size_t)pinfo->size));
		this->free(p);
		return pnew;
	};
	inline void free_all()
	{
		m_list.clear();
	}

protected:
	gvt_slist< block > m_list;
};
}