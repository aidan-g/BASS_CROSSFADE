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
		printf("%d,%.4f\n", position, curve[position]);
	}
	printf("\n\n");

	printf("CF_LOGARITHMIC\n");
	crossfade_curve_populate(CF_LOGARITHMIC, 0, 1, curve);
	for (position = 0; position < ENVELOPE_POINTS; position++) {
		printf("%d,%.4f\n", position, curve[position]);
	}
	printf("\n\n");

	printf("CF_EXPONENTIAL\n");
	crossfade_curve_populate(CF_EXPONENTIAL, 0, 1, curve);
	for (position = 0; position < ENVELOPE_POINTS; position++) {
		printf("%d,%.4f\n", position, curve[position]);
	}
	printf("\n\n");

	printf("CF_EASE_IN\n");
	crossfade_curve_populate(CF_EASE_IN, 0, 1, curve);
	for (position = 0; position < ENVELOPE_POINTS; position++) {
		printf("%d,%.4f\n", position, curve[position]);
	}
	printf("\n\n");

	printf("CF_EASE_OUT\n");
	crossfade_curve_populate(CF_EASE_OUT, 0, 1, curve);
	for (position = 0; position < ENVELOPE_POINTS; position++) {
		printf("%d,%.4f\n", position, curve[position]);
	}
	printf("\n\n");
}