#ifdef _DEBUG
#include <stdio.h>
#endif

#include "bass_crossfade.h"
#include "crossfade_config.h"
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
__declspec(dllexport)
BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_StreamRegister)(HSTREAM handle) {
	return crossfade_sync_register(handle);
}

__declspec(dllexport)
BOOL BASSCROSSFADEDEF(BASS_CROSSFADE_StreamUnregister)(HSTREAM handle) {
	return crossfade_sync_unregister(handle);
}