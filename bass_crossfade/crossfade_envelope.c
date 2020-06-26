#ifdef _DEBUG
#include <stdio.h>
#endif

#include <math.h>

#include "../bass/bassmix.h"

#include "crossfade_config.h"
#include "crossfade_envelope.h"
#include "crossfade_mixer.h"

typedef float (CALLBACK CURVEPROC)(float from, float to, DWORD position);

float CALLBACK crossfade_curve_linear(float from, float to, DWORD position) {
	float index = (float)position / (ENVELOPE_POINTS - 1);
	if (from < to) {
		return index;
	}
	else {
		return 1.0f - index;
	}
}

float CALLBACK crossfade_curve_in_quad(float from, float to, DWORD position) {
	float index = (float)position / (ENVELOPE_POINTS - 1);
	if (from < to) {
		return index * index;
	}
	else {
		return 1.0f - (index * (2.0f - index));
	}
}

float CALLBACK crossfade_curve_out_quad(float from, float to, DWORD position) {
	float index = (float)position / (ENVELOPE_POINTS - 1);
	if (from < to) {
		return index * (2.0f - index);
	}
	else {
		return 1.0f - (index * index);
	}
}

float CALLBACK crossfade_curve_in_expo(float from, float to, DWORD position) {
	float index = (float)position / (ENVELOPE_POINTS - 1);
	if (from < to) {
		return (powf(2.0f, 8.0f * index) - 1.0f) / 255.0f;
	}
	else {
		return 1.0f - (1.0f - powf(2.0f, -8.0f * index));
	}
}

float CALLBACK crossfade_curve_out_expo(float from, float to, DWORD position) {
	float index = (float)position / (ENVELOPE_POINTS - 1);
	if (from < to) {
		return 1.0f - powf(2.0f, -8.0f * index);
	}
	else {
		return 1.0f - ((powf(2.0f, 8.0f * index) - 1.0f) / 255.0f);
	}
}

BOOL crossfade_curve_populate(DWORD type, float from, float to, float curve[ENVELOPE_POINTS]) {
	CURVEPROC* proc;
	DWORD position;
	switch (type)
	{
	default:
	case CF_LINEAR:
		proc = &crossfade_curve_linear;
		break;
	case CF_IN_QUAD:
		proc = &crossfade_curve_in_quad;
		break;
	case CF_OUT_QUAD:
		proc = &crossfade_curve_out_quad;
		break;
	case CF_IN_EXPO:
		proc = &crossfade_curve_in_expo;
		break;
	case CF_OUT_EXPO:
		proc = &crossfade_curve_out_expo;
		break;
	}
	for (position = 0; position < ENVELOPE_POINTS; position++) {
		curve[position] = proc(from, to, position);
	}
	curve[0] = from;
	curve[ENVELOPE_POINTS - 1] = to;
	return TRUE;
}

BOOL crossfade_envelope_populate(DWORD period, DWORD type, float from, float to, BASS_MIXER_NODE* envelope) {
	HSTREAM mixer;
	DWORD step;
	QWORD period_size;
	DWORD position;
	static float curve[ENVELOPE_POINTS];
	if (!crossfade_mixer_get(&mixer)) {
		return FALSE;
	}
	if (!crossfade_curve_populate(type, from, to, curve)) {
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