//---object event------------------------------------------------>
namespace gv
{

bool gv_sandbox::register_processor(gv_class_info* cls, gv_int channel)
{
	GV_PROFILE_EVENT(gv_sandbox__register_processor, 0);
	GV_ASSERT(channel < gvc_max_object_event_channel);
	GV_ASSERT(cls);
	gv_object_name name = gv_object_name(cls->get_name_id());
	gv_event_processor* p = gvt_cast< gv_event_processor >(
		this->create_object(cls, this->get_native_module()));
	GV_ASSERT(p);
	p->set_current_channel((gv_ushort)channel);
	this->m_pimpl->m_event_processors[channel] = p;
	this->m_pimpl->m_active_event_processors.push_back(p);
	p->register_events();
	return true;
};

bool gv_sandbox::register_processor(const gv_id& cls_id, gv_int channel)
{
	gv_class_info* pcls = this->find_class(cls_id);
	GV_ASSERT(pcls);
	if (!pcls)
		return false;
	return this->register_processor(pcls, channel);
};

bool gv_sandbox::register_processor_group(const gv_id& cls_id,
										  gv_int start_channel,
										  gv_int channel_number,
										  bool synchronized, bool autonomous)
{
	register_processor(cls_id, start_channel);
	for (int i = 1; i < channel_number; i++)
	{
		register_processor(cls_id, start_channel + i);
		get_event_processor(start_channel + i)
			->set_base_channel((gv_ushort)start_channel);
	}
	for (int i = 0; i < channel_number; i++)
	{
		get_event_processor(start_channel + i)->set_synchronization(synchronized);
		get_event_processor(start_channel + i)->set_autonomous(autonomous);
	}
	return true;
};

void gv_sandbox::set_processor(gv_event_processor* p, gv_int channel)
{
	gv_event_processor* old = this->m_pimpl->m_event_processors[channel];
	m_pimpl->m_active_event_processors.erase_item_fast(old);
	this->m_pimpl->m_event_processors[channel] = p;
	this->m_pimpl->m_active_event_processors.push_back(p);
	p->set_current_channel((gv_ushort)channel);
};

bool gv_sandbox::share_event_processors(gv_sandbox* b)
{
	for (int i = 0; i < m_pimpl->m_active_event_processors.size(); i++)
	{
		gv_event_processor* p = m_pimpl->m_active_event_processors[i];
		b->set_processor(p, p->get_current_channel());
	}
	return true;
};

bool gv_sandbox::unregister_processor(gv_int channel)
{
	GV_PROFILE_EVENT(gv_sandbox__unregister_processor, 0);
	gv_event_processor* p = this->m_pimpl->m_event_processors[channel];
	if (!p)
		return false;
	this->m_pimpl->m_active_event_processors.erase_item_fast(p);
	this->m_pimpl->m_event_processors[channel] = NULL;
	return true;
};

gv_event_processor* gv_sandbox::get_event_processor(gv_int channel)
{
	return this->m_pimpl->m_event_processors[channel];
};

bool gv_sandbox::is_valid_event(gv_object_event* event)
{
	// TODO
	return true;
};

bool gv_sandbox::process_event(gv_object_event* event, gv_int channel)
{
	if (!this->m_pimpl->m_event_processors[channel])
		return false;
	return this->m_pimpl->m_event_processors[channel]->process_event(event);
};

bool gv_sandbox::process_named_event(gv_object_event* event, gv_int channel)
{
	if (!this->m_pimpl->m_event_processors[channel])
		return false;
	return this->m_pimpl->m_event_processors[channel]->process_named_event(event);
};

bool gv_sandbox::post_event(gv_object_event* event, gv_int channel)
{
	if (!this->m_pimpl->m_event_processors[channel])
	{
		delete event;
		return false;
	}
	if (!event->is_mute()) // hide ui event
	{
		GVM_DEBUG_LOG(event,
					  "post_event id["
						  << event->m_id << "]"
						  << "name [" << event->get_id_string() << "] to channel :"
						  << gv_framework_channel_enum_to_name(channel) << gv_endl)
	}
	return this->m_pimpl->m_event_processors[channel]->post_event(event);
};

bool gv_sandbox::post_job(gv_job_base* job, gv_int channel)
{
	if (!this->m_pimpl->m_event_processors[channel])
		return false;
	this->m_pimpl->m_event_processors[channel]->post_job(job);
	return true;
}

#pragma GV_REMINDER( \
	"TODO , change the name ,this is just bad name , see macro GVM_POST_JOB1 for detail , need pass the processor as paramter, used for thread queue")
bool gv_sandbox::post_job1(gv_job_base* job, gv_int channel)
{
	if (!this->m_pimpl->m_event_processors[channel])
		return false;
	this->m_pimpl->m_event_processors[channel]->post_job1(job);
	return true;
};

void gv_sandbox::register_event(gv_object_event_id id,
								gvi_object_event_handler_info* prec,
								gv_int channel)
{
	if (!this->m_pimpl->m_event_processors[channel])
		return;
	return this->m_pimpl->m_event_processors[channel]->register_event(id, prec);
};

void gv_sandbox::unregister_event(gv_object_event_id id, gv_object* object,
								  gv_int channel)
{
	if (!this->m_pimpl->m_event_processors[channel])
		return;
	return this->m_pimpl->m_event_processors[channel]->unregister_event(id,
																		object);
};

void gv_sandbox::register_named_event(gv_id id,
									  gvi_object_event_handler_info* prec,
									  gv_int channel)
{
	if (!this->m_pimpl->m_event_processors[channel])
		return;
	return this->m_pimpl->m_event_processors[channel]->register_named_event(id,
																			prec);
};

void gv_sandbox::unregister_named_event(gv_id id, gv_object* object,
										gv_int channel)
{
	if (!this->m_pimpl->m_event_processors[channel])
		return;
	return this->m_pimpl->m_event_processors[channel]->unregister_named_event(
		id, object);
};

gv_importer_exporter* gv_sandbox::get_impexp(const gv_string_tmp& file_name)
{
	gvt_array< gv_class_info* > result;
	gv_importer_exporter* importer = NULL;
	gv_string s;
	s = file_name;
	s.to_lower();
	s = gv_global::fm->get_extension(s);
	s = s.right(s.strlen() - 1);
	gv_id ext(*s);
	this->query_classes_derived_from(gv_id("gv_importer_exporter"), result);
	for (int i = 0; i < result.size(); i++)
	{
		gvt_array< gv_id > exts;
		gv_object* impexp = this->create_object(result[i]);
		importer = gvt_cast< gv_importer_exporter >(impexp);
		importer->get_import_extension(exts);
		if (exts.find(ext))
		{
			return importer;
		}
		else
			this->delete_object(importer);
	}
	return NULL;
};

bool gv_sandbox::import_external_format(gv_object* pobj,
										const gv_string_tmp& file_name,
										const gv_string_tmp& configuration)
{
	GV_PROFILE_EVENT(import_external_format, 0)
	GV_ASSERT(pobj);
	gv_string_tmp real_file_name = this->get_physical_resource_path(file_name);
	gv_importer_exporter* importer = this->get_impexp(real_file_name);
	if (importer)
	{
		try
		{
			importer->set_import_config(configuration);
			if (importer->do_import(real_file_name, pobj))
			{
				delete_object(importer);
				return true;
			}
		}
		catch (...)
		{
			// error occur
			delete_object(importer);
			GVM_WARNING(" !!!!error in  import_external_format from"
						<< file_name << " as " << pobj->get_class()->get_name_id()
						<< gv_endl);
			return false;
		}
	}
	GVM_WARNING(" !!!!error in  import_external_format from"
				<< file_name << " as " << pobj->get_class()->get_name_id()
				<< gv_endl);
	return false;
}

gv_object*
gv_sandbox::import_external_format(const gv_string_tmp& file_name,
								   const gv_string_tmp& configuration)
{
	gv_string s;
	gv_string_tmp real_file_name = this->get_physical_resource_path(file_name);
	gv_importer_exporter* importer = this->get_impexp(real_file_name);
	if (importer)
	{
		try
		{
			gvt_ref_ptr< gv_object > pobj =
				importer->get_import_target_class()->create_object();
			importer->set_import_config(configuration);
			if (importer->do_import(real_file_name, pobj))
			{
				delete importer;
				return pobj;
			}
		}
		catch (...)
		{
			// error occur
			delete importer;
			GVM_WARNING(" !!!!error in  import_external_format from" << file_name
																	 << gv_endl);
			GV_ASSERT(0);
			return NULL;
		}
		delete importer;
	}
	GVM_WARNING(" !!!!error in  import_external_format from" << file_name
															 << gv_endl);
	return NULL;
};

bool gv_sandbox::force_reimport(const gv_object_location& location)
{
	gv_module* mod = this->try_load_module(location[0].get_id());
	GV_ASSERT(mod);
	gv_resource* res = find_object< gv_resource >(location);
	GV_ASSERT(res);
	gv_object_name name = res->get_name();
	gv_object* new_object = create_object(res->get_class(), res->get_owner());
	import_external_format(new_object, *res->get_file_name());
	delete_object_tree(res);
	new_object->rename(name);
	return true;
};

bool gv_sandbox::export_external_format(gv_object* pobj,
										const gv_string_tmp& file_name)
{
	gv_string s;
	s = file_name;
	s.to_lower();
	s = gv_global::fm->get_extension(s);
	s = s.right(s.strlen() - 1);
	gv_id ext(*s);
	gv_importer_exporter* importer;
	gvt_array< gv_class_info* > result;
	this->query_classes_derived_from(gv_id("gv_importer_exporter"), result);
	gv_string_tmp real_file_name = this->get_physical_resource_path(file_name);
	for (int i = 0; i < result.size(); i++)
	{
		gv_object* impexp = this->create_nameless_object(result[i]);
		importer = gvt_cast< gv_importer_exporter >(impexp);
		if (importer)
		{
			try
			{
				gvt_array< gv_id > exts;
				importer->get_export_extension(exts);
				if (exts.find(ext))
				{
					importer->do_export(real_file_name, pobj);
					delete impexp;
					return true;
				}
			}
			catch (...)
			{
				// error occur
				delete impexp;
				GVM_WARNING(" !!!!error in  export_external_format to" << file_name
																	   << gv_endl);
				GV_ASSERT(0);
				return false;
			}
		}
		delete impexp;
	}
	GVM_WARNING(" !!!!error in  export_external_format to" << file_name
														   << gv_endl);
	return false;
}
}