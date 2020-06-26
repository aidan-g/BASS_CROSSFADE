#include "../bass/bass.h"

HSTREAM* crossfade_queue_get_all(DWORD* count);

BOOL crossfade_queue_peek(HSTREAM* handle);

BOOL crossfade_queue_add(HSTREAM handle);

BOOL crossfade_queue_insert(HSTREAM handle, DWORD position);

BOOL crossfade_queue_remove(HSTREAM handle);