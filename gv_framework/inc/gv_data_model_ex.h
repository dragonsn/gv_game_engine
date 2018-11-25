#if !defined(GV_DOM_FILE)
#error
#endif

#if defined(GV_DOM_SUPER_CLASS)
#define GV_DOM_SUPER_CLASS_DCL		:public GV_DOM_SUPER_CLASS
#define GV_DOM_SUPER_CLASS_FUNC                 \
	GV_DOM_SUPER_CLASS& get_super()             \
	{                                           \
		return *this;                           \
	}                                           \
	const GV_DOM_SUPER_CLASS& get_super() const \
	{                                           \
		return *this;                           \
	}
#else
#define GV_DOM_SUPER_CLASS_DCL
#define GV_DOM_SUPER_CLASS_FUNC
#endif

#if defined(GVM_DOM_STATIC_CLASS)
#define GVM_DOM_STATIC_CLASS_DCL static class gv_class_info* static_class();
#define GVM_DOM_STATIC_CLASS_IMP(cls) GVM_IMP_STATIC_CLASS(cls)
#else
#define GVM_DOM_STATIC_CLASS_DCL
#define GVM_DOM_STATIC_CLASS_IMP(cls)
#endif

#if !defined(GV_DOM_CONSTRUCT) && !defined(GV_DOM_SUPER_CLASS)
#define GV_DOM_CONSTRUCT(T) \
	{                       \
		GVM_ZERO_ME;        \
	}
#endif

#if !defined(GV_DOM_DESTRUCT)
#define GV_DOM_DESTRUCT(T) \
	{                      \
	}
#endif

#if defined(GV_DOM_STREAM)
#define GVM_DOM_STREAM_OP                        \
	GV_DOM_STREAM& operator>>(GV_DOM_STREAM& s); \
	GV_DOM_STREAM& operator<<(GV_DOM_STREAM& s);
#else
#define GVM_DOM_STREAM_OP
#endif

//         /
// HHHH   |=====================================================================\
	//H@@@@###|-------------------------[TOOL FOR DEFINE SIMPLE STRUCTURE!]---------->
// HHHH |=====================================================================/
//        \   


//============================================================================================
//								:decl
//============================================================================================
#if defined(GVM_DOM_DECL)
#define GVM_DOM_ELEMENT(T)         \
	class T GV_DOM_SUPER_CLASS_DCL \
	{                              \
	public:                        \
		T()                        \
		GV_DOM_CONSTRUCT(T);       \
		~T() GV_DOM_DESTRUCT(T);   \
		GV_DOM_SUPER_CLASS_FUNC;   \
		GVM_DOM_STATIC_CLASS_DCL;  \
		GVM_DOM_STREAM_OP;         \
		T(const T& src)            \
		{                          \
			(*this) = src;         \
		};                         \
		T& operator=(const T& src);

#define GVM_DOM_END_E \
	}                 \
	;
#define GVM_DOM_ATTRIB(type, name) type name;
#define GVM_DOM_ATTRIB_ENUM(enum, name) gv_int name;
#define GVM_DOM_FLAG(x)
#define GVM_DOM_CLEAR_FLAG(x)
#define GVM_DOM_ATTRIB_SET GVM_EMPTY

#include GV_DOM_FILE

#undef GVM_DOM_ELEMENT
#undef GVM_DOM_END_E
#undef GVM_DOM_ATTRIB
#undef GVM_DOM_ATTRIB_ENUM
#undef GVM_DOM_FLAG
#undef GVM_DOM_CLEAR_FLAG
#undef GVM_DOM_ATTRIB_SET

#endif

//============================================================================================
//								:rtti
//============================================================================================
#if defined(GVM_DOM_RTTI)
#ifndef GV_DOM_SUPER_CLASS
#define GVM_DOM_ELEMENT GVM_IMP_STRUCT
#else
#define GVM_DOM_ELEMENT(x) GVM_IMP_STRUCT_WITH_SUPER(x, GV_DOM_SUPER_CLASS)
#endif
#define GVM_DOM_END_E GVM_END_STRUCT
#define GVM_DOM_ATTRIB GVM_VAR
#define GVM_DOM_ATTRIB_ENUM GVM_VAR_ENUM
#define GVM_DOM_FLAG GVM_VAR_ATTRIB_SET
#define GVM_DOM_CLEAR_FLAG GVM_VAR_ATTRIB_UNSET
#define GVM_DOM_ATTRIB_SET GVM_VAR_SET

#include GV_DOM_FILE

#undef GVM_DOM_ELEMENT
#undef GVM_DOM_ATTRIB
#undef GVM_DOM_ATTRIB_ENUM
#undef GVM_DOM_END_E
#undef GVM_DOM_FLAG
#undef GVM_DOM_CLEAR_FLAG
#undef GVM_DOM_ATTRIB_SET

#define GVM_DOM_ELEMENT GVM_DOM_STATIC_CLASS_IMP
#define GVM_DOM_END_E
#define GVM_DOM_ATTRIB(type, name)
#define GVM_DOM_ATTRIB_ENUM(enum, name)
#define GVM_DOM_FLAG(x)
#define GVM_DOM_CLEAR_FLAG(x)
#define GVM_DOM_ATTRIB_SET GVM_EMPTY
#include GV_DOM_FILE
#undef GVM_DOM_ELEMENT
#undef GVM_DOM_END_E
#undef GVM_DOM_ATTRIB
#undef GVM_DOM_ATTRIB_ENUM
#undef GVM_DOM_FLAG
#undef GVM_DOM_CLEAR_FLAG
#undef GVM_DOM_ATTRIB_SET

#endif

//============================================================================================
//								:copy implement
//============================================================================================
#if defined(GVM_DOM_IMP_COPY)
#define GVM_DOM_ELEMENT(T)               \
	inline T& T::operator=(const T& src) \
	{
#define GVM_DOM_ATTRIB(type, name) name = src.name;
#define GVM_DOM_ATTRIB_ENUM(type, name) name = src.name;
#define GVM_DOM_END_E \
	return *this;     \
	}
#define GVM_DOM_FLAG(x)
#define GVM_DOM_ATTRIB_SET GVM_EMPTY
#define GVM_DOM_CLEAR_FLAG

#include GV_DOM_FILE

#undef GVM_DOM_ELEMENT
#undef GVM_DOM_END_E
#undef GVM_DOM_ATTRIB
#undef GVM_DOM_ATTRIB_ENUM
#undef GVM_DOM_FLAG
#undef GVM_DOM_ATTRIB_SET
#undef GVM_DOM_CLEAR_FLAG
#endif

//============================================================================================
//								:register , and
//unregister
//============================================================================================
#if defined(GVM_DOM_REGISTER)
#define GVM_DOM_ELEMENT GVM_REGISTER_STRUCT
#define GVM_DOM_END_E
#define GVM_DOM_ATTRIB(type, name)
#define GVM_DOM_ATTRIB_ENUM(type, name)
#define GVM_DOM_FLAG(x)
#define GVM_DOM_ATTRIB_SET GVM_EMPTY
#define GVM_DOM_CLEAR_FLAG(x)
#include GV_DOM_FILE
#undef GVM_DOM_ELEMENT
#undef GVM_DOM_END_E
#undef GVM_DOM_ATTRIB
#undef GVM_DOM_ATTRIB_ENUM
#undef GVM_DOM_FLAG
#undef GVM_DOM_ATTRIB_SET
#undef GVM_DOM_CLEAR_FLAG
#endif
//============================================================================================
//								:is_equal
//implement
//============================================================================================

#if defined(GVM_DOM_IMP_IS_EQUAL)
#define GVM_DOM_ELEMENT(T)                           \
	inline bool operator==(const T& t1, const T& t2) \
	{
#define GVM_DOM_ATTRIB(type, name) \
	if (!(t1.name == t2.name))     \
		return false;
#define GVM_DOM_ATTRIB_ENUM(type, name) \
	if (!(t1.name == t2.name))          \
		return false;
#define GVM_DOM_END_E \
	return true;      \
	}
#define GVM_DOM_FLAG(x)
#define GVM_DOM_ATTRIB_SET GVM_EMPTY
#define GVM_DOM_CLEAR_FLAG(x)
#include GV_DOM_FILE
#undef GVM_DOM_ELEMENT
#undef GVM_DOM_END_E
#undef GVM_DOM_ATTRIB
#undef GVM_DOM_ATTRIB_ENUM
#undef GVM_DOM_FLAG
#undef GVM_DOM_ATTRIB_SET
#undef GVM_DOM_CLEAR_FLAG
#endif
//============================================================================================
//								:is_less
//implement
//============================================================================================
#if defined(GVM_DOM_IMP_IS_LESS)
#define GVM_DOM_ELEMENT(T)                          \
	inline bool operator<(const T& t1, const T& t2) \
	{
#define GVM_DOM_ATTRIB(type, name) \
	if ((t1.name < t2.name))       \
		return true;               \
	if (!(t1.name == t2.name))     \
		return false;
#define GVM_DOM_ATTRIB_ENUM(type, name) \
	if ((t1.name < t2.name))            \
		return true;                    \
	if (!(t1.name == t2.name))          \
		return false;
#define GVM_DOM_END_E \
	return false;     \
	}
#define GVM_DOM_FLAG(x)
#define GVM_DOM_ATTRIB_SET GVM_EMPTY
#define GVM_DOM_CLEAR_FLAG(x)
#include GV_DOM_FILE
#undef GVM_DOM_ELEMENT
#undef GVM_DOM_END_E
#undef GVM_DOM_ATTRIB
#undef GVM_DOM_ATTRIB_ENUM
#undef GVM_DOM_FLAG
#undef GVM_DOM_ATTRIB_SET
#undef GVM_DOM_CLEAR_FLAG
#endif

//============================================================================================
//								:serialize
//implement
//============================================================================================
#if defined(GVM_DOM_IMP_SERIALIZE)
#if defined(GV_DOM_STREAM)
#define GVM_DOM_ELEMENT(T)                              \
	GV_DOM_STREAM& T::operator>>(GV_DOM_STREAM& stream) \
	{                                                   \
		T& t = *this;
#else
#define GVM_DOM_ELEMENT(T)                                                \
	template < class type_of_stream >                                     \
	inline type_of_stream& operator<<(type_of_stream& stream, const T& t) \
	{
#endif
#define GVM_DOM_ATTRIB(type, name) stream << t.name;
#define GVM_DOM_ATTRIB_ENUM(type, name) stream << t.name;
#define GVM_DOM_END_E \
	return stream;    \
	}
#define GVM_DOM_FLAG(x)
#define GVM_DOM_ATTRIB_SET GVM_EMPTY
#define GVM_DOM_CLEAR_FLAG(x)
#include GV_DOM_FILE
#undef GVM_DOM_ELEMENT
#undef GVM_DOM_END_E
#undef GVM_DOM_ATTRIB
#undef GVM_DOM_ATTRIB_ENUM
#undef GVM_DOM_FLAG
#undef GVM_DOM_ATTRIB_SET
#undef GVM_DOM_CLEAR_FLAG

#if defined(GV_DOM_STREAM)
#define GVM_DOM_ELEMENT(T)                              \
	GV_DOM_STREAM& T::operator<<(GV_DOM_STREAM& stream) \
	{                                                   \
		T& t = *this;
#else
#define GVM_DOM_ELEMENT(T)                                          \
	template < class type_of_stream >                               \
	inline type_of_stream& operator>>(type_of_stream& stream, T& t) \
	{
#endif
#define GVM_DOM_ATTRIB(type, name) stream >> t.name;
#define GVM_DOM_ATTRIB_ENUM(type, name) stream >> t.name;
#define GVM_DOM_END_E \
	return stream;    \
	}
#define GVM_DOM_FLAG(x)
#define GVM_DOM_ATTRIB_SET GVM_EMPTY
#define GVM_DOM_CLEAR_FLAG(x)
#include GV_DOM_FILE
#undef GVM_DOM_ELEMENT
#undef GVM_DOM_END_E
#undef GVM_DOM_ATTRIB
#undef GVM_DOM_ATTRIB_ENUM
#undef GVM_DOM_FLAG
#undef GVM_DOM_ATTRIB_SET
#undef GVM_DOM_CLEAR_FLAG
#endif
//============================================================================================
//								:serialize XML
//============================================================================================
#if defined(GVM_DOM_IMP_XML)
#define GVM_DOM_ELEMENT(T)                                                    \
	template < class type_of_stream >                                         \
	inline type_of_stream& gvt_exp_to_xml(type_of_stream& stream, const T& t) \
	{                                                                         \
		stream << "< " << #T << " ";
#define GVM_DOM_ATTRIB(type, name) stream << #name << "=\"" << t.name << "\"  ";
#define GVM_DOM_ATTRIB_ENUM(type, name) \
	stream << #name << "=\"" << t.name << "\"  ";
#define GVM_DOM_END_E      \
	return stream << "/>"; \
	}
#define GVM_DOM_FLAG(x)
#define GVM_DOM_ATTRIB_SET GVM_EMPTY
#define GVM_DOM_CLEAR_FLAG(x)
#include GV_DOM_FILE
#undef GVM_DOM_ELEMENT
#undef GVM_DOM_END_E
#undef GVM_DOM_ATTRIB
#undef GVM_DOM_ATTRIB_ENUM
#undef GVM_DOM_FLAG
#undef GVM_DOM_ATTRIB_SET
#undef GVM_DOM_CLEAR_FLAG
#endif

//============================================================================================
//								declare name
//============================================================================================
#if defined(GVM_DOM_MAKE_ID)
#define GVM_DOM_ELEMENT GVM_MAKE_ID
#define GVM_DOM_END_E
#define GVM_DOM_ATTRIB(type, name)
#define GVM_DOM_ATTRIB_ENUM(type, name)
#define GVM_DOM_FLAG(x)
#define GVM_DOM_ATTRIB_SET
#define GVM_DOM_CLEAR_FLAG(x)
#include GVM_MAKE_ID
#undef GVM_DOM_ELEMENT
#undef GVM_DOM_END_E
#undef GVM_DOM_ATTRIB
#undef GVM_DOM_ATTRIB_ENUM
#undef GVM_DOM_FLAG
#undef GVM_DOM_ATTRIB_SET
#undef GVM_DOM_CLEAR_FLAG
#endif

//============================================================================================
//
//============================================================================================
#undef GV_DOM_STREAM
#undef GV_DOM_FILE
#undef GVM_DOM_REGISTER
#undef GVM_DOM_IMP_COPY
#undef GVM_DOM_RTTI
#undef GVM_DOM_DECL
#undef GVM_DOM_IMP_IS_EQUAL
#undef GVM_DOM_IMP_IS_LESS
#undef GV_DOM_SUPER_CLASS
#undef GV_DOM_CONSTRUCT
#undef GV_DOM_DESTRUCT
#undef GV_DOM_SUPER_CLASS_DCL
#undef GVM_DOM_MAKE_ID
#undef GVM_DOM_IMP_XML
#undef GVM_DOM_IMP_SERIALIZE
#undef GV_DOM_SUPER_CLASS_FUNC
#undef GVM_DOM_STREAM_OP
#undef GVM_DOM_STATIC_CLASS
#undef GVM_DOM_STATIC_CLASS_DCL
#undef GVM_DOM_STATIC_CLASS_IMP
