#include "gv_hook_gameplay3d.h"
namespace gv
{
static bool g_init_from_existed_window = false;
class gv_debug_renderer_gameplay3d;
class gv_debug_renderer_gp_imp
{
	friend class gv_debug_renderer_gameplay3d;

protected:
	struct vertex
	{
		vertex()
		{
		}
		vertex(const gv_vector3& v, const gv_color& c, bool is_2d = true)
		{
			if (is_2d)
			{
				x = v.get_x() - Game::getInstance()->getWidth() / 2;
				y = Game::getInstance()->getHeight() / 2 - v.get_y();
			}
			else
			{
				x = v.x;
				y = v.y;
			}
			z = v.z;
			color = gv_colorf(c).v;
		}

		float x, y, z;	// The transformed position for the vertex
		gv_vector3 color; // The vertex color
	};

	static const gv_int gvc_string_info_length = 512;
	struct string_info
	{
		char string[512];
		gv_color color;
		gv_vector2i pos;
		string_info()
		{
		}
		string_info(const string_info& info)
		{
			(*this) = info;
		}
		string_info& operator=(const string_info& s)
		{
			strncpy(string, s.string, gvc_string_info_length - 1);
			pos = s.pos;
			color = s.color;
			return *this;
		}
	};

protected:
	static const int c_nb_vb_buffer = 2;

	gv_vector2i m_window_size;
	int m_max_tri_vertex;
	int m_max_line_vertex;
	int m_current_vb;
	int last_vb()
	{
		return !m_current_vb;
	}

	gvt_array< string_info > m_strings_to_render[2];
	gvt_array< string_info >& cu_strings_to_render()
	{
		return m_strings_to_render[m_current_vb];
	}
	gvt_array< string_info >& last_strings_to_render()
	{
		return m_strings_to_render[!m_current_vb];
	}

	gvt_array< vertex > m_vb_triangle[2];
	gvt_array< vertex >& cu_vb_triangle()
	{
		return m_vb_triangle[m_current_vb];
	}
	gvt_array< vertex >& last_vb_triangle()
	{
		return m_vb_triangle[!m_current_vb];
	}

	gvt_array< vertex > m_vb_line[2];
	gvt_array< vertex >& cu_vb_line()
	{
		return m_vb_line[m_current_vb];
	}
	gvt_array< vertex >& last_vb_line()
	{
		return m_vb_line[!m_current_vb];
	}

	gvt_array< vertex > m_vb_line_3d[2];
	gvt_array< vertex >& cu_vb_line_3d()
	{
		return m_vb_line_3d[m_current_vb];
	}
	gvt_array< vertex >& last_vb_line_3d()
	{
		return m_vb_line_3d[!m_current_vb];
	}

	gvt_array< vertex > m_vb_triangle_3d[2];
	gvt_array< vertex >& cu_vb_triangle_3d()
	{
		return m_vb_triangle_3d[m_current_vb];
	}
	gvt_array< vertex >& last_vb_triangle_3d()
	{
		return m_vb_triangle_3d[!m_current_vb];
	}

public:
	gv_debug_renderer_gp_imp()
		: m_window_size(0, 0), m_max_tri_vertex(0), m_max_line_vertex(0),
		  m_current_vb(0)
	{
	}

	~gv_debug_renderer_gp_imp()
	{
	}

	void draw_triangle(const gv_vector3& v0, const gv_vector3& v1,
					   const gv_vector3& v2, const gv_color& color0,
					   const gv_color& color1, const gv_color& color2)
	{
		if (cu_vb_triangle().size() + 3 >= m_max_tri_vertex)
			return;
		this->cu_vb_triangle().add(vertex(v0, color0));
		;
		this->cu_vb_triangle().add(vertex(v1, color1));
		;
		this->cu_vb_triangle().add(vertex(v2, color2));
		;
	};

	void draw_line(const gv_vector3& v0, const gv_vector3& v1,
				   const gv_color& color0, const gv_color& color1)
	{
		if (cu_vb_line().size() + 2 >= m_max_line_vertex)
		{
			return;
		}
		this->cu_vb_line().add(vertex(v0, color0));
		;
		this->cu_vb_line().add(vertex(v1, color1));
		;
	};

	void draw_line_3d(const gv_vector3& v0, const gv_vector3& v1,
					  const gv_color& color0, const gv_color& color1)
	{
		if (cu_vb_line_3d().size() + 2 >= m_max_line_vertex)
		{
			return;
		}
		this->cu_vb_line_3d().add(vertex(v0, color0, false));
		;
		this->cu_vb_line_3d().add(vertex(v1, color1, false));
		;
	};

	void draw_string(const char* string, const gv_vector2i& screen_pos,
					 const gv_color& color)
	{
		string_info s;
		s.color = color;
		s.pos = screen_pos;
		strncpy(s.string, string, gvc_string_info_length - 1);
		cu_strings_to_render().add(s);
	};

	void set_debug_texture(class gv_texture* p, bool use_alpha_blending,
						   bool for_3d)
	{
		GVM_UNDER_CONSTRUCT;
	};
	void draw_tex_triangle(const gv_vector3 v[3], const gv_vector2 uv[3],
						   const gv_color color[3])
	{
		GVM_UNDER_CONSTRUCT;
	};
	void draw_tex_triangle_3d(const gv_vector3 v[3], const gv_vector2 uv[3],
							  const gv_color color[3]){GVM_UNDER_CONSTRUCT};
	gv_vector2i get_window_size()
	{
		return m_window_size;
	}

	void do_synchronization()
	{
		m_current_vb = !m_current_vb;
		cu_vb_triangle().clear_and_reserve();
		cu_vb_line().clear_and_reserve();
		cu_strings_to_render().clear_and_reserve();
		cu_vb_line_3d().clear_and_reserve();
	}

	void init_vb(const int max_triangle, const int max_line)
	{
		m_max_tri_vertex = max_triangle * 3;
		m_max_line_vertex = max_line * 2;
		m_current_vb = 0;
		m_vb_triangle[0].reserve(m_max_tri_vertex);
		m_vb_line[0].reserve(m_max_line_vertex);
		m_vb_line_3d[0].reserve(m_max_line_vertex);

		m_vb_triangle[1].reserve(m_max_tri_vertex);
		m_vb_line[1].reserve(m_max_line_vertex);
		m_vb_line_3d[1].reserve(m_max_line_vertex);
	}

	//=======================================
	// gameplay resource
	//=======================================

	Matrix m_world_view_projection_matrix_2d;
	Matrix m_world_view_projection_matrix_3d;

	gvt_ptr< Font > m_font;
	gvt_ptr< Material > m_material;
	gvt_ptr< MeshBatch > m_debug_line_batch;
	gvt_ptr< MeshBatch > m_debug_triangle_batch;
	gvt_ptr< MeshBatch > m_debug_line_3d_batch;

	void initialize_gameplay3d_resource()
	{
		// Create the font for drawing the framerate.
		m_font = Font::create("res/common/arial14.gpb");
		Matrix::createOrthographic(Game::getInstance()->getWidth(),
								   Game::getInstance()->getHeight(), -1.0f, 1.0f,
								   &m_world_view_projection_matrix_2d);
		m_world_view_projection_matrix_3d = m_world_view_projection_matrix_2d;
		createMaterial();
		m_debug_line_batch = createMeshBatch(Mesh::LINES);
		m_debug_line_3d_batch = createMeshBatch(Mesh::LINES);
		m_debug_triangle_batch = createMeshBatch(Mesh::TRIANGLES);
	}

	void finalize_gameplay3d_resource()
	{
		SAFE_RELEASE(m_font);
		SAFE_DELETE(m_debug_triangle_batch);
		SAFE_DELETE(m_debug_line_batch);
		SAFE_DELETE(m_debug_line_3d_batch);
	}

	Material* createMaterial()
	{
		m_material =
			Material::create("res/shaders/colored-unlit.vert",
							 "res/shaders/colored-unlit.frag", "VERTEX_COLOR");
		GP_ASSERT(m_material && m_material->getStateBlock());
		return m_material;
	}

#pragma GV_REMINDER("why this function don't work??")
	/*		MeshBatch* createMeshBatch(Mesh::PrimitiveType primitiveType)
                  {
                     VertexFormat::Element elements[] =
                    {
                            VertexFormat::Element(VertexFormat::POSITION, 3),
                            VertexFormat::Element(VertexFormat::COLOR, 3)
                    };
                    unsigned int elementCount = sizeof(elements) /
     sizeof(VertexFormat::Element);
                    MeshBatch* meshBatch =
     MeshBatch::create(VertexFormat(elements, elementCount), primitiveType,
     m_material, false);
                    return meshBatch;
                  }*/

	MeshBatch* createMeshBatch(Mesh::PrimitiveType primitiveType)
	{
		Material* material = createMaterial();
		VertexFormat::Element elements[] = {
			VertexFormat::Element(VertexFormat::POSITION, 3),
			VertexFormat::Element(VertexFormat::COLOR, 3)};
		unsigned int elementCount =
			sizeof(elements) / sizeof(VertexFormat::Element);
		MeshBatch* meshBatch = MeshBatch::create(
			VertexFormat(elements, elementCount), primitiveType, material, false);
		SAFE_RELEASE(material);
		return meshBatch;
	}

	void drawFrameRate(Font* font, const Vector4& color, unsigned int x,
					   unsigned int y, unsigned int fps)
	{
		char buffer[10];
		sprintf(buffer, "%u", fps);
		font->start();
		font->drawText(buffer, x, y, color, font->getSize());
		font->finish();
	}

	void draw_batch(MeshBatch* batch, gvt_array< vertex >& vertices,
					bool is_3d = false)
	{
		if (!vertices.size())
			return;
		batch->start();
		batch->add(vertices.get_data(), (unsigned int)vertices.size());
		if (!is_3d)
		{
			batch->getMaterial()->getStateBlock()->setCullFace(false);
			batch->getMaterial()->getStateBlock()->setDepthTest(false);
			batch->getMaterial()->getStateBlock()->setDepthWrite(false);
			batch->getMaterial()
				->getParameter("u_worldViewProjectionMatrix")
				->setValue(m_world_view_projection_matrix_2d);
		}
		else
		{
			m_material->getStateBlock()->setCullFace(false);
			batch->getMaterial()->getStateBlock()->setDepthTest(true);
			batch->getMaterial()->getStateBlock()->setDepthWrite(true);
			batch->getMaterial()
				->getParameter("u_worldViewProjectionMatrix")
				->setValue(m_world_view_projection_matrix_3d);
		}
		batch->draw();
		batch->finish();
	}
	void draw_debug_strings()
	{

		if (last_strings_to_render().size())
		{
			GV_PROFILE_EVENT(debug_draw_string, 0);
			m_font->start();
			for (int i = 0; i < last_strings_to_render().size(); i++)
			{
				string_info& s = last_strings_to_render()[i];
				gv_colorf c(s.color);
				m_font->drawText(s.string, s.pos.get_x(), s.pos.get_y(),
								 gv_gp_helper::to_gp(c.v), m_font->getSize());
			}
			m_font->finish();
		}
	}
	void render()
	{
		if (Game::getInstance() && m_font)
		{
			draw_batch(m_debug_line_batch, last_vb_line());
			draw_batch(m_debug_line_3d_batch, last_vb_line_3d(), true);
			draw_batch(m_debug_triangle_batch, last_vb_triangle());
			// drawFrameRate(m_font, Vector4(0, 0.5f, 1, 1), 5, 1,
			// Game::getInstance()->getFrameRate() );
			draw_debug_strings();
		}
	}
};

//=====================================================================================
gv_debug_renderer_gameplay3d* gv_debug_renderer_gameplay3d::s_debug_draw_gameplay3d = NULL;
gv_debug_renderer_gameplay3d::gv_debug_renderer_gameplay3d()
{
	m_impl = new gv_debug_renderer_gp_imp;
	GV_ASSERT(!s_debug_draw_gameplay3d);
	s_debug_draw_gameplay3d = this;
}
gv_debug_renderer_gameplay3d::~gv_debug_renderer_gameplay3d()
{
	s_debug_draw_gameplay3d = NULL;
	GVM_SAFE_DELETE(m_impl);
}

void gv_debug_renderer_gameplay3d::init_vb(const int max_triangle,
										   const int max_line)
{
	this->m_impl->init_vb(max_triangle, max_line);
};

void gv_debug_renderer_gameplay3d::draw_triangle(
	const gv_vector3& v0, const gv_vector3& v1, const gv_vector3& v2,
	const gv_color& color0, const gv_color& color1, const gv_color& color2)
{
	this->m_impl->draw_triangle(v0, v1, v2, color0, color1, color2);
};
void gv_debug_renderer_gameplay3d::draw_triangle_3d(
	const gv_vector3& v0, const gv_vector3& v1, const gv_vector3& v2,
	const gv_color& color0, const gv_color& color1, const gv_color& color2){
	// this->m_impl->draw_tex_triangle_3d(v0,v1,v2,color0, color1 , color2);
};
void gv_debug_renderer_gameplay3d::draw_line(const gv_vector3& v0,
											 const gv_vector3& v1,
											 const gv_color& color0,
											 const gv_color& color1)
{
	this->m_impl->draw_line(v0, v1, color0, color1);
};
void gv_debug_renderer_gameplay3d::draw_line_3d(const gv_vector3& v0,
												const gv_vector3& v1,
												const gv_color& color0,
												const gv_color& color1)
{
	this->m_impl->draw_line_3d(v0, v1, color0, color1);
};
void gv_debug_renderer_gameplay3d::set_debug_texture(class gv_texture*,
													 bool use_alpha_blending,
													 bool for_3d){
	//
};
void gv_debug_renderer_gameplay3d::draw_tex_triangle(const gv_vector3 v[3],
													 const gv_vector2 uv[3],
													 const gv_color color[3]){
	//
};
void gv_debug_renderer_gameplay3d::draw_tex_triangle_3d(
	const gv_vector3 v[3], const gv_vector2 uv[3], const gv_color color[3]){

};
void gv_debug_renderer_gameplay3d::draw_string(const char* string,
											   const gv_vector2i& screen_pos,
											   const gv_color& color)
{
	this->m_impl->draw_string(string, screen_pos, color);
};
void gv_debug_renderer_gameplay3d::render()
{
	GV_PROFILE_EVENT(gv_debug_renderer_gameplay3d__drawScene, 0);
	m_impl->render();
};
void gv_debug_renderer_gameplay3d::capture_mouse(){
	//	Platform::setMouseCaptured(true);
};
void gv_debug_renderer_gameplay3d::release_mouse(){
	//	Platform::setMouseCaptured(false);
};

gv_vector2i gv_debug_renderer_gameplay3d::get_window_size()
{
	return gv_vector2i(Game::getInstance()->getWidth(),
					   Game::getInstance()->getHeight());
};

void gv_debug_renderer_gameplay3d::do_synchronization()
{
	m_impl->do_synchronization();
}

bool gv_debug_renderer_gameplay3d::initialize_gameplay3d()
{
	m_impl->initialize_gameplay3d_resource();
	m_impl->init_vb(gv_global::rnd_opt.m_max_debug_tri,
					gv_global::rnd_opt.m_max_debug_line);
	return true;
};
void gv_debug_renderer_gameplay3d::finalize_gameplay3d()
{
	m_impl->finalize_gameplay3d_resource();
};

gv_debug_renderer_gameplay3d* gv_debug_renderer_gameplay3d::static_get()
{
	return s_debug_draw_gameplay3d;
};
Font* gv_debug_renderer_gameplay3d::get_debug_font()
{
	return m_impl->m_font;
};
void gv_debug_renderer_gameplay3d::set_view_projection_matrix_3d(
	const Matrix& matrix)
{
	m_impl->m_world_view_projection_matrix_3d = matrix;
}
const Matrix& gv_debug_renderer_gameplay3d::get_view_projection_matrix_3d()
{
	return m_impl->m_world_view_projection_matrix_3d;
};
MeshBatch* gv_debug_renderer_gameplay3d::get_line_batch()
{
	return m_impl->m_debug_line_batch;
}

int gv_debug_renderer_gameplay3d::get_line_vertex(void*& pv)
{
	pv = (void*)m_impl->last_vb_line().get_data();
	return m_impl->last_vb_line().size();
};

//=====================================================================================
}
