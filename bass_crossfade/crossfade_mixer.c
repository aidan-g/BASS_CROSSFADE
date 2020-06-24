#ifdef _DEBUG
#include <stdio.h>
#endif

#include "../bass/bassmix.h"
#include "crossfade_config.h"
#include "crossfade_envelope.h"
#include "crossfade_mixer.h"
#include "crossfade_queue.h"

static void CALLBACK __crossfade_mixer_free(HSYNC handle, DWORD channel, DWORD data, void* user) {
	crossfade_config_set(CF_MIXER, 0);
}

BOOL crossfade_mixer_get(HSTREAM* handle) {
	return crossfade_config_get(CF_MIXER, handle);
}

HSTREAM crossfade_mixer_create(DWORD freq, DWORD chans, DWORD flags, void* user) {
	HSTREAM mixer = BASS_Mixer_StreamCreate(freq, chans, flags);
	HSTREAM handle;
	if (!mixer) {
		return 0;
	}
	BASS_ChannelSetSync(mixer, BASS_SYNC_FREE, 0, &__crossfade_mixer_free, NULL);
	crossfade_config_set(CF_MIXER, mixer);
	if (crossfade_queue_peek(&handle)) {
		crossfade_queue_remove(handle);
		crossfade_mixer_add(handle);
	}
	return mixer;
}

HSTREAM* crossfade_mixer_get_all(DWORD* count) {
	HSTREAM mixer;
	static HSTREAM handles[MAX_CHANNELS] = { 0 };
	if (!crossfade_mixer_get(&mixer)) {
		*count = 0;
		return NULL;
	}
	*count = BASS_Mixer_StreamGetChannels(mixer, handles, MAX_CHANNELS);
	return handles;
}

BOOL crossfade_mixer_playing() {
	DWORD position;
	DWORD count;
	HSTREAM* handles = crossfade_mixer_get_all(&count);
	for (position = 0; position < count; position++) {
		if (BASS_Mixer_ChannelGetMixer(handles[position]) && BASS_ChannelIsActive(handles[position]) == BASS_ACTIVE_PLAYING) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL crossfade_mixer_add(HSTREAM handle) {
	HSTREAM mixer;
	DWORD mode;
	if (!crossfade_mixer_get(&mixer)) {
		return FALSE;
	}
	crossfade_config_get(CF_MODE, &mode);
	if (mode == CF_ALWAYS || crossfade_mixer_playing()) {
		if (!BASS_Mixer_StreamAddChannel(mixer, handle, BASS_MIXER_CHAN_PAUSE | BASS_MIXER_NORAMPIN)) {
			return FALSE;
		}
		crossfade_envelope_apply_in(handle);
		BASS_Mixer_ChannelFlags(handle, 0, BASS_MIXER_CHAN_PAUSE);
	}
	else {
		if (!BASS_Mixer_StreamAddChannel(mixer, handle, BASS_MIXER_NORAMPIN)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL crossfade_mixer_wait(HSTREAM handle) {
	DWORD buffer = BASS_GetConfig(BASS_CONFIG_BUFFER);
	while (BASS_Mixer_ChannelGetMixer(handle)) {
		Sleep(10);
	}
	//This seems dumb but I can't get a smooth fade out otherwise.
	//Seems removing a stream from the mixer removes unplayed data from the buffers.
	if (buffer > 0) {
		Sleep(buffer);
	}
	return TRUE;
}

BOOL crossfade_mixer_remove(HSTREAM handle, BOOL fade_out) {
	DWORD period;
	if (fade_out) {
		if (BASS_Mixer_ChannelGetMixer(handle) && BASS_ChannelIsActive(handle) == BASS_ACTIVE_PLAYING) {
			crossfade_mixer_next();
			crossfade_config_get(CF_OUT_PERIOD, &period);
			if (period) {
				crossfade_envelope_apply_out(handle, TRUE);
				return crossfade_mixer_wait(handle);
			}
		}
	}
	return BASS_Mixer_ChannelRemove(handle);
}

BOOL crossfade_mixer_next() {
	HSTREAM handle;
	if (!crossfade_queue_peek(&handle)) {
		return FALSE;
	}
	crossfade_queue_remove(handle);
	return crossfade_mixer_add(handle);
}