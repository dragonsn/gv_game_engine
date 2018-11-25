//xna test
//#include <windows.h>
typedef float FLOAT;
typedef unsigned int UINT;
typedef int INT;
typedef unsigned short USHORT;
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef signed char INT8, *PINT8;
typedef signed short INT16, *PINT16;
typedef signed int INT32, *PINT32;
typedef signed __int64 INT64, *PINT64;
typedef unsigned char UINT8, *PUINT8;
typedef unsigned short UINT16, *PUINT16;
typedef unsigned int UINT32, *PUINT32;
typedef unsigned __int64 UINT64, *PUINT64;

typedef _W64 int INT_PTR, *PINT_PTR;
typedef _W64 unsigned int UINT_PTR, *PUINT_PTR;
typedef _W64 long LONG_PTR, *PLONG_PTR;
typedef _W64 unsigned long ULONG_PTR, *PULONG_PTR;

#define __int3264 __int32
#define CONST const
#define VOID void
#define DbgPrint(...)

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif

#define NO_OUTPUT_DEBUG_STRING

//#include <windef.h>
#include "../../gv_base/math/gv_math_xna.h"
namespace unit_test_matrix44_xna
{

gv_vector2 fast_sincos(gv_float angle)
{
	gv_vector2 r;
	__asm fld angle __asm fsincos __asm fstp r.y __asm fstp r.x return r;
}

gv_vector2 sincos(gv_float angle)
{
	gv_vector2 v;
	v.x = std::sin(angle);
	v.y = std::cos(angle);
	return v;
}

void main(gvt_array< gv_string >& args)
{
	int align = __alignof(gv_matrix44_xna);
	{
		sub_test_timer timer("matrix_mul");
		const static int nb_test = 1000;
		for (int i = 0; i < nb_test; i++)
		{
			gv_matrix44_xna mat1, mat2, mat3;
			mat1.mat_gv.matrix44.randomize();
			mat2.mat_gv.matrix44.randomize();
			gvt_matrix_generic< gv_float, 4, 4 > mat_gen1 = mat1.mat_gv.matrix44;
			gvt_matrix_generic< gv_float, 4, 4 > mat_gen2 = mat2.mat_gv.matrix44;
			gvt_matrix_generic< gv_float, 4, 4 > mat_gen3;

			mat3 = mat1 * mat2;
			mat_gen3 = mat_gen1 * mat_gen2;
			GV_ASSERT(mat3.mat_gv.matrix44 == mat_gen3);
		}
	}

	{
		gv_vector2 v = fast_sincos(gvt_value< gv_float >::pi() / 4.0f);
		v = fast_sincos(gvt_value< gv_float >::pi() / 6.0f);
		GV_ASSERT(gv_float_trait::is_almost_equal(v.x, 0.5f));
		gv_vector2 v2 = fast_sincos(gvt_value< gv_float >::pi() / 6.0f + gv_float_trait::pi() * 4.f);
		;
		GV_ASSERT(v.is_almost_equal(v2));
		gvf_randomize< gv_float > rand(-100.f, 100.f);
		for (int i = 0; i < 1000; i++)
		{
			gv_float f;
			rand(f);
			gv_vector2 v = fast_sincos(f);
			gv_vector2 v2 = sincos(f);
			;
			GV_ASSERT(v.is_almost_equal(v2));
		}
	}
	/*		
		{  
			sub_test_timer timer("matrix_44_determinant");
			const static int nb_test=1000;
			for (int i=0; i<nb_test; i++)
			{
				gv_matrix44_xna mat1;
				mat1.matrix44.randomize();
				gvt_matrix_generic<gv_float,4,4> mat_gen=mat1.matrix44;
				gv_float d1=mat1.determinant();
				gv_float d2=mat_gen.determinant(); 
				GV_ASSERT(gvt_value<gv_float>::is_almost_equal(d1,d2));
			}
		}//test 1

		{
			sub_test_timer timer("matrix_44_inverse_no_scale");
			const static int nb_test=1000;
			gvt_random_range<gv_float> random;
			for (int i=0; i<nb_test; i++)
			{
				gv_matrix44 mat1;
				mat1=gv_matrix44::get_rotation_by_z( random() );
				mat1*=gv_matrix44::get_rotation_by_y( random() );
				mat1*=gv_matrix44::get_rotation_by_x( random() );

				gvt_matrix_generic<gv_float,4,4> mat_gen=mat1.matrix44;
				gv_matrix44 mat_r=mat1.get_inverse_noscale_3dtransform();
				gvt_matrix_generic<gv_float,4,4> mat_gen_r=mat_gen.inverse();
				//mat_r.axis_x.x+=0.001f;
				GV_ASSERT(mat_r.matrix44==mat_gen_r );
			}
		}

		{
			sub_test_timer timer("matrix_44_inverse_with_scale");
			const static int nb_test=1000;
			gvt_random_range<gv_float> random;
			for (int i=0; i<nb_test; i++)
			{
				gv_matrix44 mat1;
				mat1=gv_matrix44::get_rotation_by_z( random() );
				mat1*=gv_matrix44::get_rotation_by_y( random() );
				mat1*=gv_matrix44::get_rotation_by_x( random() );
				gv_vector3 v; 
				v.v.randomize(0.2f,2.0f);
				mat1.scale_by(v);

				gvt_matrix_generic<gv_float,4,4> mat_gen=mat1.matrix44;
				gv_matrix44 mat_r=mat1.get_inverse_with_scale_3dtransform();
				gvt_matrix_generic<gv_float,4,4> mat_gen_r=mat_gen.inverse();
				GV_ASSERT(mat_r.matrix44==(mat_gen_r) );
			}
		}

		{
			sub_test_timer timer("matrix_mul");
			const static int nb_test=1000;
			for (int i=0; i<nb_test; i++)
			{
				gv_matrix44 mat1,mat2,mat3;
				mat1.matrix44.randomize();
				mat2.matrix44.randomize();
				gvt_matrix_generic<gv_float,4,4> mat_gen1=mat1.matrix44;
				gvt_matrix_generic<gv_float,4,4> mat_gen2=mat2.matrix44;
				gvt_matrix_generic<gv_float,4,4> mat_gen3;

				mat3=mat1*mat2;
				mat_gen3=mat_gen1*mat_gen2;
				GV_ASSERT(mat3.matrix44==mat_gen3 );
			}
		}


		{
			sub_test_timer timer("matrix_mul_by");
			const static int nb_test=1000;
			gvt_random_range<gv_float> random;
			for (int i=0; i<nb_test; i++)
			{
				gv_matrix44 mat1;
				mat1=gv_matrix44::get_rotation_by_z( random() );
				mat1*=gv_matrix44::get_rotation_by_y( random() );
				mat1*=gv_matrix44::get_rotation_by_x( random() );
				gv_vector3 v,v1,vr; 
				v.v.randomize(0.2f,2.0f);
				mat1.scale_by(v);
				v1.v.randomize();
				gvt_matrix_generic<gv_float,4,4> mat_gen=mat1.matrix44;
				gvt_vector_generic<gv_float,3> vec_gen=mat_gen.mul_by(v1.v);
				vr=mat1.mul_by(v1);
				GV_ASSERT(vr.v==vec_gen );
				vr=v1*mat1;
				GV_ASSERT(vr.v==vec_gen );
			}
		}
		*/

} //main

} //
