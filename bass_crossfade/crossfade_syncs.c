#include "crossfade_config.h"
#include "crossfade_mixer.h"
#include "crossfade_syncs.h"
#include "crossfade_volume.h"

typedef struct {
	HSTREAM handle;
	HSYNC fade_in;
	HSYNC fade_out;
	HSYNC next;
	HSYNC end;
	HSYNC free;
} CF_SYNCS;

static CF_SYNCS syncs[MAX_CHANNELS] = { 0 };

void CALLBACK crossfade_sync_fade_in(HSYNC handle, DWORD channel, DWORD data, void* user) {
	DWORD mode;
	crossfade_config_get(CF_MODE, &mode);
	if (mode != CF_ALWAYS) {
		DWORD count;
		crossfade_mixer_get_all(&count);
		if (count == 0) {
			return;
		}
	}
	crossfade_fade_in_async(channel);
}

void CALLBACK crossfade_sync_fade_out(HSYNC handle, DWORD channel, DWORD data, void* user) {
	DWORD mode;
	crossfade_config_get(CF_MODE, &mode);
	if (mode != CF_ALWAYS) {
		return;
	}
	crossfade_fade_out_async(channel);
}

void CALLBACK crossfade_sync_next(HSYNC handle, DWORD channel, DWORD data, void* user) {
	DWORD mode;
	crossfade_config_get(CF_MODE, &mode);
	if (mode != CF_ALWAYS) {
		return;
	}
	crossfade_mixer_next();
}

void CALLBACK crossfade_sync_end(HSYNC handle, DWORD channel, DWORD data, void* user) {
	DWORD mode;
	crossfade_config_get(CF_MODE, &mode);
	if (mode != CF_MANUAL) {
		return;
	}
	crossfade_mixer_next();
}

void CALLBACK crossfade_sync_free(HSYNC handle, DWORD channel, DWORD data, void* user) {
	crossfade_sync_unregister(channel);
}

QWORD crossfade_sync_position(HSTREAM handle, CF_ATTRIBUTE attrib) {
	DWORD period;
	crossfade_config_get(attrib, &period);
	return BASS_ChannelGetLength(handle, BASS_POS_BYTE) - BASS_ChannelSeconds2Bytes(handle, period / 1000);
}

BOOL crossfade_sync_refresh() {
	DWORD position;
	for (position = 0; position < MAX_CHANNELS; position++) {
		if (!syncs[position].handle) {
			continue;
		}
		if (syncs[position].fade_out) {
			BASS_ChannelRemoveSync(syncs[position].handle, syncs[position].fade_out);
			syncs[position].fade_out = BASS_ChannelSetSync(
				syncs[position].handle,
				BASS_SYNC_POS | BASS_SYNC_ONETIME,
				crossfade_sync_position(syncs[position].handle, CF_OUT_PERIOD),
				&crossfade_sync_fade_out,
				NULL
			);
			syncs[position].next = BASS_ChannelSetSync(
				syncs[position].handle,
				BASS_SYNC_POS | BASS_SYNC_ONETIME,
				crossfade_sync_position(syncs[position].handle, CF_OVERLAP_PERIOD),
				&crossfade_sync_next,
				NULL
			);
		}
	}
	return TRUE;
}

BOOL crossfade_sync_register(HSTREAM handle) {
	DWORD position;
	for (position = 0; position < MAX_CHANNELS; position++) {
		if (syncs[position].handle) {
			continue;
		}
		syncs[position].handle = handle;
		syncs[position].fade_in = BASS_ChannelSetSync(
			handle,
			BASS_SYNC_POS | BASS_SYNC_ONETIME,
			0,
			&crossfade_sync_fade_in,
			NULL
		);
		syncs[position].fade_out = BASS_ChannelSetSync(
			handle,
			BASS_SYNC_POS | BASS_SYNC_ONETIME,
			crossfade_sync_position(handle, CF_OUT_PERIOD),
			&crossfade_sync_fade_out,
			NULL
		);
		syncs[position].next = BASS_ChannelSetSync(
			handle,
			BASS_SYNC_POS | BASS_SYNC_ONETIME,
			crossfade_sync_position(handle, CF_OVERLAP_PERIOD),
			&crossfade_sync_next,
			NULL
		);
		syncs[position].end = BASS_ChannelSetSync(
			handle,
			BASS_SYNC_END | BASS_SYNC_ONETIME,
			0,
			&crossfade_sync_end,
			NULL
		);
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
		BASS_ChannelRemoveSync(handle, syncs[position].fade_in);
		BASS_ChannelRemoveSync(handle, syncs[position].fade_out);
		BASS_ChannelRemoveSync(handle, syncs[position].next);
		BASS_ChannelRemoveSync(handle, syncs[position].end);
		BASS_ChannelRemoveSync(handle, syncs[position].free);
		syncs[position].handle = 0;
		syncs[position].fade_in = 0;
		syncs[position].fade_out = 0;
		syncs[position].next = 0;
		syncs[position].end = 0;
		syncs[position].free = 0;
		return TRUE;
	}
	return FALSE;
}