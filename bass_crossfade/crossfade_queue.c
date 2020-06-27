#ifdef _DEBUG
#include <stdio.h>
#endif

#include "bass_crossfade.h"
#include "crossfade_queue.h"
#include "queue.h"

QUEUE* queue = NULL;

BOOL crossfade_queue_create() {
	if (queue) {
		return FALSE;
	}
	queue = queue_create(MAX_CHANNELS, TRUE);
	if (!queue) {
#if _DEBUG
		printf("Failed to create queue.\n");
#endif
		return FALSE;
	}
	return TRUE;
}

BOOL crossfade_queue_contains(HSTREAM handle) {
	if (!queue) {
		return FALSE;
	}
	return queue_contains(queue, (void*)handle);
}

BOOL crossfade_queue_enqueue(HSTREAM handle) {
	if (!queue) {
		return FALSE;
	}
#if _DEBUG
	printf("Enqueuing channel: %d\n", handle);
#endif
	return queue_enqueue(queue, (void*)handle, TRUE);
}

BOOL crossfade_queue_dequeue(HSTREAM* handle) {
	if (!queue) {
		return FALSE;
	}
#if _DEBUG
	printf("Dequeuing channel: %d\n", handle);
#endif
	return queue_dequeue(queue, (void**)handle);
}

BOOL crossfade_queue_push(HSTREAM handle) {
	if (!queue) {
		return FALSE;
	}
#if _DEBUG
	printf("Enqueuing channel: %d\n", handle);
#endif
	return queue_push(queue, (void*)handle, TRUE);
}

BOOL crossfade_queue_peek(HSTREAM* handle) {
	if (!queue) {
		return FALSE;
	}
	return queue_peek(queue, (void**)handle);
}

BOOL crossfade_queue_remove(HSTREAM handle) {
	if (!queue) {
		return FALSE;
	}
#if _DEBUG
	printf("Removing channel: %d\n", handle);
#endif
	return queue_remove(queue, (void*)handle);
}

BOOL crossfade_queue_is_empty(BOOL* empty) {
	if (!queue) {
		return FALSE;
	}
	return queue_is_empty(queue, empty);
}

BOOL crossfade_queue_count(DWORD* length) {
	if (!queue) {
		return FALSE;
	}
	*length = queue->length;
	return TRUE;
}

HSTREAM* crossfade_queue_get_all(DWORD* length) {
	static HSTREAM buffer[MAX_CHANNELS];
	if (!queue) {
		return NULL;
	}
	if (!queue_get_all(queue, (void**)buffer, sizeof(HSTREAM), length)) {
		*length = 0;
	}
	return buffer;
}

BOOL crossfade_queue_free() {
	if (!queue) {
		return FALSE;
	}
	queue_free(queue);
	queue = NULL;
	return TRUE;
}