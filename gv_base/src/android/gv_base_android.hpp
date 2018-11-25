#include <unistd.h>
#include <android/sensor.h>

#if GV_WITH_OS_API


#include <android_native_app_glue.h>
#include <android/log.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <unistd.h>
#define __EXT_POSIX2
#include <libgen.h>
#include <dirent.h>
#include "../../inc/android/gv_android.h"
#include "gv_stream_android_asset.hpp"
#include "gv_android_app.hpp"

extern struct android_app* __state;

namespace gv
{
AAssetManager* g_assetManager = NULL;
void gv_os_init()
{
	// Get the android application's activity.
	GVM_CONSOLE_OUT(
		"---------------------gv_os_init--------------------------------------");
	GV_ASSERT(__state);
	GVM_CONSOLE_OUT(
		"---------------------create the new android application --------------");
	gv_android_app::static_create(__state);

	ANativeActivity* activity = __state->activity;
	g_assetManager = activity->assetManager;
	JavaVM* jvm = __state->activity->vm;
	JNIEnv* env = NULL;
	GVM_DEBUG_CONSOLE_OUT(
		"-----------------------------------------------------------");
	jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
	GVM_DEBUG_CONSOLE_OUT(
		"-----------------------------------------------------------");
	jint res = jvm->AttachCurrentThread(&env, NULL);
	if (res == JNI_ERR)
	{
		GV_ERROR_OUT(
			"Failed to retrieve JVM environment when entering message pump.");
		return;
	}
	GV_ASSERT(env);
	GVM_DEBUG_CONSOLE_OUT(
		"---------------------env--------------------------------------" << env);
	/* Get external files directory on Android; this will result in a directory
   * where all app files
   * should be stored, like /mnt/sdcard/android/<package-name>/files/
   */
	jboolean isCopy;

	jclass clazz = env->GetObjectClass(activity->clazz);
	jmethodID methodGetExternalStorage = env->GetMethodID(
		clazz, "getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;");
	GVM_DEBUG_CONSOLE_OUT(
		"-----------------------------------------------------------");
	jclass clazzFile = env->FindClass("java/io/File");
	jmethodID methodGetPath =
		env->GetMethodID(clazzFile, "getPath", "()Ljava/lang/String;");

	GVM_DEBUG_CONSOLE_OUT(
		"-----------------------------------------------------------");
	// Now has java.io.File object pointing to directory
	jobject objectFile =
		env->CallObjectMethod(activity->clazz, methodGetExternalStorage, NULL);

	// Now has String object containing path to directory
	jstring stringExternalPath =
		static_cast< jstring >(env->CallObjectMethod(objectFile, methodGetPath));
	const char* externalPath =
		env->GetStringUTFChars(stringExternalPath, &isCopy);
	GVM_DEBUG_CONSOLE_OUT(
		"-----------------------------------------------------------");
	// Set the default path to store the resources.
	std::string assetsPath(externalPath);
	if (externalPath[strlen(externalPath) - 1] != '/')
		assetsPath += "/";
	GVM_DEBUG_CONSOLE_OUT("-------------asset path:" << assetsPath.c_str());
	gv_global::config.path_for_write = assetsPath.c_str();

	// Get path to cache dir (/data/data/org.wikibooks.OpenGL/cache)
	{
		jclass activityClass = env->GetObjectClass(__state->activity->clazz);
		jmethodID getCacheDir =
			env->GetMethodID(activityClass, "getCacheDir", "()Ljava/io/File;");
		jobject file = env->CallObjectMethod(__state->activity->clazz, getCacheDir);
		jclass fileClass = env->FindClass("java/io/File");
		jmethodID getAbsolutePath =
			env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
		jstring jpath = (jstring)env->CallObjectMethod(file, getAbsolutePath);
		const char* app_dir = env->GetStringUTFChars(jpath, NULL);
		gv_global::config.path_for_save_data = app_dir;
		gv_global::config.path_for_save_data += "/";
		GVM_DEBUG_CONSOLE_OUT(
			"-------------cache_dir: " << gv_global::config.path_for_save_data);
		env->ReleaseStringUTFChars(jpath, app_dir);
	}

	// chdir in the application cache directory

	GVM_DEBUG_CONSOLE_OUT("path_for_write =" << gv_global::config.path_for_write);
	// Release string data
	env->ReleaseStringUTFChars(stringExternalPath, externalPath);
	jvm->DetachCurrentThread();
	GVM_DEBUG_OUT("---------------------gv_os_init "
				  "done--------------------------------------");
};
void gv_os_deinit(){

};

void make_path(std::string path, int mode)
{
	std::vector< std::string > dirs;
	while (path.length() > 0)
	{
		int index = path.find('/');
		std::string dir = (index == -1) ? path : path.substr(0, index);
		if (dir.length() > 0)
			dirs.push_back(dir);

		if (index + 1 >= path.length() || index == -1)
			break;

		path = path.substr(index + 1);
	}

	struct stat s;
	std::string dirPath;
	for (unsigned int i = 0; i < dirs.size(); i++)
	{
		dirPath += "/";
		dirPath += dirs[i];
		if (stat(dirPath.c_str(), &s) != 0)
		{
			// Directory does not exist.
			if (mkdir(dirPath.c_str(), 0777) != 0)
			{
				GVM_ERROR("Failed to create directory: '%s'" << dirPath.c_str());
				return;
			}
		}
	}

	return;
}

/**
* Returns true if the file exists in the android read-only asset directory.
*/
bool android_asset_exists(const char* filePath)
{
	AAsset* asset =
		AAssetManager_open(g_assetManager, filePath, AASSET_MODE_RANDOM);
	if (asset)
	{
		int length = AAsset_getLength(asset);
		AAsset_close(asset);
		return length > 0;
	}
	return false;
}

// from gameplay
bool create_file_from_asset(const char* path)
{
	static std::set< std::string > upToDateAssets;
	GV_ASSERT(path);
	std::string fullPath(*gv_global::config.path_for_write);
	fullPath += path;

	std::string directoryPath = fullPath.substr(0, fullPath.rfind('/'));
	struct stat s;
	if (stat(directoryPath.c_str(), &s) != 0)
		make_path(directoryPath, 0777);
	GVM_DEBUG_OUT("[fileio] cache file from asset:" << path << " to "
													<< directoryPath.c_str());
	// To ensure that the files on the file system corresponding to the assets in
	// the APK bundle
	// are always up to date (and in sync), we copy them from the APK to the file
	// system once
	// for each time the process (game) runs.
	{
		AAsset* asset =
			AAssetManager_open(g_assetManager, path, AASSET_MODE_RANDOM);
		if (asset)
		{
			const void* data = AAsset_getBuffer(asset);
			int length = AAsset_getLength(asset);
			FILE* file = fopen(fullPath.c_str(), "wb");
			if (file != NULL)
			{
				int ret = fwrite(data, sizeof(unsigned char), length, file);
				if (fclose(file) != 0)
				{
					GVM_WARNING(
						"Failed to close file on file system created from APK asset"
						<< path);
					return false;
				}
				if (ret != length)
				{
					GVM_WARNING("Failed to write all data from APK asset '%s' to file on "
								"file system."
								<< path);
					return false;
				}
			}
			else
			{
				GVM_WARNING("Failed to create file on file system from APK asset '%s'."
							<< path);
				return false;
			}
		}
		else
		{
			GVM_WARNING("Failed to find APK asset '%s'." << path);
		}
	}
}
//========================================================================================================

gv_stream_android_asset::gv_stream_android_asset()
{
	_asset = NULL;
}

gv_stream_android_asset::~gv_stream_android_asset()
{
	if (_asset)
		close();
}

bool gv_stream_android_asset::open(const char* filePath)
{
	_asset = AAssetManager_open(g_assetManager, filePath, AASSET_MODE_RANDOM);
	if (_asset)
	{
		return true;
	}
	return false;
}

bool gv_stream_android_asset::close()
{
	if (_asset)
		AAsset_close(_asset);
	_asset = NULL;
	return true;
}

gv_int gv_stream_android_asset::read(void* ptr, gv_int size)
{
	int result = AAsset_read(_asset, ptr, size);
	return result > 0 ? ((size_t)result) / size : 0;
}

void gv_stream_android_asset::flush(){

};
gv_int gv_stream_android_asset::write(const void* ptr, gv_int size)
{
	GV_ASSERT(0);
	return 0;
}

bool gv_stream_android_asset::eof()
{
	return tell() >= size();
}

gv_int gv_stream_android_asset::size()
{
	return (size_t)AAsset_getLength(_asset);
}

gv_int gv_stream_android_asset::tell()
{
	return AAsset_getLength(_asset) - AAsset_getRemainingLength(_asset);
}

bool gv_stream_android_asset::seek(gv_uint pos, std::ios_base::seekdir dir)
{
	gv_int origin;
	switch (dir)
	{
	case (std::ios_base::beg):
		origin = SEEK_SET;
		break;
	case (std::ios_base::end):
		origin = SEEK_END;
		break;
	case (std::ios_base::cur):
		origin = SEEK_CUR;
		break;
	}
	return AAsset_seek(_asset, pos, origin) != -1;
}

void gv_stream_android_asset::set_buf_size(gv_int size){

};
//=============================================================================================
// bool FileSystem::listFiles(const char* dirPath, std::vector<std::string>&
// files)
gv_int find_file_with_pattern_for_android(const gv_string& pattern,
										  gvt_array< gv_string >& files,
										  const gv_string& folder,
										  bool include_subfolder)
{
	gv_string_tmp pattern_string;
	gvt_array< gv_string > found_file_names;
	pattern_string = *pattern;
	pattern_string.replace_all("*", ""); // repla
	GVM_DEBUG_OUT("[fileio] find_file_with_pattern " << *pattern << " in "
													 << folder);
	gv_string_tmp path(*gv_global::config.path_for_write);
	path += folder;
	path += "/";
	bool result = false;
	struct dirent* dp;
	GVM_DEBUG_OUT("[fileio] start to search in writable dir :" << *path);
	DIR* dir = opendir(*path);
	if (dir != NULL)
	{
		while ((dp = readdir(dir)) != NULL)
		{
			gv_string_tmp filepath(path);
			filepath += dp->d_name;
			GVM_DEBUG_OUT("[fileio] find  " << *filepath);
			struct stat buf;
			if (!stat(*filepath, &buf))
			{
				// Add to the list if this is not a directory
				if (!S_ISDIR(buf.st_mode))
				{
					gv_string_tmp fname = dp->d_name;
					if (fname.find_string(pattern_string))
					{
						fname = path;
						fname += dp->d_name;
						;
						files.push_back(*fname);
						found_file_names.push_back(dp->d_name);
						GVM_DEBUG_OUT("[fileio] match pattern " << dp->d_name);
					}
				}
			}
		} // while next file
		closedir(dir);
	} // if find the dir
	// List the files that are in the android APK at this path
	GVM_DEBUG_OUT("[fileio] start to search in asset dir: ");
	AAssetDir* assetDir = AAssetManager_openDir(g_assetManager, *folder);
	if (assetDir != NULL)
	{
		AAssetDir_rewind(assetDir);
		const char* file = NULL;
		while ((file = AAssetDir_getNextFileName(assetDir)) != NULL)
		{
			GVM_DEBUG_OUT("[fileio] find  " << file);
			if (!found_file_names.find(file))
			{
				gv_string_tmp fname = file;
				if (fname.find_string(pattern_string))
				{
					GVM_DEBUG_OUT("[fileio]  match pattern " << file);
					fname = folder;
					fname += "/";
					fname += file;
					// HACK for android , cache the file in SD card
					create_file_from_asset(*fname);
					fname = path;
					fname += file;
					files.push_back(*fname);
				}
			}
			else
			{
				GVM_DEBUG_OUT("[fileio] file already cached, skip!  " << file);
			}
		}
		AAssetDir_close(assetDir);
	}
	else
	{
		GVM_WARNING(" [fileio] failed to open the path " << folder);
	}
	return files.size();
}
}

#endif
