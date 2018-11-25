// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "../../inc/android/gv_android.h"
#define JNI_CURRENT_VERSION JNI_VERSION_1_6
#if !defined(GV_JAVA_ACTIVITY_CLASS)
#define GV_JAVA_ACTIVITY_CLASS "org/gv_game_engine/engine/game"
// org.gv_game_engine.engine.game
#endif

// for JNI programming reference
// http://journals.ecs.soton.ac.uk/java/tutorial/native1.1/implementing/index.html

namespace gv
{
static gv_android_app* s_android_app = NULL;

gv_android_app* gv_android_app::static_get()
{
	return s_android_app;
};

gv_android_app::gv_android_app()
{
	s_android_app = this;
}

JNIEnv* gv_android_app::get_env()
{
	JNIEnv* env = NULL;
	m_vm->GetEnv((void**)&env, JNI_VERSION_1_6);
	jint res = m_vm->AttachCurrentThread(&env, NULL);
	if (res == JNI_ERR)
	{
		GV_ERROR_OUT(
			"Failed to retrieve JVM environment when entering message pump.");
		return NULL;
	}
	GVM_CONSOLE_OUT_VAR("get anroid env :" << env << gv_endl);
	GV_ASSERT(env);
	return env;
};

void gv_android_app::init(android_app* state)
{
	m_android_app_state = state;
	ANativeActivity* activity = state->activity;
	m_main_activity = activity;

	m_assets_manager = activity->assetManager;
	m_vm = state->activity->vm;
	bool optional = false;

	// m_game_activity_obj =
	//			(jobject)get_env()->NewGlobalRef((jobject)activity->instance)
	//;
	// find_class("java/io/File");
	jclass clazz = get_env()->GetObjectClass(activity->clazz);
	m_game_activity_class =
		clazz; // find_class_global(GV_JAVA_ACTIVITY_CLASS, optional);
	// new_/staticInstance to get the object
	m_func_launch_URL =
		find_method(m_game_activity_class, "LaunchURL", "(Ljava/lang/String;)V");
	m_static_get_instance =
		find_static_method(m_game_activity_class, "StaticJavaGameInstance",
						   "()L" GV_JAVA_ACTIVITY_CLASS ";");
	m_game_activity_obj = get_env()->CallStaticObjectMethod(
		m_game_activity_class, m_static_get_instance);

	GVM_CONSOLE_OUT_VAR(m_game_activity_class);
	GVM_CONSOLE_OUT_VAR(m_func_launch_URL);
	GVM_CONSOLE_OUT_VAR(m_static_get_instance);
	GVM_CONSOLE_OUT_VAR(m_game_activity_obj);
	m_game_activity_obj =
		(jobject)get_env()->NewGlobalRef((jobject)m_game_activity_obj);
	GVM_CONSOLE_OUT_VAR(m_game_activity_obj);
	// test JNI  call
	// jstring arg = get_env()->NewStringUTF("http://www.163.com");
	// call_void_method(gv_android_app::static_get()->m_game_activity_obj,
	// gv_android_app::static_get()->m_func_launch_URL, arg);
	// get_env()->DeleteLocalRef(arg);
};

jclass gv_android_app::find_class_global(const char* class_name, bool opt)
{
	jclass cls = find_class(class_name, opt);
	jclass new_cls = (jclass)get_env()->NewGlobalRef(cls);
	get_env()->DeleteLocalRef(cls);
	return new_cls;
};

gv_android_app* gv_android_app::static_create(android_app* __state)
{
	GV_ASSERT(!s_android_app);
	s_android_app = new gv_android_app;
	gv_android_app* app = s_android_app;
	app->init(__state);
	return app;
}

jclass gv_android_app::find_class(const char* class_name, bool opt)
{
	jclass cls = get_env()->FindClass(class_name);
	if (!opt)
		GV_ASSERT_WITH_MSG(cls, "Failed to find JAVA CLASS: " << class_name
															  << " cls=" << cls);
	return cls;
}

jmethodID gv_android_app::find_method(jclass cls, const char* method_name,
									  const char* method_signature, bool opt)
{
	jmethodID method =
		cls == NULL ? NULL
					: get_env()->GetMethodID(cls, method_name, method_signature);
	if (!opt)
		GV_ASSERT_WITH_MSG(method, "Failed to find JAVA METHOD: " << method_name);
	return method;
}

jmethodID gv_android_app::find_static_method(jclass cls,
											 const char* method_name,
											 const char* method_signature,
											 bool opt)
{
	jmethodID method =
		cls == NULL ? NULL : get_env()->GetStaticMethodID(cls, method_name,
														  method_signature);
	if (!opt)
		GV_ASSERT_WITH_MSG(method, "Failed to find JAVA METHOD: " << method_name);
	return method;
}

jfieldID gv_android_app::find_field(jclass cls, const char* field_name,
									const char* field_type, bool opt)
{
	jfieldID field =
		cls == NULL ? NULL : get_env()->GetFieldID(cls, field_name, field_type);
	if (!opt)
		GV_ASSERT_WITH_MSG(field, "Failed to find JAVA FIELD: " << field_name);
	return field;
}

void gv_android_app::call_void_method(jobject obj, jmethodID method, ...)
{
	// make sure the function exists
	if (method == NULL || obj == NULL)
	{
		return;
	}
	va_list args;
	va_start(args, method);
	get_env()->CallVoidMethodV(obj, method, args);
	va_end(args);
}

jobject gv_android_app::call_object_method(jobject obj, jmethodID method, ...)
{
	if (method == NULL || obj == NULL)
	{
		return NULL;
	}
	va_list args;
	va_start(args, method);
	jobject ret = get_env()->CallObjectMethodV(obj, method, args);
	va_end(args);
	return ret;
}

gv_int gv_android_app::call_int_method(jobject obj, jmethodID method, ...)
{
	if (method == NULL || obj == NULL)
	{
		return false;
	}

	va_list args;
	va_start(args, method);
	jint ret = get_env()->CallIntMethodV(obj, method, args);
	va_end(args);
	return (gv_int)ret;
}

bool gv_android_app::call_bool_method(jobject obj, jmethodID method, ...)
{
	if (method == NULL || obj == NULL)
	{
		return false;
	}
	va_list args;
	va_start(args, method);
	jboolean ret = get_env()->CallBooleanMethodV(obj, method, args);
	va_end(args);
	return (bool)ret;
}
}