#include "../bass/bass.h"

BOOL crossfade_queue_create();

BOOL crossfade_queue_contains(HSTREAM handle);

BOOL crossfade_queue_enqueue(HSTREAM handle);

BOOL crossfade_queue_dequeue(HSTREAM* handle);

BOOL crossfade_queue_push(HSTREAM handle);

BOOL crossfade_queue_peek(HSTREAM* handle);

BOOL crossfade_queue_is_empty(BOOL* empty);

BOOL crossfade_queue_count(DWORD* length);

HSTREAM* crossfade_queue_get_all(DWORD* length);

BOOL crossfade_queue_remove(HSTREAM handle);

BOOL crossfade_queue_free();