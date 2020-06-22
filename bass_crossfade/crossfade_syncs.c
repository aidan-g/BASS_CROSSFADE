#include "crossfade_config.h"
#include "crossfade_mixer.h"
#include "crossfade_syncs.h"
#include "crossfade_volume.h"

typedef struct {
	HSTREAM handle;
	HSYNC fade_in;
	HSYNC fade_out;
	HSYNC free;
} CF_SYNCS;

static CF_SYNCS syncs[MAX_CHANNELS] = { 0 };

void CALLBACK crossfade_sync_begin(HSYNC handle, DWORD channel, DWORD data, void* user) {
	crossfade_slide_volume(channel, 1);
}

void CALLBACK crossfade_sync_end(HSYNC handle, DWORD channel, DWORD data, void* user) {
	crossfade_mixer_next();
	crossfade_slide_volume(channel, 0);
}

void CALLBACK crossfade_sync_free(HSYNC handle, DWORD channel, DWORD data, void* user) {
	crossfade_sync_unregister(channel);
}

DWORD crossfade_sync_period() {
	DWORD period;
	crossfade_config_get(CF_PERIOD, &period);
	if (!period) {
		period = DEFAULT_PERIOD;
	}
	return period;
}

QWORD crossfade_sync_end_position(HSTREAM handle) {
	return BASS_ChannelGetLength(handle, BASS_POS_BYTE) - BASS_ChannelSeconds2Bytes(handle, crossfade_sync_period() / 1000);
}

BOOL crossfade_sync_register(HSTREAM handle) {
	DWORD position;
	for (position = 0; position < MAX_CHANNELS; position++) {
		if (syncs[position].handle) {
			continue;
		}
		syncs[position].handle = handle;
		syncs[position].fade_in = BASS_ChannelSetSync(handle, BASS_SYNC_POS | BASS_SYNC_ONETIME, 0, &crossfade_sync_begin, NULL);
		syncs[position].fade_out = BASS_ChannelSetSync(handle, BASS_SYNC_POS | BASS_SYNC_ONETIME, crossfade_sync_end_position(handle), &crossfade_sync_end, NULL);
		syncs[position].free = BASS_ChannelSetSync(handle, BASS_SYNC_FREE | BASS_SYNC_ONETIME, 0, &crossfade_sync_free, NULL);
		return TRUE;
	}
	return FALSE;
}

BOOL crossfade_sync_unregister(HSTREAM handle) {
	DWORD position;
	for (position = 0; position < MAX_CHANNELS; position++) {
		if (syncs[position].handle != handle) {
			continue;
		}
		BASS_ChannelRemoveSync(handle, syncs[position].fade_in);
		BASS_ChannelRemoveSync(handle, syncs[position].fade_out);
		BASS_ChannelRemoveSync(handle, syncs[position].free);
		syncs[position].handle = 0;
		syncs[position].fade_in = 0;
		syncs[position].fade_out = 0;
		syncs[position].free = 0;
		return TRUE;
	}
	return FALSE;
}