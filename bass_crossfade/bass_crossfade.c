#ifdef _DEBUG
#include <stdio.h>
#endif

#include "bass_crossfade.h"
#include "crossfade_config.h"
#include "crossfade_mixer.h"
#include "crossfade_queue.h"
#include "crossfade_syncs.h"
#include "crossfade_volume.h"

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
	if (!crossfade_queue_create()) {
		return FALSE;
	}
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
	else {
		success &= crossfade_queue_free();
	}
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
	return crossfade_config_set(attrib, value);
}

BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_GetConfig)(CF_ATTRIBUTE attrib, DWORD* value) {
	return crossfade_config_get(attrib, value);
}

DWORD* BASSCROSSFADEDEF(BASS_CROSSFADE_GetChannels)(DWORD* count) {
	DWORD position;
	DWORD mixer_count;
	DWORD queue_count;
	static HSTREAM handles[MAX_CHANNELS];
	HSTREAM* mixer_handles = crossfade_mixer_get(&mixer_count);
	HSTREAM* queue_handles = crossfade_mixer_get(&queue_count);
	for (position = 0; position < mixer_count; position++, (*count)++) {
		handles[*count] = mixer_handles[position];
	}
	for (position = 0; position < queue_count; position++, (*count)++) {
		handles[*count] = queue_handles[position];
	}
	return handles;
}

BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_StreamEnqueue)(HSTREAM handle) {
	return crossfade_mixer_add(handle, FALSE) || crossfade_sync_register(handle);
}

BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_StreamRemove)(HSTREAM handle) {
	return crossfade_mixer_remove(handle) || crossfade_sync_unregister(handle);
}

BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_FadeIn)(HSTREAM handle) {
	return crossfade_slide_volume(handle, 1);
}

BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_FadeOut)(HSTREAM handle) {
	return crossfade_slide_volume(handle, 0);
}