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
	crossfade_config_set(CF_IN_TYPE, CF_OUT_QUAD);
	crossfade_config_set(CF_OUT_TYPE, CF_OUT_QUAD);
	crossfade_config_set(CF_MIX, FALSE);
	is_initialized = TRUE;
#if _DEBUG
	printf("BASS CROSSFADE initialized.\n");
#endif
	return TRUE;
}

BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_Free)() {
	BOOL success = TRUE;
	DWORD* handles;
	DWORD count;
	DWORD position;
	if (!is_initialized) {
		success = FALSE;
	}
	handles = BASS_CROSSFADE_GetChannels(&count);
	for (position = 0; position < count; position++) {
		success &= crossfade_sync_unregister(handles[position]);
		success &= (crossfade_mixer_remove(handles[position], FALSE) || crossfade_queue_remove(handles[position]));
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
	DWORD handle;
	DWORD mode;
	HSTREAM mixer = crossfade_mixer_create(freq, chans, flags, user);
	if (mixer) {
		if (crossfade_queue_peek(&handle)) {
			crossfade_queue_remove(handle);
			crossfade_config_get(CF_MODE, &mode);
			if (mode == CF_ALWAYS) {
				crossfade_mixer_add(handle, TRUE);
			}
			else {
				crossfade_mixer_add(handle, FALSE);
			}
		}
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
	DWORD mode;
	BOOL success = TRUE;
	if (!crossfade_sync_register(handle)) {
		return FALSE;
	}
	if (!crossfade_mixer_playing()) {
		crossfade_config_get(CF_MODE, &mode);
		if (mode == CF_ALWAYS) {
			success &= crossfade_mixer_add(handle, TRUE);
		}
		else {
			success &= crossfade_mixer_add(handle, FALSE);
		}
		if (success) {
			return TRUE;
		}
	}
	success = crossfade_queue_add(handle);
	return success;
}

BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_ChannelRemove)(HSTREAM handle) {
	DWORD mix;
	BOOL success = TRUE;
	crossfade_config_get(CF_MIX, &mix);
	success &= crossfade_sync_unregister(handle);
	if (mix) {
		crossfade_mixer_next(TRUE);
		success &= crossfade_mixer_remove(handle, TRUE) || crossfade_queue_remove(handle);
	}
	else {
		success &= crossfade_mixer_remove(handle, TRUE) || crossfade_queue_remove(handle);
		crossfade_mixer_next(TRUE);
	}
	return success;
}