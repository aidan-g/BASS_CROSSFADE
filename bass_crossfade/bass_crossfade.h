#ifndef BASSCROSSFADE_H
#define BASSCROSSFADE_H

#include "../bass/bass.h"

#ifndef BASSCROSSFADEDEF
#define BASSCROSSFADEDEF(f) WINAPI f
#endif

#define MAX_CHANNELS 10

#define DEFAULT_PERIOD 2000

#define DEFAULT_TYPE 1

typedef enum {
	CF_MIXER = 1,
	CF_PERIOD = 2,
	CF_TYPE = 3
} CF_ATTRIBUTE;

typedef enum {
	CF_LINEAR = 1,
	CF_LOGARITHMIC = 2,
	CF_EXPONENTIAL = 3
};

__declspec(dllexport)
BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_Init)();

__declspec(dllexport)
BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_Free)();

__declspec(dllexport)
BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_SetConfig)(CF_ATTRIBUTE attrib, DWORD value);

__declspec(dllexport)
BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_GetConfig)(CF_ATTRIBUTE attrib, DWORD* value);

__declspec(dllexport)
HSTREAM BASSCROSSFADEDEF(BASS_CROSSFADE_StreamCreate)(DWORD freq, DWORD chans, DWORD flags, void* user);

__declspec(dllexport)
DWORD* BASSCROSSFADEDEF(BASS_CROSSFADE_GetChannels)(DWORD* count);

__declspec(dllexport)
BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_ChannelEnqueue)(HSTREAM handle);

__declspec(dllexport)
BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_ChannelRemove)(HSTREAM handle);

__declspec(dllexport)
BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_FadeIn)(HSTREAM handle);

__declspec(dllexport)
BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_FadeOut)(HSTREAM handle);

#endif