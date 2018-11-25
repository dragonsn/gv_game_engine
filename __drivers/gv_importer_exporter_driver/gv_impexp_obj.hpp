#include "gv_framework_private.h"
#include "gv_importer_exporter.h"

#include "..\renderer\gv_image.h"
#include "..\renderer\gv_texture.h"
#include "..\renderer\gv_material.h"
#include "..\renderer\gv_index_buffer.h"
#include "..\renderer\gv_vertex_buffer.h"
#include "..\renderer\gv_static_mesh.h"
namespace gv
{

class gv_impexp_obj : public gv_importer_exporter
{
public:
	GVM_DCL_CLASS(gv_impexp_obj, gv_importer_exporter);
	gv_impexp_obj()
	{
		link_class(gv_impexp_obj::static_class());
	};
	~gv_impexp_obj(){};

public:
	void get_import_extension(gvt_array< gv_id >& result)
	{
		result.push_back(gv_id_obj);
	};

	void get_export_extension(gvt_array< gv_id >& result)
	{
		result.push_back(gv_id_obj);
	};

	gv_class_info* get_import_target_class()
	{
		return gv_static_mesh::static_class();
	}

	bool do_import(const gv_string_tmp& file_name, gv_object* target)
	{
		GV_PROFILE_EVENT(gv_impexp_obj__do_import, 0)
		gv_string ext = gv_global::fm->get_extension(*file_name);
		;
		ext.to_upper();
		if (ext != ".OBJ")
			return false;

		gv_static_mesh* pmesh = gvt_cast< gv_static_mesh >(target);
		if (!pmesh)
			return false;
		gv_lexer lexer;
		if (!lexer.load_file(*file_name))
			return false;

		gv_vertex_buffer* pvb =
			pmesh->get_sandbox()->create_object< gv_vertex_buffer >(pmesh);
		pmesh->m_vertex_buffer = pvb;
		gv_index_buffer* pib =
			pmesh->get_sandbox()->create_object< gv_index_buffer >(pmesh);
		pmesh->m_index_buffer = pib;
		gvt_array< gv_vector2 >& tex_coord = pvb->m_raw_texcoord0;

		gvt_array< gv_vector3 > av;
		gvt_array< gv_vector3 > avn;
		gvt_array< gv_vector2 > avt;
		av.reserve(3000);
		avn.reserve(3000);
		avt.reserve(3000);

		gv_int token = lexer.lex();
		while (token)
		{
			if (token == gv_lang_cpp::TOKEN_ID)
			{
				if (lexer.get_id() == "mtllib")
				{
					lexer.strip_this_line();
					continue;
				}
				if (lexer.get_id() == "v")
				{
					gv_vector3 v;
					lexer >> v.x;
					lexer >> v.y;
					lexer >> v.z;
					av.push_back(v);
				}

				if (lexer.get_id() == "vn")
				{
					gv_vector3 v;
					lexer >> v.x;
					lexer >> v.y;
					lexer >> v.z;
					avn.push_back(v);
				}

				if (lexer.get_id() == "vt")
				{
					gv_vector2 v;
					lexer >> v.x;
					lexer >> v.y;
					avt.push_back(v);
				}

				if (lexer.get_id() == "f")
				{
					int start_index = pvb->m_raw_pos.size();
					while (gvt_is_num((char)lexer.look_ahead()))
					{
						int pidx, nidx = -1, tidx = -1;
						lexer >> pidx;
						if (avt.size())
						{
							lexer >> "/" >> tidx;
						}
						if (avn.size())
						{
							lexer >> "/" >> nidx;
						}
						//==to be optimized!!!
						int s = pvb->m_raw_pos.size();
						pvb->m_raw_pos.push_back(av[pidx - 1]);
						if (nidx != -1)
							pvb->m_raw_normal.push_back(avn[nidx - 1]);
						if (tidx != -1)
							pvb->m_raw_texcoord0.push_back(avt[tidx - 1]);

						if (s - start_index >= 3)
						{
							// a new fan!
							pib->m_raw_index_buffer.push_back(start_index);
							pib->m_raw_index_buffer.push_back(s - 1);
							pib->m_raw_index_buffer.push_back(s);
						}
						else
							pib->m_raw_index_buffer.push_back(s);
					}
				}
			}
			lexer.strip_this_line();
			token = lexer.lex();
		}
		pmesh->merge_optimize_vertex();
		return true;
	};

	gv_class_info* get_export_source_class()
	{
		return NULL;
	};
	bool do_export(const gv_string_tmp& file_name, gv_object* source)
	{
		gv_static_mesh* pmesh = gvt_cast< gv_static_mesh >(source);
		if (!pmesh)
			return false;
		gv_vertex_buffer* vb = pmesh->get_vb();
		gv_string_tmp text;

		// text << "o cube" << gv_endl;
		// text << "mtllib cube.mtl" << gv_endl;

		for (int v = 0; v < vb->m_raw_pos.size(); v++)
		{
			gv_vector3 v3 = vb->m_raw_pos[v];
			text << "v  " << v3.x << " " << v3.y << " " << v3.z << " " << gv_endl;
		}
		text << gv_endl;
		/*
    for (int v = 0; v < vb->m_raw_normal.size(); v++)
    {
            gv_vector3 v3 = vb->m_raw_normal[v];
            text << "vn  " << v3.x << " " << v3.y << " " << v3.z << " " <<
    gv_endl;
    }
    text << gv_endl;
    */

		for (int v = 0; v < vb->m_raw_texcoord0.size(); v++)
		{
			gv_vector3 v2 = vb->m_raw_texcoord0[v];
			text << "vt  " << v2.x << " " << v2.y << " " << gv_endl;
		}
		text << gv_endl;

		text << "g cube" << gv_endl;
		text << "s 1" << gv_endl;

		gv_index_buffer* ib = pmesh->get_ib();
		for (int t = 0; t < ib->get_nb_index(); t += 3)
		{
			text << "f  ";
			gv_int idx0 = ib->m_raw_index_buffer[t] + 1;
			gv_int idx1 = ib->m_raw_index_buffer[t + 1] + 1;
			gv_int idx2 = ib->m_raw_index_buffer[t + 2] + 1;

			text << idx0 << "/" << idx0 << "  ";
			text << idx1 << "/" << idx1 << "  ";
			text << idx2 << "/" << idx2 << gv_endl;
		}
		gv_save_string_to_file(file_name, text);
		return false;
	};
};

GVM_IMP_CLASS(gv_impexp_obj, gv_importer_exporter)
GVM_END_CLASS
}