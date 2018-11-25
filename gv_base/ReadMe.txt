========================================================================
    STATIC LIBRARY : gv_base Project Overview
========================================================================
provide basic os level function. 
config: 
debug 
release
X
64bit
32bit 


//Barycentric coordinate 
http://www.blackpawn.com/texts/pointinpoly/default.html

use GNU 
APP_STL := gnustl_static
APP_CPPFLAGS += -fexceptions


build something sed-like in vbscript. 
Below is a gross, off-the-cuff stab at it. 
Your command line would look like 
cscript sed.vbs s/(oldpat)/(newpat)/ <inpfile.txt >outfile.txt 
where oldpat and newpat are Microsoft vbscript regex patterns. 
http://msdn.microsoft.com/en-us/library/f97kw5ka%28VS.85%29.aspx





==============================
coding standard
==============================
LLVM coding style , can be formated with CLANG-FORMAT
ColumnLimit set line width
Naming of classes, structs, functions and variables must follow STL naming conventions: no capital letters, use underscores between words.
template begin with "gvt_"
others begin with "gv_" to avoid name collision 
The smaller the scope of a variable is, the shorter its name should be.
Member attributes of larger classes should be prefixed with m_ ! Attributes of smaller structs can omit m_.

==>take boost coding standard as reference.
==>prefer longer file and narrow each line for easier read
==>'*'and '&' follow without space
==>'{' '}' need to change line
==>space after ','
==>there is an empty line between each function implementation
==>All preprocessor macros should begin with GV
==>Acronyms should be treated as ordinary names (e.g. xml_parser instead of XML_parser).
==>use tab instead of space , 8 space= 1 tab
==>save write space


==>[?]function return 1  and 0 ?
==>[?] Use of "using namespace" is absolutely prohibited.
==>[?] All public interfaces must be documented using doxygen (see tags in example).
==>[?] All containers and extensions must provide a simple tutorial and example. Design documentation is greatly recommended.
==>[?] All extensions and subsystems must provide tests which sufficiently cover the functions.

//=======================================
//sample from boost
//=======================================
  inline bool is_separator(fs::path::value_type c)
  {
    return c == separator
#     ifdef BOOST_WINDOWS_API
      || c == path::preferred_separator
#     endif
      ;
  }

  bool is_root_separator(const string_type& str, size_type pos);
    // pos is position of the separator

  size_type filename_pos(const string_type& str,
                          size_type end_pos); // end_pos is past-the-end position
  //  Returns: 0 if str itself is filename (or empty)

  size_type root_directory_start(const string_type& path, size_type size);
  //  Returns:  npos if no root_directory found
  template<class _Elem,
	class _Traits,
	class _Alloc> inline
	basic_istream<_Elem, _Traits>& operator>>(
		basic_istream<_Elem, _Traits>&& _Istr,
		basic_string<_Elem, _Traits, _Alloc>& _Str)
	{	// extract a string
	typedef ctype<_Elem> _Ctype;
	typedef basic_istream<_Elem, _Traits> _Myis;
	typedef basic_string<_Elem, _Traits, _Alloc> _Mystr;
	typedef typename _Mystr::size_type _Mysizt;


	template<class _Ty,
	class _Container> inline
	bool operator>=(const stack<_Ty, _Container>& _Left,
		const stack<_Ty, _Container>& _Right)
	{	// test if _Left >= _Right for stacks
	return (!(_Left < _Right));
	}

//=======================================

some reference site:
http://www.noctua-graphics.de/english/freetex_e.htm
http://tech.it168.com/a2011/0412/1177/000001177134.shtml
http://download.microsoft.com/download/e/1/c/e1c773de-73ba-494a-a5ba-f24906ecf088/vcredist_x86.exe
http://database.51cto.com/art/201010/231410_1.htm
//database 
http://www.mongodb.org/
http://sebug.net/paper/databases/nosql/Nosql.html#_3648342117667198_092538481578
No-sql
http://nosql-database.org/
Mongodb
http://www.cnblogs.com/lovecindywang/archive/2011/03/02/1969324.html
redis 
http://www.cnblogs.com/lovecindywang/archive/2011/03/03/1969633.html
zookeeper
http://zookeeper.apache.org/

Texture Converter (Windows Explorer Extension) http://msdn.microsoft.com/en-us/library/bb206244(VS.85).aspx 
//==========================================================================
step to add persistent type : 
1.modify gv_persistent_type.h 
2.modify gv_id_space
3.modify gv_ids.lx

