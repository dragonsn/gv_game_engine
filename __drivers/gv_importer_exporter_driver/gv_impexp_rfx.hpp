#include "gv_framework_private.h"
#include "gv_importer_exporter.h"

#include "..\renderer\gv_image.h"
#include "..\renderer\gv_texture.h"
#include "..\renderer\gv_material.h"
#include "..\renderer\gv_index_buffer.h"
#include "..\renderer\gv_vertex_buffer.h"
#include "..\renderer\gv_static_mesh.h"
#include "..\renderer\gv_shader.h"
#include "..\renderer\gv_effect.h"

#pragma GV_REMINDER( \
	"[MEMO][matrix]:must be RowMajor or will have problem with gv matrix system:")
namespace gv
{
#define GV_DOM_NAMESPACE rfx_dom
#define GV_DOM_FILE "rfx/gv_rfx_dom.h"
#include "gv_data_model.h"

using namespace gv_lang_xml;
using namespace rfx_dom;
class gv_impexp_rfx : public gv_importer_exporter
{
public:
	GVM_DCL_CLASS(gv_impexp_rfx, gv_importer_exporter);
	gv_impexp_rfx()
	{
		link_class(gv_impexp_rfx::static_class());
	};
	~gv_impexp_rfx(){};
	gv_xml_parser lexer;
	gv_effect_project* peffect_proj;
	gv_sandbox* my_sandbox;
	gvt_stack_static< gv_effect*, 16 > effect_stack;

public:
	void get_import_extension(gvt_array< gv_id >& result)
	{
		result.push_back(gv_id_rfx);
	};

	gv_class_info* get_import_target_class()
	{
		return gv_effect_project::static_class();
	}

	bool do_import(const gv_string_tmp& file_name, gv_object* target)
	{

		GV_PROFILE_EVENT(gv_impexp_rfx__do_import, 0)
		gv_string ext = gv_global::fm->get_extension(*file_name);
		;
		ext.to_upper();
		if (ext != ".RFX")
			return false;

		peffect_proj = gvt_cast< gv_effect_project >(target);
		if (!peffect_proj)
			return false;
		my_sandbox = peffect_proj->get_sandbox();
		if (!lexer.load_file(*file_name))
			return false;
		gv_int token = lexer.lex();
		//<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
		if (token != TOKEN_XML_PROCESS_BLOCK)
			return false;
		//<!DOCTYPE RENDER_MONKEY []]>
		token = lexer.lex();
		if (token != TOKEN_XML_DOCTYPE_BLOCK)
			return false;
		lexer.read_element_open("RENDER_MONKEY");
		{
			gv::rfx_dom::scope_register r(my_sandbox);
			do_workspace();
			lexer.read_element_close("RENDER_MONKEY");
		}
		return true;
	};

	void do_shader(gv_shader* shader, gv_effect* effect)
	{
		gv_any any;
		gv_string_tmp name;
		while (!lexer.is_element_will_close())
		{
			if (!my_sandbox->import_xml_dom_attrib(&lexer, any))
			{
				break;
			}
			if (any.get_class()->get_name_id() == "RmStringVariable")
			{
				RmStringVariable* string_var = GVM_ANY_TO(any, RmStringVariable);
				GV_ASSERT(string_var);
				// some notes  , ignore!
			}
			else if (any.get_class()->get_name_id() == "RmShaderConstant")
			{
				RmShaderConstant* var = GVM_ANY_TO(any, RmShaderConstant);
				GV_ASSERT(var);
				gv_shader_const c;
				c.m_register = var->REGISTER;
				c.m_name = var->NAME;
				shader->add_constant_var(c);
			}
			else if (any.get_class()->get_name_id() == "RmSampler")
			{
				RmSampler* var = GVM_ANY_TO(any, RmSampler);
				GV_ASSERT(var);
				gv_shader_const c;
				c.m_register = var->REGISTER;
				c.m_name = var->NAME;
				c.m_type = e_shader_data_sampler_2d;
				shader->add_sampler(c);
			}
			else if (any.get_class()->get_name_id() == "RmShaderCode")
			{
				RmShaderCode* var = GVM_ANY_TO(any, RmShaderCode);
				GV_ASSERT(var);
				lexer.next_match_token(TOKEN_XML_CDATA_BLOCK);
				gv_string_tmp content = lexer.get_cdata_content();
				lexer.read_element_close("RmShaderCode");
				shader->set_source_code(*content);
			}
		}
	}

	void do_pass(gv_effect_pass* pass)
	{
		pass->m_pass_effect =
			this->peffect_proj->add_effect(*pass->m_name, pass->m_owner);
		this->do_effect(pass->m_pass_effect);
	}

	void do_stream_map(gv_effect_stream_map* stream_map)
	{
		/*
    <!ELEMENT RmStreamMap
    (
    (
    RmStringVariable |
    RmStream
    )*
    )>*/
		//<!ELEMENT RmStream               ( RmStreamChannel* )>
		/*
    <!ATTLIST RmStreamMap
            NAME                           CDATA #REQUIRED
    >

    <!ATTLIST RmStream
    NAME                          CDATA #REQUIRED
    STRIDE                        CDATA #REQUIRED
    >

    <!ATTLIST RmStreamChannel
    NAME                          CDATA #REQUIRED
    DATA_SIZE                     CDATA #REQUIRED
    DATA_TYPE                     CDATA #REQUIRED
    REGISTER                      CDATA #REQUIRED
    USAGE                         CDATA #REQUIRED
    USAGE_INDEX                   CDATA #REQUIRED
    >*/
		gv_any any;
		gv_string_tmp name;
		while (!lexer.is_element_will_close())
		{
			if (!my_sandbox->import_xml_dom_attrib(&lexer, any))
			{
				break;
			}
			if (any.get_class()->get_name_id() == "RmStringVariable")
			{
				RmStringVariable* string_var = GVM_ANY_TO(any, RmStringVariable);
				GV_ASSERT(string_var);
				// some notes  , ignore!
			}
			else if (any.get_class()->get_name_id() == "RmStream")
			{
				RmStream* var = GVM_ANY_TO(any, RmStream);
				GV_ASSERT(var);
				while (!lexer.is_element_will_close())
				{
					gv_any any_channel;
					if (my_sandbox->import_xml_dom_attrib(&lexer, any_channel))
					{
						if (any_channel.get_class()->get_name_id() == "RmStreamChannel")
						{
							RmStreamChannel* var = GVM_ANY_TO(any_channel, RmStreamChannel);
							GV_ASSERT(var);
							gv_effect_stream_channel ch;
							ch.m_name = var->NAME;
							ch.m_format = (gve_vertex_format)var->DATA_TYPE;
							ch.m_usage = (gve_vertex_usage)var->USAGE;
							ch.m_usage_index = var->USAGE_INDEX;
							stream_map->m_map.push_back(ch);
						}
						else
						{
							GV_ASSERT(0);
						}
					}
				};
				lexer.read_element_close("RmStream");
			}
		}
	}
	void do_effect(gv_effect* peffect)
	{
		gv_any any;
		gv_string_tmp name;
		while (!lexer.is_element_will_close())
		{
			gv_bool ele_closed;
			if (!my_sandbox->import_xml_dom_attrib(&lexer, any, &ele_closed))
			{
				break;
			}
			if (any.get_class()->get_name_id() == "RmRegistryBranch")
			{
				// ignore;
				lexer.step_out_element();
				continue;
			}

			else if (any.get_class()->get_name_id() == "RmStringVariable")
			{
				RmStringVariable* string_var = GVM_ANY_TO(any, RmStringVariable);
				GV_ASSERT(string_var);
				gv_any data_block;
				my_sandbox->import_xml_dom_attrib(&lexer, data_block);
				RmTextDataBlock* pdata_block = GVM_ANY_TO(data_block, RmTextDataBlock);
				GV_ASSERT(pdata_block);
				lexer.next_match_token(TOKEN_XML_CDATA_BLOCK);
				gv_string_tmp content = lexer.get_cdata_content();
				lexer.read_element_close("RmTextDataBlock");
				lexer.read_element_close("RmStringVariable");
				gv_var_info* pvar =
					new gv_var_info(peffect->get_sandbox(),
									peffect->get_param_struct_info(), *string_var->NAME,
									"gv_text", sizeof(gv_text), NULL, NULL, -1, NULL);
				pvar->set_default_add_quote(*content);
				gv_effect_param_info extra_info;
				extra_info.m_semantic = string_var->VARIABLE_SEMANTIC;
				extra_info.m_type = e_shader_data_text;
				extra_info.m_name = pvar->get_name_id();
				peffect->add_param_info(extra_info);
				continue;
			}

			else if (any.get_class()->get_name_id() == "RmBooleanVariable")
			{
				RmBooleanVariable* var = GVM_ANY_TO(any, RmBooleanVariable);
				GV_ASSERT(var);
				gv_var_info* my_var = new gv_var_info(
					peffect->get_sandbox(), peffect->get_param_struct_info(),
					*var->NAME, "gv_bool", sizeof(gv_bool), NULL, NULL, -1, NULL);
				if (var->VALUE == "TRUE")
					my_var->set_default_add_quote("1");
				else
					my_var->set_default_add_quote("0");
				gv_effect_param_info extra_info;
				extra_info.m_type = e_shader_data_bool;
				extra_info.m_semantic = var->VARIABLE_SEMANTIC;
				extra_info.m_name = my_var->get_name_id();
				peffect->add_param_info(extra_info);
			}
			// RmFloatVariable
			else if (any.get_class()->get_name_id() == "RmFloatVariable")
			{
				RmFloatVariable* var = GVM_ANY_TO(any, RmFloatVariable);
				GV_ASSERT(var);
				gv_var_info* my_var = new gv_var_info(
					peffect->get_sandbox(), peffect->get_param_struct_info(),
					*var->NAME, "gv_float", sizeof(gv_float), NULL, NULL, -1, NULL);
				my_var->set_default_add_quote(*var->VALUE);
				if (var->MIN.size() && var->MAX.size())
				{
					gv_string_tmp s;
					s << "\"" << var->MIN << "\"  ";
					s << "\"" << var->MAX << "\"  ";
					my_var->set_range(*s);
				}
				gv_effect_param_info extra_info;
				extra_info.m_type = e_shader_data_float;
				extra_info.m_semantic = var->VARIABLE_SEMANTIC;
				extra_info.m_name = my_var->get_name_id();
				peffect->add_param_info(extra_info);
			}
			// RmIntegerVariable
			else if (any.get_class()->get_name_id() == "RmIntegerVariable")
			{
				RmIntegerVariable* var = GVM_ANY_TO(any, RmIntegerVariable);
				GV_ASSERT(var);
				gv_var_info* my_var = new gv_var_info(
					peffect->get_sandbox(), peffect->get_param_struct_info(),
					*var->NAME, "gv_int", sizeof(gv_int), NULL, NULL, -1, NULL);
				my_var->set_default_add_quote(*var->VALUE);
				if (var->MIN.size() && var->MAX.size())
				{
					gv_string_tmp s;
					s << "\"" << var->MIN << "\"  ";
					s << "\"" << var->MAX << "\"  ";
					my_var->set_range(*s);
				}
				gv_effect_param_info extra_info;
				extra_info.m_type = e_shader_data_int;
				extra_info.m_semantic = var->VARIABLE_SEMANTIC;
				extra_info.m_name = my_var->get_name_id();
				peffect->add_param_info(extra_info);
			}
			else if (any.get_class()->get_name_id() == "RmDirectXEffect" ||
					 any.get_class()->get_name_id() == "RmDirectXEffect")
			{
				RmDirectXEffect* rmfx = GVM_ANY_TO(any, RmDirectXEffect);
				gv_effect* new_effect =
					this->peffect_proj->add_effect(*rmfx->NAME, peffect);
				new_effect->set_active_camera(gv_id(rmfx->ACTIVE_CAMERA));
				this->do_effect(new_effect);
				lexer.read_element_close(*any.get_class()->get_name_id());
			}
			else if (any.get_class()->get_name_id() == "RmVectorVariable")
			{
				RmVectorVariable* var = GVM_ANY_TO(any, RmVectorVariable);
				GV_ASSERT(var);
				gv_var_info* my_var = new gv_var_info(
					peffect->get_sandbox(), peffect->get_param_struct_info(),
					*var->NAME, "gv_vector4", sizeof(gv_vector4), NULL, NULL, -1, NULL);
				gv_string_tmp s;
				gv_vector4 v4;
				v4.set(var->VALUE_0, var->VALUE_1, var->VALUE_2, var->VALUE_3);
				s << v4;
				my_var->set_default_add_quote(*s);
				gv_effect_param_info extra_info;
				extra_info.m_type = e_shader_data_vector4;
				extra_info.m_semantic = var->VARIABLE_SEMANTIC;
				extra_info.m_name = my_var->get_name_id();
				peffect->add_param_info(extra_info);
			}
			// RmMatrixVariable |
			else if (any.get_class()->get_name_id() == "RmMatrixVariable")
			{
				RmMatrixVariable* var = GVM_ANY_TO(any, RmMatrixVariable);
				GV_ASSERT(var);
				gv_var_info* my_var = new gv_var_info(
					peffect->get_sandbox(), peffect->get_param_struct_info(),
					*var->NAME, "gv_matrix44", sizeof(gv_matrix44), NULL, NULL, -1,
					NULL);
				gv_string_tmp s;
				gv_matrix44 mat44;
				mat44.load((gv_float*)&var->VALUE_0);
				mat44.transpose_self();
				s << mat44;
				my_var->set_default_add_quote(*s);
				gv_effect_param_info extra_info;
				extra_info.m_type = e_shader_data_matrix4;
				extra_info.m_semantic = var->VARIABLE_SEMANTIC;
				extra_info.m_name = my_var->get_name_id();
				peffect->add_param_info(extra_info);
			}
			// RmDynamicVariable |
			else if (any.get_class()->get_name_id() == "RmDynamicVariable")
			{
				RmDynamicVariable* var = GVM_ANY_TO(any, RmDynamicVariable);
				GV_ASSERT(var);
				gvt_array< gv_string > values;
				gv_int array_len = var->COLUMNS * var->ROWS;
				values.resize(array_len);
				lexer.read_element_open("RmDynamicVariableValueContainer");
				for (int i = 0; i < array_len; i++)
				{
					gv_any dyn_any;
					my_sandbox->import_xml_dom_attrib(&lexer, dyn_any, &ele_closed);
					RmDynamicVariableValue* value =
						GVM_ANY_TO(dyn_any, RmDynamicVariableValue);
					values[i] = value->VALUE;
				}
				lexer.read_element_close("RmDynamicVariableValueContainer");
				lexer.read_element_close(*any.get_class()->get_name_id());
				// VARIABLE_TYPE                 ( FLOAT | INTEGER | BOOLEAN ) #REQUIRED
				gv_string_tmp type_string;
				gv_int e_type = 0;
				gv_int el_size = 0;
				if (var->VARIABLE_TYPE == "FLOAT")
				{
					e_type = e_shader_data_float;
					type_string = "gv_float";
					el_size = 4;
				}
				else if (var->VARIABLE_TYPE == "INTEGER")
				{
					e_type = e_shader_data_int;
					type_string = "gv_int";
					el_size = 4;
				}
				else if (var->VARIABLE_TYPE == "BOOLEAN")
				{
					e_type = e_shader_data_bool;
					type_string = "gv_bool";
					el_size = 1;
				}
				else
				{
					GV_ASSERT(0 && "! unknow dynmic type");
				}
				gv_string_tmp var_type_name;
				var_type_name << "gvt_array_static< " << type_string << ", "
							  << array_len << ">";
				gv_var_info* my_var = new gv_var_info(
					peffect->get_sandbox(), peffect->get_param_struct_info(),
					*var->NAME, *var_type_name, el_size * array_len, NULL, NULL, -1,
					NULL);
				gv_string_tmp s;
				s << "< static_array "
				  << " array_size=\"" << array_len << "\" >";
				for (int i = 0; i < array_len; i++)
				{
					s << "  \"" << values[i] << "\"    ";
				}
				s << "</static_array>";
				my_var->set_default(*s);

				gv_effect_param_info extra_info;
				extra_info.m_type = e_shader_data_dynamic;
				extra_info.m_sub_type = e_type;
				extra_info.m_row = var->COLUMNS;
				extra_info.m_col = var->ROWS;
				extra_info.m_semantic = var->VARIABLE_SEMANTIC;
				extra_info.m_name = my_var->get_name_id();
				peffect->add_param_info(extra_info);
			}
			// RmColorVariable |
			else if (any.get_class()->get_name_id() == "RmColorVariable")
			{
				RmColorVariable* var = GVM_ANY_TO(any, RmColorVariable);
				GV_ASSERT(var);
				gv_var_info* my_var = new gv_var_info(
					peffect->get_sandbox(), peffect->get_param_struct_info(),
					*var->NAME, "gv_colorf", sizeof(gv_colorf), NULL, NULL, -1, NULL);
				gv_string_tmp s;
				gv_colorf c;
				c.set(var->VALUE_0, var->VALUE_1, var->VALUE_2, var->VALUE_3);
				s << c;
				my_var->set_default_add_quote(s);
				gv_effect_param_info extra_info;
				extra_info.m_type = e_shader_data_colorf;
				extra_info.m_semantic = var->VARIABLE_SEMANTIC;
				extra_info.m_name = my_var->get_name_id();
				peffect->add_param_info(extra_info);
			}
			// Rm2DTextureVariable |
			else if (any.get_class()->get_name_id() == "Rm2DTextureVariable")
			{
				Rm2DTextureVariable* var = GVM_ANY_TO(any, Rm2DTextureVariable);
				GV_ASSERT(var);
				gv_effect_texture texture;
				texture.m_file_name = *var->FILE_NAME;
				texture.m_name = var->NAME;
				peffect->add_texture_sample(texture);
			}
			// RmCubemapVariable |
			else if (any.get_class()->get_name_id() == "RmCubemapVariable")
			{
				RmCubemapVariable* var = GVM_ANY_TO(any, RmCubemapVariable);
				GV_ASSERT(var);
				gv_effect_texture texture;
				texture.m_file_name = *var->FILE_NAME;
				texture.m_name = var->NAME;
				texture.m_is_cubemap = true;
				peffect->add_texture_sample(texture);
			}
			// Rm3DTextureVariable |
			else if (any.get_class()->get_name_id() == "Rm3DTextureVariable")
			{
				Rm3DTextureVariable* var = GVM_ANY_TO(any, Rm3DTextureVariable);
				GV_ASSERT(var);
				gv_effect_texture texture;
				texture.m_file_name = *var->FILE_NAME;
				texture.m_name = var->NAME;
				texture.m_is_3dmap = true;
				peffect->add_texture_sample(texture);
			}
			// RmLightVariable |
			else if (any.get_class()->get_name_id() == "RmLightVariable")
			{
				RmLightVariable* var = GVM_ANY_TO(any, RmLightVariable);
				GV_ASSERT(var);
				// old data , ignore now
			}
			// RmMaterialVariable |
			else if (any.get_class()->get_name_id() == "RmMaterialVariable")
			{
				RmMaterialVariable* var = GVM_ANY_TO(any, RmMaterialVariable);
				GV_ASSERT(var);
				// old data , ignore now
			}
			// RmRenderableTexture
			else if (any.get_class()->get_name_id() == "RmRenderableTexture")
			{
				RmRenderableTexture* var = GVM_ANY_TO(any, RmRenderableTexture);
				GV_ASSERT(var);
				gv_effect_renderable_texture texture;
				(gv_string) texture.m_file_name = var->FILE_NAME;
				texture.m_name = var->NAME;
				var->FORMAT >> texture.m_format;
				var->WIDTH >> texture.m_size.x;
				var->HEIGHT >> texture.m_size.y;
				if (var->GENERATE_MIPMAPS == "TRUE")
					texture.m_use_mipmap = true;
				if (var->USE_VIEWPORT_DIMENSIONS == "TRUE")
					texture.m_use_window_size = true;
				var->WIDTH_RATIO >> texture.m_width_ratio;
				var->HEIGHT_RATIO >> texture.m_height_ratio;
				peffect->add_renderable_texture(texture);
			}
			// RmStreamMap |
			else if (any.get_class()->get_name_id() == "RmStreamMap")
			{
				RmStreamMap* var = GVM_ANY_TO(any, RmStreamMap);
				GV_ASSERT(var);
				gv_effect_stream_map stream_map;
				stream_map.m_name = *var->NAME;
				this->do_stream_map(&stream_map);
				lexer.read_element_close("RmStreamMap");
				peffect->add_stream_map(stream_map);
			}
			// RmModelData |
			else if (any.get_class()->get_name_id() == "RmModelData")
			{
				RmModelData* var = GVM_ANY_TO(any, RmModelData);
				GV_ASSERT(var);
				gv_effect_model model;
				model.m_name = var->NAME;
				(gv_string) model.m_file_name = var->FILE_NAME;
				peffect->add_model(model);
			}
			// RmEffectGroup
			else if (any.get_class()->get_name_id() == "RmEffectGroup")
			{
				RmEffectGroup* var = GVM_ANY_TO(any, RmEffectGroup);
				GV_ASSERT(var);
				gv_effect* new_effect =
					this->peffect_proj->add_effect(*var->NAME, peffect);
				this->do_effect(new_effect);
				lexer.read_element_close("RmEffectGroup");
			}
			// RmPass
			else if (any.get_class()->get_name_id() == "RmPass")
			{
				RmPass* var = GVM_ANY_TO(any, RmPass);
				GV_ASSERT(var);
				gv_effect_pass pass;
				pass.m_name = var->NAME;
				pass.m_enabled = var->ENABLED == "TRUE";
				pass.m_owner = peffect;
				this->do_pass(&pass);
				peffect->add_pass(pass);
				lexer.read_element_close("RmPass");
			}
			// RmCameraReference
			else if (any.get_class()->get_name_id() == "RmCamera")
			{
				RmCamera* var = GVM_ANY_TO(any, RmCamera);
				GV_ASSERT(var);
				gv_effect_camera camera;
				camera.m_name = var->NAME;
				camera.m_far_clip = var->FAR_CLIP_PLANE;
				camera.m_near_clip = var->NEAR_CLIP_PLANE;
				camera.m_fov = var->FOV;
				camera.m_look_at.set(var->LOOK_AT_VECTOR_X, var->LOOK_AT_VECTOR_Y,
									 var->LOOK_AT_VECTOR_Z);
				camera.m_pos.set(var->CAMERA_POSITION_X, var->CAMERA_POSITION_Y,
								 var->CAMERA_POSITION_Z);
				camera.m_up.set(var->UP_VECTOR_X, var->UP_VECTOR_Y, var->UP_VECTOR_Z);
				peffect->add_camera(camera);
			}
			else if (any.get_class()->get_name_id() == "RmCameraReference")
			{
				RmCameraReference* var = GVM_ANY_TO(any, RmCameraReference);
				GV_ASSERT(var);
				peffect->set_active_camera(gv_id(*var->NAME));
			}
			// RmModelReference
			else if (any.get_class()->get_name_id() == "RmModelReference")
			{
				RmModelReference* var = GVM_ANY_TO(any, RmModelReference);
				GV_ASSERT(var);
				peffect->set_active_model(gv_id(*var->NAME));
			}
			// RmRenderStateBlock
			else if (any.get_class()->get_name_id() == "RmRenderStateBlock")
			{
				// TODO
				RmRenderStateBlock* var = GVM_ANY_TO(any, RmRenderStateBlock);
				GV_ASSERT(var);
				gv_any state;
				if (!ele_closed)
				{
					while (!lexer.is_element_will_close())
					{
						if (!my_sandbox->import_xml_dom_attrib(&lexer, state))
						{
							break;
						}
						if (state.get_class()->get_name_id() == "RmState")
						{
							RmState* var = GVM_ANY_TO(state, RmState);
							GV_ASSERT(var);
							gv_effect_render_state s;
							s.m_name = var->NAME;
							s.m_state = var->STATE;
							s.m_value = var->VALUE;
							peffect->add_render_state(s);
						}
						else
						{
							GV_ASSERT(0);
						}
					}
					lexer.read_element_close("RmRenderStateBlock");
				} // closed
			}
			// RmHLSLShader
			else if (any.get_class()->get_name_id() == "RmHLSLShader")
			{
				RmHLSLShader* var = GVM_ANY_TO(any, RmHLSLShader);
				GV_ASSERT(var);
				gv_shader* pshader = my_sandbox->create_object< gv_shader >(
					gv_object_name(*var->NAME), peffect);
				if (var->PIXEL_SHADER == "TRUE")
					pshader->set_pixel_shader(true);
				else
					pshader->set_pixel_shader(false);
				pshader->set_compile_entry(gv_id(*var->ENTRY_POINT));
				pshader->set_compile_target(gv_id(*var->TARGET));
				this->do_shader(pshader, peffect);
				if (pshader->is_pixel_shader())
					peffect->set_pixel_shader(pshader);
				else
					peffect->set_vertex_shader(pshader);
				lexer.read_element_close("RmHLSLShader");
			}
			// RmShader
			else if (any.get_class()->get_name_id() == "RmShader")
			{
				RmShader* var = GVM_ANY_TO(any, RmShader);
				GV_ASSERT(var);
				// gv_shader * pshader=my_sandbox->create_object<gv_shader>(*var->NAME,
				// peffect);
				// pshader->set_is_asm(true);
				// this->do_shader(pshader,peffect);
				// if	(pshader->is_pixel_shader()) peffect->set_pixel_shader(pshader);
				// else	peffect->set_vertex_shader(pshader);

				GVM_WARNING("asm shader is not supported any more "
							<< var->NAME << " in effect" << peffect->get_location()
							<< gv_endl);
				lexer.step_out_element();
			}
			// RmRenderTarget
			else if (any.get_class()->get_name_id() == "RmRenderTarget")
			{
				// TODO
				RmRenderTarget* var = GVM_ANY_TO(any, RmRenderTarget);
				gv_effect_render_target target;
				target.m_name = var->NAME;
				target.m_is_clear_color = var->COLOR_CLEAR == "TRUE";
				target.m_is_clear_depth = var->DEPTH_CLEAR == "TRUE";
				// var->DEPTH_CLEAR_VALUE>>target.m_clear_depth_value;
				gv_uint icolor;
				var->CLEAR_COLOR_VALUE >> icolor;
				target.m_clear_color_value.fixed32 = icolor;
				peffect->set_active_render_target(target);
			}
			// RmStreamMapReference
			else if (any.get_class()->get_name_id() == "RmStreamMapReference")
			{
				RmStreamMapReference* var = GVM_ANY_TO(any, RmStreamMapReference);
				GV_ASSERT(var);
				peffect->set_active_stream_map(gv_id(*var->NAME));
			}
			else if (any.get_class()->get_name_id() == "RmTextureObject")
			{
				RmTextureObject* var = GVM_ANY_TO(any, RmTextureObject);
				GV_ASSERT(var);
				gv_effect_texture_object tex;
				tex.m_is_vertex_texture = var->VERTEX_TEXTURE == "TRUE";
				tex.m_name = var->NAME;
				tex.m_stage = var->STATE_INDEX;
				do_texture_object(tex);
				peffect->add_texture_object(tex);
				lexer.read_element_close("RmTextureObject");
			}
			else if (any.get_class()->get_name_id() == "RmPlugInData")
			{
				GV_ASSERT(0 && "plugin not support !!! line :");
				lexer.step_out_element();
			}
			else
			{
				GV_ASSERT(0);
			}
		}

		peffect->get_param_struct_info()->initialize();
	}

	void do_workspace()
	{
		//<RmEffectWorkspace NAME="Effect Workspace" API="" ACTIVE_EFFECT="Effect
		//Workspace.Anisotropic Effect Group.BrushedMetal" VALIDATION_STRING=""
		//XML_VERSION="RenderMonkey 1.71">
		gv_any any;
		bool b = my_sandbox->import_xml_dom_attrib(&lexer, any);
		GV_ASSERT(b);
		RmEffectWorkspace* workspace = GVM_ANY_TO(any, RmEffectWorkspace);
		GV_ASSERT(workspace);
		peffect_proj->add_effect("base");
		this->do_effect(peffect_proj->get_base_effect());
		lexer.read_element_close("RmEffectWorkspace");
		peffect_proj->get_base_effect()->load_default_params();
	}

	void do_texture_object(gv_effect_texture_object& obj)
	{
		gv_any any;
		gv_string_tmp name;
		while (!lexer.is_element_will_close())
		{
			if (!my_sandbox->import_xml_dom_attrib(&lexer, any))
			{
				break;
			}
			if (any.get_class()->get_name_id() == "RmStringVariable")
			{
				RmStringVariable* string_var = GVM_ANY_TO(any, RmStringVariable);
				GV_ASSERT(string_var);
				lexer.step_out_element();
				// some notes  , ignore!
			}
			else if (any.get_class()->get_name_id() == "RmTextureReference")
			{
				RmTextureReference* var = GVM_ANY_TO(any, RmTextureReference);
				GV_ASSERT(var);
				obj.m_texture_name = var->NAME;
			}
			else if (any.get_class()->get_name_id() == "RmState")
			{
				RmState* var = GVM_ANY_TO(any, RmState);
				GV_ASSERT(var);
				gv_effect_render_state s;
				s.m_name = var->NAME;
				s.m_state = var->STATE;
				s.m_value = var->VALUE;
				obj.m_texture_states.push_back(s);
			}
			else
			{
				GV_ASSERT(0);
			}
		}
	}

	void get_export_extension(gvt_array< gv_id >& result){

	};
	gv_class_info* get_export_source_class()
	{
		return NULL;
	};
	bool do_export(const gv_string_tmp& file_name, gv_object* source)
	{
		return false;
	};
};

GVM_IMP_CLASS(gv_impexp_rfx, gv_importer_exporter)
GVM_END_CLASS
}
