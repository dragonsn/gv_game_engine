//====================================================================
// this file give all the data type that supported by  script engine
//====================================================================
#pragma once
namespace gv
{
class gv_class_info;
class gv_module;
class gv_sandbox;
class gv_object;
enum gve_data_type
{
#define GVM_DCL_PERSISTENT_TYPE_MAKE_ENUM
#include "gv_persistent_type.h"

	gve_data_type_ptr,
	gve_data_type_ref_ptr,
	gve_data_type_object,		// a struct or a class, complex data type.
	gve_data_type_array,		// dynamic array using gvt_array have element type
	gve_data_type_static_array, // static  one dimension array.
	gve_data_type_type,			// gvi_type ! internal usage!
	gve_data_type_text,
	gve_data_type_max,
};
// helper function to make gv_byte ptr
template < class T >
inline gv_byte* gvt_byte_ptr(T& t)
{
	return (gv_byte*)(&t);
}
template < class T >
inline const gv_byte* gvt_byte_ptr(const T& t)
{
	return (gv_byte*)(&t);
}

template < class T >
inline T& gvt_from_byte_ptr(gv_byte* p)
{
	return *((T*)(p));
}

class gvi_object_ptr_visitor
{
public:
	gvi_object_ptr_visitor(){};
	virtual ~gvi_object_ptr_visitor(){};
	virtual gvi_object_ptr_visitor&
	operator()(gvt_ptr< gv_object >& obj_ptr,
			   gvt_ref_ptr< gv_object >& obj_ref_ptr) = 0;
};
bool gv_can_directly_copy(gve_data_type type);
//==================================================================================>
class gvi_type : public gv_refable
{
	friend class gv_sandbox;

public:
	typedef gve_data_type type;
	gvi_type(){};

	virtual int get_size() = 0;
	virtual int get_alignment() = 0;
	virtual gve_data_type get_type() = 0;
	virtual gv_string_tmp get_type_string() = 0;
	virtual bool is_the_same_type(gvi_type*) = 0;
	virtual bool is_equal(gv_byte* psrc, gv_byte* ptgt) = 0;
	virtual bool is_less(gv_byte* psrc, gv_byte* ptgt) = 0;
	virtual gv_int compare(gv_byte* p1, gv_byte* p2) = 0;
	virtual void construct(gv_byte* pdata) = 0;
	virtual void destroy(gv_byte* pdata) = 0;
	virtual void read_data(gv_byte* pdata, gvi_stream*) = 0;
	virtual void write_data(gv_byte* pdata, gvi_stream*) = 0;
	virtual void copy_to(const gv_byte* psrc, gv_byte* ptgt) = 0;
	virtual void export_to_xml(gv_byte* pdata, gv_string_tmp& text,
							   int indent = 0) = 0;
	virtual bool import_from_xml(gv_byte* pdata, gv_xml_parser* ps,
								 gv_sandbox* sandbox = 0,
								 gv_module* module = 0) = 0;
	virtual void export_to_txt(gv_byte* pdata, gv_string_tmp& text){};
	virtual bool import_from_txt(gv_byte* pdata, gv_xml_parser* ps)
	{
		return true;
	};

	virtual int get_serialize_size(gv_byte* pdata)
	{
		return this->get_size();
	}; // if type is ptr or array or string , these two size is not the same!
	virtual gv_byte* create_instance()
	{
		gv_int size = this->get_size();
		gv_byte* pb = new gv_byte[size];
		this->construct(pb);
		return pb;
	};
	virtual void delete_instance(gv_byte* pb)
	{
		this->destroy(pb);
		delete[] pb;
	}
	virtual bool is_complex_type()
	{
		return true;
	};
	virtual void visit_ptr(gv_byte* pb, gvi_object_ptr_visitor&){};
	virtual bool is_copyable()
	{
		return gv_can_directly_copy(get_type());
	}
};

void gv_sort_with_type_info(gvi_type* type, gv_byte* first, gv_int size,
							bool ascend = true);

#define GVM_DCL_TYPE(tname) static const int c_type_id = gve_data_type_##tname;

#define GVM_IMP_TYPE_BASE(tname)                             \
	typedef gv_##tname type_of_data;                         \
	static const int c_type_id = gve_data_type_##tname;      \
	virtual int get_size()                                   \
	{                                                        \
		return sizeof(type_of_data);                         \
	};                                                       \
	virtual int get_alignment()                              \
	{                                                        \
		return gvt_get_alignment< type_of_data >();          \
	}                                                        \
	virtual gve_data_type get_type()                         \
	{                                                        \
		return gve_data_type_##tname;                        \
	}                                                        \
	virtual bool is_equal(gv_byte* psrc, gv_byte* ptgt)      \
	{                                                        \
		return get(psrc) == get(ptgt);                       \
	}                                                        \
	virtual bool is_less(gv_byte* psrc, gv_byte* ptgt)       \
	{                                                        \
		return get(psrc) < get(ptgt);                        \
	}                                                        \
	virtual void construct(gv_byte* pdata)                   \
	{                                                        \
		gvt_construct< type_of_data >((type_of_data*)pdata); \
	}                                                        \
	virtual void destroy(gv_byte* pdata)                     \
	{                                                        \
		gvt_destroy< type_of_data >((type_of_data*)pdata);   \
	}                                                        \
	virtual gv_int compare(gv_byte* p1, gv_byte* p2)         \
	{                                                        \
		type_of_data& t1 = get(p1);                          \
		type_of_data& t2 = get(p2);                          \
		if (t1 == t2)                                        \
			return 0;                                        \
		if (t1 < t2)                                         \
			return -1;                                       \
		return 1;                                            \
	}                                                        \
	inline type_of_data& get(gv_byte* p)                     \
	{                                                        \
		return *(type_of_data*)p;                            \
	}                                                        \
	inline const type_of_data& get(const gv_byte* p)         \
	{                                                        \
		return *(const type_of_data*)p;                      \
	}

#define GVM_IMP_TYPE_SIMPLE(tname)                                  \
	virtual void copy_to(const gv_byte* psrc, gv_byte* ptgt)        \
	{                                                               \
		get(ptgt) = get(psrc);                                      \
	}                                                               \
	virtual void read_data(gv_byte* pdata, gvi_stream* ps)          \
	{                                                               \
		(*ps) >> get(pdata);                                        \
	}                                                               \
	virtual void write_data(gv_byte* pdata, gvi_stream* ps)         \
	{                                                               \
		(*ps) << get(pdata);                                        \
	}                                                               \
	virtual bool is_the_same_type(gvi_type* p)                      \
	{                                                               \
		if (p)                                                      \
			return p->get_type() == this->get_type();               \
		return false;                                               \
	};                                                              \
	virtual gv_string_tmp get_type_string()                         \
	{                                                               \
		static gv_string_tmp name("gv_" #tname);                    \
		return name;                                                \
	}                                                               \
	virtual void export_to_xml(gv_byte* pdata, gv_string_tmp& text, \
							   int indent = 0)                      \
	{                                                               \
		text << "\"" << get(pdata) << "\"";                         \
	};                                                              \
	virtual bool import_from_xml(gv_byte* pdata, gv_xml_parser* ps, \
								 gv_sandbox* sandbox = 0,           \
								 gv_module* module = 0)             \
	{                                                               \
		ps->read_attribute_value(get(pdata));                       \
		return true;                                                \
	};                                                              \
	virtual void export_to_txt(gv_byte* pdata, gv_string_tmp& text) \
	{                                                               \
		text << get(pdata);                                         \
	};                                                              \
	virtual bool import_from_txt(gv_byte* pdata, gv_xml_parser* ps) \
	{                                                               \
		(*ps) >> get(pdata);                                        \
		return true;                                                \
	};                                                              \
	virtual bool is_complex_type()                                  \
	{                                                               \
		return false;                                               \
	};

#define GVM_NEW_TYPE(tname)                 \
	class gv_type_##tname : public gvi_type \
	{                                       \
	public:                                 \
		friend class gv_sandbox;            \
		gv_type_##tname(){};                \
		~gv_type_##tname(){};               \
		GVM_IMP_TYPE_BASE(tname);           \
		GVM_IMP_TYPE_SIMPLE(tname);         \
	};

#define GVM_DCL_PERSISTENT_TYPE_NEW_GV_TYPE
#include "gv_persistent_type.h"

class gv_type_array : public gvi_type
{
	friend class gv_sandbox;

public:
	GVM_DCL_TYPE(array);
	typedef gvt_array< gv_byte > type_of_byte_array;

	gv_type_array();
	~gv_type_array()
	{
	}
	virtual int get_size();
	virtual int get_alignment();
	virtual gve_data_type get_type();
	virtual gv_string_tmp get_type_string();
	virtual bool is_the_same_type(gvi_type*);
	virtual bool is_equal(gv_byte* psrc, gv_byte* ptgt);
	virtual bool is_less(gv_byte* psrc, gv_byte* ptgt);
	virtual gv_int compare(gv_byte* p1, gv_byte* p2);
	virtual void construct(gv_byte* pdata);
	virtual void destroy(gv_byte* pdata);
	virtual void read_data(gv_byte* pdata, gvi_stream*);
	virtual void write_data(gv_byte* pdata, gvi_stream*);
	virtual void copy_to(const gv_byte* psrc, gv_byte* ptgt);
	virtual void export_to_xml(gv_byte* pdata, gv_string_tmp& text,
							   int indent = 0);
	virtual bool import_from_xml(gv_byte* pdata, gv_xml_parser* ps,
								 gv_sandbox* sandbox = 0, gv_module* module = 0);
	// ARRAY METHOD
	gv_int get_array_size(gv_byte* parray);
	gvi_type* get_element_type();
	gv_int get_element_size()
	{
		return m_element_type->get_size();
	}
	gv_byte* get_element(gv_byte* parray, int index);
	void insert_element(gv_byte* parray, gv_byte* pelement, int index);
	bool find(gv_byte* parray, gv_byte* pelement, int& index);
	void resize_array(gv_byte* parray, int size);
	void erase(gv_byte* parray, int index);
	gv_byte* get_array_data(gv_byte* parray);
	void set_element_type(gvi_type* pelement_type)
	{
		m_element_type = pelement_type;
	}
	void push_back(gv_byte* parray, gv_byte* pelement);
	void pop_back(gv_byte* parray);
	void sort(gv_byte* parray, bool ascend = true);
	void visit_ptr(gv_byte* pb, gvi_object_ptr_visitor& v);

protected:
	type_of_byte_array& get_byte_array(gv_byte* pb)
	{
		return *((type_of_byte_array*)pb);
	}
	gvt_ref_ptr< gvi_type > m_element_type;
};

//
class gv_type_static_array : public gvi_type
{
	friend class gv_sandbox;

public:
	GVM_DCL_TYPE(static_array);
	gv_type_static_array();
	~gv_type_static_array(){};
	virtual int get_size();
	virtual int get_alignment();
	virtual gve_data_type get_type();
	virtual gv_string_tmp get_type_string();
	virtual bool is_the_same_type(gvi_type*);
	virtual bool is_equal(gv_byte* psrc, gv_byte* ptgt);
	virtual bool is_less(gv_byte* psrc, gv_byte* ptgt);
	virtual gv_int compare(gv_byte* p1, gv_byte* p2);
	virtual void construct(gv_byte* pdata);
	virtual void destroy(gv_byte* pdata);
	virtual void read_data(gv_byte* pdata, gvi_stream*);
	virtual void write_data(gv_byte* pdata, gvi_stream*);
	virtual void copy_to(const gv_byte* psrc, gv_byte* ptgt);
	virtual void export_to_xml(gv_byte* pdata, gv_string_tmp& text,
							   int indent = 0);
	virtual bool import_from_xml(gv_byte* pdata, gv_xml_parser* ps,
								 gv_sandbox* sandbox = 0, gv_module* module = 0);
	// ARRAY METHOD
	gvi_type* get_element_type()
	{
		return m_element_type;
	}
	gv_int get_element_size()
	{
		return m_element_type->get_size();
	}
	gv_byte* get_element(gv_byte* parray, int index);
	bool find(gv_byte* pdata, gv_byte* pelement, int& index);
	void set_array_size(gv_int size)
	{
		m_array_size = size;
	}
	gv_int get_array_size()
	{
		return m_array_size;
	}
	void set_element_type(gvi_type* pelement_type)
	{
		m_element_type = pelement_type;
	}
	void sort(gv_byte* parray, bool ascend = true);
	void visit_ptr(gv_byte* pb, gvi_object_ptr_visitor& v);

protected:
	gvt_ref_ptr< gvi_type > m_element_type;
	gv_int m_array_size;
};

typedef gvt_ptr< gv_byte > gv_ptr;
class gv_type_ptr : public gvi_type
{
	friend class gv_sandbox;

public:
	gv_type_ptr(){};
	GVM_IMP_TYPE_BASE(ptr);
	virtual void copy_to(const gv_byte* psrc, gv_byte* ptgt);
	virtual void read_data(gv_byte* pdata, gvi_stream*);
	virtual void write_data(gv_byte* pdata, gvi_stream*);
	virtual void export_to_xml(gv_byte* pdata, gv_string_tmp& text,
							   int indent = 0);
	virtual bool import_from_xml(gv_byte* pdata, gv_xml_parser* ps,
								 gv_sandbox* sandbox = 0, gv_module* module = 0);
	virtual bool is_complex_type();
	virtual bool is_the_same_type(gvi_type*);
	virtual gv_string_tmp get_type_string();
	void visit_ptr(gv_byte* pb, gvi_object_ptr_visitor& v);

	gvi_type* get_pointed_type();
	bool is_object_ptr();
	bool is_struct_ptr();

protected:
	gvt_ref_ptr< gvi_type > m_pointed_type;
};

typedef gvt_ref_ptr< gv_refable > gv_ref_ptr;
class gv_type_ref_ptr : public gvi_type
{
	friend class gv_sandbox;

public:
	gv_type_ref_ptr();
	GVM_IMP_TYPE_BASE(ref_ptr);
	virtual void copy_to(const gv_byte* psrc, gv_byte* ptgt);
	virtual void read_data(gv_byte* pdata, gvi_stream*);
	virtual void write_data(gv_byte* pdata, gvi_stream*);
	virtual void export_to_xml(gv_byte* pdata, gv_string_tmp& text,
							   int indent = 0);
	virtual bool import_from_xml(gv_byte* pdata, gv_xml_parser* ps,
								 gv_sandbox* sandbox = 0, gv_module* module = 0);
	virtual bool is_complex_type();
	virtual bool is_the_same_type(gvi_type* p);
	virtual gv_string_tmp get_type_string();
	void visit_ptr(gv_byte* pb, gvi_object_ptr_visitor& v);

	gvi_type* get_pointed_type()
	{
		return m_pointed_type;
	};
	bool is_object_ptr();
	bool is_struct_ptr();

protected:
	gvt_ref_ptr< gvi_type > m_pointed_type;
};

// internal use!!
class gv_type_type : public gvi_type
{
	friend class gv_sandbox;
	typedef gv_uint gv_type;

public:
	gv_type_type(){};
	GVM_IMP_TYPE_BASE(type);
	virtual void copy_to(const gv_byte* psrc, gv_byte* ptgt)
	{
		get(ptgt) = get(psrc);
	}
	virtual void read_data(gv_byte* pdata, gvi_stream*)
	{
		GV_ASSERT(0);
	};
	virtual void write_data(gv_byte* pdata, gvi_stream*)
	{
		GV_ASSERT(0);
	};
	virtual void export_to_xml(gv_byte* pdata, gv_string_tmp& text,
							   int indent = 0)
	{
		GV_ASSERT(0);
	};
	virtual bool import_from_xml(gv_byte* pdata, gv_xml_parser* ps,
								 gv_sandbox* sandbox = 0, gv_module* module = 0)
	{
		GV_ASSERT(0);
		return true;
	};
	virtual bool is_complex_type()
	{
		return false;
	};
	virtual bool is_the_same_type(gvi_type* p)
	{
		if (p)
			return p->get_type() == this->get_type();
		return false;
	}
	virtual gv_string_tmp get_type_string()
	{
		return "gvi_type";
	}
};

class gv_type_object : public gvi_type
{
	friend class gv_sandbox;
	friend class gv_type_ptr;
	friend class gv_type_ref_ptr;

public:
	GVM_DCL_TYPE(object);

	gv_type_object();
	virtual int get_size();
	virtual int get_alignment();
	virtual gve_data_type get_type();
	virtual gv_string_tmp get_type_string();
	virtual bool is_the_same_type(gvi_type*);
	virtual bool is_equal(gv_byte* psrc, gv_byte* ptgt);
	virtual bool is_less(gv_byte* psrc, gv_byte* ptgt);
	virtual gv_int compare(gv_byte* p1, gv_byte* p2);
	virtual void construct(gv_byte* pdata);
	virtual void destroy(gv_byte* pdata);
	virtual void read_data(gv_byte* pdata, gvi_stream*);
	virtual void write_data(gv_byte* pdata, gvi_stream*);
	virtual void copy_to(const gv_byte* psrc, gv_byte* ptgt);
	virtual void export_to_xml(gv_byte* pdata, gv_string_tmp& text,
							   int indent = 0);
	virtual bool import_from_xml(gv_byte* pdata, gv_xml_parser* ps,
								 gv_sandbox* sandbox = 0, gv_module* module = 0);
	virtual void visit_ptr(gv_byte* pb, gvi_object_ptr_visitor& v);
	virtual void set_class(gv_class_info* pcls)
	{
		m_class = pcls;
	}
	virtual gv_class_info* get_class()
	{
		return m_class;
	}
	virtual bool is_copyable();

protected:
	gvt_ptr< gv_class_info > m_class;
};

class gv_type_text : public gvi_type
{
public:
	gv_type_text(){};
	GVM_IMP_TYPE_BASE(text);
	virtual void copy_to(const gv_byte* psrc, gv_byte* ptgt)
	{
		get(ptgt) = get(psrc);
	}
	virtual void read_data(gv_byte* pdata, gvi_stream*);
	virtual void write_data(gv_byte* pdata, gvi_stream*);
	virtual void export_to_xml(gv_byte* pdata, gv_string_tmp& text,
							   int indent = 0);
	virtual bool import_from_xml(gv_byte* pdata, gv_xml_parser* ps,
								 gv_sandbox* sandbox = 0, gv_module* module = 0);
	virtual bool is_complex_type()
	{
		return true;
	};
	virtual bool is_the_same_type(gvi_type* p)
	{
		if (p)
			return p->get_type() == this->get_type();
		return false;
	}
	virtual gv_string_tmp get_type_string()
	{
		return "gv_text";
	}
};

template < class type_of_type >
inline type_of_type* gvt_cast(gvi_type* ptype)
{
	if (ptype && ptype->get_type() == type_of_type::c_type_id)
		return (type_of_type*)ptype;
	return NULL;
}

} //
