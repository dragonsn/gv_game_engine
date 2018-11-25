#include <vector>
#include <set>

namespace gv
{
gv_static_mesh::gv_static_mesh()
{
	m_is_instancing_copy = false;
	GVM_SET_CLASS(gv_static_mesh);
}
void gv_static_mesh::rebuild_binormal_tangent()
{
	using namespace std;
	// Calculate neighbor information for every vertex,
	// consisting of an array with all adjacent vertices.
	vector< set< int > > aAdjacentVertices(get_nb_vertex());
	gv_index_buffer* pib = this->m_index_buffer;
	gv_vertex_buffer* pvb = this->m_vertex_buffer;
	for (int iTriangle = 0; iTriangle < get_nb_triangle(); ++iTriangle)
	{
		aAdjacentVertices[pib->get_index(3 * iTriangle + 0)].insert(
			pib->get_index(3 * iTriangle + 1));
		aAdjacentVertices[pib->get_index(3 * iTriangle + 0)].insert(
			pib->get_index(3 * iTriangle + 2));
		aAdjacentVertices[pib->get_index(3 * iTriangle + 1)].insert(
			pib->get_index(3 * iTriangle + 2));
		aAdjacentVertices[pib->get_index(3 * iTriangle + 1)].insert(
			pib->get_index(3 * iTriangle + 0));
		aAdjacentVertices[pib->get_index(3 * iTriangle + 2)].insert(
			pib->get_index(3 * iTriangle + 0));
		aAdjacentVertices[pib->get_index(3 * iTriangle + 2)].insert(
			pib->get_index(3 * iTriangle + 1));
	}

	// Calculate a deltaU for each vertex and use it as weight
	// for each vector from the center vertex to the neighboring
	// vertex.
	pvb->m_raw_tangent.resize(get_nb_vertex());
	pvb->m_raw_tangent = gv_vector3::get_zero_vector();
	pvb->m_raw_binormal.resize(get_nb_vertex());
	pvb->m_raw_binormal = gv_vector3::get_zero_vector();
	for (int iVertex = 0; iVertex < get_nb_vertex(); ++iVertex)
	{
		for (set< int >::const_iterator iAdjacent =
				 aAdjacentVertices[iVertex].begin();
			 iAdjacent != aAdjacentVertices[iVertex].end(); ++iAdjacent)
		{
			float nDeltaU = pvb->m_raw_texcoord0[(*iAdjacent)].x -
							pvb->m_raw_texcoord0[iVertex].x;
			gv_vector3 vDeltaP = pvb->m_raw_pos[*iAdjacent] - pvb->m_raw_pos[iVertex];
			float d = vDeltaP.dot(vDeltaP);
			if (gvt_is_almost_zero(d))
			{
				d = 0.00001f;
			}
			pvb->m_raw_tangent[iVertex] += vDeltaP * (nDeltaU / d);
		}
		// Calculate the binormal as the cross-product of the tangent
		// and the normal.
		pvb->m_raw_binormal[iVertex] =
			pvb->m_raw_normal[iVertex].cross(pvb->m_raw_tangent[iVertex]);
		// If degenerate case we try alternative tangents,
		// until we're successful.
		float nLength = pvb->m_raw_binormal[iVertex].normalize();
		while (gvt_is_almost_zero(nLength))
		{
			pvb->m_raw_tangent[iVertex] += gv_vector3(0.1f, -0.2f, 0.3f);
			pvb->m_raw_binormal[iVertex] =
				pvb->m_raw_normal[iVertex].cross(pvb->m_raw_tangent[iVertex]);
			nLength = pvb->m_raw_binormal[iVertex].normalize();
		}
		pvb->m_raw_binormal[iVertex].normalize();
		if (pvb->m_raw_binormal[iVertex].x < -1.0 ||
			pvb->m_raw_binormal[iVertex].x > 1.0)
		{
			GV_DEBUG_BREAK;
		}
		if (pvb->m_raw_binormal[iVertex].x != pvb->m_raw_binormal[iVertex].x)
		{
			GV_DEBUG_BREAK;
		}
		pvb->m_raw_tangent[iVertex] =
			pvb->m_raw_binormal[iVertex].cross(pvb->m_raw_normal[iVertex]);
	}
}

// rebuld normal
void gv_static_mesh::rebuild_normal(bool smooth)
{
	if (!this->m_vertex_buffer || !this->m_index_buffer)
		return;
	gv_int nb_vertex = this->get_nb_vertex();
	gv_index_buffer* pib = this->m_index_buffer;
	gv_int nb_index = pib->get_nb_index();
	m_vertex_buffer->m_raw_normal.resize(get_nb_vertex());
	if (!smooth)
	{
		for (int i = 0; i < nb_index; i += 3)
		{
			gv_vector3 v1 = m_vertex_buffer->m_raw_pos[pib->get_index(i + 0)];
			;
			gv_vector3 v2 = m_vertex_buffer->m_raw_pos[pib->get_index(i + 1)];
			;
			gv_vector3 v3 = m_vertex_buffer->m_raw_pos[pib->get_index(i + 2)];
			;
			gv_vector3 normal = gv_plane(v1, v2, v3).normal;
			m_vertex_buffer->m_raw_normal[pib->get_index(i + 0)] = normal;
			m_vertex_buffer->m_raw_normal[pib->get_index(i + 1)] = normal;
			m_vertex_buffer->m_raw_normal[pib->get_index(i + 2)] = normal;
		}
	}
	else
	{
		m_vertex_buffer->m_raw_normal = gv_vector3::get_zero_vector();
		for (int i = 0; i < nb_index; i += 3)
		{
			gv_vector3 v1 = m_vertex_buffer->m_raw_pos[pib->get_index(i + 0)];
			gv_vector3 v2 = m_vertex_buffer->m_raw_pos[pib->get_index(i + 1)];
			gv_vector3 v3 = m_vertex_buffer->m_raw_pos[pib->get_index(i + 2)];
			gv_vector3 normal = gv_plane(v1, v2, v3).normal;
			m_vertex_buffer->m_raw_normal[pib->get_index(i + 0)] += normal;
			m_vertex_buffer->m_raw_normal[pib->get_index(i + 1)] += normal;
			m_vertex_buffer->m_raw_normal[pib->get_index(i + 2)] += normal;
		}

		for (int i = 0; i != get_nb_vertex(); ++i)
			m_vertex_buffer->m_raw_normal[i].normalize();
	}
	// for ( int i = 0; i!= get_nb_vertex(); ++i )
	//	m_vertex_buffer->m_raw_normal[i].negate();
}

void gv_static_mesh::rebuild_bounding_volumn()
{
	if (!this->m_vertex_buffer)
		return;
	this->m_local_bounding_box.empty();
	for (int i = 0; i < this->m_vertex_buffer->m_raw_pos.size(); i++)
	{
		m_local_bounding_box.add(this->m_vertex_buffer->m_raw_pos[i]);
	}
	this->m_local_bounding_box.get_sphere(m_local_bounding_sphere.pos,
										  m_local_bounding_sphere.r);
};

void gv_static_mesh::create_planar_tex_mapping()
{
	gv_vertex_buffer* pvb = this->m_vertex_buffer;
	if (!pvb)
		return;
	pvb->m_raw_texcoord0.resize(pvb->m_raw_pos.size());
	pvb->m_raw_texcoord0 = gv_vector2(0, 0);
};

void gv_static_mesh::create_sphere_tex_mapping(){

};

void gv_static_mesh::transform_pos(const gv_matrix44& mat)
{
	if (!this->m_vertex_buffer)
		return;
	for (int i = 0; i < this->m_vertex_buffer->m_raw_pos.size(); i++)
	{
		this->m_vertex_buffer->m_raw_pos[i] =
			mat.mul_by(this->m_vertex_buffer->m_raw_pos[i]);
	}
	this->rebuild_bounding_volumn();
	this->m_vertex_buffer->uncache();
};
void gv_static_mesh::transform_normal(const gv_matrix44& mat)
{
	if (!this->m_vertex_buffer)
		return;
	for (int i = 0; i < this->m_vertex_buffer->m_raw_normal.size(); i++)
	{
		this->m_vertex_buffer->m_raw_normal[i] =
			mat.rotate(this->m_vertex_buffer->m_raw_normal[i]);
	}
	this->m_vertex_buffer->uncache();
};

struct large_vertex
{
	large_vertex()
	{
		GVM_ZERO_ME;
	}
	gv_vector3 m_raw_pos;
	gv_vector3 m_raw_normal;
	gv_vector2 m_raw_texcoord0;
	gv_vector2 m_raw_texcoord1;
	gv_vector2 m_raw_texcoord2;
	gv_vector2 m_raw_texcoord3;
	gv_int m_old_index;
	gv_int m_new_index;
	large_vertex* next_vertex;

	bool operator==(const large_vertex& v)
	{
		return (m_raw_pos == v.m_raw_pos && m_raw_normal == v.m_raw_normal &&
				m_raw_texcoord0 == v.m_raw_texcoord0 &&
				m_raw_texcoord1 == v.m_raw_texcoord1 &&
				m_raw_texcoord2 == v.m_raw_texcoord2 &&
				m_raw_texcoord3 == v.m_raw_texcoord3);
	}
};

inline gv_int gv_get_grid(gv_float x, gv_float min_x, gv_float max_x,
						  gv_int grid_size)
{
	gv_int ix;
	gv_float dx = x - min_x;
	gv_float delta = max_x - min_x;
	if (!gvt_value< gv_float >::is_almost_zero(delta))
	{
		dx = (dx * grid_size) / delta;
		ix = gvt_clamp((gv_int)dx, 0, grid_size - 1);
	}
	else
		ix = 0;
	return ix;
}

static const int devide_grid_number = 128;
int get_grid_index_3d(const gv_vector3i& grid_size, gv_box& box,
					  const gv_vector3& pos)
{
	gv_int ix, iy, iz;
	ix = gv_get_grid(pos.get_x(), box.min_p.get_x(), box.max_p.get_x(),
					 grid_size.get_x());
	iy = gv_get_grid(pos.get_y(), box.min_p.get_y(), box.max_p.get_y(),
					 grid_size.get_y());
	iz = gv_get_grid(pos.get_z(), box.min_p.get_z(), box.max_p.get_z(),
					 grid_size.get_z());
	return ix + iy * grid_size.x + iz * grid_size.x * grid_size.y;
}
// amazing , if use the ?= operator the override assign op will not work!!!
#define GVM_FILL_BIG_VERTEX(x)               \
	if (pvb->m_raw_##x.size())               \
		big_v.m_raw_##x = pvb->m_raw_##x[i]; \
	else                                     \
		big_v.m_raw_##x = 0;

#define GVM_FILL_NEW_VERTEX(x)                                        \
	if (old_vb->m_raw_##x.size())                                     \
	{                                                                 \
		new_vb->m_raw_##x.resize(new_vertex_number);                  \
		for (int i = 0; i < new_vertex_number; i++)                   \
		{                                                             \
			large_vertex& lv = big_vertex_buffer[i];                  \
			new_vb->m_raw_##x[i] = old_vb->m_raw_##x[lv.m_old_index]; \
		}                                                             \
	}

void gv_static_mesh::merge_optimize_vertex()
{

	if (!get_nb_vertex())
		return;
	this->rebuild_bounding_volumn();
	gvt_array< gv_int > new_index_map; // contain new index;
	gvt_array< large_vertex* > grid_3d;
	gvt_array< large_vertex, gvp_memory_default, gvp_single_thread,
			   gvp_array_size_calculator_dynamic >
		big_vertex_buffer;

	big_vertex_buffer.reserve(this->get_nb_vertex() * 4);
	//!!the big vertex buffer should not be resized!! for we need to use pointer
	//!to point to the vertex inside.
	large_vertex* pfirst_begin = big_vertex_buffer.begin();

	new_index_map.resize(get_nb_vertex());

	gv_vector3i grid_size;
	gv_float box_round = m_local_bounding_box.get_xdelta() +
						 m_local_bounding_box.get_ydelta() +
						 m_local_bounding_box.get_zdelta();
	if (!box_round)
		return; // strange
	grid_size.set_x(
		(gv_int)((m_local_bounding_box.get_xdelta() / box_round) * 256.f) + 1);
	grid_size.set_y(
		(gv_int)((m_local_bounding_box.get_ydelta() / box_round) * 256.f) + 1);
	grid_size.set_z(
		(gv_int)((m_local_bounding_box.get_zdelta() / box_round) * 256.f) + 1);

	grid_3d.resize(grid_size.get_x() * grid_size.get_y() * grid_size.get_z());
	grid_3d = 0;
	gv_vertex_buffer* pvb = this->m_vertex_buffer;

	for (int i = 0; i < this->get_nb_vertex(); i++)
	{
		large_vertex big_v;

		GVM_FILL_BIG_VERTEX(pos)
		GVM_FILL_BIG_VERTEX(normal)
		GVM_FILL_BIG_VERTEX(texcoord0)
		GVM_FILL_BIG_VERTEX(texcoord1)
		GVM_FILL_BIG_VERTEX(texcoord2)
		GVM_FILL_BIG_VERTEX(texcoord3)
		big_v.m_old_index = i;

		gv_int grid_index =
			get_grid_index_3d(grid_size, m_local_bounding_box, big_v.m_raw_pos);
		large_vertex* plv = grid_3d[grid_index];
		while (plv)
		{
			if (*plv == big_v)
				break;
			plv = plv->next_vertex;
		}
		if (plv)
		{
			// the vertex already there!!
			new_index_map[i] = plv->m_new_index;
		}
		else
		{
			// add the big v;
			big_v.m_new_index = big_vertex_buffer.size();
			big_vertex_buffer.push_back(big_v);
			GV_ASSERT(pfirst_begin == big_vertex_buffer.begin());
			large_vertex* pnew = big_vertex_buffer.last();
			pnew->next_vertex = grid_3d[grid_index];
			grid_3d[grid_index] = pnew;
			new_index_map[i] = big_v.m_new_index;
		}
	}
	// okey_let's rebuild the vertex buffer & index buffer;
	//============================================
	gv_vertex_buffer* old_vb = this->m_vertex_buffer;
	gv_vertex_buffer* new_vb = get_sandbox()->create_object< gv_vertex_buffer >();
	gv_index_buffer* new_iv = get_sandbox()->create_object< gv_index_buffer >();
	new_iv->m_raw_index_buffer.resize(
		this->m_index_buffer->m_raw_index_buffer.size());
	for (int i = 0; i < new_iv->m_raw_index_buffer.size(); i++)
	{
		gv_int index = this->m_index_buffer->m_raw_index_buffer[i];
		new_iv->m_raw_index_buffer[i] = new_index_map[index];
	}
	//============================================
	int new_vertex_number = big_vertex_buffer.size();
	/*
  if (old_vb->m_raw_pos.size() )
  {
          new_vb->m_raw_pos.resize(new_vertex_number);
          for ( int i=0;i< new_vertex_number ; i++ )
          {
                  large_vertex & lv=big_vertex_buffer[i];
                  new_vb->m_raw_pos[i]=old_vb->m_raw_pos[lv.m_old_index];
          }
  }*/
	GVM_FILL_NEW_VERTEX(pos);
	GVM_FILL_NEW_VERTEX(normal);
	GVM_FILL_NEW_VERTEX(binormal);
	GVM_FILL_NEW_VERTEX(tangent);
	GVM_FILL_NEW_VERTEX(blend_index);
	GVM_FILL_NEW_VERTEX(blend_weight);
	GVM_FILL_NEW_VERTEX(color);
	GVM_FILL_NEW_VERTEX(texcoord0);
	GVM_FILL_NEW_VERTEX(texcoord1);
	GVM_FILL_NEW_VERTEX(texcoord2);
	GVM_FILL_NEW_VERTEX(texcoord3);
	GVM_FILL_NEW_VERTEX(texcoord4);
	GVM_FILL_NEW_VERTEX(texcoord5);
	GVM_FILL_NEW_VERTEX(texcoord6);
	GVM_FILL_NEW_VERTEX(texcoord7);
	new_vb->set_owner(this);
	new_iv->set_owner(this);
	gv_index_buffer* old_ib = this->m_index_buffer;
	this->m_vertex_buffer = new_vb;
	this->m_index_buffer = new_iv;
	this->get_sandbox()->delete_object(old_vb);
	this->get_sandbox()->delete_object(old_ib);
	//============================================
}

void gv_static_mesh::rebuild_normal_smooth()
{
	// use position to merge the vertex.
	if (!get_nb_vertex())
		return;

	gvt_array< gv_int > new_index_map; // contain new index;
	gvt_array< large_vertex* > grid_3d;
	gvt_array< large_vertex > big_vertex_buffer;

	big_vertex_buffer.reserve(this->get_nb_vertex() * 4);
	//!!the big vertex buffer should not be resized!! for we need to use pointer
	//!to point to the vertex inside.
	large_vertex* pfirst_begin = big_vertex_buffer.begin();
	new_index_map.resize(get_nb_vertex());

	gv_vector3i grid_size;
	gv_float box_round = m_local_bounding_box.get_xdelta() +
						 m_local_bounding_box.get_ydelta() +
						 m_local_bounding_box.get_zdelta();
	if (!box_round)
		return; // strange
	grid_size.set_x(
		(gv_int)((m_local_bounding_box.get_xdelta() / box_round) * 256.f) + 1);
	grid_size.set_y(
		(gv_int)((m_local_bounding_box.get_ydelta() / box_round) * 256.f) + 1);
	grid_size.set_z(
		(gv_int)((m_local_bounding_box.get_zdelta() / box_round) * 256.f) + 1);
	grid_3d.resize(grid_size.get_x() * grid_size.get_y() * grid_size.get_z());
	grid_3d = 0;
	gv_vertex_buffer* pvb = this->m_vertex_buffer;

	for (int i = 0; i < this->get_nb_vertex(); i++)
	{
		large_vertex big_v;

		GVM_FILL_BIG_VERTEX(pos)
		big_v.m_old_index = i;
		gv_int grid_index =
			get_grid_index_3d(grid_size, m_local_bounding_box, big_v.m_raw_pos);
		large_vertex* plv = grid_3d[grid_index];
		while (plv)
		{
			if (*plv == big_v)
				break;
			plv = plv->next_vertex;
		}
		if (plv)
		{
			// the vertex already there!!
			new_index_map[i] = plv->m_new_index;
		}
		else
		{
			// add the big v;
			big_v.m_new_index = big_vertex_buffer.size();
			big_vertex_buffer.push_back(big_v);
			GV_ASSERT(pfirst_begin == big_vertex_buffer.begin());
			large_vertex* pnew = big_vertex_buffer.last();
			pnew->next_vertex = grid_3d[grid_index];
			grid_3d[grid_index] = pnew;
			new_index_map[i] = big_v.m_new_index;
		}
	}

	// merge the normal
	m_vertex_buffer->m_raw_normal = gv_vector3::get_zero_vector();
	gvt_array< gv_vector3 > new_normals;
	new_normals.resize(big_vertex_buffer.size());
	int nb_index = get_ib()->get_nb_index();
	for (int i = 0; i < nb_index; i += 3)
	{
		int new_idx[3];
		for (int j = 0; j < 3; j++)
		{
			new_idx[j] = new_index_map[get_ib()->get_index(i + j)];
		}
		// int idx0=new_index_map[]
		gv_vector3 v1 = big_vertex_buffer[new_idx[0]].m_raw_pos;
		gv_vector3 v2 = big_vertex_buffer[new_idx[1]].m_raw_pos;
		gv_vector3 v3 = big_vertex_buffer[new_idx[2]].m_raw_pos;
		gv_vector3 normal = gv_plane(v1, v2, v3).normal;
		new_normals[new_idx[0]] += normal;
		new_normals[new_idx[1]] += normal;
		new_normals[new_idx[2]] += normal;
	}
	for (int i = 0; i != new_normals.size(); ++i)
	{
		new_normals[i].normalize();
	}
	for (int i = 0; i < new_index_map.size(); i++)
	{
		this->get_vb()->m_raw_normal[i] = new_normals[new_index_map[i]];
	}
}

gv_int gv_static_mesh::get_nb_vertex()
{
	if (this->m_vertex_buffer)
	{
		return this->m_vertex_buffer->m_raw_pos.size();
	}
	return 0;
}

gv_int gv_static_mesh::get_nb_normal()
{
	if (this->m_vertex_buffer)
	{
		return this->m_vertex_buffer->m_raw_normal.size();
	}
	return 0;
}

gv_int gv_static_mesh::get_nb_triangle()
{
	if (this->m_index_buffer)
	{
		return this->m_index_buffer->m_raw_index_buffer.size() / 3;
	}
	return 0;
}

gv_int gv_static_mesh::get_index(int i)
{
	return this->m_index_buffer->m_raw_index_buffer[i];
};

gv_int gv_static_mesh::get_nb_index()
{
	if (this->m_index_buffer)
	{
		return this->m_index_buffer->m_raw_index_buffer.size();
	}
	return 0;
};

void gv_static_mesh::prepare_vertex_stream(gv_uint stream_decl)
{
	gv_vertex_buffer* pvb = this->m_vertex_buffer;
	if (!pvb)
		return;
	if (stream_decl & e_vtx_with_normal && !pvb->m_raw_normal.size())
	{
		this->rebuild_normal();
	}
	if (stream_decl & e_vtx_with_tangent && !pvb->m_raw_tangent.size())
	{
		this->rebuild_binormal_tangent();
	}
	// always with texturecoord
	if (/*stream_decl& e_vtx_with_texcoord0&&*/ !pvb->m_raw_texcoord0.size())
	{
		this->create_planar_tex_mapping();
	}
	if (stream_decl & e_vtx_with_texcoord1 && !pvb->m_raw_texcoord1.size())
	{
		pvb->m_raw_texcoord1 = pvb->m_raw_texcoord0;
	}
	if (stream_decl & e_vtx_with_texcoord2 && !pvb->m_raw_texcoord2.size())
	{
		pvb->m_raw_texcoord1 = pvb->m_raw_texcoord0;
	}
	if (stream_decl & e_vtx_with_texcoord3 && !pvb->m_raw_texcoord3.size())
	{
		pvb->m_raw_texcoord1 = pvb->m_raw_texcoord0;
	}
	if (stream_decl & e_vtx_with_texcoord4 && !pvb->m_raw_texcoord4.size())
	{
		pvb->m_raw_texcoord1 = pvb->m_raw_texcoord0;
	}
}

void gv_static_mesh::create_vb_ib()
{
	if (!this->is_nameless())
	{
		this->m_vertex_buffer =
			get_sandbox()->create_object< gv_vertex_buffer >(this);
		this->m_index_buffer = get_sandbox()->create_object< gv_index_buffer >(this);
	}
	else
	{
		this->m_vertex_buffer =
			get_sandbox()->create_nameless_object< gv_vertex_buffer >();
		this->m_index_buffer =
			get_sandbox()->create_nameless_object< gv_index_buffer >();
	}
};

void gv_static_mesh::create_vb_ib(gv_int vertex_size, gv_uint stream_decl,
								  gv_int index_size)
{
	create_vb_ib();
	this->m_index_buffer->m_raw_index_buffer.resize(index_size);
	gv_vertex_buffer* pvb = this->m_vertex_buffer;
	pvb->m_raw_pos.resize(vertex_size);
	if (stream_decl & e_vtx_with_normal)
	{
		pvb->m_raw_normal.resize(vertex_size);
		pvb->m_raw_normal = gv_vector3(0, 0, 1);
	}
	if (stream_decl & e_vtx_with_tangent)
	{
		pvb->m_raw_tangent.resize(vertex_size);
		pvb->m_raw_tangent = gv_vector3(1, 0, 0);
	}
	if (stream_decl & e_vtx_with_binormal)
	{
		pvb->m_raw_binormal.resize(vertex_size);
		pvb->m_raw_binormal = gv_vector3(0, 1, 0);
	}
	if (stream_decl & e_vtx_with_texcoord0)
	{
		pvb->m_raw_texcoord0.resize(vertex_size);
	}
	if (stream_decl & e_vtx_with_texcoord1)
	{
		pvb->m_raw_texcoord1.resize(vertex_size);
	}
	if (stream_decl & e_vtx_with_texcoord2)
	{
		pvb->m_raw_texcoord2.resize(vertex_size);
	}
	if (stream_decl & e_vtx_with_texcoord3)
	{
		pvb->m_raw_texcoord3.resize(vertex_size);
	}
	if (stream_decl & e_vtx_with_texcoord4)
	{
		pvb->m_raw_texcoord4.resize(vertex_size);
	}
	if (stream_decl & e_vtx_with_blend_index)
	{
		pvb->m_raw_blend_index.resize(vertex_size);
		pvb->m_raw_blend_index = gv_vector4i(0, 0, 0, 0);
	}
	if (stream_decl & e_vtx_with_blend_weight)
	{
		pvb->m_raw_blend_weight.resize(vertex_size);
		pvb->m_raw_blend_weight = gv_vector4(1, 0, 0, 0);
	}
}

void gv_static_mesh::add_segment(gv_int start_index, gv_material* material,
								 gv_int size)
{
	gv_mesh_segment* pseg = this->m_segments.add_dummy();
	pseg->m_material = material;
	pseg->m_start_index = start_index;
	pseg->m_index_size = size == -1 ? this->get_nb_index() : size;
	return;
};

gv_mesh_segment* gv_static_mesh::get_segment(gv_int idx)
{
	if (idx < m_segments.size())
		return &m_segments[idx];
	return NULL;
};

gv_int gv_static_mesh::get_nb_segment()
{
	return m_segments.size();
};

void gv_static_mesh::create_skin_vertex()
{
	get_vb()->m_raw_blend_weight.resize(get_nb_vertex());
	get_vb()->m_raw_blend_index.resize(get_nb_vertex());
	get_vb()->m_raw_blend_weight = gv_vector4(0, 0, 0, 0);
	get_vb()->m_raw_blend_index = gv_vector4i(-1, -1, -1, -1);
};

void gv_static_mesh::set_nb_segment(gv_int nb)
{
	this->m_segments.resize(nb);
}

void gv_static_mesh::create_instancing_copy(gv_int nb_batch)
{
	this->m_instancing_copy =
		get_sandbox()->create_nameless_object< gv_static_mesh >();
	this->m_instancing_copy->m_is_instancing_copy = true;
	this->get_instancing_copy()->create_vb_ib(
		0, get_vb()->get_biggest_vertex_format(), 0);
	gv_vertex_buffer* new_vb = this->get_instancing_copy()->get_vb();
	gv_vertex_buffer* old_vb = this->get_vb();
	for (int i = 0; i < nb_batch; i++)
	{
		new_vb->m_raw_pos += old_vb->m_raw_pos;
		new_vb->m_raw_normal += old_vb->m_raw_normal;
		new_vb->m_raw_binormal += old_vb->m_raw_binormal;
		new_vb->m_raw_tangent += old_vb->m_raw_tangent;
		new_vb->m_raw_texcoord0 += old_vb->m_raw_texcoord0;
		new_vb->m_raw_texcoord1 += old_vb->m_raw_texcoord1;
		new_vb->m_raw_texcoord2 += old_vb->m_raw_texcoord2;
		new_vb->m_raw_texcoord3 += old_vb->m_raw_texcoord3;
		new_vb->m_raw_texcoord4 += old_vb->m_raw_texcoord4;
		new_vb->m_raw_texcoord5 += old_vb->m_raw_texcoord5;
		new_vb->m_raw_texcoord6 += old_vb->m_raw_texcoord6;
		new_vb->m_raw_texcoord7 += old_vb->m_raw_texcoord7;
		new_vb->m_raw_color += old_vb->m_raw_color;
		new_vb->m_raw_blend_index.add(gv_vector4i(i, 0, 0, 0),
									  this->get_nb_vertex());
		new_vb->m_raw_blend_weight.add(gv_vector4(1, 0, 0, 0),
									   this->get_nb_vertex());
	}
	gv_index_buffer* new_ib = this->get_instancing_copy()->get_ib();
	gv_index_buffer* old_ib = this->get_ib();
	new_ib->m_raw_index_buffer.reserve(this->get_nb_index() * nb_batch);
	this->get_instancing_copy()->set_nb_segment(this->get_nb_segment());
	for (int seg = 0; seg < this->get_nb_segment(); seg++)
	{
		gv_mesh_segment* old_seg = this->get_segment(seg);
		gv_mesh_segment* new_seg = this->get_instancing_copy()->get_segment(seg);
		new_seg->m_start_index = new_ib->m_raw_index_buffer.size();
		new_seg->m_index_size = old_seg->m_index_size * nb_batch;
		new_seg->m_material = old_seg->m_material;
		for (int batch = 0; batch < nb_batch; batch++)
		{
			gv_int offset = batch * get_nb_vertex();
			for (int index = 0; index < old_seg->m_index_size; index++)
			{
				new_ib->m_raw_index_buffer.push_back(
					offset +
					old_ib->m_raw_index_buffer[old_seg->m_start_index + index]);
			}
		}
	}
};
gv_static_mesh* gv_static_mesh::get_instancing_copy()
{
	return this->m_instancing_copy;
};

GVM_IMP_CLASS(gv_static_mesh, gv_resource)
GVM_VAR(gv_box, m_local_bounding_box)
GVM_VAR(gv_sphere, m_local_bounding_sphere)
GVM_VAR(gvt_array< gv_mesh_segment >, m_segments)
GVM_VAR(gvt_ref_ptr< gv_vertex_buffer >, m_vertex_buffer)
GVM_VAR(gvt_ref_ptr< gv_index_buffer >, m_index_buffer)
GVM_VAR(gvt_ref_ptr< gv_texture >, m_diffuse_texture)
GVM_END_CLASS

GVM_BGN_FUNC(gv_static_mesh, scp_rebuild_normal)
this->rebuild_normal();
this->rebuild_binormal_tangent();
this->m_vertex_buffer->uncache();
return 1;
GVM_END_FUNC

GVM_BGN_FUNC(gv_static_mesh, scp_rebuild_normal_smooth)
this->rebuild_normal_smooth();
this->rebuild_binormal_tangent();
this->m_vertex_buffer->uncache();
return 1;
GVM_END_FUNC

GVM_BGN_FUNC(gv_static_mesh, scp_rebuild_bounding)
this->rebuild_bounding_volumn();
return 1;
GVM_END_FUNC

GVM_BGN_FUNC(gv_static_mesh, scp_rescale)
GVM_PARAM(gv_vector3, scale)
gv_matrix44 mat;
mat.set_identity();
mat.set_scale(scale);
transform_pos(mat);
return 1;
GVM_END_FUNC

GVM_BGN_FUNC(gv_static_mesh, scp_rotate)
GVM_PARAM(gv_euler, rotation)
gv_matrix44 mat;
gv_math::convert(mat, rotation);
transform_pos(mat);
rebuild_binormal_tangent();
return 1;
GVM_END_FUNC
}