#ifdef _DEBUG
#include <stdio.h>
#endif

#include <math.h>

#include "../bass/bassmix.h"

#include "crossfade_config.h"
#include "crossfade_envelope.h"
#include "crossfade_mixer.h"

#define ENVELOPE_POINTS 20

typedef float (CALLBACK CURVEPROC)(float value);

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

BOOL crossfade_curve_populate(DWORD type, float min, float max, float curve[ENVELOPE_POINTS]) {
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
	step = 1 / (float)ENVELOPE_POINTS;
	diff = max - min;
	for (value = step, position = 0; value <= 1; value += step, position++) {
		curve[position] = min + (diff * proc(value));
	}
	curve[0] = min;
	curve[ENVELOPE_POINTS - 1] = max;
	return TRUE;
}

BOOL crossfade_envelope_populate(DWORD period, DWORD type, float min, float max, BASS_MIXER_NODE* envelope) {
	HSTREAM mixer;
	DWORD step;
	QWORD period_size;
	DWORD position;
	static float curve[ENVELOPE_POINTS];
	if (!crossfade_mixer_get(&mixer)) {
		return FALSE;
	}
	if (!crossfade_curve_populate(type, min, max, curve)) {
		return FALSE;
	}
	step = period / ENVELOPE_POINTS;
	period_size = BASS_ChannelSeconds2Bytes(mixer, 0.001) * step;
	for (position = 0; position < ENVELOPE_POINTS; position++) {
		envelope[position].pos = period_size * position;
		envelope[position].value = curve[position];
	}
	return TRUE;
}


BOOL crossfade_envelope_apply_in(HSTREAM handle) {
	DWORD period;
	DWORD type;
	BASS_MIXER_NODE envelope[ENVELOPE_POINTS];
	crossfade_config_get(CF_IN_PERIOD, &period);
	crossfade_config_get(CF_IN_TYPE, &type);
	if (!period) {
		return TRUE;
	}
	if (!crossfade_envelope_populate(period, type, 0, 1, envelope)) {
		return FALSE;
	}
	return BASS_Mixer_ChannelSetEnvelope(handle, BASS_MIXER_ENV_VOL, envelope, ENVELOPE_POINTS);
}

BOOL crossfade_envelope_apply_out(HSTREAM handle, BOOL remove) {
	DWORD period;
	DWORD type;
	DWORD flags;
	BASS_MIXER_NODE envelope[ENVELOPE_POINTS];
	crossfade_config_get(CF_OUT_PERIOD, &period);
	crossfade_config_get(CF_OUT_TYPE, &type);
	if (!period) {
		return TRUE;
	}
	if (!crossfade_envelope_populate(period, type, 1, 0, envelope)) {
		return FALSE;
	}
	flags = BASS_MIXER_ENV_VOL;
	if (remove) {
		flags |= BASS_MIXER_ENV_REMOVE;
	}
	return BASS_Mixer_ChannelSetEnvelope(handle, flags, envelope, ENVELOPE_POINTS);
}