#NDK_TOOLCHAIN_VERSION=clang3.5

APP_PLATFORM := android-9
APP_STL :=gnustl_static
APP_CPPFLAGS += -fexceptions -fpermissive
# APP_CPPFLAGS +=  -frtti  
APP_CPPFLAGS += -D_DEBUG
APP_MODULES := gv_framework
APP_CPPFLAGS +=-std=c++11
APP_CPPFLAGS +=-Os
APP_ABI := armeabi-v7a


#NDK_TOOLCHAIN_VERSION=4.6 #GCC4.7  

