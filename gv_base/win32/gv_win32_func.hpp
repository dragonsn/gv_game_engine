
#include <windows.h>
#include <mmsystem.h>
#include <dbghelp.h>
#include <malloc.h>

#pragma warning(disable : 4740)
#pragma warning(disable : 4748)
namespace gv
{
GV_STATIC_ASSERT(sizeof(gv_int) == sizeof(LONG));
static int s_time_zone_bias = 0;
//============================================================
void gv_os_init()
{
	TIMECAPS caps;
	timeGetDevCaps(&caps, sizeof(caps));
	timeBeginPeriod(1);
	TIME_ZONE_INFORMATION info;
	GetTimeZoneInformation(&info);
	s_time_zone_bias = info.Bias / 60;
};
void gv_os_deinit()
{
	std::locale::global(std::locale::classic());
	timeEndPeriod(10);
};

gv_int gv_get_time_zone_bias()
{
	return s_time_zone_bias;
}

gv_int gv_system_call(const char* p)
{
	return system(p);
};

gv_string_tmp GetErrorString(const char* lpszFunction);
void gv_throw(const char* format, ...)
{
	va_list ArgPtr;
	va_start(ArgPtr, format);
	static char s_temp_buffer[4096];
	gv_get_var_args(s_temp_buffer, gvt_array_length(s_temp_buffer), format,
					ArgPtr);

	gv_debug_output(s_temp_buffer);
	gv_debug_output("the last error is !");
	gv_debug_output(*GetErrorString(""));
	MessageBoxA(NULL, s_temp_buffer, "error!!", MB_OK);
	GV_DEBUG_BREAK;
	throw(s_temp_buffer);
}

void gv_qsort(void* base, gv_int num, gv_int width, GV_COMP_FUNC compare)
{
	if (!base || !num)
		return;
	GV_ASSERT(num > 0);
	qsort(base, (size_t)num, (size_t)width, compare);
};

void gv_debug_output(const char* s)
{
	OutputDebugStringA(s);
}

void gv_console_output(const char* s)
{
	printf("%s\n", s);
}

void gv_halt()
{
	*(int*)13 = 13;
};

void gv_debug_break()
{
#if !defined(_WIN64)
	_asm {int 3}
	;
#else
	gv_halt();
#endif
};

// http://athile.net/library/blog/?p=44
void gv_send_console_key_event(char input)
{
	// HWND wnd=GetConsoleWindow();
	// SendMessage(wnd,	WM_CHAR,(WPARAM)input,NULL);
	DWORD dwWrite;
	INPUT_RECORD inRecords[128];
	inRecords[0].EventType = KEY_EVENT;
	KEY_EVENT_RECORD& evt = inRecords[0].Event.KeyEvent;
	gvt_zero(evt);
	evt.bKeyDown = true;
	evt.dwControlKeyState = 0;
	evt.uChar.AsciiChar = input;
	// const char c = evt.uChar.AsciiChar;
	::WriteConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &inRecords[0], 1,
						&dwWrite);
}

void* gv_alloc_page64k()
{
	return _aligned_malloc(65536, 65536);
};
void gv_free_page64k(void* p)
{
	_aligned_free(p);
};
gv_int gv_atomic_increment(volatile gv_int* lpAddend)
{
	return InterlockedIncrement((volatile LONG*)lpAddend);
};

gv_int gv_atomic_decrement(volatile gv_int* lpAddend)
{
	return InterlockedDecrement((volatile LONG*)lpAddend);
};
gv_int gv_atomic_exchange(volatile gv_int* Target, gv_int Value)
{
	return InterlockedExchange((volatile LONG*)Target, Value);
};

gv_int gv_atomic_add(volatile gv_int* Addend, gv_int Value)
{
	gv_int ret = InterlockedExchangeAdd((volatile LONG*)Addend, Value);
	return ret;
};
gv_long gv_atomic_add64(gv_long volatile* addend, gv_long v)
{
	gv_long ret = InterlockedExchangeAdd64(addend, v);
	return ret;
};
gv_int gv_atomic_if_equal_exchange(volatile gv_int* Destination,
								   gv_int Comparand, gv_int Exchange)
{
	return InterlockedCompareExchange((volatile LONG*)Destination, Exchange,
									  Comparand);
}

// Define intrinsic for InterlockedCompareExchange64
extern "C" __int64 __cdecl _InterlockedCompareExchange64(
	__int64 volatile* Destination, __int64 Exchange, __int64 Comperand);

gv_long gv_atomic_if_equal_exchange64(gv_long volatile* Destination,
									  gv_long Comparand, gv_long Exchange)
{
	return _InterlockedCompareExchange64(
		reinterpret_cast< LONGLONG volatile* >(Destination), Exchange, Comparand);
};

#pragma GV_REMINDER("[NOTES] interlock everything!")
/*
InterlockedAnything        KJK_Hyperion   |   Edit   |   Show History
InterlockedCompareExchange can be used to implement any interlocked operation
that operates on a single word of data.
The pattern is as follows:
        1.Atomically read variable into old_value
        2.Operate on old_value to produce new_value
        3.Set variable to new_value if the current value of variable is still
old_value
        otherwise, go back to 1
For example, here's how a Lehmer PRNG can be implemented in a lock-free way:
LONG Random(LONG seed){    return (LONG)(DWORD)(0x41C64E6D *
(DWORD64)(DWORD)seed + 0x3039) % ((DWORD64)MAXDWORD + 1);}
        LONG InterlockedRandom(volatile LONG * seed)
                {
                        LONG old_value = *seed;
                        LONG cur_value = old_value;
                        LONG new_value;
                        for(;;)
                        {        // calculate the function
                                new_value = Random(old_value);     // set the
new value if the current value is still the expected one
                                cur_value = InterlockedCompareExchange(seed,
new_value, old_value);?
                                // we found the expected value: the exchange
happened
                                if(cur_value == old_value)            break;?
                                // recalculate the function on the unexpected
value
                                old_value = cur_value;
                        }    // success
                        return new_value;
                }
*/
#if defined(_DEBUG) && defined(WIN32) && GV_WITH_OS_API && GV_WITH_STACKWALK
namespace stack_walker
{

#include "stackwalker/StackWalker.h"
#include "stackwalker/StackWalker.cpp"
StackWalker* instance = NULL;
};
int gv_load_callstack(void** pbuffer, gv_uint max_depth)
{

	using namespace stack_walker;
	void** old = pbuffer;
	if (!stack_walker::instance)
	{
		bool b = gvp_memory_default::static_enable_tracking(false);
		stack_walker::instance = new StackWalker;
		stack_walker::instance->ShowCallstack();
		gvp_memory_default::static_enable_tracking(b);
	}
	CONTEXT c;
	;
	HANDLE hProcess = GetCurrentProcess();
	HANDLE hThread = GetCurrentThread();
	int frameNum;
	CONTEXT* context = NULL;
	if (context == NULL)
	{
		GET_CURRENT_CONTEXT(c, USED_CONTEXT_FLAGS);
	}
	else
		c = *context;

	// init STACKFRAME for first call
	STACKFRAME64 s; // in/out stackframe
	memset(&s, 0, sizeof(s));
	DWORD imageType;
#ifdef _M_IX86
	// normally, call ImageNtHeader() and use machine info from PE header
	imageType = IMAGE_FILE_MACHINE_I386;
	s.AddrPC.Offset = c.Eip;
	s.AddrPC.Mode = AddrModeFlat;
	s.AddrFrame.Offset = c.Ebp;
	s.AddrFrame.Mode = AddrModeFlat;
	s.AddrStack.Offset = c.Esp;
	s.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
	imageType = IMAGE_FILE_MACHINE_AMD64;
	s.AddrPC.Offset = c.Rip;
	s.AddrPC.Mode = AddrModeFlat;
	s.AddrFrame.Offset = c.Rsp;
	s.AddrFrame.Mode = AddrModeFlat;
	s.AddrStack.Offset = c.Rsp;
	s.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
	imageType = IMAGE_FILE_MACHINE_IA64;
	s.AddrPC.Offset = c.StIIP;
	s.AddrPC.Mode = AddrModeFlat;
	s.AddrFrame.Offset = c.IntSp;
	s.AddrFrame.Mode = AddrModeFlat;
	s.AddrBStore.Offset = c.RsBSP;
	s.AddrBStore.Mode = AddrModeFlat;
	s.AddrStack.Offset = c.IntSp;
	s.AddrStack.Mode = AddrModeFlat;
#else
#error "Platform not supported!"
#endif

	for (frameNum = 0; frameNum < (int)max_depth; ++frameNum)
	{
		// get next stack frame (StackWalk64(), SymFunctionTableAccess64(),
		// SymGetModuleBase64())
		// if this returns ERROR_INVALID_ADDRESS (487) or ERROR_NOACCESS (998), you
		// can
		// assume that either you are done, or that the stack is so hosed that the
		// next
		// deeper frame could not be found.
		// CONTEXT need not to be suplied if imageTyp is IMAGE_FILE_MACHINE_I386!
		if (!StackWalk64(imageType, hProcess, hThread, &s, &c,
						 StackWalker::myReadProcMem, SymFunctionTableAccess64,
						 SymGetModuleBase64, NULL))
		{
			// this->OnDbgHelpErr("StackWalk64", GetLastError(), s.AddrPC.Offset);
			break;
		}
		if (s.AddrPC.Offset == s.AddrReturn.Offset)
		{
			// this->OnDbgHelpErr("StackWalk64-Endless-Callstack!", 0,
			// s.AddrPC.Offset);
			break;
		}
		(*pbuffer++) = ((void*)s.AddrPC.Offset);

		if (s.AddrReturn.Offset == 0)
		{
			SetLastError(ERROR_SUCCESS);
			break;
		}
	} // for ( frameNum )
	return (gv_int)(pbuffer - old);
}

bool gv_get_symbol_from_offset(const void* offset, char* func_name,
							   int func_name_size, char* file_name,
							   int file_name_size, int& line_no)
{
	using namespace stack_walker;
	const static int STACKWALK_MAX_NAMELEN = 1024;
	strncpy(file_name, "unknown", file_name_size);
	strncpy(func_name, "unknown", func_name_size);

	if (!stack_walker::instance)
	{
		stack_walker::instance = new StackWalker;
		stack_walker::instance->ShowCallstack();
	}

	DWORD64 offsetFromSmybol;
	IMAGEHLP_SYMBOL64* pSym;
	IMAGEHLP_LINE64 Line;
	DWORD offsetFromLine;
	HANDLE hProcess = GetCurrentProcess();
	char symbol_buffer[sizeof(IMAGEHLP_SYMBOL64) + STACKWALK_MAX_NAMELEN];
	pSym = (IMAGEHLP_SYMBOL64*)symbol_buffer;
	memset(pSym, 0, sizeof(IMAGEHLP_SYMBOL64) + STACKWALK_MAX_NAMELEN);
	pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
	pSym->MaxNameLength = STACKWALK_MAX_NAMELEN;
	memset(&Line, 0, sizeof(Line));
	Line.SizeOfStruct = sizeof(Line);
	if (SymGetSymFromAddr64(hProcess, (DWORD64)offset, &offsetFromSmybol, pSym) !=
		FALSE)
	{
		UnDecorateSymbolName(pSym->Name, func_name, func_name_size,
							 UNDNAME_COMPLETE);
	}

	// show line number info, NT5.0-method (SymGetLineFromAddr64())

	if (SymGetLineFromAddr64(hProcess, (DWORD64)offset, &offsetFromLine, &Line) !=
		FALSE)
	{
		line_no = Line.LineNumber;
		strncpy(file_name, Line.FileName, file_name_size);
	}
	return TRUE;
}
#else
int gv_load_callstack(void** pbuffer, gv_uint max_depth)
{
	return 0;
};
bool gv_get_symbol_from_offset(const void* offset, char* func_name,
							   int func_name_size, char* file_name,
							   int file_name_size, int& line_no)
{
	return false;
}
#endif
gv_string_tmp GetErrorString(const char* lpszFunction)
{
	// Retrieve the system error message for the last-error code
	gv_string_tmp error_out;
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();

	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
					   FORMAT_MESSAGE_IGNORE_INSERTS,
				   NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				   (char*)&lpMsgBuf, 0, NULL);

	// Display the error message and exit the process
	/*
  lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
  (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) *
  sizeof(TCHAR));
  StringCchPrintf((LPTSTR)lpDisplayBuf,
  LocalSize(lpDisplayBuf) / sizeof(TCHAR),
  TEXT("%s failed with error %d: %s"),
  lpszFunction, dw, lpMsgBuf);
  //MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);
  */
	error_out << "error code " << (gv_uint)dw << "reason is ";
	error_out += (char*)lpMsgBuf;
	LocalFree(lpMsgBuf);
	// LocalFree(lpDisplayBuf);
	return error_out;
}

//============================================================================================
//								:
//============================================================================================
gv_string_tmp gv_multi_byte_to_utf8(const char* multi_byte);
gv_string_tmp gv_utf8_to_multi_byte(const char* utf8);

gv_string_tmp gv_utf8_to_multi_byte(const char* utf8)
{
	gv_string_tmp result;
	int buffLen = 0;
	WCHAR wbuff[5120];
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wbuff, 5120);
	buffLen = WideCharToMultiByte(CP_ACP, 0, wbuff, -1, NULL, 0, 0, 0);
	result.resize(buffLen + 1);
	WideCharToMultiByte(CP_ACP, 0, wbuff, -1, result.begin(), buffLen, 0, 0);
	return result;
}

gv_string_tmp gv_multi_byte_to_utf8(const char* gb2312)
{
	gv_string_tmp result;
	int buffLen = 0;
	WCHAR wbuff[5120];
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wbuff, 5120);
	buffLen = WideCharToMultiByte(CP_UTF8, 0, wbuff, -1, NULL, 0, 0, 0);
	result.resize(buffLen + 1);
	WideCharToMultiByte(CP_UTF8, 0, wbuff, -1, result.begin(), buffLen, 0, 0);
	return result;
}

gv_string_tmp gv_get_full_path_name(const gv_string_tmp& fname)
{
	char buffer[1024];
	GetFullPathNameA(*fname, 1023, buffer, NULL);
	return gv_string_tmp(buffer);
}

void gv_create_device_key_mapping(gv_int to_d[e_key_max + 1],
								  gv_int to_g[e_key_max + 1])
{
#undef GVM_MAP_KEYS
#define GVM_MAP_KEYS(my_key, vk) \
	to_d[my_key] = vk;           \
	to_g[vk] = my_key;
#include "gv_key_mapping_win32.h"
#undef GVM_MAP_KEYS
}

} // namespace gv
BOOL XMVerifyCPUSupport()
{
#if defined(_XM_NO_INTRINSICS_) || !defined(_XM_SSE_INTRINSICS_)
	return TRUE;
#else // _XM_SSE_INTRINSICS_
	// Note that on Windows 2000 or older, SSE2 detection is not supported so this
	// will always fail
	// Detecting SSE2 on older versions of Windows would require using cpuid
	// directly
	return (IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE) &&
			IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE));
#endif
}
