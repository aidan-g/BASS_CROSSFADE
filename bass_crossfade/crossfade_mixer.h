#include "../bass/bass.h"

BOOL crossfade_mixer_playing();

BOOL crossfade_mixer_get(HSTREAM* handle);

HSTREAM* crossfade_mixer_get_all(DWORD* count);

BOOL crossfade_mixer_add(HSTREAM handle);

BOOL crossfade_mixer_remove(HSTREAM handle);

BOOL crossfade_mixer_next();