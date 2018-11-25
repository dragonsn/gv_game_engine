namespace gv
{

struct stats_category_info
{
	stats_category_info()
	{
		GVM_ZERO_ME;
	}
	stats_category_info(gv_color c)
	{
		GVM_ZERO_ME;
		color = c;
	}
	gv_color color;
	bool hided;
};

struct stats_info
{
	stats_info()
	{
		GVM_ZERO_ME;
	}
	gv_stats::gve_stats_type type;
	gv_int int_a;
	gv_float float_a;
	gv_string string_a;
	bool not_clean_per_frame;
	gv_id category;
	const char* display_name;
	void* binded_ptr;

	template < class T >
	T& value()
	{
		return *(T*)ptr(type);
	}

	void* ptr(gv_stats::gve_stats_type t)
	{
		validate_type(t);
		if (binded_ptr)
			return binded_ptr;
		switch (type)
		{
		case gv_stats::stats_type_int:
			return &int_a;
		case gv_stats::stats_type_float:
			return &float_a;
		case gv_stats::stats_type_string:
			return &string_a;
		}
		return NULL;
	}

	void validate_type(gv_stats::gve_stats_type t)
	{
		if (type == gv_stats::stats_type_unknown)
		{
			type = t;
		}
		GV_ASSERT(type == t);
	}

	void reset_frame()
	{
		if (not_clean_per_frame)
			return;
		switch (type)
		{
		case gv_stats::stats_type_int:
			value< gv_int >() = 0;
			break;
		case gv_stats::stats_type_float:
			value< gv_float >() = 0;
			break;
		case gv_stats::stats_type_string:
			value< gv_string >() = "";
			break;
		}
	}
	gv_string_tmp to_string()
	{
		gv_string_tmp s;
		if (type == gv_stats::stats_type_unknown)
			return s;
		s << display_name << "=";
		switch (type)
		{
		case gv_stats::stats_type_int:
			s << value< gv_int >();
			break;
		case gv_stats::stats_type_float:
			s << value< gv_float >();
			break;
		case gv_stats::stats_type_string:
			s << "\"" << value< gv_string >() << "\"";
			break;
		}
		s << "  ";
		return s;
	}
};

class gv_stats_impl
{
public:
	gv_stats_impl(){};
	stats_info* get_stats_info(gv_int id, gv_stats::gve_stats_type type)
	{
		stats_info* p = m_stats_map.find(id);
		GV_ASSERT(p);
		cu_dynamic_id = gvc_stats_id_dynamic_start;
		return p;
	}

	gvt_hash_map< gv_id, stats_category_info > m_category_map;
	gvt_hash_map< gv_int, stats_info > m_stats_map;
	gv_uint_ptr thread_id;
	gv_int cu_dynamic_id;
};

gv_stats::gv_stats(){};

gv_stats::~gv_stats()
{
	destroy();
};

void gv_stats::init()
{
	m_pimpl = new gv_stats_impl;
	m_pimpl->thread_id = gv_thread::current_thread_id();
	gv_thread::current();
};

void gv_stats::destroy()
{
	GVM_SAFE_DELETE(m_pimpl);
};

void gv_stats::reset_frame()
{
	if (m_pimpl->thread_id != gv_thread::current_thread_id())
		return;
	gvt_hash_map< gv_int, stats_info >::iterator it = m_pimpl->m_stats_map.begin();
	while (it != m_pimpl->m_stats_map.end())
	{
		if (!it.is_empty())
		{
			it->reset_frame();
		}
		it++;
	}
};

void gv_stats::register_category(const gv_id& category, const gv_color& color,
								 bool show)
{
	GV_ASSERT(!m_pimpl->m_category_map.find(category));
	GV_ASSERT((m_pimpl->thread_id == gv_thread::current_thread_id()));
	m_pimpl->m_category_map.add(category, color);
	show_category(category, show);
};

void gv_stats::show_category(const gv_id& category, bool show)
{
	stats_category_info* p = m_pimpl->m_category_map.find(category);
	GV_ASSERT(p);
	p->hided = !show;
};

void gv_stats::register_stats(gv_int id, const char* display_name,
							  const gv_id& category, bool reset_per_frame,
							  gve_stats_type type, void* binded_var)
{
	GV_ASSERT((m_pimpl->thread_id == gv_thread::current_thread_id()));
	stats_info* p = m_pimpl->m_stats_map.find(id);
	GV_ASSERT(!p);
	p = m_pimpl->m_stats_map.add(id, stats_info());
	p->category = category;
	p->display_name = display_name;
	p->not_clean_per_frame = !reset_per_frame;
	p->type = type;
	p->binded_ptr = binded_var;
};

void* gv_stats::get_stats_ptr(gv_int id, gve_stats_type type)
{
	return m_pimpl->get_stats_info(id, type)->ptr(type);
};
gv_int gv_stats::get_dynamic_stats_id()
{
	return m_pimpl->cu_dynamic_id++;
}; // for stats that only need to bind

gv_string_tmp gv_stats::to_string(gv_int char_per_line, const gv_id& category,
								  gv_int id)
{
	gv_string_tmp s;
	gvt_hash_map< gv_id, gvt_array_cached< stats_info* > > stats_by_cat;
	gvt_hash_map< gv_int, stats_info >::iterator it = m_pimpl->m_stats_map.begin();
	while (it != m_pimpl->m_stats_map.end())
	{
		if (!it.is_empty())
		{
			if (it->type != gve_stats_type::stats_type_unknown)
			{
				stats_by_cat[it->category].add(it.data_ptr());
			}
		}
		it++;
	}
	gvt_hash_map< gv_id, gvt_array_cached< stats_info* > >::iterator it_cat =
		stats_by_cat.begin();
	while (it_cat != stats_by_cat.end())
	{
		if (!it_cat.is_empty() && (*it_cat).size())
		{
			stats_category_info* pinfo =
				m_pimpl->m_category_map.find(it_cat.get_key());
			if (pinfo && pinfo->hided == false)
			{
				// s << pinfo->color << gv_endl;
				s << "<<<" << it_cat.get_key() << ">>>" << gv_endl;
				for (int i = 0; i < (*it_cat).size(); i++)
				{
					s << "  " << (*it_cat)[i]->to_string() << gv_endl;
				}
			}
		}
		it_cat++;
	}
	return s;
};
}
