#include "gv_framework/world/gv_world_2d.h"
namespace tool_sin_table_generator
{
	void main(gvt_array<gv_string > &args)
	{
		gv_math_fixed h; 
		gv_math_fixed * helper = gv_math_fixed::static_get();
		helper->rebuild_sin_table(); 
		gv_string_tmp s; 
		s << " static gv_int s_int_sin_table[]= {";
		static int number_per_line = 16; 
		for (int i = 0; i < helper->m_sin_table.size(); i++){
			if (i%number_per_line == 0){
				s << gv_endl; 
			}
			s << "\t" << helper->m_sin_table[i]<<","; 
		}
		s << "}; " << gv_endl;

		s << " static gv_int s_int_tan_table[]= {";
		for (int i = 0; i < helper->m_tan_table.size(); i++){
			if (i%number_per_line == 0){
				s << gv_endl;
			}
			s << "\t" << helper->m_tan_table[i] << ",";
		}
		s << "}; " << gv_endl;
		gv_save_string_to_file("int_math_table.h",s);
	
	}

}
