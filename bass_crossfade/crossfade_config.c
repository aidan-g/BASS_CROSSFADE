#ifdef _DEBUG
#include <stdio.h>
#endif

#include "crossfade_config.h"

#define MAX_CONFIGS 10

static DWORD config[MAX_CONFIGS] = { 0 };

BOOL crossfade_config_set(CF_ATTRIBUTE attrib, DWORD value) {
	config[attrib] = value;
#if _DEBUG
	printf("Setting config: %d = %d\n", attrib, value);
#endif
	return TRUE;
}

BOOL crossfade_config_get(CF_ATTRIBUTE attrib, DWORD* value) {
	if (*value = config[attrib]) {
#if _DEBUG
		printf("Getting config: %d = %d\n", attrib, *value);
#endif
		return TRUE;
	}
	return FALSE;
}