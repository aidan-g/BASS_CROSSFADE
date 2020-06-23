#include "crossfade_config.h"
#include "crossfade_envelope.h"
#include "crossfade_mixer.h"
#include "crossfade_syncs.h"

typedef struct {
	HSTREAM handle;
	HSYNC next;
	HSYNC remove;
	HSYNC free;
} CF_SYNCS;

static CF_SYNCS syncs[MAX_CHANNELS] = { 0 };

void CALLBACK crossfade_next(HSYNC handle, DWORD channel, DWORD data, void* user) {
	crossfade_mixer_next();
	crossfade_mixer_remove(channel);
}

void CALLBACK crossfade_next_mix(HSYNC handle, DWORD channel, DWORD data, void* user) {
	crossfade_mixer_next();
	crossfade_envelope_apply_out(channel);
}

void CALLBACK crossfade_sync_free(HSYNC handle, DWORD channel, DWORD data, void* user) {
	crossfade_sync_unregister(channel);
}

BOOL crossfade_sync_register(HSTREAM handle) {
	DWORD position;
	DWORD mode;
	DWORD period;
	crossfade_config_get(CF_MODE, &mode);
	crossfade_config_get(CF_OUT_PERIOD, &period);
	for (position = 0; position < MAX_CHANNELS; position++) {
		if (syncs[position].handle) {
			continue;
		}
		syncs[position].handle = handle;
		if (mode == CF_ALWAYS) {
			syncs[position].next = BASS_ChannelSetSync(
				handle,
				BASS_SYNC_POS | BASS_SYNC_ONETIME,
				BASS_ChannelGetLength(handle, BASS_POS_BYTE) - BASS_ChannelSeconds2Bytes(handle, period / 1000),
				&crossfade_next_mix,
				NULL
			);
			syncs[position].remove = BASS_ChannelSetSync(
				handle,
				BASS_SYNC_END | BASS_SYNC_ONETIME,
				0,
				&crossfade_sync_free,
				NULL
			);
		}
		else {
			syncs[position].next = BASS_ChannelSetSync(
				handle,
				BASS_SYNC_END | BASS_SYNC_ONETIME,
				0,
				&crossfade_next,
				NULL
			);
		}
		syncs[position].free = BASS_ChannelSetSync(
			handle,
			BASS_SYNC_FREE | BASS_SYNC_ONETIME,
			0,
			&crossfade_sync_free,
			NULL
		);
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
		if (syncs[position].next) {
			BASS_ChannelRemoveSync(handle, syncs[position].next);
		}
		if (syncs[position].remove) {
			BASS_ChannelRemoveSync(handle, syncs[position].remove);
		}
		if (syncs[position].free) {
			BASS_ChannelRemoveSync(handle, syncs[position].free);
		}
		syncs[position].handle = 0;
		syncs[position].next = 0;
		syncs[position].remove = 0;
		syncs[position].free = 0;
		return TRUE;
	}
	return FALSE;
}

BOOL crossfade_sync_refresh() {
	DWORD position;
	DWORD handle;
	BOOL success = TRUE;
	for (position = 0; position < MAX_CHANNELS; position++) {
		handle = syncs[position].handle;
		if (!handle) {
			continue;
		}
		crossfade_sync_unregister(handle);
		crossfade_sync_register(handle);
	}
	return success;
}