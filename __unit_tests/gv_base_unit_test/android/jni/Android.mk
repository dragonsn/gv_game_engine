# Copyright (C) 2010 The Android Open Source Project
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

SAMPLE_PATH := $(call my-dir)/../../../../../../gv_game_engine/__unit_tests/gv_base_unit_test
LIBPNG_PATH := $(call my-dir)/../../../../../../gv_external/gameplay3d/external-deps/libpng/lib/android/arm
ZLIB_PATH :=   $(call my-dir)/../../../../../../gv_external/gameplay3d/external-deps/zlib/lib/android/arm
LUA_PATH :=    $(call my-dir)/../../../../../../gv_external/gameplay3d/external-deps/lua/lib/android/arm
BULLET_PATH := $(call my-dir)/../../../../../../gv_external/gameplay3d/external-deps/bullet/lib/android/arm
VORBIS_PATH := $(call my-dir)/../../../../../../gv_external/gameplay3d/external-deps/oggvorbis/lib/android/arm
OPENAL_PATH := $(call my-dir)/../../../../../../gv_external/gameplay3d/external-deps/openal/lib/android/arm
GP3D_PATH   := $(call my-dir)/../../../../../../gv_external/gameplay3d/gameplay/android/obj/local/armeabi
LIBGVB_PATH := $(call my-dir)/../../../../../../gv_game/lib/Debug_Android/gv_base/android/obj/local/armeabi-v7a


# gameplay
LOCAL_PATH := $(GP3D_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := libgameplay
LOCAL_SRC_FILES := libgameplay.a
include $(PREBUILT_STATIC_LIBRARY)

# libpng
LOCAL_PATH := $(LIBPNG_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := libpng 
LOCAL_SRC_FILES := libpng.a
include $(PREBUILT_STATIC_LIBRARY)

# libzlib
LOCAL_PATH := $(ZLIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := libzlib
LOCAL_SRC_FILES := libzlib.a
include $(PREBUILT_STATIC_LIBRARY)

# liblua
LOCAL_PATH := $(LUA_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := liblua
LOCAL_SRC_FILES := liblua.a
include $(PREBUILT_STATIC_LIBRARY)

# libbullet
LOCAL_PATH := $(BULLET_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := libbullet
LOCAL_SRC_FILES := libbullet.a
include $(PREBUILT_STATIC_LIBRARY)

# libvorbis
LOCAL_PATH := $(VORBIS_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := libvorbis
LOCAL_SRC_FILES := libvorbis.a
include $(PREBUILT_STATIC_LIBRARY)

# libOpenAL
LOCAL_PATH := $(OPENAL_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := libOpenAL
LOCAL_SRC_FILES := libOpenAL.a
include $(PREBUILT_STATIC_LIBRARY)

# libgv_base
LOCAL_PATH := $(LIBGVB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := libgv_base
LOCAL_SRC_FILES := libgv_base.a
include $(PREBUILT_STATIC_LIBRARY)

# gv_base_unit_test
LOCAL_PATH := $(SAMPLE_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE    := gv_base_unit_test
LOCAL_SRC_FILES :=	gv_base_unit_test.cpp\
					gv_unit_tests_base_all_in_one.cpp \
					gv_unit_tests_math_all_in_one.cpp \
					gv_unit_tests_network_all_in_one.cpp \
					gv_unit_tests_tools_all_in_one.cpp
					

LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -lOpenSLES 
LOCAL_CFLAGS    := -D__ANDROID__ -Wno-psabi 
LOCAL_CFLAGS    += -I"../../../../../gv_external/gameplay3d/external-deps/lua/include" 
LOCAL_CFLAGS    += -I"../../../../../gv_external/gameplay3d/external-deps/bullet/include" 
LOCAL_CFLAGS    += -I"../../../../../gv_external/gameplay3d/external-deps/libpng/include" 
LOCAL_CFLAGS    += -I"../../../../../gv_external/gameplay3d/external-deps/oggvorbis/include" 
LOCAL_CFLAGS    += -I"../../../../../gv_external/gameplay3d/external-deps/openal/include" 
LOCAL_CFLAGS    += -I"../../../../../gv_external/gameplay3d/gameplay/src"
LOCAL_CFLAGS    += -I"../../../../../gv_external"
LOCAL_CFLAGS    += -I"../../../../../gv_game_engine"
LOCAL_CFLAGS    += -I"../../../../../gv_game_engine/gv_base/inc"
LOCAL_CFLAGS += -Wno-invalid-offsetof -Wno-int-to-pointer-cast

LOCAL_STATIC_LIBRARIES := android_native_app_glue libgameplay libpng libzlib liblua libbullet libvorbis libOpenAL libgv_base

include $(BUILD_SHARED_LIBRARY)
$(call import-module,android/native_app_glue)
