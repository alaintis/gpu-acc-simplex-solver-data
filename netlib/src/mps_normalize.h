#pragma once
#include "mps.h"

// Turn bounds into all the options we can consider.
void normalize_bounds(mps_model_t *m);
// Turn each L / G row into a new constraint.
void normalize_rows(mps_model_t *m);


void normalize(mps_model_t *m);
