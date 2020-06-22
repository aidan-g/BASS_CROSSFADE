#include <math.h>

#include "crossfade_volume.h"
#include "crossfade_config.h"

#define BASS_SLIDE_LOG 0x10000

typedef float (CALLBACK CURVEPROC)(float value);

typedef struct {
	HSTREAM handle;
	DWORD period;
	DWORD type;
	float value;
} CF_HANDLER;

static CF_HANDLER handlers[MAX_CHANNELS] = { 0 };

BOOL crossfade_fading(HSTREAM handle) {
	DWORD position = 0;
	for (position = 0; position < MAX_CHANNELS; position++) {
		if (handlers[position].handle != handle) {
			continue;
		}
		return TRUE;
	}
	return FALSE;
}

float CALLBACK crossfade_curve_linear(float value) {
	return value;
}

float CALLBACK crossfade_curve_logarithmic(float value) {
	return value * 2;
}

float CALLBACK crossfade_curve_exponential(float value) {
	return value * value;
}

float CALLBACK crossfade_curve_ease_in(float value) {
	return powf(value, 1.7f);
}

float CALLBACK crossfade_curve_ease_out(float value) {
	return powf(value, 0.48f);
}

BOOL crossfade_generate_curve(DWORD period, DWORD type, float min, float max, float* curve) {
	CURVEPROC* proc;
	float step;
	float diff;
	float value;
	DWORD position;
	switch (type)
	{
	default:
	case CF_LINEAR:
		proc = &crossfade_curve_linear;
		break;
	case CF_LOGARITHMIC:
		proc = &crossfade_curve_logarithmic;
		break;
	case CF_EXPONENTIAL:
		proc = &crossfade_curve_exponential;
		break;
	case CF_EASE_IN:
		proc = &crossfade_curve_ease_in;
		break;
	case CF_EASE_OUT:
		proc = &crossfade_curve_ease_out;
		break;
	}
	step = 1 / (float)period;
	diff = max - min;
	for (value = step, position = 0; value <= 1; value += step, position++) {
		curve[position] = min + (diff * proc(value));
	}
	curve[0] = min;
	curve[period - 1] = max;
	return TRUE;
}

BOOL crossfade_apply_curve(HSTREAM handle, DWORD period, float* curve) {
	DWORD position;
	for (position = 0; position < period; position++) {
		if (!BASS_ChannelSetAttribute(handle, BASS_ATTRIB_VOL, curve[position])) {
			return FALSE;
		}
		Sleep(1);
	}
	return TRUE;
}

DWORD WINAPI crossfade_slide_volume_handler(void* args) {
	float value;
	float* curve;
	BOOL success;
	CF_HANDLER* handler = args;
	if (!handler) {
		return FALSE;
	}
	if (BASS_ChannelGetAttribute(handler->handle, BASS_ATTRIB_VOL, &value)) {

		curve = malloc(sizeof(float) * handler->period);
		success =
			crossfade_generate_curve(handler->period, handler->type, value, handler->value, curve) &&
			crossfade_apply_curve(handler->handle, handler->period, curve);
	}
	else {
		success = FALSE;
	}
	handler->handle = 0;
	handler->period = 0;
	handler->type = 0;
	handler->value = 0;
	return success;
}

BOOL crossfade_slide_volume(HSTREAM handle, DWORD period, DWORD type, float value) {
	DWORD position = 0;
	HANDLE thread;
	for (position = 0; position < MAX_CHANNELS; position++) {
		if (handlers[position].handle) {
			continue;
		}
		handlers[position].handle = handle;
		handlers[position].period = period;
		handlers[position].type = type;
		handlers[position].value = value;
		thread = CreateThread(NULL, 0, &crossfade_slide_volume_handler, &handlers[position], 0, NULL);
		if (!thread) {
			handlers[position].handle = 0;
			handlers[position].period = 0;
			handlers[position].type = 0;
			handlers[position].value = 0;
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL crossfade_fade_in(HSTREAM handle) {
	DWORD period;
	DWORD type;
	float value = 1;
	crossfade_config_get(CF_IN_PERIOD, &period);
	crossfade_config_get(CF_IN_TYPE, &type);
	return crossfade_slide_volume(handle, period, type, value);
}

BOOL crossfade_fade_out(HSTREAM handle) {
	DWORD period;
	DWORD type;
	float value = 0;
	crossfade_config_get(CF_OUT_PERIOD, &period);
	crossfade_config_get(CF_OUT_TYPE, &type);
	return crossfade_slide_volume(handle, period, type, value);
}