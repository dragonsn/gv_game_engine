#pragma once
namespace gv
{
class gv_object_builder : public gv_object
{
public:
	GVM_DCL_CLASS(gv_object_builder, gv_object)
	gv_object_builder(){};
	~gv_object_builder(){};

public:
	virtual gv_class_info* get_target_class()
	{
		return NULL;
	}
	virtual gv_object* build(gv_object* outer)
	{
		return NULL;
	};
};
}