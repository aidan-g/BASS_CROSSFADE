#include "../bass/bassmix.h"
#include "crossfade_config.h"
#include "crossfade_mixer.h"
#include "crossfade_queue.h"
#include "crossfade_volume.h"

BOOL crossfade_mixer_get(HSTREAM* handle) {
	return crossfade_config_get(CF_MIXER, handle);
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

BOOL crossfade_mixer_add(HSTREAM handle, BOOL force) {
	HSTREAM mixer;
	DWORD count;
	if (!crossfade_mixer_get(&mixer)) {
		return FALSE;
	}
	if (!force) {
		count = BASS_Mixer_StreamGetChannels(mixer, NULL, 0);
		if (count) {
			return FALSE;
		}
	}
	return BASS_Mixer_StreamAddChannel(mixer, handle, BASS_MIXER_NORAMPIN);
}

BOOL crossfade_mixer_remove(HSTREAM handle) {
	if (BASS_ChannelIsActive(handle) == BASS_ACTIVE_PLAYING) {
		crossfade_mixer_next();
		crossfade_fade_out(handle);
	}
	return BASS_Mixer_ChannelRemove(handle);
}

BOOL crossfade_mixer_next() {
	HSTREAM handle;
	if (!crossfade_queue_peek(&handle)) {
		return FALSE;
	}
	crossfade_queue_remove(handle);
	return crossfade_mixer_add(handle, TRUE);
}