# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
ROOT_PATH  =$(call my-dir)/../../../../../
LOCAL_PATH :=$(ROOT_PATH)gv_game_engine/gv_base

include $(CLEAR_VARS)
LOCAL_MODULE    := gv_base
LOCAL_SRC_FILES := \
    all_in_one/gv_base_all_in_one.cpp \
    math/gv_math_all_in_one.cpp \
    boost_lib/gv_boost_all_in_one.cpp\
	lua/gv_lua_all_in_one.cpp\
	libpng/gv_png_all_in_one.cpp\
	zlib/gv_zlib_all_in_one.cpp\
     all_in_one/gv_os_all_in_one.cpp

    
LOCAL_CFLAGS := -D__ANDROID__  
LOCAL_CFLAGS += -I"../../../../gv_game_engine/gv_base/inc/"
LOCAL_CFLAGS += -I"../../../../gv_external/boost/"
LOCAL_CFLAGS += -I"../../../../gv_external/"
LOCAL_CFLAGS += -I"../../../../gv_game_engine/"
LOCAL_CFLAGS += -Wno-invalid-offsetof -Wno-int-to-pointer-cast

LOCAL_STATIC_LIBRARIES := android_native_app_glue
include $(BUILD_STATIC_LIBRARY)
$(call import-module,android/native_app_glue)
