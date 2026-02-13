#pragma once
#include <stdio.h>
#include "mps.h"

/* ============================
   Writer to Compressed Sparse Column format.
   ============================ */
void write_csc(FILE *out, const mps_model_t *m);
