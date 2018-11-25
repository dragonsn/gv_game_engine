
#pragma once
#include <boost/boost/pool/singleton_pool.hpp>
#include "../terrain/roam/gv_roam_chunk.hpp"
#include "../terrain/roam/gv_roam_patch.hpp"
#include "../terrain/roam/gv_roam_node.hpp"

namespace gv
{
#define MAX_FRAGMENT_PER_CHUNK 65536
#define ERROR_THRESHOLD 5.f
gv_com_terrain_roam::gv_com_terrain_roam()
{
	GVM_SET_CLASS(gv_com_terrain_roam);
	this->m_com_render_id[gve_render_pass_opaque] = gv_id("gv_com_roam_renderer");
	m_has_visible_callback = true;
	m_is_wireframe_mode = false;
	m_lod_threshold = ERROR_THRESHOLD;
	m_updated = false;
	m_init_height_scale = 1.0f;
	m_island_generation_enabled = false;
	m_do_export_normal_map = true;
	m_enable_dynamic_LOD = false;
	m_size_in_patch = m_size_in_patch = gv_vector2i::get_zero_vector();
};

gv_com_terrain_roam::~gv_com_terrain_roam()
{
	this->release_map();
}

bool gv_com_terrain_roam::set_resource(gv_resource* resource)
{
	super::set_resource(resource);
	gv_string_tmp name =
		get_sandbox()->get_physical_resource_path(*resource->get_file_name());
	if (name.has_postfix("raw") || name.has_postfix("RAW"))
	{
		return this->init_from_raw_file(name);
	}
	return false;
};

bool gv_com_terrain_roam::init_from_raw_file(const gv_string_tmp& in_name,
											 gv_int w, gv_int h)
{
	if (!this->get_resource())
	{
		gv_resource* pres = get_sandbox()->create_object< gv_resource >(this);
		pres->set_file_name(in_name);
		this->set_resource(pres);
	}
	gvt_array< gv_byte > array;
	gv_string_tmp name;
	name = get_sandbox()->get_physical_resource_path(in_name);
	if (!gv_load_file_to_array(name, array))
	{
		GVM_ERROR("failed to load raw file " << in_name << " as terrain heightmap "
											 << gv_endl);
		return false;
	}
	return this->init_from_byte_array(array, w, h);
};

bool gv_com_terrain_roam::init_from_image_file(const gv_string_tmp& in_name)
{
	if (!this->get_resource())
	{
		gv_resource* pres = get_sandbox()->create_object< gv_resource >(this);
		pres->set_file_name(in_name);
		this->set_resource(pres);
	}
	gv_image_2d image;
	image.m_sandbox = this->get_sandbox();
	if (!image.import_file(*in_name))
	{
		GVM_ERROR("failed to load raw file " << in_name << " as terrain heightmap "
											 << gv_endl);
		return false;
	}
	gvt_array< gv_byte > array;
	gv_vector2i size;
	size = image.get_image_size();
	gv_int array_size = size.get_x() * size.get_y();
	array.resize(array_size);
	for (int i = 0; i < array_size; i++)
		array[i] = image.m_pixels[i].r;
	return this->init_from_byte_array(array, size.get_x(), size.get_y());
};

bool gv_com_terrain_roam::init_from_byte_array(gvt_array< gv_byte >& array,
											   gv_int w, gv_int h)
{
	gvt_array< gv_float > float_array;
	gvt_array< gv_float > float_array_2;
	float_array = array;
	if (w == 0)
	{
		w = (gv_int)sqrtf((float)array.size());
		h = w;
	}
	float_array_2.resize(float_array.size());
	gvt_gaussian_blur(float_array.begin(), float_array_2.begin(), w, h);
	gvt_gaussian_blur(float_array_2.begin(), float_array.begin(), w, h);
	for (int i = 0; i < float_array.size(); i++)
		float_array[i] *= this->m_init_height_scale;
	return this->init(float_array, w, h);
};

bool gv_com_terrain_roam::init(gvt_array< gv_float >& pheightmap, gv_int w,
							   gv_int l)
{
	GV_ASSERT(w % GV_ROAM_PATCH_SIZE == 0);
	GV_ASSERT(l % GV_ROAM_PATCH_SIZE == 0);
	GV_ASSERT(w >= GV_ROAM_PATCH_SIZE);
	GV_ASSERT(l >= GV_ROAM_PATCH_SIZE);
	build_island(pheightmap, w, l);
	m_updated = true;
	m_size_in_point.set(w, l);
	m_size_in_patch = m_size_in_point / GV_ROAM_PATCH_SIZE;
	this->m_map.resize(m_size_in_patch.x * m_size_in_patch.y);
	gv_int x = 0, y = 0;
	this->m_world_aabb.empty();
	gvt_array< gv_vector3 > normals;
	gvt_array< gv_vector3 > binormals;
	gvt_array< gv_vector3 > tangents;
	rebuild_normal(pheightmap, w, l, normals, binormals, tangents);
	if (m_do_export_normal_map)
	{
		// here it comes!!!
		gv_image_2d* pimage = get_sandbox()->create_object< gv_image_2d >();
		pimage->init_image(gv_color::RED(), gv_vector2i(w, l));
		;
		for (int i = 0; i < l; i++)
		{
			for (int j = 0; j < w; j++)
			{
				gv_vector3 n = normals[i * w + j];
				n.normalize();
				n *= 0.5;
				n += 0.5;
				gv_color c(n.x, n.y, n.z, 1.0f);
				pimage->get_pixel(gv_vector2i(j, i)) = c;
			}
		}
		pimage->export_file("terrain_normal.tga");
		get_sandbox()->delete_object(pimage, true);
	}

	for (gv_int iy = 0; iy < this->m_size_in_patch.y; iy++)
	{
		for (gv_int ix = 0; ix < this->m_size_in_patch.x; ix++)
		{
			gv_roam_patch* patch = get_patch(gv_vector2i(ix, iy));
			patch->m_chunk = get_sandbox()->create_object< gv_roam_chunk >(this);
			gv_uint offset = ix * GV_ROAM_PATCH_SIZE +
							 iy * GV_ROAM_PATCH_SIZE * m_size_in_point.get_x();
			gv_uint stride = m_size_in_point.x;
			const float* f = &pheightmap[offset];
			patch->m_chunk->initialize(f, stride, m_enable_dynamic_LOD);
			patch->m_chunk->initialize_normal(&normals[offset], &binormals[offset],
											  &tangents[offset], stride);
			patch->m_offset =
				gv_vector2i(ix * GV_ROAM_PATCH_SIZE, iy * GV_ROAM_PATCH_SIZE);
			patch->m_chunk_name = patch->m_chunk->get_name();
			patch->m_is_visible = false;
			patch->m_terrain = this;
			patch->update_world_aabb();
			this->m_world_aabb.add(patch->get_world_aabb());
		}
	}
	return true;
};

void gv_com_terrain_roam::rebuild_normal(gvt_array< gv_float >& height_map,
										 gv_int w, gv_int h,
										 gvt_array< gv_vector3 >& normals,
										 gvt_array< gv_vector3 >& binormals,
										 gvt_array< gv_vector3 >& tangents)
{
	normals.resize(w * h);
	binormals.resize(w * h);
	tangents.resize(w * h);
	for (int iy = 0; iy < h; iy++)
		for (int ix = 0; ix < w; ix++)
		{
			gv_vector3 norm;
			gv_float d_z0 = gvt_get_pixel(ix, iy - 1, height_map, w, h);
			gv_float d_z1 = gvt_get_pixel(ix, iy + 1, height_map, w, h);
			gv_float d_x0 = gvt_get_pixel(ix - 1, iy, height_map, w, h);
			gv_float d_x1 = gvt_get_pixel(ix + 1, iy, height_map, w, h);
			float d_h_x = (d_x1 - d_x0) / 2.f;
			float d_h_z = (d_z1 - d_z0) / 2.f;

			gv_vector3 v_x(2.0f, d_h_x, 0.0f);
			gv_vector3 v_z(0.f, d_h_z, 2.0f);
			v_x.normalize();
			v_z.normalize();
			norm = v_z.cross(v_x);
			norm.normalize();
			normals[ix + iy * w] = norm;
			binormals[ix + iy * w] = -v_x;
			tangents[ix + iy * w] = -v_z;
		}
	gvt_array< gv_vector3 > n_cpy;
	n_cpy.resize(w * h);
	gvt_gaussian_blur(normals.begin(), n_cpy.begin(), w, h);
	gvt_gaussian_blur(n_cpy.begin(), normals.begin(), w, h);
};

void gv_com_terrain_roam::reset()
{
	clear_roam_node_stack();
	for (gv_int i = 0; i < m_visible_image.size(); i++)
	{
		m_visible_image[i]->reset();
	}
	this->m_visible_image.clear();
	m_tri_number = 0;
}

void gv_com_terrain_roam::get_normalf(gv_float x, gv_float y,
									  gv_vector3& norm)
{
	gv_float d_z0 = get_heightf(x, y - 1);
	gv_float d_z1 = get_heightf(x, y + 1);
	gv_float d_x0 = get_heightf(x - 1, y);
	gv_float d_x1 = get_heightf(x + 1, y);
	float d_h_x = (d_x1 - d_x0) / 2.f;
	float d_h_z = (d_z1 - d_z0) / 2.f;

	gv_vector3 v_x(2.0f, d_h_x, 0.0f);
	gv_vector3 v_z(0.f, d_h_z, 2.0f);
	v_x.normalize();
	v_z.normalize();
	norm = v_z.cross(v_x);
	/*
          norm.x=(this->get_heightf(x-1,y)-this->get_heightf(x+1,y))/2;
          norm.y=(this->get_heightf(x,y-1)-this->get_heightf(x,y+1))/2;
          norm.z=2.0f;
          norm.normalize();
  */
}

gv_vector3 gv_com_terrain_roam::get_normal_world(const gv_vector3& world_pos)
{
	gv_vector3 n;
	gv_vector3 pos;
	pos = world_to_roam(world_pos);
	get_normalf(pos.get_x(), pos.get_y(), n);
	return n;
};

void gv_com_terrain_roam::tessellate(gv_com_camera* pcamera)
{
	GV_PROFILE_EVENT(gv_com_terrain_roam__tessellate, 0)
	this->link_patch();
	gv_vector3 v = pcamera->get_world_position();
	for (gv_int i = 0; i < m_visible_image.size(); i++)
	{
		{
			gv_float dist =
				(m_visible_image[i]->m_world_aabb.get_center() - v).length();
			gv_float err = dist / m_lod_threshold;
			m_visible_image[i]->tessellate(err);
		}
	}
}

void gv_com_terrain_roam::link_patch()
{

	// first link all the visible patch together
	gv_roam_patch* ppatch;
	for (gv_int i = 0; i < this->m_size_in_patch.y; i++)
		for (gv_int j = 0; j < this->m_size_in_patch.x; j++)
		{
			ppatch = get_patch(j, i);
			if (ppatch->is_visible())
			{
				//
				//				|------/|
				//				| l  /  |
				//				|  /    |
				//				|/------|
				//					0

				if (i > 0 && this->get_patch(j, (i - 1))->is_visible())
				{
					ppatch->get_rroot()->set_right_neighbour(
						this->get_patch(j, (i - 1))->get_lroot());
				}

				//
				//				|------/|
				//				| l  /  | o
				//				|  /    |
				//				|/------|

				if (j < m_size_in_patch.x - 1 &&
					this->get_patch(j + 1, i)->is_visible())
				{
					ppatch->get_rroot()->set_left_neighbour(
						this->get_patch(j + 1, i)->get_lroot());
				}
				//
				//				|------/ |
				//			o   | l  /   |
				//				|  /     |
				//				|/-------|

				if (j > 0 && this->get_patch(j - 1, i)->is_visible())
				{
					ppatch->get_lroot()->set_left_neighbour(
						this->get_patch(j - 1, i)->get_rroot());
				}
				//				    o
				//				|------/|
				//			    | l  /  |
				//				|  /    |
				//				|/------|
				if (i < m_size_in_patch.y - 1 &&
					this->get_patch(j, i + 1)->is_visible())
				{
					ppatch->get_lroot()->set_right_neighbour(
						this->get_patch(j, i + 1)->get_rroot());
				}
			}
		}
}

void gv_com_terrain_roam::on_visible(class gv_com_camera* pcam,
									 gv_ulong visible_test_tag)
{
	GV_PROFILE_EVENT(gv_com_terrain_roam__on_visible, 0)
	gv_int i;
	this->reset();
	gvt_array_cached< gvt_ptr< gv_roam_patch >, 512 >& image = this->m_visible_image;
	for (i = 0; i < this->m_map.size(); i++)
	{
		if (pcam->get_world_frustum().intersect(this->m_map[i].get_world_aabb()))
		// if (!i)
		{
			image.add(&this->m_map[i]);
			this->m_map[i].reset();
			this->m_map[i].set_visibility(true);
			if (is_wireframe_mode())
				get_sandbox()->get_debug_renderer()->draw_box_3d(
					this->m_map[i].get_world_aabb(), gv_color::BLUE_B());
			gv_roam_patch* patch = &this->m_map[i];
			patch->set_visible_tag(visible_test_tag + 1);
		}
		else
		{
			this->m_map[i].set_visibility(false);
		}
	}
	if (this->is_dynamic_LOD_on())
		this->tessellate(pcam);
}

void gv_com_terrain_roam::release_map()
{
}
gv_float gv_com_terrain_roam::get_height(gv_int x, gv_int y)
{
	x = gvt_clamp(x, 0, m_size_in_point.x - 1);
	y = gvt_clamp(y, 0, m_size_in_point.y - 1);
	return this->get_patch_unsafe(x / GV_ROAM_PATCH_SIZE, y / GV_ROAM_PATCH_SIZE)
		->get_height(x % GV_ROAM_PATCH_SIZE, y % GV_ROAM_PATCH_SIZE);
}

void gv_com_terrain_roam::get_normal(gv_int x, gv_int y, gv_vector4b& norm,
									 gv_vector4b& binormal,
									 gv_vector4b& tangent)
{
	x = gvt_clamp(x, 0, m_size_in_point.x - 1);
	y = gvt_clamp(y, 0, m_size_in_point.y - 1);
	return this->get_patch_unsafe(x / GV_ROAM_PATCH_SIZE, y / GV_ROAM_PATCH_SIZE)
		->get_normal(x % GV_ROAM_PATCH_SIZE, y % GV_ROAM_PATCH_SIZE, norm,
					 binormal, tangent);
};

void gv_com_terrain_roam::get_normal(gv_int x, gv_int y, gv_vector3& normal)
{
	gv_vector4b norm;
	gv_vector4b binormal;
	gv_vector4b tangent;
	this->get_normal(x, y, norm, binormal, tangent);
	normal = gv_from_normalized_byte_v4(norm);
};

gv_vector3
gv_com_terrain_roam::terrain_local_to_world(const gv_vector3& roam_pos)
{
	gv_vector3 world_pos = roam_to_world(roam_pos);
	world_pos = get_entity()->get_tm().mul_by(world_pos);
	return world_pos;
};

gv_vector3
gv_com_terrain_roam::world_to_terrain_local(const gv_vector3& world_pos_old)
{
	gv_vector3 world_pos = get_entity()->get_inv_tm().mul_by(world_pos_old);
	gv_vector3 roam_pos = world_to_roam(world_pos);
	return roam_pos;
};
gv_float
gv_com_terrain_roam::get_height_world(const gv_vector3& world_pos_old)
{
	gv_vector3 world_pos = get_entity()->get_inv_tm().mul_by(world_pos_old);
	gv_vector3 roam_pos = world_to_roam(world_pos);
	return get_height_interpolated(roam_pos.get_x(), roam_pos.get_y()) *
			   get_entity()->get_scale().y +
		   get_entity()->get_position().get_y();
};

gv_rect gv_com_terrain_roam::world_to_terrain_local(const gv_box& world_aabb)
{
	gv_vector3 a = world_to_terrain_local(world_aabb.min_p);
	gv_vector3 b = world_to_terrain_local(world_aabb.max_p);
	if (a.get_x() > b.get_x())
		gvt_swap(a.x, b.x);
	if (a.get_y() > b.get_y())
		gvt_swap(a.y, b.y);
	return gv_rect(a.get_x(), a.get_y(), b.get_x(), b.get_y());
};

gv_box
gv_com_terrain_roam::terrain_local_to_world(const gv_rect& terrain_rect)
{
	gv_vector3 a = terrain_rect.min_p;
	gv_vector3 b = terrain_rect.max_p;
	a.set_z(get_heightf(a.get_x(), a.get_y()));
	b.set_z(get_heightf(b.get_x(), b.get_y()));
	a = terrain_local_to_world(a);
	b = terrain_local_to_world(b);
	gv_box box;
	box.add(a);
	box.add(b);
	return box;
};

gv_float gv_com_terrain_roam::get_heightf(gv_float fx, gv_float fy)
{
	return this->get_height((gv_int)fx, (gv_int)fy);
}

gv_float gv_com_terrain_roam::get_height_interpolated(gv_float x, gv_float y)
{
	gv_float maxx = ceilf(x);
	gv_float minx = floorf(x);

	gv_float maxy = ceilf(y);
	gv_float miny = floorf(y);

	gv_float z1, z2, z3, z4, z5, z6;

	z1 = this->get_height((gv_int)minx, (gv_int)miny);
	z2 = this->get_height((gv_int)minx, (gv_int)maxy);
	z3 = gvt_lerp(z1, z2, y - miny);

	z4 = this->get_height((gv_int)maxx, (gv_int)miny);
	z5 = this->get_height((gv_int)maxx, (gv_int)maxy);
	z6 = gvt_lerp(z4, z5, y - miny);

	return gvt_lerp(z3, z6, x - minx);
};

bool gv_com_terrain_roam::intersect_world_line_seg(
	const gv_line_segment& line_seg, float& s)
{
	if (!m_map.size())
	{
		return false;
	}
	gv_line_segment result;
	gv_line ray(line_seg.start_p, line_seg.dir());
	if (gv_geom::clip_line_against_box_3d(line_seg, m_world_aabb, result))
	{
		s = ray.get_param(result.start_p);
		gv_vector2 p0 = world_to_terrain_local(result.start_p).v2;
		gv_vector2 p1 = world_to_terrain_local(result.end_p).v2;
		gv_vector2 pp0 = p0 / (gv_float)GV_ROAM_PATCH_SIZE;
		gv_vector2 pp1 = p1 / (gv_float)GV_ROAM_PATCH_SIZE;
		gv_vector3 hit_pos;
		gv_bool hit = false;
		gvt_trace_line2d(pp0, pp1,
						 boost::bind(&gv_com_terrain_roam::intersect_patch_line,
									 this, line_seg, &hit_pos, &hit, _1, _2));
		if (hit)
		{
			hit_pos.y = get_height_world(hit_pos);
			s = ray.get_param(hit_pos);
			return true;
		}
	}
	return false;
};

bool gv_com_terrain_roam::intersect_patch_line(const gv_line_segment& line_seg,
											   gv_vector3* phit_point,
											   gv_bool* phit, gv_int ix,
											   gv_int iy)
{
	gv_roam_patch* patch = get_patch(ix, iy);
	gv_vector3& hit_point = *phit_point;
	gv_bool& hit = *phit;
	// select_patch(patch, true);
	// to do: optimization;
	gv_line_segment result;
	hit = false;
	if (gv_geom::clip_line_against_box_3d(line_seg, patch->get_world_aabb(),
										  result))
	{
		result.start_p = world_to_terrain_local(result.start_p);
		result.end_p = world_to_terrain_local(result.end_p);
		gv_vector3 v0 = result.start_p;
		gv_float dist = result.length();
		if (gvt_is_almost_zero(dist))
		{
			hit_point = v0;
			hit = true;
			hit_point.z = get_heightf(hit_point.x, hit_point.y);
			hit_point = terrain_local_to_world(hit_point);
			return false;
		}
		gv_vector3 dir = result.dir();
		gv_vector3 last_pos = v0;
		gv_float last_height = get_heightf(v0.x, v0.y);
		for (float i = 0; i < dist + 1.1f; i += 1.0f)
		{
			v0 += dir;
			gv_float height = get_heightf(v0.x, v0.y);
			if ((last_height - last_pos.z) * (height - v0.z) <= 0)
			{
				// hit happen;
				float alpha =
					(gvt_abs(last_height - last_pos.z)) /
					(gvt_abs(last_height - last_pos.z) + gvt_abs(height - v0.z));
				hit_point = gvt_lerp(v0, last_pos, alpha);
				hit_point.z = get_heightf(hit_point.x, hit_point.y);
				hit_point = terrain_local_to_world(hit_point);
				hit = true;
				return false;
			}
			last_pos = v0;
			last_height = height;
		}
	}
	return true;
};

/*bool  gv_com_terrain_roam::find_line_collision (const gv_line_segment & line ,
gv_float max_len, gvt_array_cached<gv_vector3, 32> &xpoints, bool
stop_at_first_hit)
//{
//	bool  ret=false;

        // first clamp the line into the map region
        gv_line_segment l=line;
        gv_rect rect(0,0,(gv_float)this->m_size_in_point.x,
(gv_float)this->m_size_in_point.y);
        if ( gv_geom::clip(line,rect,l)  && l.length()>0.01f )
        {
                gv_float dx=line.end_p.x-line.start_p.x;
                gv_float dy=line.end_p.y-line.start_p.y;
                gv_float dz=line.end_p.z-line.start_p.z;
                if (gvt_is_almost_zero(dx) && gvt_is_almost_zero(dy))
                {
                        gv_float a =this->get_height(line.start_p.x
,line.start_p.y);
                        if ((a-l.start_p.z)*(a-l.end_p.z)>0.0f  )
                        {
                                return false;
                        }
                        else
                        {
                                return true;
                        }
                }
                else
                {
                        //Bresenham version
                        gv_int xa=(gv_int)l.start_p.x;		gv_int
ya=(gv_int)l.start_p.y;
                        gv_int xb=(gv_int)l.end_p.x;
gv_int yb=(gv_int)l.end_p.y;
                        gv_int dx=gvt_abs(xa-xb);
gv_int dy=gvt_abs(ya-yb);
                        if	(gvt_abs(dx ) > gvt_abs(dy))
                        {
                                gv_int p=(gv_int)2*dy -dx;
                                gv_int two_dy =2*dy, two_dy_dx =2*(dy-dx);
                                gv_int x, y, x_end;
                                gv_int w=this->m_size_in_point.x;
                                gv_int y_sgn=1;
                                gv_float zinc , z;
                                if (xa>xb)
                                {
                                        x=xb; 	y=yb; x_end=xa;
                                        z=l.end_p.z;
zinc=(l.start_p.z -l.end_p.z ) /(x_end-x);
                                        if (dy) y_sgn=dy/(ya-yb);

                                }
                                else
                                {
                                        x=xa; 	y=ya; 	x_end=xb;
                                        z=l.start_p.z;zinc=(l.end_p.z
-l.start_p.z ) /(x_end-x);
                                        if (dy) y_sgn=dy/(yb-ya);
                                }
                                gv_float * ph=this->m_height_map.begin()+x+y*w;
                                bool  pre_side_flag=z<(*ph);
                                while(x<x_end)
                                {
                                        x++; z+=zinc;ph++;
                                        if (p<0)
                                        {
                                                p+=two_dy;
                                        }
                                        else
                                        {
                                                y+=y_sgn;
p+=two_dy_dx;ph+=w*y_sgn;
                                        }
                                        bool  flag=z<*ph;
                                        if (flag !=pre_side_flag)
                                        {//hit happened
                                                xpoints.add(gv_vector3((gv_float
)x,(gv_float) y, *ph));
                                                ret=true;
                                                if (stop_at_first_hit) break;
                                        }
                                        pre_side_flag=flag;
                                }
                        }
                        else
                        {
                                gv_int p=(gv_int)2*dx -dy;
                                gv_int two_dx =2*dx, two_dx_dy =2*(dx-dy);
                                gv_int x, y, y_end;
                                gv_int w=this->m_size_in_point.x+1;
                                gv_int x_sgn=1;
                                gv_float zinc , z;
                                if (ya>yb)
                                {
                                        x=xb; 	y=yb; y_end=ya;
                                        z=l.end_p.z;
zinc=(l.start_p.z -l.end_p.z ) /(y_end-y);
                                        if (dx) x_sgn=dx/(xa-xb);

                                }
                                else
                                {
                                        x=xa; 	y=ya; 	y_end=yb;
                                        z=l.start_p.z;zinc=(l.end_p.z
-l.start_p.z ) /(y_end-y);
                                        if (dx) x_sgn=dx/(xb-xa);
                                }
                                gv_float * ph=this->m_height_map.begin()+x+y*w;
                                bool  pre_side_flag=z<(*ph);
                                while(y<y_end)
                                {
                                        y++; z+=zinc;ph+=w;
                                        if (p<0) {
                                                p+=two_dx;
                                        }
                                        else{
                                                x+=x_sgn;
p+=two_dx_dy;ph+=x_sgn;
                                        }
                                        bool  flag=z<*ph;
                                        if (flag !=pre_side_flag){//hit happend
                                                xpoints.add(gv_vector3((gv_float
)x,(gv_float) y, *ph));
                                                ret=true;
                                                if (stop_at_first_hit) break;
                                        }
                                        pre_side_flag=flag;
                                }
                        }
                }

        }
        return ret;
}*/

gv_byte* gv_com_terrain_roam::gen_light_map_with_sun(const gv_vector3& sun_dir,
													 gv_int w, gv_int l)
{
	return NULL;
	/*gv_byte * plightmap =new gv_byte[w*l*3];
  gv_byte * pt=plightmap;
  gv_float wscale=this->m_size_in_point.x/((gv_float )w);
  gv_float lscale=this->m_size_in_point.y/((gv_float )l);
  for ( gv_int i=0; i<w;i ++)
  {
          for (gv_int j=0; j<l; j++)
          {
                  gv_float x =(gv_float)j*wscale;
                  gv_float y =(gv_float)i*lscale;

                  gv_vector3 start_p( x, y, this->get_heightf(x,y)+0.001f);
                  gv_vector3 end_p;
                  end_p=start_p+sun_dir*1000.f;
                  gv_line_segment line(start_p,end_p);
                  gvt_array_cached<gv_vector3,32> xpoints;
                  if(this->find_line_collision(line,1000.f , xpoints))
                  {
                          *pt++=0x40;
                          *pt++=0x40;
                          *pt++=0x40;
                  }
                  else
                  {
                          gv_vector3 norm;
                          this->get_normal(x, y,norm);
                          gv_float a= norm.dot(sun_dir);
                          a=gvt_clamp(a,0.0f ,1.0f);
                          *pt++=(gv_byte)(a*180.f+64.f);
                          *pt++=(gv_byte)(a*180.f+64.f);
                          *pt++=(gv_byte)(a*180.f+64.f);
                  }

          }
  }
  return	plightmap;*/
}

gv_roam_patch* gv_com_terrain_roam::get_patch_unsafe(gv_int ix, gv_int iy)
{
	return &m_map[ix + iy * m_size_in_patch.get_x()];
}

gv_roam_patch* gv_com_terrain_roam::get_patch(const gv_vector2i& pos_in_patch)
{
	gv_int ix = gvt_clamp(pos_in_patch.get_x(), 0, m_size_in_patch.get_x() - 1);
	gv_int iy = gvt_clamp(pos_in_patch.get_y(), 0, m_size_in_patch.get_y() - 1);
	return get_patch_unsafe(ix, iy);
}

gv_roam_patch* gv_com_terrain_roam::get_patch(int ix, int iy)
{
	return get_patch(gv_vector2i(ix, iy));
}

gv_roam_patch*
gv_com_terrain_roam::get_patch_world(const gv_vector3& world_pos)
{
	gv_vector3 local_pos = get_entity()->get_inv_tm().mul_by(world_pos);
	gv_vector3 roam_pos = world_to_roam(local_pos);
	gv_vector2i v =
		gv_vector2i((gv_int)roam_pos.get_x(), (gv_int)roam_pos.get_y());
	return this->get_patch_by_point_pos(v);
};

gv_roam_patch*
gv_com_terrain_roam::get_patch_by_point_pos(const gv_vector2i& pos_in_points)
{
	gv_vector2i v = pos_in_points;
	v /= GV_ROAM_PATCH_SIZE;
	return get_patch(v);
}

bool gv_com_terrain_roam::fill_index(gv_int* pindex, gv_int& length,
									 gv_int max_length)
{
	GV_PROFILE_EVENT(gv_com_terrain_roam__fill_index, 0)
	length = 0;
	for (int i = 0; i < m_visible_image.size(); i++)
	{
		gv_int l = 0;
		gv_roam_patch* patch = m_visible_image[i];
		patch->fill_index_buffer(pindex, l, max_length);
		patch->m_index_buffer_offset = length;
		patch->m_index_buffer_size = l;
		length += l;
		pindex += l;
		m_tri_number += l / 3;
	}
	return true;
};

gv_int gv_com_terrain_roam::get_nb_visible_patch()
{
	return m_visible_image.size();
};

gv_roam_patch* gv_com_terrain_roam::get_nth_visible_patch(gv_int idx)
{
	return m_visible_image[idx];
};

void gv_com_terrain_roam::get_nth_patch_index_buffer(gv_int idx, gv_int& offset,
													 gv_int& size)
{
	gv_roam_patch* p = get_nth_visible_patch(idx);
	offset = p->m_index_buffer_offset;
	size = p->m_index_buffer_size;
};

bool gv_com_terrain_roam::sync_to_entity(gv_entity* entity)
{
	if (m_updated)
	{
		entity->set_local_aabb(get_local_bbox());
		entity->update_matrix();
		m_updated = false;
	}
	return true;
}
gv_box gv_com_terrain_roam::get_local_bbox()
{
	gv_box b = this->m_world_aabb;
	b = b.transform(get_entity()->get_inv_tm());
	return b;
};
gv_sphere gv_com_terrain_roam::get_local_bsphere()
{
	return gv_geom::get_bounding_sphere(get_local_bbox());
};

void gv_com_terrain_roam::enable_island_generation(gv_vector2i center,
												   gv_float height,
												   float power)
{
	m_island_generation_enabled = true;
	m_island_center = center;
	m_island_height = height;
	m_island_height_power = power;
};

void gv_com_terrain_roam::build_island(gvt_array< gv_float >& pheightmap,
									   gv_int w, gv_int l)
{
	if (!this->m_island_generation_enabled)
		return;
	if (m_island_center == gv_vector2i(0, 0))
	{
		m_island_center.set(w / 2, l / 2);
	}
	gv_vector2 center = gv_vector2((float)m_island_center.get_x(),
								   (float)m_island_center.get_y());
	for (int iy = 0; iy < l; iy++)
		for (int ix = 0; ix < w; ix++)
		{
			gv_vector2 v((float)ix, (float)iy);
			float dist = ((v - center) / gv_vector2((float)w, (float)l)).length();
			dist = powf(1.0f - dist, m_island_height_power);
			pheightmap[iy * w + ix] *= dist * m_island_height;
		}
}

void gv_com_terrain_roam::attach_a_renderable(gv_com_graphic* com)
{
	gv_roam_patch* patch =
		this->get_patch_world(com->get_entity()->get_position());
	com->enable_visible_test_tag();
	patch->attach_a_renderable(&com->get_visible_node());
};
void gv_com_terrain_roam::detach_a_renderable(gv_com_graphic* com)
{
	gv_roam_patch* patch =
		this->get_patch_world(com->get_entity()->get_position());
	com->enable_visible_test_tag(false);
	patch->detach_a_renderable(&com->get_visible_node());
};

void gv_com_terrain_roam::add_grass_layer(
	gv_material* grass_material, gv_float density, gv_float normal_factor,
	gv_vector2 height_range, float grass_height, float grass_width)
{
	gv_roam_patch* ppatch;
	if (!m_normal_texure)
		rebuild_normal_texture();
	for (gv_int i = 0; i < this->m_size_in_patch.y; i++)
		for (gv_int j = 0; j < this->m_size_in_patch.x; j++)
		{
			// SN_TEST	//if (i>5 ||j>5) continue;
			ppatch = get_patch(j, i);
			float height_0 = ppatch->get_world_aabb().min_p.y - 0.1f;
			float height_1 = ppatch->get_world_aabb().max_p.y + 0.1f;
			float h0, h1;
			if (!gvt_overlap_range(height_range.get_x(), height_range.get_y(),
								   height_0, height_1, h0, h1))
			{
				continue;
			};
			float height_factor = (h1 - h0) / (height_1 - height_0);
			if (height_factor <= 0.f)
				continue;
			float f_nb_grass = (density * height_factor);
			float h = ppatch->get_world_aabb().get_height();
			// gv_vector3 normal;
			// this->get_normalf((float)ppatch->m_offset.get_x(),
			// (float)ppatch->m_offset.get_y(), normal);
			// float cu_normal_factor=normal_factor*normal.get_y();//gvt_clamp( 1.f-
			// h/128.f, 0.f, 1.f);
			// f_nb_grass*=cu_normal_factor;
			int nb_grass = (int)f_nb_grass;
			if (nb_grass >= (int)density / 10)
			{
				//============================================================================================
				// create the grass static mesh component , and attach to the same
				// entity of terrain
				//============================================================================================
				gv_static_mesh* pmesh = get_sandbox()->create_object< gv_static_mesh >(
					get_sandbox()->get_unique_object_name(gv_id("grass_mesh")), this);
				;
				if (this->build_grass_mesh(nb_grass, pmesh, ppatch, grass_height,
										   grass_width, normal_factor,
										   height_range) > 10)
				{
					gv_com_static_mesh* com_static_mesh =
						get_sandbox()->create_object< gv_com_static_mesh >(
							(gv_object*)get_entity());
					gv_material* pmat = get_sandbox()->clone_object(grass_material);
					gv_material_tex tex;
					tex.m_id = "TexTerrainNormal";
					pmat->set_owner(this);
					tex.m_texture = m_normal_texure;
					pmat->add_material_texture(tex);
					gv_material_param_float4 terrain_dem;
					terrain_dem.m_id = gv_id("g_TerranDimention");
					float tw = (float)this->m_size_in_point.get_x();
					float th = (float)this->m_size_in_point.get_y();
					terrain_dem.m_data = gv_vector4(tw, th, 1.0f / tw, 1.0f / th);
					pmat->update_param_float4(terrain_dem);
					com_static_mesh->set_material(pmat);
					com_static_mesh->set_resource(pmesh);
					com_static_mesh->set_renderer_id(gve_render_pass_opaque,
													 gv_id("gv_com_effect_renderer"));
					com_static_mesh->set_always_visible(true);
					((gv_entity*)get_entity())->add_component(com_static_mesh);
					ppatch->attach_a_renderable(&com_static_mesh->get_visible_node());
					com_static_mesh->enable_visible_test_tag();
				}
				else
				{
					get_sandbox()->delete_object(pmesh);
				}
				// this->attach_a_renderable(com_static_mesh);
			}
		}
};

gv_int
gv_com_terrain_roam::build_grass_mesh(int nb_grass, gv_static_mesh* static_mesh,
									  gv_roam_patch* patch, float grass_height,
									  float grass_width, float normal_factor,
									  gv_vector2 height_range)
{
	static gvt_random< gv_float > random;
	gv_vector2i offset = patch->m_offset;
	static_mesh->create_vb_ib();
	static_mesh->get_vb()->m_raw_pos.reserve(nb_grass * 4);
	static_mesh->get_vb()->m_raw_texcoord0.reserve(nb_grass * 4);
	static_mesh->get_ib()->m_raw_index_buffer.reserve(nb_grass * 6);
	int cu_nb_grass = 0;
	int nb_idx = 0;
	for (int i = 0; i < nb_grass; i++)
	{
		float x = (float)offset.get_x() +
				  random.get_uniform() * (float)GV_ROAM_PATCH_SIZE;
		float y = (float)offset.get_y() +
				  random.get_uniform() * (float)GV_ROAM_PATCH_SIZE;
		gv_vector3 normal;
		get_normalf(x, y, normal);
		if (normal.y < normal_factor)
			continue;
		float angle = random.get_uniform() * gv_float_trait::pi() * 2.0f;
		gv_vector2 dir = gv_math::angle_to_dir(angle) * grass_width / 2.0f;
		float z = this->get_height_interpolated(x, y) - 0.1f;
		if (!gvt_between(z, height_range.get_x(), height_range.get_y()))
			continue;

		gv_vector3 v0 = gv_vector3(x, y, z) + dir;
		gv_vector3 v1 = gv_vector3(x, y, z) - dir;
		gv_vector3 v2 =
			gv_vector3(x, y, z) + dir + gv_vector3(0, 0, 1) * grass_height;
		gv_vector3 v3 =
			gv_vector3(x, y, z) - dir + gv_vector3(0, 0, 1) * grass_height;
		int cu_nb_v = static_mesh->get_vb()->m_raw_pos.size();
		static_mesh->get_vb()->m_raw_pos.push_back(roam_to_world(v0));
		static_mesh->get_vb()->m_raw_pos.push_back(roam_to_world(v1));
		static_mesh->get_vb()->m_raw_pos.push_back(roam_to_world(v2));
		static_mesh->get_vb()->m_raw_pos.push_back(roam_to_world(v3));

		static_mesh->get_vb()->m_raw_texcoord0.push_back(gv_vector2(0, 1));
		static_mesh->get_vb()->m_raw_texcoord0.push_back(gv_vector2(1, 1));
		static_mesh->get_vb()->m_raw_texcoord0.push_back(gv_vector2(0, 0));
		static_mesh->get_vb()->m_raw_texcoord0.push_back(gv_vector2(1, 0));

		static_mesh->get_ib()->m_raw_index_buffer.push_back(cu_nb_v);
		static_mesh->get_ib()->m_raw_index_buffer.push_back(cu_nb_v + 1);
		static_mesh->get_ib()->m_raw_index_buffer.push_back(cu_nb_v + 2);

		static_mesh->get_ib()->m_raw_index_buffer.push_back(cu_nb_v + 2);
		static_mesh->get_ib()->m_raw_index_buffer.push_back(cu_nb_v + 3);
		static_mesh->get_ib()->m_raw_index_buffer.push_back(cu_nb_v + 1);
	}
	static_mesh->rebuild_bounding_volumn();
	return static_mesh->get_vb()->m_raw_pos.size() / 4;
};

bool gv_com_terrain_roam::build_static_mesh(int detail_level,
											bool keep_terrain)
{
	GV_ASSERT(GV_ROAM_PATCH_SIZE % detail_level == 0);
	if (!this->m_map.size())
		return false;
	gv_roam_patch* patch;
	for (gv_int i = 0; i < this->m_size_in_patch.y; i++)
		for (gv_int j = 0; j < this->m_size_in_patch.x; j++)
		{
			patch = get_patch(j, i);
			gv_vector2i offset = patch->m_offset;
			gv_string_tmp mesh_name;
			mesh_name = "terrain_patch";
			mesh_name << "_" << j << "_" << i;
			gv_static_mesh* static_mesh =
				get_sandbox()->create_object< gv_static_mesh >(
					gv_object_name(*mesh_name), this);
			static_mesh->create_vb_ib();
			int s = GV_ROAM_PATCH_SIZE / detail_level;
			int nb_vertex = (s + 1) * (s + 1);
			int nb_index = s * s * 6;
			static_mesh->get_vb()->m_raw_pos.resize(nb_vertex);
			static_mesh->get_vb()->m_raw_texcoord0.resize(nb_vertex);
			static_mesh->get_vb()->m_raw_normal.resize(nb_vertex);
			static_mesh->get_ib()->m_raw_index_buffer.resize(nb_index);
			int cu_vertex = 0;
			int cu_index = 0;
			for (int iy = 0; iy <= GV_ROAM_PATCH_SIZE; iy += detail_level)
				for (int ix = 0; ix <= GV_ROAM_PATCH_SIZE;
					 ix += detail_level, cu_vertex++)
				{
					gv_vector3 v0 =
						gv_vector3((float)(offset.x + ix), (float)(offset.y + iy),
								   get_height(offset.x + ix, offset.y + iy));
					static_mesh->get_vb()->m_raw_pos[cu_vertex] = roam_to_world(v0);
					gv_vector3 normal;
					get_normal(offset.x + ix, offset.y + iy, normal);
					static_mesh->get_vb()->m_raw_normal[cu_vertex] = normal;
					gv_vector2 texcoord;
					texcoord = offset + gv_vector2i(ix, iy);
					texcoord /=
						gv_vector2((float)m_size_in_point.x, (float)m_size_in_point.y);
					static_mesh->get_vb()->m_raw_texcoord0[cu_vertex] = texcoord;
				}
			for (int iy = 0; iy < GV_ROAM_PATCH_SIZE; iy += detail_level)
				for (int ix = 0; ix < GV_ROAM_PATCH_SIZE; ix += detail_level)
				{
					static_mesh->get_ib()->m_raw_index_buffer[cu_index++] =
						iy * (s + 1) + ix;
					static_mesh->get_ib()->m_raw_index_buffer[cu_index++] =
						iy * (s + 1) + ix + 1;
					static_mesh->get_ib()->m_raw_index_buffer[cu_index++] =
						(iy + 1) * (s + 1) + ix + 1;
					static_mesh->get_ib()->m_raw_index_buffer[cu_index++] =
						iy * (s + 1) + ix;
					static_mesh->get_ib()->m_raw_index_buffer[cu_index++] =
						(iy + 1) * (s + 1) + ix + 1;
					static_mesh->get_ib()->m_raw_index_buffer[cu_index++] =
						(iy + 1) * (s + 1) + ix;
				}

			static_mesh->rebuild_bounding_volumn();
			gv_com_static_mesh* com_static_mesh =
				get_sandbox()->create_object< gv_com_static_mesh >(
					(gv_object*)get_entity());
			gv_material* pmat = get_sandbox()->clone_object(get_material());
			pmat->set_owner(com_static_mesh);
			com_static_mesh->set_material(pmat);
			com_static_mesh->set_resource(static_mesh);
			com_static_mesh->set_renderer_id(gve_render_pass_opaque,
											 gv_id("gv_com_effect_renderer"));
			((gv_entity*)get_entity())->add_component(com_static_mesh);
		}
	return true;
}

void gv_com_terrain_roam::select_all_patch()
{
	for (int i = 0; i < this->m_map.size(); i++)
	{
		this->m_map[i].m_is_selected = true;
	}
};

void gv_com_terrain_roam::unselect_all_patch()
{
	for (int i = 0; i < this->m_map.size(); i++)
	{
		this->m_map[i].m_is_selected = false;
	}
};

void gv_com_terrain_roam::select_patch(gv_roam_patch* patch, bool b)
{
	patch->m_is_selected = b;
};

void gv_com_terrain_roam::switch_wireframe()
{
	m_is_wireframe_mode = !m_is_wireframe_mode;
}

void gv_com_terrain_roam::rebuild_editor_data()
{
	if (m_editor_height_map.size())
		return;
	m_editor_height_map.resize(m_size_in_point.get_x() * m_size_in_point.get_y());
	for (int iy = 0; iy < m_size_in_point.get_y(); iy++)
		for (int ix = 0; ix < m_size_in_point.get_x(); ix++)
		{
			gvt_set_pixel(get_height(ix, iy), ix, iy, m_editor_height_map.begin(),
						  m_size_in_point.get_x(), m_size_in_point.get_y());
		}
	rebuild_normal(m_editor_height_map, m_size_in_point.get_x(),
				   m_size_in_point.get_y(), m_editor_normal, m_editor_binormal,
				   m_editor_tangent);
}

void gv_com_terrain_roam::collect_patch(
	const gv_rect& terrain_rect,
	gvt_array_cached< gv_roam_patch*, 256 >& result)
{

	gv_vector2 pp0 = terrain_rect.min_p / (gv_float)GV_ROAM_PATCH_SIZE;
	gv_vector2 pp1 = terrain_rect.max_p / (gv_float)GV_ROAM_PATCH_SIZE;
	gv_int x0 = (gv_int)gvt_floor(pp0.get_x());
	gv_int x1 = (gv_int)gvt_floor(pp1.get_x());
	gv_int y0 = (gv_int)gvt_floor(pp0.get_y());
	gv_int y1 = (gv_int)gvt_floor(pp1.get_y());
	for (int ix = x0; ix <= x1; ix++)
		for (int iy = y0; iy <= y1; iy++)
		{
			if (!gvt_between(ix, 0, m_size_in_patch.x - 1))
				continue;
			if (!gvt_between(iy, 0, m_size_in_patch.y - 1))
				continue;
			result.push_back(get_patch_unsafe(ix, iy));
		}
};

void gv_com_terrain_roam::update_heightmap(const gv_rect& position,
										   gvt_array< gv_float >& pattern,
										   gv_int operation, gv_float power)
{
	this->rebuild_editor_data();
	GV_ASSERT(pattern.size() == position.area());
	gv_int w = m_size_in_point.get_x();
	gv_int h = m_size_in_point.get_y();
	gv_rect r(0, 0, (gv_float)w, (gv_float)h);
	gv_rect result = r.clip(position);
	if (result.is_empty())
		return;
	for (int iy = (int)result.top; iy < (int)result.bottom; iy++)
		for (int ix = (int)result.left; ix < (int)result.right; ix++)
		{
			gv_float f = gvt_get_pixel(ix - (int)position.left,
									   iy - (int)position.top, pattern.begin(),
									   (int)position.width(), (int)position.height());
			f *= power;
			gv_float f_old = gvt_get_pixel(ix, iy, m_editor_height_map.begin(), w, h);
			switch (operation)
			{
			case 0:
				f = f + f_old;
				break;
			case 1:
				f = f_old - f;
				break;
			case 2:
				f = f * f_old;
				break;
			}
			gvt_set_pixel(f, ix, iy, m_editor_height_map.begin(), w, h);
		}

	for (int iy = (int)result.top; iy < (int)result.bottom; iy++)
		for (int ix = (int)result.left; ix < (int)result.right; ix++)
		{
			gv_vector3 norm;
			gv_float d_z0 =
				gvt_get_pixel(ix, iy - 1, m_editor_height_map.begin(), w, h);
			gv_float d_z1 =
				gvt_get_pixel(ix, iy + 1, m_editor_height_map.begin(), w, h);
			gv_float d_x0 =
				gvt_get_pixel(ix - 1, iy, m_editor_height_map.begin(), w, h);
			gv_float d_x1 =
				gvt_get_pixel(ix + 1, iy, m_editor_height_map.begin(), w, h);
			float d_h_x = (d_x1 - d_x0) / 2.f;
			float d_h_z = (d_z1 - d_z0) / 2.f;
			gv_vector3 v_x(2.0f, d_h_x, 0.0f);
			gv_vector3 v_z(0.f, d_h_z, 2.0f);
			v_x.normalize();
			v_z.normalize();
			norm = v_z.cross(v_x);
			norm.normalize();
			m_editor_normal[ix + iy * w] = norm;
			m_editor_binormal[ix + iy * w] = -v_x;
			m_editor_tangent[ix + iy * w] = -v_z;
		}

	gvt_array_cached< gv_roam_patch*, 256 > patches;
	this->collect_patch(result, patches);
	for (int i = 0; i < patches.size(); i++)
	{
		gv_roam_patch* patch = patches[i];
		gv_int ix = patch->get_offset().get_x();
		gv_int iy = patch->get_offset().get_y();
		gv_uint offset = ix + iy * w;
		gv_uint stride = w;
		const float* f = &m_editor_height_map[offset];
		patch->m_chunk->initialize(f, stride, false);
		patch->m_chunk->initialize_normal(&m_editor_normal[offset],
										  &m_editor_binormal[offset],
										  &m_editor_tangent[offset], stride);
		patch->update_world_aabb();
		this->m_world_aabb.add(patch->get_world_aabb());
	}
}

gv_terrain_grass_layer_info* gv_com_terrain_roam::get_grass_layer(gv_int idx)
{
	if (this->m_grass_layers.is_valid(idx))
	{
		return &m_grass_layers[idx];
	}
	return NULL;
};
gv_terrain_mesh_layer_info* gv_com_terrain_roam::get_mesh_layer(gv_int idx)
{
	if (this->m_mesh_layers.is_valid(idx))
	{
		return &m_mesh_layers[idx];
	}
	return NULL;
};
gv_terrain_fur_layer_info* gv_com_terrain_roam::get_fur_layer(gv_int idx)
{
	if (this->m_fur_layers.is_valid(idx))
	{
		return &m_fur_layers[idx];
	}
	return NULL;
};
gv_int
gv_com_terrain_roam::add_grass_layer(const gv_terrain_grass_layer_info& info)
{
	m_grass_layers.add(info);
	for (int i = 0; i < this->m_map.size(); i++)
	{
		gv_roam_patch& patch = this->m_map[i];
		patch.m_grass_layer_instances.add_dummy();
	}
	return m_grass_layers.size() - 1;
};
gv_int
gv_com_terrain_roam::add_mesh_layer(const gv_terrain_mesh_layer_info& info)
{
	for (int i = 0; i < this->m_map.size(); i++)
	{
		gv_roam_patch& patch = this->m_map[i];
		patch.m_mesh_layer_instances.add_dummy();
	}
	m_mesh_layers.add(info);
	return m_mesh_layers.size() - 1;
};
gv_int
gv_com_terrain_roam::add_fur_layer(const gv_terrain_fur_layer_info& info)
{
	m_fur_layers.add(info);
	gv_terrain_fur_layer_info* pinfo = m_fur_layers.last();
	pinfo->patch_mask.resize(m_size_in_patch.x * m_size_in_patch.y);
	pinfo->patch_mask = true;
	pinfo->fur_material = get_sandbox()->create_object< gv_material >(
		gv_object_name("fur_material"), this);
	pinfo->fur_material->set_effect(pinfo->fur_effect);
	pinfo->fur_density_tex = get_sandbox()->create_object< gv_texture >(
		gv_object_name("fur_blend"), this);
	pinfo->fur_density_tex->set_file_name(info.fur_density_tex_src.begin());
	return m_fur_layers.size() - 1;
};
gv_int gv_com_terrain_roam::remove_grass_layer(gv_int idx)
{
	if (!get_grass_layer(idx))
		return 0;
	m_grass_layers.erase(idx);
	for (int i = 0; i < this->m_map.size(); i++)
	{
		gv_roam_patch& patch = this->m_map[i];
		patch.m_grass_layer_instances.erase(idx);
	}
	return 1;
};

gv_int gv_com_terrain_roam::remove_mesh_layer(gv_int idx)
{
	if (!get_mesh_layer(idx))
		return 0;
	m_mesh_layers.erase(idx);
	for (int i = 0; i < this->m_map.size(); i++)
	{
		gv_roam_patch& patch = this->m_map[i];
		patch.m_mesh_layer_instances.erase(idx);
	}
	return 1;
};

gv_int gv_com_terrain_roam::remove_fur_layer(gv_int idx)
{
	if (!get_fur_layer(idx))
		return 0;
	m_fur_layers.erase(idx);
	return 1;
};

void gv_com_terrain_roam::update_mesh_layer(gv_int layer,
											const gv_rect& position,
											gvt_array< gv_float >& pattern,
											gv_int operation, gv_float power)
{
	gv_terrain_mesh_layer_info* layer_info = get_mesh_layer(layer);
	if (!layer_info)
		return;
	gv_int nb_instance = (gv_int)(power * position.area() / 10000.f);
	nb_instance = 1;
	if (!pattern.size())
		nb_instance = 1;
	if (operation == 0) // add
	{
		gvt_random_range< gv_float > random(-6.f, 6.f);
		gvt_random_range< gv_float > px(position.min_p.get_x(),
										position.max_p.get_x());
		gvt_random_range< gv_float > py(position.min_p.get_y(),
										position.max_p.get_y());
		for (int i = 0; i < nb_instance; i++)
		{
			gv_vector3 local(px(), py(), 0);
			local.set_z(get_heightf(local.get_x(), local.get_y()));
			// gv_box b=terrain_local_to_world(position);
			// gv_vector4 pos=b.max_p;
			gv_vector4 pos = terrain_local_to_world(local);
			pos.set_w(random());
			gv_roam_patch* patch = get_patch_world(pos);
			patch->m_mesh_layer_instances[layer].push_back(pos);
		}
	}
	else
	{
		//=remove
		gvt_array_cached< gv_roam_patch*, 256 > patches;
		this->collect_patch(position, patches);
		for (int ipatch = 0; ipatch < patches.size(); ipatch++)
		{
			gv_roam_patch* patch = patches[ipatch];
			for (int inst = 0; inst < patch->m_mesh_layer_instances[layer].size();
				 inst++)
			{
				gv_vector4 v = patch->m_mesh_layer_instances[layer][inst];
				gv_vector3 v_local = world_to_terrain_local(v);
				if (position.intersect(v_local.v2))
				{
					patch->m_mesh_layer_instances[layer].erase(inst);
					inst--;
				}
			}
		}
	}
};
void gv_com_terrain_roam::update_grass_layer(gv_int layer,
											 const gv_rect& position,
											 gvt_array< gv_float >& pattern,
											 gv_int operation, gv_float power){

};
void gv_com_terrain_roam::update_fur_layer(gv_int layer,
										   const gv_rect& position,
										   gvt_array< gv_float >& pattern,
										   gv_int operation, gv_float power){

};
void gv_com_terrain_roam::rebuild_normal_texture()
{
	gv_image_2d* pimage = get_sandbox()->create_object< gv_image_2d >();
	pimage->init_image(gv_color::RED(), this->get_size_in_point());
	;
	for (int iy = 0; iy < get_size_in_point().y; iy++)
		for (int ix = 0; ix < get_size_in_point().x; ix++)
		{
			gv_vector3 n;
			get_normal(ix, iy, n);
			n.normalize();
			n *= 0.5;
			n += 0.5;
			gv_color c(n.x, n.y, n.z, 1.0f);
			pimage->get_pixel(gv_vector2i(ix, iy)) = c;
		}
	pimage->export_file("terrain_normal.tga");
	gv_texture* ptexture = get_sandbox()->create_object< gv_texture >(
		gv_id("tex_terrain_normal"), this);
	gv_text e;
	ptexture->import_from_external_file("terrain_normal.tga", e);
	get_sandbox()->delete_object(pimage, true);
	this->m_normal_texure = ptexture;
}
//============================================================================================
//								:
//============================================================================================
GVM_IMP_CLASS(gv_com_terrain_roam, gv_com_graphic)
GVM_VAR(gvt_array< gv_roam_patch >, m_map)
GVM_VAR(gv_vector2i, m_size_in_point)
GVM_VAR(gv_vector2i, m_size_in_patch)
GVM_VAR(gv_bool, m_enable_dynamic_LOD)
GVM_VAR(gv_box, m_world_aabb)
GVM_VAR(gv_float, m_lod_threshold)
GVM_VAR(gvt_array< gv_terrain_grass_layer_info >, m_grass_layers)
GVM_VAR(gvt_array< gv_terrain_mesh_layer_info >, m_mesh_layers)
GVM_VAR(gvt_array< gv_terrain_fur_layer_info >, m_fur_layers)
GVM_VAR(gvt_ref_ptr< gv_texture >, m_normal_texure)
GVM_END_CLASS

GVM_IMP_STRUCT(gv_terrain_grass_layer_info)
GVM_VAR(gvt_ref_ptr< gv_material >, grass_material)
GVM_VAR(gv_float, fade_out_radius)
GVM_VAR(gv_bool, is_look_at)
GVM_VAR(gv_vector2, grass_size)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_terrain_mesh_layer_info)
GVM_VAR(gvt_ref_ptr< gv_com_static_mesh >, mesh)
GVM_VAR(gv_bool, is_colidable)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_terrain_fur_layer_info)
GVM_VAR(gvt_ref_ptr< gv_material >, fur_material)
GVM_VAR(gv_int, fur_pass)
GVM_VAR(gvt_ref_ptr< gv_texture >, fur_density_tex)
GVM_VAR(gvt_ref_ptr< gv_effect >, fur_effect)
GVM_VAR(gvt_array< gv_bool >, patch_mask)
GVM_VAR_ATTRIB_SET(file_name);
GVM_VAR(gv_text, fur_density_tex_src)
GVM_END_STRUCT

#define GV_DOM_FILE "../component/gv_com_terrain_structs.h"
#define GVM_DOM_RTTI
#define GVM_DOM_IMP_COPY
#define GVM_DOM_STATIC_CLASS
#include "gv_data_model_ex.h"

//============================================================================================
//								:
//============================================================================================
GVM_BGN_FUNC(gv_com_terrain_roam, scp_add_grass_layer)
GVM_PARAM(gv_terrain_grass_layer_info, info)
return this->add_grass_layer(info) + 1;
GVM_END_FUNC

GVM_BGN_FUNC(gv_com_terrain_roam, scp_add_mesh_layer)
GVM_PARAM(gv_terrain_mesh_layer_info, info)
return this->add_mesh_layer(info) + 1;
GVM_END_FUNC

GVM_BGN_FUNC(gv_com_terrain_roam, scp_add_fur_layer)
GVM_PARAM(gv_terrain_fur_layer_info, info)
return this->add_fur_layer(info) + 1;
GVM_END_FUNC

GVM_BGN_FUNC(gv_com_terrain_roam, scp_remove_grass_layer)
GVM_PARAM(gv_int, index)
return this->remove_grass_layer(index);
GVM_END_FUNC

GVM_BGN_FUNC(gv_com_terrain_roam, scp_remove_mesh_layer)
GVM_PARAM(gv_int, index)
return this->remove_mesh_layer(index);
GVM_END_FUNC

GVM_BGN_FUNC(gv_com_terrain_roam, scp_remove_fur_layer)
GVM_PARAM(gv_int, index)
return this->remove_fur_layer(index);
GVM_END_FUNC

GVM_BGN_FUNC(gv_com_terrain_roam, scp_create_grass_mesh)
GVM_PARAM(gvt_ptr< gv_material >, material)
GVM_PARAM(gv_float, density)
GVM_PARAM(gv_float, normal_factor)
GVM_PARAM(gv_vector2, height_range)
GVM_PARAM(gv_float, grass_height)
GVM_PARAM(gv_float, grass_width)
add_grass_layer(material, density, normal_factor, height_range, grass_height,
				grass_width);
return 1;
GVM_END_FUNC

//============================================================================================
//								:
//============================================================================================
}