#include "crossfade_config.h"
#include "crossfade_envelope.h"
#include "crossfade_mixer.h"
#include "crossfade_syncs.h"

typedef struct {
	HSTREAM handle;
	HSYNC next;
	HSYNC remove;
	HSYNC fade_out;
	HSYNC unregister;
} CF_SYNCS;

static CF_SYNCS syncs[MAX_CHANNELS] = { 0 };

static void CALLBACK __crossfade_sync_next(HSYNC handle, DWORD channel, DWORD data, void* user) {
	crossfade_mixer_next();
}

static void CALLBACK __crossfade_sync_remove(HSYNC handle, DWORD channel, DWORD data, void* user) {
	crossfade_mixer_remove(channel);
}

static void CALLBACK __crossfade_sync_next_remove(HSYNC handle, DWORD channel, DWORD data, void* user) {
	crossfade_mixer_next();
	crossfade_mixer_remove(channel);
}

static void CALLBACK __crossfade_sync_fade_out(HSYNC handle, DWORD channel, DWORD data, void* user) {
	crossfade_envelope_apply_out(channel);
}

static void CALLBACK __crossfade_sync_next_fade_out(HSYNC handle, DWORD channel, DWORD data, void* user) {
	crossfade_mixer_next();
	crossfade_envelope_apply_out(channel);
}

static void CALLBACK __crossfade_sync_unregister(HSYNC handle, DWORD channel, DWORD data, void* user) {
	crossfade_sync_unregister(channel);
}

BOOL crossfade_sync_register(HSTREAM handle) {
	DWORD position;
	DWORD mode;
	DWORD out_period;
	DWORD overlap_period;
	crossfade_config_get(CF_MODE, &mode);
	crossfade_config_get(CF_OUT_PERIOD, &out_period);
	crossfade_config_get(CF_OVERLAP_PERIOD, &overlap_period);
	for (position = 0; position < MAX_CHANNELS; position++) {
		if (syncs[position].handle) {
			continue;
		}
		syncs[position].handle = handle;
		if (mode == CF_ALWAYS && (out_period > 0 || overlap_period > 0)) {
			if (out_period == overlap_period) {
				syncs[position].fade_out = BASS_ChannelSetSync(
					handle,
					BASS_SYNC_POS | BASS_SYNC_ONETIME,
					BASS_ChannelGetLength(handle, BASS_POS_BYTE) - BASS_ChannelSeconds2Bytes(handle, out_period / 1000),
					&__crossfade_sync_next_fade_out,
					NULL
				);
			}
			else if (out_period > 0) {
				syncs[position].fade_out = BASS_ChannelSetSync(
					handle,
					BASS_SYNC_POS | BASS_SYNC_ONETIME,
					BASS_ChannelGetLength(handle, BASS_POS_BYTE) - BASS_ChannelSeconds2Bytes(handle, out_period / 1000),
					&__crossfade_sync_fade_out,
					NULL
				);
			}
			else {
				syncs[position].next = BASS_ChannelSetSync(
					handle,
					BASS_SYNC_END | BASS_SYNC_ONETIME,
					0,
					&__crossfade_sync_remove,
					NULL
				);
			}
			if (out_period != overlap_period && overlap_period > 0) {
				syncs[position].next = BASS_ChannelSetSync(
					handle,
					BASS_SYNC_POS | BASS_SYNC_ONETIME,
					BASS_ChannelGetLength(handle, BASS_POS_BYTE) - BASS_ChannelSeconds2Bytes(handle, overlap_period / 1000),
					&__crossfade_sync_next,
					NULL
				);
			}
			else {
				syncs[position].next = BASS_ChannelSetSync(
					handle,
					BASS_SYNC_END | BASS_SYNC_ONETIME,
					0,
					&__crossfade_sync_next,
					NULL
				);
			}
		}
		else {
			syncs[position].next = BASS_ChannelSetSync(
				handle,
				BASS_SYNC_END | BASS_SYNC_ONETIME,
				0,
				&__crossfade_sync_next_remove,
				NULL
			);
		}
		syncs[position].unregister = BASS_ChannelSetSync(
			handle,
			BASS_SYNC_FREE | BASS_SYNC_ONETIME,
			0,
			&__crossfade_sync_unregister,
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
		if (syncs[position].fade_out) {
			BASS_ChannelRemoveSync(handle, syncs[position].fade_out);
		}
		if (syncs[position].unregister) {
			BASS_ChannelRemoveSync(handle, syncs[position].unregister);
		}
		syncs[position].handle = 0;
		syncs[position].next = 0;
		syncs[position].remove = 0;
		syncs[position].fade_out = 0;
		syncs[position].unregister = 0;
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