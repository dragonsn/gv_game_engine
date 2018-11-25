#pragma once
#include "android/asset_manager.h"
#include "jni.h"
#include "android_native_app_glue.h"

#include "..\..\inc\gv_base_internal.h"
#include "..\..\inc\gv_string.h"

namespace gv
{
class gv_android_app
{
protected:
	gv_android_app();
	~gv_android_app();

public:
	static gv_android_app* static_get();
	static gv_android_app* static_create(android_app* info);

public:
	JNIEnv* get_env();
	jclass find_class(const char* class_name, bool opt = false);
	jclass find_class_global(const char* class_name, bool opt = false);
	jmethodID find_method(jclass the_class, const char* method_name,
						  const char* method_signature, bool is_optional = false);
	jmethodID find_static_method(jclass the_class, const char* method_name,
								 const char* method_signature,
								 bool is_optional = false);
	jfieldID find_field(jclass the_class, const char* field_name,
						const char* field_type, bool is_optional = false);

	void call_void_method(jobject obj, jmethodID method, ...);
	jobject call_object_method(jobject obj, jmethodID method, ...);
	gv_int call_int_method(jobject obj, jmethodID method, ...);
	bool call_bool_method(jobject obj, jmethodID ethod, ...);

public:
	void init(android_app*);
	android_app* m_android_app_state;
	jclass m_game_activity_class;
	jobject m_game_activity_obj;
	AAssetManager* m_assets_manager;
	ANativeActivity* m_main_activity;
	JavaVM* m_vm;

public:
	gv_string m_file_path_base;
	gv_string m_external_file_path;
	jmethodID m_func_launch_URL;
	jmethodID m_static_get_instance;
};
}