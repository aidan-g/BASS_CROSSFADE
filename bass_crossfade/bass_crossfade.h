#ifndef BASSCROSSFADE_H
#define BASSCROSSFADE_H

#include "../bass/bass.h"

#ifndef BASSCROSSFADEDEF
#define BASSCROSSFADEDEF(f) WINAPI f
#endif

#define MAX_CHANNELS 10

typedef enum {
	CF_MIXER = 1,
	CF_MODE = 2,
	CF_IN_PERIOD = 3,
	CF_OUT_PERIOD = 4,
	CF_OVERLAP_PERIOD = 5,
	CF_IN_TYPE = 6,
	CF_OUT_TYPE = 7
} CF_ATTRIBUTE;

typedef enum {
	CF_ALWAYS = 1,
	CF_MANUAL = 2
};

typedef enum {
	CF_LINEAR = 1,
	CF_LOGARITHMIC = 2,
	CF_EXPONENTIAL = 3,
	CF_EASE_IN = 4,
	CF_EASE_OUT = 5
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

#endif