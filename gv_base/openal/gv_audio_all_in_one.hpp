#include "gv_base_internal.h"


#if GV_WITH_OPENAL

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#pragma warning(disable : 4200)
#pragma warning(disable : 4324)
#pragma warning(disable : 4065)
#pragma warning(disable : 4510)
#pragma warning(disable : 4610)
#pragma warning(disable : 4706)
#pragma warning(disable : 4800)
#pragma warning(disable : 4312)
#pragma warning(disable : 4267)

#endif



#define AL_BUILD_LIBRARY
#define AL_ALEXT_PROTOTYPES
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define AL_LIBTYPE_STATIC
#define restrict

// openal
// vorbis_1_3_4

#include "openal/common/atomic.hpp"
#include "openal/common/rwlock.hpp"
#include "openal/common/threads.hpp"
#include "openal/common/uintmap.hpp"

#include "openal/OpenAL32/alAuxEffectSlot.hpp"
#include "openal/OpenAL32/alBuffer.hpp"
#include "openal/OpenAL32/alEffect.hpp"
#include "openal/OpenAL32/alError.hpp"
#include "openal/OpenAL32/alExtension.hpp"
#include "openal/OpenAL32/alFilter.hpp"
#include "openal/OpenAL32/alFontsound.hpp"
#include "openal/OpenAL32/alListener.hpp"
#include "openal/OpenAL32/alMidi.hpp"
#include "openal/OpenAL32/alPreset.hpp"
#include "openal/OpenAL32/alSoundfont.hpp"
#include "openal/OpenAL32/alSource.hpp"
#include "openal/OpenAL32/alState.hpp"
#include "openal/OpenAL32/alThunk.hpp"
#include "openal/OpenAL32/sample_cvt.hpp"

#include "openal/Alc/ALc.hpp"
#include "openal/Alc/ALu.hpp"
#include "openal/Alc/alcConfig.hpp"
#include "openal/Alc/alcRing.hpp"
#include "openal/Alc/bs2b.hpp"
#include "openal/Alc/effects/autowah.hpp"
#include "openal/Alc/effects/chorus.hpp"
#include "openal/Alc/effects/compressor.hpp"
#include "openal/Alc/effects/dedicated.hpp"
#include "openal/Alc/effects/distortion.hpp"
#include "openal/Alc/effects/echo.hpp"
#include "openal/Alc/effects/equalizer.hpp"
#include "openal/Alc/effects/flanger.hpp"
#include "openal/Alc/effects/modulator.hpp"
#include "openal/Alc/effects/null.hpp"
#include "openal/Alc/effects/reverb.hpp"

#include "openal/Alc/helpers.hpp"
#include "openal/Alc/hrtf.hpp"
#include "openal/Alc/panning.hpp"
#include "openal/Alc/mixer.hpp"
#include "openal/Alc/mixer_c.hpp"
#if defined(WIN32)
#include "openal/Alc/mixer_sse.hpp"
#include "openal/Alc/mixer_sse2.hpp"
#include "openal/Alc/mixer_sse41.hpp"
#endif
#include "openal/Alc/midi/base.hpp"
#include "openal/Alc/midi/sf2load.hpp"
#include "openal/Alc/midi/dummy.hpp"
#include "openal/Alc/midi/fluidsynth.hpp"
#include "openal/Alc/midi/soft.hpp"
#include "openal/Alc/backends/base.hpp"
#include "openal/Alc/backends/loopback.hpp"
#include "openal/Alc/backends/null.hpp"
#include "openal/Alc/backends/wave.hpp"

#include "vorbis/lib/synthesis.hpp"
#include "vorbis/lib/block.hpp"
#include "vorbis/lib/floor0.hpp"
#include "vorbis/lib/lookup.hpp"
#include "vorbis/lib/analysis.hpp"
#include "vorbis/lib/sharedbook.hpp"
#include "vorbis/lib/window.hpp"
#include "vorbis/lib/info.hpp"
#include "vorbis/lib/mdct.hpp"
#include "vorbis/lib/smallft.hpp"
#include "vorbis/lib/codebook.hpp"
#include "vorbis/lib/lpc.hpp"
#include "vorbis/lib/mapping0.hpp"
#include "vorbis/lib/floor1.hpp"
#include "vorbis/lib/bitrate.hpp"
#include "vorbis/lib/res0.hpp"
#include "vorbis/lib/psy.hpp"
#include "vorbis/lib/envelope.hpp"
#include "vorbis/lib/lsp.hpp"
#include "vorbis/lib/registry.hpp"
#include "vorbis/lib/vorbisfile.hpp"

#include "ogg/src/framing.c"
#include "ogg/src/bitwise.c"

#endif