#include "../bass/bass.h"

HSTREAM* crossfade_mixer_get_all(DWORD* count);

BOOL crossfade_mixer_add(HSTREAM handle, BOOL force);

BOOL crossfade_mixer_remove(HSTREAM handle);

BOOL crossfade_mixer_next();