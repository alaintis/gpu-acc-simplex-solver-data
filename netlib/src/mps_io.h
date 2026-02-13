#pragma once
#include <stdio.h>
#include "mps.h"

/* ============================
   Parsing Logic and Writer
   ============================ */
void parse_mps(FILE *f, mps_model_t *m);
void write_mps(FILE *out, const mps_model_t *m);
