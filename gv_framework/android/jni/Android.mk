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
LOCAL_PATH :=$(ROOT_PATH)gv_game_engine/gv_framework

include $(CLEAR_VARS)
LOCAL_MODULE    := gv_framework
LOCAL_SRC_FILES := \
     animation/gv_animation_all_in_one.cpp \
    component/gv_com_controller_all_in_one.cpp \
    component/gv_com_graphic_all_in_one.cpp \
    component/gv_com_terrain_all_in_one.cpp\
    _all_in_one/gv_engine_all_in_one.cpp\
    component/gv_entity_all_in_one.cpp\
    renderer/gv_material_all_in_one.cpp\
    network/gv_network_manager_all_in_one.cpp\
     _all_in_one/gv_object_all_in_one.cpp \
    renderer/gv_renderer_all_in_one.cpp\
    _all_in_one/gv_sandbox_all_in_one.cpp\
    user_interface/gv_ui_all_in_one.cpp\
    world/gv_world_all_in_one.cpp\
    src/gv_framework.cpp\
    src/gv_object.cpp\
    ai/gv_ai.cpp\
    utility/gv_id_lexer.cpp\
    src/gv_utility.cpp
    
    
    
LOCAL_CFLAGS := -D__ANDROID__ 
LOCAL_CFLAGS += -I"../../../../gv_game_engine/"
LOCAL_CFLAGS += -I"../../../../gv_game_engine/gv_base/inc/"
LOCAL_CFLAGS += -I"../../../../gv_game_engine/gv_framework/inc/"
LOCAL_CFLAGS += -I"../../../../gv_external/boost/"
LOCAL_CFLAGS += -I"../../../../gv_external/"
LOCAL_CFLAGS += -Wno-invalid-offsetof -Wno-int-to-pointer-cast
LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/native_app_glue)
