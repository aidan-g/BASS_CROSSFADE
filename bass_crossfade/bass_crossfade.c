#ifdef _DEBUG
#include <stdio.h>
#endif

#include "../bass/bassmix.h"
#include "bass_crossfade.h"
#include "crossfade_config.h"
#include "crossfade_envelope.h"
#include "crossfade_mixer.h"
#include "crossfade_queue.h"
#include "crossfade_syncs.h"

BOOL is_initialized = FALSE;

//I have no idea how to prevent linking against this routine in msvcrt.
//It doesn't exist on Windows XP.
//Hopefully it doesn't do anything important.
int _except_handler4_common() {
	return 0;
}

BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_Init)() {
	if (is_initialized) {
		return FALSE;
	}
	crossfade_config_set(CF_MIXER, 0);
	crossfade_config_set(CF_MODE, CF_MANUAL);
	crossfade_config_set(CF_IN_PERIOD, 100);
	crossfade_config_set(CF_OUT_PERIOD, 100);
	crossfade_config_set(CF_OVERLAP_PERIOD, 0);
	crossfade_config_set(CF_IN_TYPE, CF_EASE_IN);
	crossfade_config_set(CF_OUT_TYPE, CF_EASE_OUT);
	is_initialized = TRUE;
#if _DEBUG
	printf("BASS CROSSFADE initialized.\n");
#endif
	return TRUE;
}

BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_Free)() {
	BOOL success = TRUE;
	if (!is_initialized) {
		success = FALSE;
	}
	crossfade_config_set(CF_MIXER, 0);
	if (success) {
		is_initialized = FALSE;
#if _DEBUG
		printf("BASS CROSSFADE released.\n");
#endif
	}
	else {
#if _DEBUG
		printf("Failed to release BASS CROSSFADE.\n");
#endif
	}
	return success;
}

BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_SetConfig)(CF_ATTRIBUTE attrib, DWORD value) {
	if (!crossfade_config_set(attrib, value)) {
		return FALSE;
	}
	crossfade_sync_refresh();
	return TRUE;
}

BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_GetConfig)(CF_ATTRIBUTE attrib, DWORD* value) {
	return crossfade_config_get(attrib, value);
}

HSTREAM BASSCROSSFADEDEF(BASS_CROSSFADE_StreamCreate)(DWORD freq, DWORD chans, DWORD flags, void* user) {
	HSTREAM mixer = BASS_Mixer_StreamCreate(freq, chans, flags);
	HSTREAM handle;
	if (!mixer) {
		return 0;
	}
	crossfade_config_set(CF_MIXER, mixer);
	if (crossfade_queue_peek(&handle)) {
		crossfade_queue_remove(handle);
		crossfade_mixer_add(handle);
	}
	return mixer;
}

DWORD* BASSCROSSFADEDEF(BASS_CROSSFADE_GetChannels)(DWORD* count) {
	DWORD position;
	DWORD mixer_count;
	DWORD queue_count;
	static HSTREAM handles[MAX_CHANNELS] = { 0 };
	HSTREAM* mixer_handles = crossfade_mixer_get_all(&mixer_count);
	HSTREAM* queue_handles = crossfade_queue_get_all(&queue_count);
	for (position = 0, *count = 0; position < mixer_count; position++, (*count)++) {
		handles[*count] = mixer_handles[position];
	}
	for (position = 0; position < queue_count; position++, (*count)++) {
		handles[*count] = queue_handles[position];
	}
	return handles;
}

BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_ChannelEnqueue)(HSTREAM handle) {
	if (!crossfade_sync_register(handle)) {
		return FALSE;
	}
	if (!crossfade_mixer_playing() && crossfade_mixer_add(handle)) {
		return TRUE;
	}
	return crossfade_queue_add(handle);
}

BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_ChannelRemove)(HSTREAM handle) {
	BOOL success = TRUE;
	success &= crossfade_sync_unregister(handle);
	success &= crossfade_mixer_remove(handle) || crossfade_queue_remove(handle);
	return success;
}