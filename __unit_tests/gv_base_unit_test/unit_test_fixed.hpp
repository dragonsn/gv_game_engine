#include "gv_base/math/gvt_fixed.h"
namespace unit_test_fixed
{
void main(gvt_array< gv_string >& args)
{

	/*{
			gv_fixed_16 a(2.5); 
			gv_fixed_16 a2(0.5); 
			gv_fixed_16 a3; 
			a3=a/a2; 
			GV_TEST_PRINT_VAR(a.to_double(),test_log());
			GV_TEST_PRINT_VAR(a2.to_double(),test_log());
			GV_TEST_PRINT_FUNC_AND_VERIFY((a+a2).to_double(),3,test_log()); 
			GV_TEST_PRINT_FUNC_AND_VERIFY((a/a2).to_double(),5,test_log()); 
			GV_TEST_PRINT_FUNC_AND_VERIFY((a*a2).to_double(),1.25,test_log()); 
			GV_TEST_PRINT_FUNC_AND_VERIFY((a-a2).to_double(),2,test_log()); 
			GV_TEST_PRINT_VAR(a.to_int(),test_log());
			GV_TEST_PRINT_VAR(a.to_float(),test_log());
			GV_TEST_PRINT_VAR(a.max_int(),test_log());
			GV_TEST_PRINT_VAR(a.min_int(),test_log());
			GV_TEST_PRINT_VAR(a.min_double(),test_log());
			GV_TEST_PRINT_VAR(a.max_double(),test_log());
			GV_TEST_PRINT_VAR(a.min_raw(),test_log());
			GV_TEST_PRINT_VAR(a.max_raw(),test_log());
		}

		{
			gv_fixed_16 a(-2.5); 
			gv_fixed_16 a2(-0.5); 
			gv_fixed_16 a3; 
			a3=a/a2; 
			GV_TEST_PRINT_VAR(a.to_double(),test_log());
			GV_TEST_PRINT_VAR(a2.to_double(),test_log());
			GV_TEST_PRINT_FUNC_AND_VERIFY((a+a2).to_double(),-3,test_log()); 
			GV_TEST_PRINT_FUNC_AND_VERIFY((a/a2).to_double(),5,test_log()); 
			GV_TEST_PRINT_FUNC_AND_VERIFY((a*a2).to_double(),1.25,test_log()); 
			GV_TEST_PRINT_FUNC_AND_VERIFY((a-a2).to_double(),-2,test_log()); 
			GV_TEST_PRINT_VAR(a.to_int(),test_log());
			GV_TEST_PRINT_VAR(a.to_float(),test_log());
			GV_TEST_PRINT_VAR(a.max_int(),test_log());
			GV_TEST_PRINT_VAR(a.min_int(),test_log());
			GV_TEST_PRINT_VAR(a.min_double(),test_log());
			GV_TEST_PRINT_VAR(a.max_double(),test_log());
			GV_TEST_PRINT_VAR(a.min_raw(),test_log());
			GV_TEST_PRINT_VAR(a.max_raw(),test_log());
		}
		
		{
			gv_fixed_16 a(2.5); 
			gv_fixed_30 a2(0.5); 
			gv_fixed_16 a3; 
			a3=a/a2; 
			GV_TEST_PRINT_VAR(a.to_double(),test_log());
			GV_TEST_PRINT_VAR(a2.to_double(),test_log());
			//GV_TEST_PRINT_FUNC_AND_VERIFY((a+a2).to_double(),3,test_log()); 
			GV_TEST_PRINT_FUNC_AND_VERIFY((a/a2).to_double(),5,test_log()); 
			GV_TEST_PRINT_FUNC_AND_VERIFY((a*a2).to_double(),1.25,test_log()); 
			//GV_TEST_PRINT_FUNC_AND_VERIFY((a-a2).to_double(),2,test_log()); 
			GV_TEST_PRINT_VAR(a2.to_int(),test_log());
			GV_TEST_PRINT_VAR(a2.to_float(),test_log());
			GV_TEST_PRINT_VAR(a2.max_int(),test_log());
			GV_TEST_PRINT_VAR(a2.min_int(),test_log());
			GV_TEST_PRINT_VAR(a2.min_double(),test_log());
			GV_TEST_PRINT_VAR(a2.max_double(),test_log());
		}*/
	{
		gv_math_fixed math;
		math.rebuild_sin_table();
		gv_vector3_fixed v;
		v = gv_value_fixed::epsilon();
		v.epsilon_to_zero();
		GV_ASSERT(v.abs_sum().get_int() == 0);
		v.x = gv_math_fixed::one();
		gv_matrix_fixed mat;
		mat.set_identity();
		mat *= gv_matrix_fixed::get_rotation_by_z(gv_math_fixed::pi() / 3);
		v = v * mat;
		//v.normalize();
		GV_ASSERT(v.x == gv_value_fixed::half());
	}
}
}