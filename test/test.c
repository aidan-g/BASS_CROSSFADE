#include <stdio.h>

#include "../bass_crossfade/bass_crossfade.h"
#include "../bass_crossfade/crossfade_envelope.h"

int main()
{
	DWORD position;
	float curve[ENVELOPE_POINTS];

	printf("CF_LINEAR\n");
	crossfade_curve_populate(CF_LINEAR, 0, 1, curve);
	for (position = 0; position < ENVELOPE_POINTS; position++) {
		printf("%d %.4f\n", position, curve[position]);
	}
	crossfade_curve_populate(CF_LINEAR, 1, 0, curve);
	for (position = 0; position < ENVELOPE_POINTS; position++) {
		printf("%d %.4f\n", position, curve[position]);
	}
	printf("\n\n");

	printf("CF_IN_QUAD\n");
	crossfade_curve_populate(CF_IN_QUAD, 0, 1, curve);
	for (position = 0; position < ENVELOPE_POINTS; position++) {
		printf("%d %.4f\n", position, curve[position]);
	}
	crossfade_curve_populate(CF_IN_QUAD, 1, 0, curve);
	for (position = 0; position < ENVELOPE_POINTS; position++) {
		printf("%d %.4f\n", position, curve[position]);
	}
	printf("\n\n");

	printf("CF_OUT_QUAD\n");
	crossfade_curve_populate(CF_OUT_QUAD, 0, 1, curve);
	for (position = 0; position < ENVELOPE_POINTS; position++) {
		printf("%d %.4f\n", position, curve[position]);
	}
	crossfade_curve_populate(CF_OUT_QUAD, 1, 0, curve);
	for (position = 0; position < ENVELOPE_POINTS; position++) {
		printf("%d %.4f\n", position, curve[position]);
	}
	printf("\n\n");

	printf("CF_IN_EXPO\n");
	crossfade_curve_populate(CF_IN_EXPO, 0, 1, curve);
	for (position = 0; position < ENVELOPE_POINTS; position++) {
		printf("%d %.4f\n", position, curve[position]);
	}
	crossfade_curve_populate(CF_IN_EXPO, 1, 0, curve);
	for (position = 0; position < ENVELOPE_POINTS; position++) {
		printf("%d %.4f\n", position, curve[position]);
	}
	printf("\n\n");

	printf("CF_OUT_EXPO\n");
	crossfade_curve_populate(CF_OUT_EXPO, 0, 1, curve);
	for (position = 0; position < ENVELOPE_POINTS; position++) {
		printf("%d %.4f\n", position, curve[position]);
	}
	crossfade_curve_populate(CF_OUT_EXPO, 1, 0, curve);
	for (position = 0; position < ENVELOPE_POINTS; position++) {
		printf("%d %.4f\n", position, curve[position]);
	}
	printf("\n\n");
}