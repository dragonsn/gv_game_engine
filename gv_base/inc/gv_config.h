//this is the header file for user to change the default included lib in gv to reduce binary size and compile time
//config GV according to your need!

#if !defined(GV_WITH_OS_API)
#define GV_WITH_OS_API 1  //1= fully function game engine , 0= algorithm only game logic lib 
#endif

#if !defined(GV_WITH_OPENAL)
#define GV_WITH_OPENAL 0
#endif

#if !defined(GV_WITH_GP)
#define GV_WITH_GP 1
#endif

#if !defined(GV_WITH_BGFX)
#define GV_WITH_BGFX 0
#endif

#if !defined(GV_WITH_LUA)
#define GV_WITH_LUA 1
#endif

#if defined(WIN32) 
#define GV_WITH_FBX 1
#else
#define GV_WITH_FBX 0
#endif

#define GV_WITH_IMPEXP 1
#define GV_WITH_STACKWALK 1

#if defined(WIN32) 
#define GV_WITH_DATABASE 1
#else 
#define GV_WITH_DATABASE 0
#endif 

#define GV_WITH_GESTURE 1

#define GV_WITH_ZLIB 1
#define GV_WITH_PNG 1 

#if defined(WIN32) 
#define GV_WITH_ASIO 1
#else 
#define GV_WITH_ASIO 0
#endif 