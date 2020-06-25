#include "../bass/bass.h"

HSTREAM crossfade_mixer_create(DWORD freq, DWORD chans, DWORD flags, void* user);

BOOL crossfade_mixer_playing();

BOOL crossfade_mixer_get(HSTREAM* handle);

HSTREAM* crossfade_mixer_get_all(DWORD* count);

BOOL crossfade_mixer_add(HSTREAM handle, BOOL fade_in);

BOOL crossfade_mixer_remove(HSTREAM handle, BOOL fade_out);

BOOL crossfade_mixer_next(BOOL fade_in);