namespace gv
{
gv_component::gv_component()
{
	link_class(gv_component::static_class());
	m_enabled = true;
};
gv_component::~gv_component(){};
gv_component* gv_component::clone(gv_sandbox* sandbox)
{
	return gvt_cast< gv_component >(sandbox->clone(this));
};
bool gv_component::tick(gv_float dt)
{
	return true;
};
bool gv_component::sync_to_entity(gv_entity*)
{
	return true;
};
bool gv_component::sync_from_entity(const gv_entity*)
{
	return true;
};
bool gv_component::is_tickable()
{
	return false;
}
bool gv_component::set_resource(gv_resource*)
{
	return false;
};
gv_resource* gv_component::get_resource()
{
	return NULL;
}
bool gv_component::is_renderable()
{
	return false;
}
bool gv_component::is_local_tm_enabled()
{
	return false;
}
const gv_matrix43& gv_component::get_local_tm()
{
	return gv_matrix43::get_identity();
}
const gv_vector3& gv_component::get_local_offset() const
{
	return gv_vector3::get_zero_vector();
}
const gv_euler& gv_component::get_local_rotation() const
{
	return gv_euler::get_zero_rotation();
}
gv_vector3 gv_component::get_local_scale() const
{
	return gv_vector3(1, 1, 1);
}
void gv_component::set_local_tm(const gv_matrix43& tm){};
void gv_component::set_local_offset(const gv_vector3& pos){};
void gv_component::set_local_scale(const gv_vector3& pos){};
void gv_component::set_local_rotation(const gv_euler& rot){};
gv_box gv_component::get_local_bbox()
{
	return gv_box();
}
gv_sphere gv_component::get_local_bsphere()
{
	return gv_sphere();
}
void gv_component::on_attach()
{
	gv_object_event_add_component* pe = new gv_object_event_add_component;
	pe->component = this;
	get_sandbox()->post_event(pe, get_channel());
};
void gv_component::on_detach()
{
	gv_object_event_remove_component* pe = new gv_object_event_remove_component;
	pe->component = this;
	get_sandbox()->post_event(pe, get_channel());
};
gve_event_channel gv_component::get_channel()
{
	return gve_event_channel_default;
};

GVM_IMP_CLASS(gv_component, gv_object)
GVM_VAR(gvt_ptr< gv_entity >, m_entity)
GVM_VAR(gvt_ref_ptr< gv_component >, m_next)
GVM_VAR(gv_bool, m_enabled)
GVM_END_CLASS
}