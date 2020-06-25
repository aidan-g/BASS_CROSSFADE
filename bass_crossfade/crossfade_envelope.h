#include "../bass/bass.h"

#define ENVELOPE_POINTS 50

__declspec(dllexport)
BOOL crossfade_curve_populate(DWORD type, float min, float max, float curve[ENVELOPE_POINTS]);

BOOL crossfade_envelope_apply_in(HSTREAM handle);

BOOL crossfade_envelope_apply_out(HSTREAM handle, BOOL remove);