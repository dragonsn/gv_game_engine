namespace gv
{
gv_func_info::gv_func_info()
{
	this->m_func_flags = 0;
	link_class(gv_func_info::static_class());
}
gv_func_info::gv_func_info(char* name)
{
	this->m_func_flags = 0;
	link_class(gv_func_info::static_class());
	this->rename(gv_id(name));
}

gv_func_info::gv_func_info(gv_sandbox* sandbox, gv_class_info* cls,
						   gv_func_init_helper* init_info)
{
	// native function declaration!
	this->m_name = gv_id(init_info->m_fn);
	this->m_cls = cls;
	this->m_callback = init_info->m_pfunc;
	this->m_func_flags = init_info->m_flags;
	// this->m_local_size=0;
	// this->m_param_size=0;
	this->m_is_native = 1;
	this->m_owner = cls;
	sandbox->add_object(this);
	cls->add_func(this);
	this->set_owner(cls);
	link_class(gv_func_info::static_class());
	this->m_param_struct = sandbox->find_class(gv_id(init_info->m_param_struct));
	// GV_ASSERT(this->m_param_struct);//OK for some no param function.
};

gv_func_info::~gv_func_info()
{
}

bool gv_func_info::is_equal(const gv_func_info* p) const
{
	return this->m_name == p->m_name;
}

gv_class_info* gv_func_info::get_param_struct()
{
	return this->m_param_struct;
}

void gv_func_info::initialize()
{
	if (m_is_inited)
		return;
	link_class(gv_func_info::static_class());
	/*USE struct to unified the parameter passing
  this->m_return_type=get_sandbox()->create_type(this->m_dcl_output);
  gv_xml_parser  ps;
  ps.goto_string(m_dcl_input);
  ps>>"(";
  while(ps.look_ahead()!=')')
  {
          gvi_type * ptype =get_sandbox()->create_type(&ps);
          this->m_parameters.add(ptype);
          this->m_param_size=gvt_align(this->m_param_size+ptype->get_size(),ptype->get_alignment()
  );
          ps.next_match(gv_lang_xml::TOKEN_XML_NAME);//name of parameter
          if (ps.look_ahead()!=')') ps>>",";
  }
  ps>>")";*/
	m_is_inited = 1;
}

/*
//current we will pend build our own script , because of resource.
int			gv_func_info::get_local_size()
{
        return this->m_local_size;
};

void gv_func_info::add_local(class gv_var_info * pinfo ){

        if (!this->m_local_vars.size()){
                pinfo->m_offset=0;
        }
        else {
                gv_var_info * pre=*this->m_local_vars.last();
                pinfo->m_offset=gvt_align(pre->m_offset+pre->get_size(),pinfo->get_type()->get_alignment());
        }

        m_local_size=gvt_align(pinfo->m_offset+pinfo->get_type()->get_size(),4);
        this->m_local_vars.add(pinfo);
        this->m_locals.add(pinfo->get_type());
        pinfo->set_owner(this);
}

void gv_func_info::add_param(class gv_var_info * pinfo ){

        if (!this->m_param_vars.size()){
                pinfo->m_offset=0;
        }
        else {
                gv_var_info * pre=*this->m_param_vars.last();
                pinfo->m_offset=gvt_align(pre->m_offset+pre->get_size(),pinfo->get_type()->get_alignment());
        }
        m_param_size=gvt_align(pinfo->m_offset+pinfo->get_type()->get_size(),4);
        this->m_param_vars.add(pinfo);
        this->m_parameters.add(pinfo->get_type());
        pinfo->set_owner(this);
}
gv_var_info*		gv_func_info::get_local_var(const gv_id & name)
{
        gv_var_info * pinfo=NULL;;
        for (int i=0; i<this->m_local_vars.size(); i++){
                if (this->m_local_vars[i]->m_name==name)
{pinfo=this->m_local_vars[i]; break;}
        }
        return pinfo;
}

gv_var_info*		gv_func_info::get_param_var(const gv_id &name)
{
        gv_var_info * pinfo=NULL;;
        for (int i=0; i<this->m_param_vars.size(); i++)
        {
                if (this->m_param_vars[i]->m_name==name)
{pinfo=this->m_param_vars[i]; break;}
        }
        return pinfo;
};

int			gv_func_info::get_param_size()
{
        return this->m_param_size;
}

int			gv_func_info::refresh_stack_size()
{
        //only called by compiler,for refresh the var info
        gvt_array	< gvt_ref_ptr <gv_var_info >		>
param_vars=m_param_vars;
        gvt_array	<gvt_ref_ptr  <gv_var_info >		>
local_vars=m_local_vars;//only usable for the gvc compiler
        this->m_parameters.clear();
        this->m_param_vars.clear();
        this->m_param_size=0;
        this->m_local_size=0;
        this->m_local_vars.clear();
        this->m_locals.clear();
        for ( int i=0; i<param_vars.size () ;i++)
this->add_param(param_vars[i]);
        for ( int i=0; i<local_vars.size () ;i++)
this->add_local(local_vars[i]);
        return this->m_param_size+this->m_local_size;
};
*/
}