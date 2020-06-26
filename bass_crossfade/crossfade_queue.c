#ifdef _DEBUG
#include <stdio.h>
#endif

#include "bass_crossfade.h"
#include "crossfade_queue.h"

static HSTREAM handles[MAX_CHANNELS] = { 0 };

HSTREAM* crossfade_queue_get_all(DWORD* count) {
	DWORD position;
	for (position = 0; position < MAX_CHANNELS; position++) {
		if (!handles[position]) {
			break;
		}
	}
	*count = position;
	return handles;
}

BOOL crossfade_queue_peek(HSTREAM* handle) {
	if (!handles[0]) {
		return FALSE;
	}
	*handle = handles[0];
	return TRUE;
}

BOOL crossfade_queue_add(HSTREAM handle) {
	DWORD position;
	for (position = 0; position < MAX_CHANNELS; position++) {
		if (handles[position]) {
			continue;
		}
		handles[position] = handle;
		return TRUE;
	}
	return FALSE;
}

BOOL crossfade_queue_insert(HSTREAM handle, DWORD index) {
	if (handles[MAX_CHANNELS - 1]) {
		return FALSE;
	}
	memmove(handles + index + 1, handles + index, sizeof(DWORD) * (MAX_CHANNELS - index));
	handles[index] = handle;
	return TRUE;
}

BOOL crossfade_queue_remove(HSTREAM handle) {
	DWORD position;
	for (position = 0; position < MAX_CHANNELS; position++) {
		if (handles[position] != handle) {
			continue;
		}
		for (; position < MAX_CHANNELS; position++) {
			if (position + 1 == MAX_CHANNELS) {
				handles[position] = 0;
			}
			else {
				handles[position] = handles[position + 1];
			}
			if (!handles[position]) {
				break;
			}
		}
		return TRUE;
	}
	return FALSE;
}