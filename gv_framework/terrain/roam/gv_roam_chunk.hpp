namespace gv
{
struct tri2d
{
	tri2d(){};
	tri2d(gv_int x0, gv_int y0, gv_int x1, gv_int y1, gv_int x2, gv_int y2,
		  gv_int tree_pos)
	{
		this->x0 = x0;
		this->y0 = y0;
		this->x1 = x1;
		this->y1 = y1;
		this->x2 = x2;
		this->y2 = y2;
		this->tree_pos = tree_pos;
	};
	gv_int x0, y0;
	gv_int x1, y1;
	gv_int x2, y2;
	gv_int tree_pos;
};

//============================================================================================
//								:
//============================================================================================
gv_roam_chunk::gv_roam_chunk()
{
	GVM_SET_CLASS(gv_roam_chunk)
	m_is_cache_dirty = true;
	m_pass_flag = 0;
}

gv_roam_chunk::~gv_roam_chunk()
{
}

gv_float gv_roam_chunk::compute_variance_recursively(
	const tri2d& tri, gvt_array< gv_float >& variancetable2d)
{

	gv_int centrex = (tri.x1 + tri.x2) / 2;
	gv_int centrey = (tri.y1 + tri.y2) / 2;
	gv_float leftz = this->get_height(tri.x2, tri.y2);
	gv_float rightz = this->get_height(tri.x1, tri.y1);
	gv_float middlez = this->get_height(tri.x0, tri.y0);
	gv_float centrez = this->get_height(centrex, centrey);
	gv_float variance = 0;
	// algo1 :simplest
	variance =
		fabsf(centrez -
			  (leftz + rightz) /
				  2.0f); //*100/sqrt((leftz-rightz)*(leftz-rightz)+patchsize)  ;
						 ///**(abs(tri.x1-tri.x2)+abs(tri.y1-tri.y2))/2*/;
	//{//algo2
	//	gv_vector3 v[]=
	//	{
	//		gv_vector3((gv_float)tri.x1, (gv_float)tri.y1, rightz),
	//		gv_vector3((gv_float)tri.x2, (gv_float)tri.y2, leftz),
	//		gv_vector3((gv_float)tri.x0, (gv_float)tri.y0, middlez),
	//		gv_vector3((gv_float)centrex, (gv_float)centrey, centrez),
	//	};
	//	gv_plane p0	  ( v[0], v[1],v[2] );
	//	gv_plane p_l   ( v[0], v[3],v[2] );
	//	gv_plane p_r   ( v[3], v[1],v[2] );
	//	variance=p0.normal.dot(p_l.normal);
	//	variance+=p0.normal.dot(p_r.normal);
	//	variance=2.0f-variance;
	//}
	if (tri.tree_pos < ((VARIANCE_FULL_TREE_SIZE) / 2))
	{
		variancetable2d[tri.tree_pos] =
			// left child
			compute_variance_recursively(tri2d(centrex, centrey, tri.x0, tri.y0,
											   tri.x1, tri.y1, tri.tree_pos * 2),
										 variancetable2d) +
			// right child
			compute_variance_recursively(tri2d(centrex, centrey, tri.x2, tri.y2,
											   tri.x0, tri.y0,
											   tri.tree_pos * 2 + 1),
										 variancetable2d) +
			variance;
		return variancetable2d[tri.tree_pos];
	}
	else
	{
		variancetable2d[tri.tree_pos] = variance;
		return variance;
	}
}

void gv_roam_chunk::compute_variance()
{

	/////////////////////////////////////////////
	// first  compute left tree
	// then compute right tree
	//
	m_variance_l.resize(VARIANCE_FULL_TREE_SIZE);
	m_variance_r.resize(VARIANCE_FULL_TREE_SIZE);
	m_variance_l = 0.f;
	m_variance_r = 0.f;

	tri2d tri;
	tri.x0 = 0;
	tri.y0 = GV_ROAM_PATCH_SIZE;
	tri.x1 = GV_ROAM_PATCH_SIZE;
	tri.y1 = GV_ROAM_PATCH_SIZE;
	tri.x2 = 0;
	tri.y2 = 0;
	tri.tree_pos = 1;
	compute_variance_recursively(tri, this->m_variance_l);

	tri.x0 = GV_ROAM_PATCH_SIZE;
	tri.y0 = 0;
	tri.x1 = 0;
	tri.y1 = 0;
	tri.x2 = GV_ROAM_PATCH_SIZE;
	tri.y2 = GV_ROAM_PATCH_SIZE;
	tri.tree_pos = 1;
	compute_variance_recursively(tri, this->m_variance_r);
}

gv_float gv_roam_chunk::get_height(gv_int x, gv_int y)
{
	x = gvt_clamp(x, 0, GV_ROAM_PATCH_SIZE - 1);
	y = gvt_clamp(y, 0, GV_ROAM_PATCH_SIZE - 1);
	return this->m_height_map[x + (y * GV_ROAM_PATCH_SIZE)];
}

void gv_roam_chunk::get_normal(gv_int x, gv_int y, gv_vector4b& norm,
							   gv_vector4b& binormal, gv_vector4b& tangent)
{
	x = gvt_clamp(x, 0, GV_ROAM_PATCH_SIZE - 1);
	y = gvt_clamp(y, 0, GV_ROAM_PATCH_SIZE - 1);
	gv_int offset = x + (y * GV_ROAM_PATCH_SIZE);
	norm = this->m_normals[offset];
	binormal = this->m_binormals[offset];
	tangent = this->m_tangents[offset];
};

gv_int gv_roam_chunk::initialize(const gv_float* pheightmap, gv_int stride,
								 bool need_dynamic_lod)
{
	int idx = 0;
	m_is_cache_dirty = true;
	m_height_map.resize(GV_ROAM_PATCH_SIZE_SQUARED);
	for (int i = 0; i < GV_ROAM_PATCH_SIZE; i++)
	{
		for (int j = 0; j < GV_ROAM_PATCH_SIZE; j++)
		{
			this->m_height_map[idx++] = *pheightmap++;
		}
		pheightmap -= GV_ROAM_PATCH_SIZE;
		pheightmap += stride;
	}
	this->build_aabb();
	if (need_dynamic_lod)
	{
		this->compute_variance();
	}
	return true;
}

gv_int gv_roam_chunk::initialize_normal(const gv_vector3* normals,
										const gv_vector3* binormals,
										const gv_vector3* tangents,
										gv_int stride)
{
	int idx = 0;
	m_normals.resize(GV_ROAM_PATCH_SIZE_SQUARED);
	m_binormals.resize(GV_ROAM_PATCH_SIZE_SQUARED);
	m_tangents.resize(GV_ROAM_PATCH_SIZE_SQUARED);
	for (int i = 0; i < GV_ROAM_PATCH_SIZE; i++)
	{
		for (int j = 0; j < GV_ROAM_PATCH_SIZE; j++)
		{
			this->m_normals[idx] = gv_to_normalized_byte_v4(*normals++);
			this->m_binormals[idx] = gv_to_normalized_byte_v4(*binormals++);
			this->m_tangents[idx++] = gv_to_normalized_byte_v4(*tangents++);
		}
		normals -= GV_ROAM_PATCH_SIZE;
		normals += stride;
		binormals -= GV_ROAM_PATCH_SIZE;
		binormals += stride;
		tangents -= GV_ROAM_PATCH_SIZE;
		tangents += stride;
	}
	return true;
};

const gv_box& gv_roam_chunk::get_local_aabb()
{
	return this->m_aabb;
};

void gv_roam_chunk::build_aabb()
{
	gv_float max_h = -100000.f;
	gv_float min_h = 100000.f;
	for (int i = 0; i < GV_ROAM_PATCH_SIZE; i++)
		for (int j = 0; j < GV_ROAM_PATCH_SIZE; j++)
		{
			gv_float h = get_height(i, j);
			max_h = gvt_max(h, max_h);
			min_h = gvt_min(h, min_h);
		}
	// don't forget our world y point  to up
	this->m_aabb.min_p = roam_to_world(gv_vector3(0, 0, min_h));
	this->m_aabb.max_p =
		roam_to_world(gv_vector3((gv_float)GV_ROAM_PATCH_SIZE + 1,
								 (gv_float)GV_ROAM_PATCH_SIZE + 1, max_h));
};

GVM_IMP_CLASS(gv_roam_chunk, gv_resource)
// GVM_VAR(gvt_array<gv_float >	,m_variance_l )
// GVM_VAR(gvt_array<gv_float >	,m_variance_r )
GVM_VAR(gvt_array< gv_float >, m_height_map)
GVM_VAR(gvt_ptr< gv_material >, m_material)
GVM_VAR(gv_box, m_aabb)
GVM_VAR(gvt_array< gv_vector4b >, m_normals)
GVM_VAR(gvt_array< gv_vector4b >, m_binormals)
GVM_VAR(gvt_array< gv_vector4b >, m_tangents)
GVM_VAR(gv_uint, m_pass_flag)
GVM_END_CLASS
}