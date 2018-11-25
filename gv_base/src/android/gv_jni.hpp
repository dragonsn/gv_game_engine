
#define JNI_CURRENT_VERSION JNI_VERSION_1_6
using namespace gv;
JNIEXPORT jint JNI_OnLoad(JavaVM* InJavaVM, void* InReserved)
{
	GVM_CONSOLE_OUT(
		"---------------------JNI_OnLoad--------------------------------------");
	return JNI_CURRENT_VERSION;
}