#include "StdAfx.h"
#include "GV_PropertyGridCtrl.h"
#include "MainFrm.h"
#include "gv_rts_editor.h"
#include <boost/boost/pool/singleton_pool.hpp>
using namespace gv;
struct proper_info
{
	proper_info()
	{
	}
	~proper_info()
	{
		int i = 0;
	}
	proper_info(const char* _l)
	{
		location = _l;
	}
	gv_string_static_1024 location;
};

gv_string_tmp get_location(CMFCPropertyGridProperty* p)
{
	proper_info* pinfo = (proper_info*)p->GetData();
	if (!pinfo)
		return NULL;
	return *pinfo->location;
}

void set_location(CMFCPropertyGridProperty* p, const gv_string_tmp& location)
{
	proper_info* pinfo = new proper_info(location);
	p->SetData((DWORD_PTR)(pinfo));
}

void AddOneProperty(CMFCPropertyGridCtrl& list,
					CMFCPropertyGridProperty* pfather, gvi_type* ptype,
					gv_byte* pdata, const gv_string_tmp& name = "",
					gv_string_tmp location = "", BOOL allow_edited = FALSE,
					BOOL is_super = FALSE, BOOL is_file_name = FALSE,
					gv_id enum_id = gv_id(),
					gv::gv_class_info* stop_class = NULL,
					gv_string_tmp tooltip = "")
{

	//================================================================================
	switch (ptype->get_type())
	{

	case (gve_data_type_object):
	{
		gv_type_object* ptc = (gv_type_object*)ptype;
		gv_class_info* pcls = ptc->get_class();
		if (pcls == stop_class)
			break;
		if (pcls->get_super())
		{
			CMFCPropertyGridProperty* pgroup1 = new CMFCPropertyGridProperty(
				GV_TO_UNICODE(*pcls->get_super()->get_name_id()));
			pgroup1->SetDescription(GV_TO_UNICODE("parent class"));
			set_location(pgroup1, location);
			gv_type_object type;
			type.set_class(pcls->get_super());
			AddOneProperty(list, pgroup1, &type, pdata, "", location, allow_edited, 1,
						   0, gv_id(), stop_class);
			if (pfather)
			{
				pfather->AddSubItem(pgroup1);
			}
			else
			{
				list.AddProperty(pgroup1);
			}
		}

		for (int i = 0; i < (int)pcls->get_nb_var(); i++)
		{
			gv_var_info* pvar = pcls->get_nth_var(i);
			if (pvar->m_is_hide_in_editor)
				continue;
			BOOL this_allow_edited = allow_edited && !pvar->is_no_clone();
			if (pvar->get_type()->get_type() == gve_data_type_object)
			{
				CMFCPropertyGridProperty* pgroup1 = new CMFCPropertyGridProperty(
					GV_TO_UNICODE(*pvar->get_name_id().string()));
				if (!pvar->get_tooltip().is_empty_string())
				{
					pgroup1->SetDescription(GV_TO_UNICODE(*pvar->get_tooltip()));
				}
				set_location(pgroup1, location + "." + *pvar->get_name_id());
				AddOneProperty(list, pgroup1, pvar->get_type(),
							   pdata + pvar->get_offset(), *pvar->get_name_id(),
							   location + "." + *pvar->get_name_id(), this_allow_edited,
							   0, pvar->m_is_file_name, gv_id(), stop_class);
				if (pfather)
				{
					pfather->AddSubItem(pgroup1);
				}
				else
				{
					list.AddProperty(pgroup1);
				}
			}
			else
			{
				AddOneProperty(list, pfather, pvar->get_type(),
							   pdata + pvar->get_offset(), *pvar->get_name_id(),
							   location, this_allow_edited, 0, pvar->m_is_file_name,
							   pvar->get_enum_id(), stop_class, *pvar->get_tooltip());
			}
		}
	}
	break;
	//================================================================================
	case (gve_data_type_color):
	{
		gv_color* pcolor = (gv_color*)pdata;
		CMFCPropertyGridProperty* pgroup1 =
			new CMFCPropertyGridProperty(GV_TO_UNICODE(*name));
		pgroup1->SetDescription(GV_TO_UNICODE(*tooltip));
		if (name[0] != '[')
			location += ".";
		location += name;
		set_location(pgroup1, location);
		{
			CMFCPropertyGridColorProperty* pColorProp =
				new CMFCPropertyGridColorProperty(
					GV_TO_UNICODE("RGB"), RGB(pcolor->r, pcolor->g, pcolor->b), NULL);
			pColorProp->EnableOtherButton(_T("Other..."));
			pColorProp->EnableAutomaticButton(_T("Default"),
											  ::GetSysColor(COLOR_3DFACE));
			set_location(pColorProp, location + ".rgb");
			pColorProp->AllowEdit(allow_edited);
			pgroup1->AddSubItem(pColorProp);

			int a = pcolor->a;
			CMFCPropertyGridProperty* pProp =
				new CMFCPropertyGridProperty(_T("A"), (_variant_t)(a), _T("alpha "));
			set_location(pProp, location + "." + ".a");
			// pProp->EnableSpinControl(TRUE, 50, 200);
			pProp->AllowEdit(allow_edited);
			pgroup1->AddSubItem(pProp);
		}
		if (pfather)
		{
			pfather->AddSubItem(pgroup1);
		}
		else
		{
			list.AddProperty(pgroup1);
		}
	}
	break;

	//================================================================================
	case (gve_data_type_text):
	{
		if (is_file_name)
		{
			gv_text* ptext = (gv_text*)pdata;
			static TCHAR BASED_CODE szFilter[] = _T("All Files(*.*)|*.*||");
			CMFCPropertyGridFileProperty* pProp = new CMFCPropertyGridFileProperty(
				GV_TO_UNICODE(*name), TRUE, (_variant_t)(CString(*ptext)), _T("*"), 0,
				szFilter, _T("select a file"));
			pProp->SetDescription(GV_TO_UNICODE(*tooltip));
			pProp->AllowEdit(allow_edited);
			if (name[0] != '[')
				location += ".";
			location += name;
			set_location(pProp, location);
			if (pfather)
			{
				pfather->AddSubItem(pProp);
			}
			else
			{
				list.AddProperty(pProp);
			}
		}
		else
		{
			gv_text* ptext = (gv_text*)pdata;
			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(
				GV_TO_UNICODE(*name), (_variant_t)(CString(*ptext)), _T(""));
			pProp->SetDescription(GV_TO_UNICODE(*tooltip));
			pProp->AllowEdit(allow_edited);
			if (name[0] != '[')
				location += ".";
			location += name;
			set_location(pProp, location);
			if (pfather)
			{
				pfather->AddSubItem(pProp);
			}
			else
			{
				list.AddProperty(pProp);
			}
		}
	}
	break;
	//================================================================================
	case (gve_data_type_array):
	{
		gv_type_array* pta = (gv_type_array*)ptype;
		gvi_type* pte = pta->get_element_type();
		CMFCPropertyGridProperty* pgroup1 =
			new CMFCPropertyGridProperty(GV_TO_UNICODE(*name));
		pgroup1->SetDescription(GV_TO_UNICODE(*tooltip));
		if (name[0] != '[')
			location += ".";
		location += name;
		set_location(pgroup1, *location);
		gv_type_int size_type;
		gv_type_int* ptsize = &(size_type);
		int size = pta->get_array_size(pdata);
		AddOneProperty(list, pgroup1, ptsize, (BYTE*)&size, "size", location,
					   allow_edited);
		gv_byte* phead = pta->get_array_data(pdata);
		for (int i = 0; i < size; i++)
		{
			gv_string_tmp idx;
			idx << "[" << i << "]";
			if (pte->get_type() == gve_data_type_object)
			{
				CMFCPropertyGridProperty* pgroup_el =
					new CMFCPropertyGridProperty(GV_TO_UNICODE(*idx));
				set_location(pgroup_el, location + idx);
				AddOneProperty(list, pgroup_el, pte, phead, *idx, location + idx,
							   allow_edited);
				pgroup1->AddSubItem(pgroup_el);
			}
			else
				AddOneProperty(list, pgroup1, pte, phead, *idx, location, allow_edited);
			phead += pte->get_size();
		}
		if (pfather)
		{
			pfather->AddSubItem(pgroup1);
		}
		else
			list.AddProperty(pgroup1);
	}
	break;
	//================================================================================
	case (gve_data_type_static_array):
	{
		gv_type_static_array* pta = (gv_type_static_array*)ptype;
		gvi_type* pte = pta->get_element_type();
		CMFCPropertyGridProperty* pgroup1 =
			new CMFCPropertyGridProperty(GV_TO_UNICODE(*name));
		pgroup1->SetDescription(GV_TO_UNICODE(*tooltip));
		if (name[0] != '[')
			location += ".";
		location += name;
		set_location(pgroup1, location);
		BYTE* phead = pdata;
		for (int i = 0; i < pta->get_array_size(); i++)
		{
			gv_string_tmp idx;
			idx << "[" << i << "]";
			if (pte->get_type() == gve_data_type_object)
			{
				CMFCPropertyGridProperty* pgroup_el =
					new CMFCPropertyGridProperty(GV_TO_UNICODE(*idx));
				set_location(pgroup_el, location + idx);
				AddOneProperty(list, pgroup_el, pte, phead, *idx, location + idx,
							   allow_edited);
				pgroup1->AddSubItem(pgroup_el);
			}
			else
				AddOneProperty(list, pgroup1, pte, phead, *idx, location, allow_edited);
			phead += pte->get_size();
		}
		if (pfather)
		{
			pfather->AddSubItem(pgroup1);
		}
		else
		{
			list.AddProperty(pgroup1);
		}
	}
	break;
	//================================================================================
	case (gve_data_type_bool):
	{
		bool* pbool = (bool*)pdata;
		CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(
			GV_TO_UNICODE(*name), (_variant_t)(*pbool), _T(""));
		pProp->SetDescription(GV_TO_UNICODE(*tooltip));
		// pProp->AllowEdit(allow_edited);
		if (name[0] != '[')
			location += ".";
		location += name;
		set_location(pProp, location);
		if (pfather)
		{
			pfather->AddSubItem(pProp);
		}
		else
		{
			list.AddProperty(pProp);
		}
	}
	break;
	default:
	{
		gv_string_tmp s;
		CMFCPropertyGridProperty* pProp = NULL;
		if (ptype->get_type() == gve_data_type_int && !enum_id.is_empty())
		{
			gv_enum_info* info = get_editor()->get_sandbox()->find_enum(enum_id);
			GV_ASSERT(info);
			gv_int* pint = (gv_int*)pdata;
			gv_string_tmp enum_value = *info->get_id(*pint);
			pProp = new CMFCPropertyGridProperty(GV_TO_UNICODE(*name),
												 GV_TO_UNICODE(*enum_value),
												 GV_TO_UNICODE("enum !!"));
			pProp->SetDescription(GV_TO_UNICODE(*tooltip));
			gvt_array< gv_id > a1;
			gvt_array< gv_int > a2;
			info->query_all(a1, a2);
			a1.sort();
			for (int i = 0; i < a1.size(); i++)
			{
				pProp->AddOption(GV_TO_UNICODE(*(a1[i])));
			}
			pProp->AllowEdit(FALSE);
		}
		else
		{
			ptype->export_to_xml(pdata, s);
			gv_cpp_string_to_string(s, false);
			pProp = new CMFCPropertyGridProperty(GV_TO_UNICODE(*name),
												 (_variant_t) * (s), _T(""));
			pProp->SetDescription(GV_TO_UNICODE(*tooltip));
			pProp->AllowEdit(allow_edited);
		}
		if (name[0] != '[')
			location += ".";
		location += name;
		set_location(pProp, location);
		if (pfather)
		{
			pfather->AddSubItem(pProp);
		}
		else
		{
			list.AddProperty(pProp);
		}
	}
	break;
	}
	return;
}

GV_PropertyGridCtrl::GV_PropertyGridCtrl()
{
}
GV_PropertyGridCtrl::~GV_PropertyGridCtrl(void)
{
}

void GV_PropertyGridCtrl::OnClickButton(CPoint point)
{

	return CMFCPropertyGridCtrl::OnClickButton(point);
}

BOOL GV_PropertyGridCtrl::EndEditItem(BOOL bUpdateData)
{
	if (!m_pSel)
		return FALSE;
	return CMFCPropertyGridCtrl::EndEditItem(bUpdateData);
};

void GV_PropertyGridCtrl::OnPropertyChanged(
	CMFCPropertyGridProperty* pProp) const
{
	return CMFCPropertyGridCtrl::OnPropertyChanged(pProp);
}

BOOL GV_PropertyGridCtrl::ValidateItemData(CMFCPropertyGridProperty* pProp)
{
	// if (!pProp->IsAllowEdit()) return TRU  E;
	pProp->OnUpdateValue();
	gv_string_tmp location = get_location(pProp);
	gv_string_tmp value;
	value << "\"" << (CStringA)pProp->GetValue() << "\"";
	gv_byte* pval;
	gvi_type* ptype;
	gv_object* pobj;
	gv_string_tmp tail;
	gv_id enum_id;
	if (!get_editor()->get_sandbox()->find_global_property(location, pval, ptype,
														   pobj, tail, enum_id))
		return FALSE;
	if (!pProp->IsAllowEdit() && pProp->GetValue().vt != VT_BOOL)
	{
		return TRUE;
	}
	if (tail.size() && ptype->get_type() == gve_data_type_color)
	{
		CMFCPropertyGridProperty* parent = pProp->GetParent();
		gv_color c;
		value = (CStringA)parent->GetSubItem(0)->GetValue();
		value >> c.fixed32;
		value = (CStringA)parent->GetSubItem(1)->GetValue();
		value >> c.a;
		location = get_location(parent);
		value = "";
		ptype->export_to_xml(gvt_byte_ptr(c), value);
	}
	else if (ptype->get_type() == gve_data_type_text)
	{
		value = "";
		value << "<![CDATA[" << (CStringA)pProp->GetValue() << "]]>";
	}
	else if (ptype->get_type() == gve_data_type_bool)
	{
		value.to_lower();
		if (value == "\"0\"")
		{
			value = "\"0\"";
		}
		else
		{
			value = "\"1\"";
		}
	}
	else if (ptype->get_type() == gve_data_type_int && !enum_id.is_empty())
	{
		gv_enum_info* info = get_editor()->get_sandbox()->find_enum(enum_id);
		GV_ASSERT(info);
		gv_string_tmp s_val;
		s_val << (CStringA)pProp->GetValue();
		value = "";
		value << "\"" << info->get_int(gv_id(*s_val)) << "\"";
		pProp->AllowEdit(FALSE);
	}

	return CMainFrame::static_get()->UpdateObject(location, value);
};

BEGIN_MESSAGE_MAP(GV_PropertyGridCtrl, CMFCPropertyGridCtrl)
ON_WM_LBUTTONDBLCLK()
ON_WM_KEYDOWN()
ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void GV_PropertyGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CMFCPropertyGridProperty* prop = this->HitTest(point);
	if (prop)
	{
		gv_string_tmp address = get_location(prop);
		gvi_type* type;
		gv_byte* data;
		gv_object* object;
		gv_string_tmp tail;
		gv_id enum_id;
		bool found = get_editor()->get_sandbox()->find_global_property(
			address, data, type, object, tail, enum_id);
		GVM_LOG(main, get_location(prop) << " selected!"
										 << "  value is: "
										 << (CStringA)prop->GetValue() << gv_endl);
		if (found)
		{
			GVM_LOG(main, "find in sandbox in object "
							  << object->get_location() << " as type "
							  << type->get_type_string() << gv_endl);
		}
		else
		{
			GVM_LOG(main, "[ERROR] the property is not found in sand box!!!"
							  << gv_endl);
			return;
		}
		if (type->get_type() == gve_data_type_text)
		{
			gv_text* ptext = (gv_text*)data;
			if (ptext)
			{
				gv_save_string_to_file("temp_temp.c", *ptext);
				gv_system_call("notepad temp_temp.c");
				gv_load_file_to_string("temp_temp.c", *ptext);
				prop->SetValue(CString(*ptext));
			}
		}
		else if (type->get_type() == gve_data_type_ptr)
		{
			gv_type_ptr* ptype = (gv_type_ptr*)type;
			if (ptype->get_pointed_type()->get_type() == gve_data_type_object)
			{
				gv_type_object* ptype_object =
					(gv_type_object*)ptype->get_pointed_type();
				if (ptype_object->get_class()->is_derive_from(
						gv_object::static_class()))
				{
					gvt_ptr< gv_object >* pp = (gvt_ptr< gv_object >*)data;
					if (pp->ptr())
						CMainFrame::static_get()->SelectObject(pp->ptr());
				}
			}
		}
		else if (type->get_type() == gve_data_type_ref_ptr)
		{
			gv_type_ref_ptr* ptype = (gv_type_ref_ptr*)type;
			if (ptype->get_pointed_type()->get_type() == gve_data_type_object)
			{
				gv_type_object* ptype_object =
					(gv_type_object*)ptype->get_pointed_type();
				if (ptype_object->get_class()->is_derive_from(
						gv_object::static_class()))
				{
					gvt_ref_ptr< gv_object >* pp = (gvt_ref_ptr< gv_object >*)data;
					if (pp->ptr())
						CMainFrame::static_get()->SelectObject(pp->ptr());
				}
			}
		}
	}
	CMFCPropertyGridCtrl::OnLButtonDblClk(nFlags, point);
}

void GV_PropertyGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar = VK_F5)
	{
		this->FillProperties(this->m_selected_object);
	}
	return CMFCPropertyGridCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void GV_PropertyGridCtrl::FillProperties(gv_object* pobj,
										 gv::gv_class_info* stop_class)
{
	// this->SetFont();
	m_selected_object = pobj;
	EnableHeaderCtrl(FALSE);
	EnableDescriptionArea();
	SetVSDotNetLook();
	RemoveAll();
	if (!pobj)
		return;
	gv_class_info* pclass = pobj->get_class();
	gv_type_object type;
	type.set_class(pclass);
	gv_module* pmod = pobj->get_module();
	BOOL allow_edited = TRUE; // pmod? !(pmod->get_name_id()==gv_id_native):FALSE;
	ShowWindow(FALSE);
	AddOneProperty(*this, NULL, &type, (BYTE*)pobj, "",
				   pobj->get_location_string(), allow_edited, 0, 0, gv_id(),
				   stop_class);
	ShowWindow(TRUE);
};
class gv_object* m_selected_object;

int GV_PropertyGridCtrl::DropObject(UINT nFlags, CPoint point,
									gv::gv_object* drop_object)
{
	if (!drop_object)
		return NULL;
	CMFCPropertyGridProperty* prop = this->GetCurSel();
	if (prop)
	{
		gv_string_tmp address = get_location(prop);
		gvi_type* type;
		gv_byte* data;
		gv_object* object;
		gv_string_tmp tail;
		gv_id enum_id;
		bool found = get_editor()->get_sandbox()->find_global_property(
			address, data, type, object, tail, enum_id);
		if (found)
		{
			GVM_LOG(main, " try to drop to " << get_location(prop) << gv_endl);
			if (type->get_type() == gve_data_type_ptr)
			{
				gv_type_ptr* ptype = (gv_type_ptr*)type;
				if (ptype->get_pointed_type()->get_type() == gve_data_type_object)
				{
					gv_type_object* ptype_object =
						(gv_type_object*)ptype->get_pointed_type();
					if (drop_object->is_a(ptype_object->get_class()))
					{
						gv_string_tmp s;
						s << drop_object->get_location();
						CString mfc = GV_TO_UNICODE(*s);
						prop->SetValue(mfc);
						this->UpdateData(FALSE);
						return 1;
					}
				}
			}
			else if (type->get_type() == gve_data_type_ref_ptr)
			{
				gv_type_ref_ptr* ptype = (gv_type_ref_ptr*)type;
				if (ptype->get_pointed_type()->get_type() == gve_data_type_object)
				{
					gv_type_object* ptype_object =
						(gv_type_object*)ptype->get_pointed_type();
					if (drop_object->is_a(ptype_object->get_class()))
					{
						gv_string_tmp s;
						s << drop_object->get_location();
						CString mfc = GV_TO_UNICODE(*s);
						prop->SetValue(mfc);
						this->UpdateData(FALSE);
						return 0;
					}
				}
			}
		}
		::MessageBoxA(NULL,
					  "failed to drop the object reference!!!,maybe type mismatch",
					  "error", MB_OK);
	}
	return 0;
};
void GV_PropertyGridCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CMFCPropertyGridCtrl::OnMouseMove(nFlags, point);
}
