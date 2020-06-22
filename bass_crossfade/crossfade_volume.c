#include "crossfade_volume.h"
#include "crossfade_config.h"

typedef struct {
	HSTREAM handle;
	float value;
} CF_HANDLER;

static CF_HANDLER handlers[MAX_CHANNELS] = { 0 };

BOOL crossfade_sliding_volume(HSTREAM handle) {
	DWORD position = 0;
	for (position = 0; position < MAX_CHANNELS; position++) {
		if (handlers[position].handle != handle) {
			continue;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL crossfade_slide_volume_linear(HSTREAM handle, DWORD period, float value) {
	FLOAT new_value;
	FLOAT current_value;
	FLOAT step;
	if (!BASS_ChannelGetAttribute(handle, BASS_ATTRIB_VOL, &current_value)) {
		return FALSE;
	}
	if (current_value == value) {
		return TRUE;
	}
	step = (current_value - value) / period;
	for (new_value = current_value; new_value != value;) {
		if (new_value < value) {
			new_value -= step;
			if (new_value > value) {
				new_value = value;
			}
		}
		else if (new_value > value) {
			new_value -= step;
			if (new_value < value) {
				new_value = value;
			}
		}
		if (!BASS_ChannelSetAttribute(handle, BASS_ATTRIB_VOL, new_value)) {
			return FALSE;
		}
		Sleep(1);
	}
	return TRUE;
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
	if (!handler) {
		return FALSE;
	}
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
	handler->handle = 0;
	handler->value = 0;
	return success;
}

BOOL crossfade_slide_volume(HSTREAM handle, float value) {
	DWORD position = 0;
	HANDLE thread;
	for (position = 0; position < MAX_CHANNELS; position++) {
		if (handlers[position].handle) {
			continue;
		}
		handlers[position].handle = handle;
		handlers[position].value = value;
		thread = CreateThread(NULL, 0, &crossfade_slide_volume_handler, &handlers[position], 0, NULL);
		if (!thread) {
			handlers[position].handle = 0;
			handlers[position].value = 0;
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}