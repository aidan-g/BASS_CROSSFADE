#include "crossfade_volume.h"
#include "crossfade_config.h"

typedef struct {
	HANDLE thread;
	HSTREAM handle;
	float value;
} CF_HANDLER;

BOOL crossfade_slide_volume_linear(HSTREAM handle, DWORD period, float value) {
	return FALSE;
}

BOOL crossfade_slide_volume_logarithmic(HSTREAM handle, DWORD period, float value) {
	return FALSE;
}

BOOL crossfade_slide_volume_exponential(HSTREAM handle, DWORD period, float value) {
	return FALSE;
}

DWORD WINAPI crossfade_slide_volume_handler(void* args) {
	DWORD period;
	DWORD type;
	BOOL success;
	CF_HANDLER* handler = args;
	if (handler) {
		crossfade_config_get(CF_PERIOD, &period);
		if (!period) {
			period = DEFAULT_PERIOD;
		}
		crossfade_config_get(CF_TYPE, &type);
		if (!type) {
			type = DEFAULT_TYPE;
		}
		switch (type)
		{
		default:
		case CF_LINEAR:
			success = crossfade_slide_volume_linear(handler->handle, period, handler->value);
			break;
		case CF_LOGARITHMIC:
			success = crossfade_slide_volume_logarithmic(handler->handle, period, handler->value);
			break;
		case CF_EXPONENTIAL:
			success = crossfade_slide_volume_exponential(handler->handle, period, handler->value);
			break;
		}
		if (!CloseHandle(handler->thread)) {
			success = FALSE;
		}
		free(handler);
	}
	else {
		success = FALSE;
	}
	return success;
}

BOOL crossfade_slide_volume(HSTREAM handle, float value) {
	CF_HANDLER* handler = malloc(sizeof(CF_HANDLER));
	if (!handler) {
		return FALSE;
	}
	handler->handle = handle;
	handler->value = 1;
	handler->thread = CreateThread(NULL, 0, &crossfade_slide_volume_handler, handler, 0, NULL);
	if (!handler->thread) {
		free(handler);
		return FALSE;
	}
	return TRUE;
}