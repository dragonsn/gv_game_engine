==============================
coding standard
==============================

Naming of classes, structs, functions and variables must follow STL naming conventions: no capital letters, use underscores between words.
template begin with "gvt_"
others begin with "gv_" to avoid name collision 
The smaller the scope of a variable is, the shorter its name should be.
Member attributes of larger classes should be prefixed with m_ ! Attributes of smaller structs can omit m_.

==>take boost coding standard as reference.
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






	/*
	RenderMonkey Documentation.pdf
	Predefined Variables

	RenderMonkey provides a set of predefined variables for added shader development
	convenience. Such variables will display an appropriate tool tip (Predefined Variable) if
	the mouse hovers over them. Predefined variables are shader constants whose values get
	filled in at run-time by the viewer module directly at every frame. You cannot modify the
	values directly through the same user interface that you can use to edit other variables of
	similar types. A properly flagged predefined variable will be denoted in the workspace
	tree view with a symbol over the nodes icon. For example:
	RenderMonkey provides this set of predefined variables for your convenience:

	Time

	"Time0_X"
	Provides a floating point time value (in seconds) which repeats itself based on the
	¡°Cycle time¡± set in the RenderMonkey Preferences dialog. By default this "Cycle
	time¡± is set to 120 seconds. This means that the value of this variable cycles from
	0 to 120 in 120 seconds and then goes back to 0 again.
	"CosTime0_X"
	This variable will provide the cosine of Time0_X.
	"SinTime0_X"
	This variable will provide the sine of Time0_X.
	"TanTime0_X"
	This variable will provide the tangent of Time0_X.
	"Time0_X_Packed"
	This variable will pack the above xxxTime0_X variables into a 4 component
	floating point vector.
	Example: float4(Time0_X,CosTime0_X,SinTime0_X,TanTime0_X).
	"Time0_1"
	This variable provides a scaled floating point time value [0..1] which repeats itself
	based on the "Cycle time" set in the RenderMonkey Preferences dialog. By
	default this "Cycle time" is set to 120 seconds. This means that the value of this
	variable cycles from 0 to 1 in 120 seconds and then goes back to 0 again.
	"CosTime0_1"
	This variable will provide the cosine of Time0_1.
	"SinTime0_1"
	This variable will provide the sine of Time0_1.
	"TanTime0_1"
	This variable will provide the tangent of Time0_1.
	"Time0_1_Packed"
	This variable will pack the above xxxTime0_1 variables into a 4 component
	floating point vector.
	Example: float4(Time0_1,CosTime0_1,SinTime0_1,TanTime0_1).
	"Time0_2PI"
	This variable provides a scaled floating point time value [0..2PI] which repeats
	itself based on the "Cycle time" set in the RenderMonkey Preferences dialog. By
	default this "Cycle time" is set to 120 seconds. This means that the value of this
	variable cycles from 0 to 2PI in 120 seconds and then goes back to 0 again.
	"CosTime0_2PI"
	This variable will provide the cosine of Time0_2PI.
	"SinTime0_2PI"
	This variable will provide the sine of Time0_2PI.
	"TanTime0_2PI"
	This variable will provide the tangent of Time0_2PI .
	"Time0_2PI_Packed"
	This variable will pack the above xxxTime0_2PI variables into a 4 component
	floating point vector.
	Example: float4(Time0_2PI,CosTime0_2PI,SinTime0_2PI,TanTime0_2PI).
	"TimeCyclePeriod"
	This variable provides the "Cycle time" floating point value, as set in the
	RenderMonkey Preferences dialog. By default this "Cycle time" is set to 120
	seconds.
	"FPS"
	This variable provides the calculated frames per second, returned as a floating
	point value.
	"TimeElapsed"
	This variable provides the elapsed time (in seconds) from the last frame to the
	current frame, returned as a floating point value.


	Viewport

	"ViewportWidth"
	This variable provides the preview window width (in pixels), returned as a
	floating point value.
	"ViewportHeight"
	This variable provides the preview window height (in pixels), returned as a
	floating point value.
	"ViewportDimensions"
	This variable provides the preview window width and height (in pixels), returned
	as a float2 value.
	"ViewportWidthInverse"
	This variable will return 1.0 / ViewportWidth.
	"ViewportHeightInverse"
	This variable will return 1.0 / ViewportHeight.
	"InverseViewportDimensions"
	This variable provides the inverse of the "ViewportDimensions", returned as a
	float2 value.
	Random Values
	"RandomFraction1PerPass"
	"RandomFraction2PerPass"
	"RandomFraction3PerPass"
	"RandomFraction4PerPass"
	Each of these variables provide a random floating point value in the range of
	[0..1]. These values are updated each pass.
	"RandomFraction1PerEffect"
	"RandomFraction2PerEffect"
	"RandomFraction3PerEffect"
	"RandomFraction4PerEffect"
	Each of these variables provide a random floating point value in the range of
	[0..1]. These values are updated each effect.

	Pass

	"PassIndex"
	This variable will provide the pass index, returned as a floating point value.

	Mouse Parameters

	"LeftMouseButton"
	This variable will return a floating point value of 1.0 if the left mouse button is
	currently pressed, or 0.0 if it is not currently pressed.
	"MiddleMouseButton"
	This variable will return a floating point value of 1.0 if the middle mouse button is
	currently pressed, or 0.0 if it is not currently pressed.
	"RightMouseButton"
	This variable will return a floating point value of 1.0 if the right mouse button is
	currently pressed, or 0.0 if it is not currently pressed.
	"MouseButtonsPacked"
	This variable will pack the above xxxMouseButton variables into a 4 component
	floating point vector.
	Example: float4(LeftMouseButton,MiddleMouseButton,RightMouseButton ,0.0).
	"MouseCoordinateX"
	This variable will return the horizontal mouse position (in pixels), relative to the
	client area of the preview window, returned as a floating point value.
	"MouseCoordinateY"
	This variable will return the vertical mouse position (in pixels), relative to the
	client area of the preview window, returned as a floating point value.
	"MouseCoordinateXNDC"
	This variable will return "MouseCoordinateX" / "ViewportWidth".
	"MouseCoordinateYNDC"
	This variable will return "MouseCoordinateY" / "ViewportHeight".
	"MouseCoordsPacked"
	This variable will pack the above MouseCoordinatexxx variables into a 4
	component floating point vector.
	Example: float4(MouseCoordinateX,MouseCoordinateY,XNDC,YNDC).
	"MouseCoordinateXY"
	This variable will return the "MouseCoordinateX" and "MouseCoordinateY"
	coordinates into a 2 component floating point vector.
	Example: float2(MouseCoordinateX,MouseCoordinateY).
	"MouseCoordinateXYNDC"
	This variable will return the "MouseCoordinateXNDC" and
	"MouseCoordinateYNDC" coordinates into a 2 component floating point vector.
	Example: float2(MouseCoordinateXNDC,MouseCoordinateYNDC).

	Model Parameters

	"ModelMoundingBoxTopLeftCorner"
	This variable provides the top left coordinate of the model as a 3 component
	floating point vector (world space).
	"ModelMoundingBoxBottomRightCorner"
	This variable provides the bottom right coordinate of the model as a 3 component
	floating point vector (world space).
	"ModelMoundingBoxCenter"
	This variable provides the bounding box center of the model as a 3 component
	floating point vector (world space).
	"ModelCentroid"
	This variable provides the centroid of the model as a 3 component floating point
	vector (world space).
	"ModelBoundingSphereCenter"
	This variable provides the bounding sphere center of the model as a 3 component
	floating point vector (world space).
	"ModelBoundingSphereRadius"
	This variable provides the bounding sphere radius of the model as a single
	component floating point value (world space).

	View Parameters

	"ViewDirection"
	This variable provides the view direction vector (world space).
	"ViewPosition"
	This variable provides the view position (world space).
	"ViewSideVector"
	This variable provides the view size vector (world space).
	"ViewUpVector"
	This variable provides the view up vector (world space).
	"FOV"
	This variable provides the field of view as a floating point value.
	"NearClipPlane¡±
	This variable provides the near clip distance as a floating point value.
	"FarClipPlane¡±
	This variable provides the far clip distance as a floating point value.

	View Matrices

	"View"
	"ViewTranspose"
	"ViewInverse"
	"ViewInverseTranspose"
	These 4x4 matrix variables provide the view matrix, its transpose, its inverse, and
	the inverse transpose.
	"Projection"
	"ProjectionTranspose"
	"ProjectionInverse"
	"ProjectionInverseTranspose"
	These 4x4 matrix variables provide the projection matrix, its transpose, its
	inverse, and the inverse transpose.

	"ViewProjection"
	"ViewProjectionTranspose"
	"ViewProjectionInverse"
	"ViewProjectionInverseTranspose"
	These 4x4 matrix variables provide the view * projection matrix, its transpose, its
	inverse, and the inverse transpose.
	"World"
	"WorldTranspose"
	"WorldInverse"
	"WorldInverseTranspose"
	These 4x4 matrix variables provide the world matrix, its transpose, its inverse,
	and the inverse transpose. Note that since this version of RenderMonkey does not
	support implementation of a scene graph, we have decided to keep the world
	matrix as identity, but provide this predefined variable for your development
	convenience. The user may apply this variable in their shader and when imported
	into their engine, they may provide appropriate value of the world view projection
	matrix through the engine's calculations.
	"WorldView"
	"WorldViewTranspose"
	"WorldViewInverse"
	"WorldViewInverseTranspose"
	These 4x4 matrix variables provide the world * view matrix, its transpose, its
	inverse, and the inverse transpose.
	"WorldViewProjection"
	"WorldViewProjectionTranspose"
	"WorldViewProjectionInverse"
	"WorldViewProjectionInverseTranspose"
	These 4x4 matrix variables provide the World * View * Projection matrix, its
	transpose, its inverse, and the inverse transpose.

	Customizing Predefined Variable Names

	All predefined variable names are customizable through editing the
	".\UserData\RmPredefinedVariabled.txt" file. The data file is organized into four
	columns. The first column contains the name that the variable will be created with by
	default. This column is editable by the user. No other column data should be modified.
	The second column specifies the variable type; the third column specifies the rendering
	update frequency, and the fourth column species the predefined variable semantic. When
	items in the first column have been modified, RenderMonkey should be restarted for the
	changes to take effect. 
	*/