#pragma once
namespace gv
{
class gv_stats_impl;
class gv_stats
{
public:
	enum gve_stats_type
	{
		stats_type_unknown,
		stats_type_int, // thread safe
		stats_type_float,
		stats_type_string
	};

public:
	gv_stats();
	~gv_stats();
	void init();
	void destroy();
	void reset_frame();
	void register_category(const gv_id& category,
						   const gv_color& color = gv_color::GREEN(),
						   bool show = false);
	void register_stats(gv_int id, const char* display_name,
						const gv_id& category, bool reset_per_frame = true,
						gve_stats_type type = stats_type_unknown,
						void* binded_var = NULL);
	void show_category(const gv_id& category, bool enable = true);
	template < class T >
	void register_stats(gv_int id, const char* display_name,
						const gv_id& category, T& t,
						bool reset_per_frame = true)
	{
		register_stats(id, display_name, category, reset_per_frame, stats_type<>(t),
					   &t);
	};
	template < class T >
	void register_stats(const char* display_name, const gv_id& category, T& t,
						bool reset_per_frame = true)
	{
		register_stats(get_dynamic_stats_id(), display_name, category,
					   reset_per_frame, stats_type<>(t), &t);
	};

	template < class T >
	inline T get_stats(gv_int id)
	{
		T t;
		return *(T*)get_stats_ptr(id, stats_type<>(t));
	};
	template < class T >
	inline T set_stats(gv_int id, const T& t)
	{
		return *(T*)get_stats_ptr(id, stats_type<>(t)) = t;
	};
	template < class T >
	inline T add_stats(gv_int id, const T& t)
	{
		return *(T*)get_stats_ptr(id, stats_type<>(t)) += t;
	};
	gv_string_tmp to_string(gv_int char_per_line = 256,
							const gv_id& category = gv_id_null, gv_int id = -1);

protected:
	template < class T >
	gve_stats_type stats_type(const T& t)
	{
		return stats_type_unknown;
	}

	// not thread safe , make sure only write to one string stats in the same
	// thread.
	void* get_stats_ptr(gv_int id, gve_stats_type type);
	gv_int get_dynamic_stats_id(); // for stats that only need to bind
	gvt_ptr< class gv_stats_impl > m_pimpl;
};
template <>
inline gv_stats::gve_stats_type gv_stats::stats_type(const float&)
{
	return stats_type_float;
}
template <>
inline gv_stats::gve_stats_type gv_stats::stats_type(const gv_string&)
{
	return stats_type_string;
}
template <>
inline gv_stats::gve_stats_type gv_stats::stats_type(const gv_int&)
{
	return stats_type_int;
}

// some reference for defining stats_id_s;
static const gv_int gvc_stats_id_base_start = 1;
static const gv_int gvc_stats_id_framework_start = 1000;
static const gv_int gvc_stats_id_driver_start = 2000;
static const gv_int gvc_stats_id_app_start = 10000;
static const gv_int gvc_stats_id_dynamic_start = 60000;

enum gve_stats_id // some most common stats ids;
{

};
}