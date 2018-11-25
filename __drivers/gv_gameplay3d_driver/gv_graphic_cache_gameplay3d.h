#pragma once
namespace gv
{

class gv_cache_mesh_gameplay : public gv_resource_cache
{
public:
	GVM_DCL_CLASS_WITH_CONSTRUCT(gv_cache_mesh_gameplay, gv_resource_cache);
	gvt_ptr< Mesh > m_mesh_gp;
};

class gv_cache_texture_gameplay : public gv_resource_cache
{
public:
	GVM_DCL_CLASS_WITH_CONSTRUCT(gv_cache_texture_gameplay, gv_resource_cache);
	gvt_ptr< Texture > m_tex_gp;
};

class gv_cache_effect_gameplay : public gv_resource_cache
{
public:
	GVM_DCL_CLASS_WITH_CONSTRUCT(gv_cache_effect_gameplay, gv_resource_cache);
	gvt_ptr< Effect > m_effect_gp;
};

class gv_cache_material_gameplay : public gv_resource_cache
{
public:
	GVM_DCL_CLASS_WITH_CONSTRUCT(gv_cache_material_gameplay, gv_resource_cache);
	gvt_ptr< Material > m_material_gp;
};
}