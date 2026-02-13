#include "mps_io.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    SEC_NONE,
    SEC_ROWS,
    SEC_COLUMNS,
    SEC_RHS,
    SEC_BOUNDS
} section_e;

row_type_e parse_row_type(const char *s) {
    if (strcmp(s, "N") == 0) return ROW_N;
    if (strcmp(s, "L") == 0) return ROW_L;
    if (strcmp(s, "G") == 0) return ROW_G;
    if (strcmp(s, "E") == 0) return ROW_E;
    fprintf(stderr, "parse_row failed: %s", s);
    exit(1);
}

bound_type_e parse_bound_type(const char *s) {
    if (strcmp(s, "LO") == 0) return BOUND_LO;
    if (strcmp(s, "UP") == 0) return BOUND_UP;
    if (strcmp(s, "FX") == 0) return BOUND_FX;
    if (strcmp(s, "FR") == 0) return BOUND_FR;
    if (strcmp(s, "PL") == 0) return BOUND_PL;
    fprintf(stderr, "parse_bound failed: %s", s);
    exit(1);
}

void parse_rows(mps_model_t *m, char *line) {
    char type[8], name[32];
    if (sscanf(line, "%s %s", type, name) == 2) {
        row_t r;
        strcpy(r.name, name);
        r.type = parse_row_type(type);
        push_row(m, r);
    }
}

void parse_columns(mps_model_t *m, char *line) {
    // Up to two row/value pairs per line
    char col[32], row1[32], row2[32];
    double val1, val2;
    int count = sscanf(line, "%s %s %lf %s %lf",
                       col, row1, &val1, row2, &val2);

                       
    col_t *c = get_col(m, col);
    if(c == NULL) {
        col_t new_col;
        strcpy(new_col.name, col);
        new_col.coeff_capacity = 0;
        new_col.coeff_count = 0;
        new_col.coeffs = NULL;

        push_col(m, new_col);
        c = get_col(m, col);
    }

    if (count >= 3) {
        coefficient_t c1;
        strcpy(c1.row, row1);
        c1.value = val1;

        push_coeff(c, c1);
    }
    if (count == 5) {
        coefficient_t c2;
        strcpy(c2.row, row2);
        c2.value = val2;
        push_coeff(c, c2);
    }
}

void parse_rhs(mps_model_t *m, char *line) {
    int pos = 0;
    int len = strlen(line);

    // Skip leading spaces
    while (pos < len && isspace(line[pos])) pos++;

    
    char rhsname[32];
    char row1[32], row2[32];
    double val1, val2;
    int count;
    if (pos < 10) {
        count = sscanf(&line[pos], "%s %s %lf %s %lf",
                        rhsname, row1, &val1, row2, &val2);
    } else {
        strcpy(rhsname, "NONAME");
        count = 1 + sscanf(&line[pos], "%s %lf %s %lf",
                            row1, &val1, row2, &val2);
    }

    if (count >= 3) {
        rhs_t r1;
        strcpy(r1.name, rhsname);
        strcpy(r1.row, row1);
        r1.value = val1;
        push_rhs(m, r1);
    }
    if (count == 5) {
        rhs_t r2;
        strcpy(r2.name, rhsname);
        strcpy(r2.row, row2);
        r2.value = val2;
        push_rhs(m, r2);
    }
}

void parse_bounds(mps_model_t *m, char *line) {
    char type[4], bndname[32], var[32];
    double val;

    if (sscanf(line, "%s %s %s %lf", type, bndname, var, &val) == 4) {
        bound_t b;
        b.type = parse_bound_type(type);
        strcpy(b.name, bndname);
        strcpy(b.var, var);
        b.value = val;
        push_bound(m, b);
    } else if(sscanf(line, "%s %s %lf", type, var, &val) == 3) {
        bound_t b;
        b.type = parse_bound_type(type);
        strcpy(b.name, "NONAME");
        strcpy(b.var, var);
        b.value = val;
        push_bound(m, b);
    } else if(sscanf(line, "%s %s %s", type, bndname, var) == 3) {
        bound_t b;
        b.type = parse_bound_type(type);
        if(b.type != BOUND_FR && b.type != BOUND_PL) fprintf(stderr, "Under specified bound: %d %s\n", b.type, var);
        strcpy(b.name, bndname);
        strcpy(b.var, var);
        b.value = 0.0;
        push_bound(m, b);
    } else if(sscanf(line, "%s %s", type, var) == 2) {
        bound_t b;
        b.type = parse_bound_type(type);
        if(b.type != BOUND_FR) fprintf(stderr, "Under specified bound: %d %s", b.type, var);
        strcpy(b.name, "NONAME");
        strcpy(b.var, var);
        b.value = 0.0;
        push_bound(m, b);
    }
}

/* ============================
   Main Parser
   ============================ */

void parse_mps(FILE *f, mps_model_t *m) {
    char buff[256];
    section_e sec = SEC_NONE;

    while (fgets(buff, sizeof(buff), f)) {
        if (buff[0] == '*') continue;  // comment

        // Trim leading space
        char *line = buff;

        if (strncmp(line, "NAME", 4) == 0) {
            sscanf(line + 4, "%s", m->name);
            continue;
        }
        if (strncmp(line, "ROWS", 4) == 0) { sec = SEC_ROWS; continue; }
        if (strncmp(line, "COLUMNS", 7) == 0) { sec = SEC_COLUMNS; continue; }
        if (strncmp(line, "RHS", 3) == 0) { sec = SEC_RHS; continue; }
        if (strncmp(line, "BOUNDS", 6) == 0) { sec = SEC_BOUNDS; continue; }
        if (strncmp(line, "ENDATA", 6) == 0) break;

        while (isspace(*line)) line++;
        if (!*line) continue;

        switch (sec) {
            case SEC_ROWS:    parse_rows(m, line); break;
            case SEC_COLUMNS: parse_columns(m, line); break;
            case SEC_RHS:     parse_rhs(m, buff); break;
            case SEC_BOUNDS:  parse_bounds(m, line); break;
            default: break;
        }
    }
}

void write_mps(FILE *out, const mps_model_t *m) {
    fprintf(out, "NAME          %s\n", m->name[0] ? m->name : "NONAME");

    fprintf(out, "ROWS\n");
    for (int i = 0; i < m->row_count; i++) {
        const row_t *r = &m->rows[i];
        const char *t = NULL;
        switch (r->type) {
            case ROW_N: t = "N"; break;
            case ROW_L: t = "L"; break;
            case ROW_G: t = "G"; break;
            case ROW_E: t = "E"; break;
        }
        fprintf(out, " %1s  %-8s\n", t, r->name);
    }

    fprintf(out, "COLUMNS\n");
    for (int i = 0; i < m->column_count; i++) {
        col_t *c = &m->columns[i];
        int j;
        for(j = 0; j+1 < c->coeff_count; j += 2) {
            const coefficient_t *c0 = &c->coeffs[j];
            const coefficient_t *c1 = &c->coeffs[j+1];
            fprintf(out, "    %-8s %-8s %lf   %-8s %lf\n", c->name, c0->row, c0->value, c1->row, c1->value);
        }
        if(j < c->coeff_count) {
            const coefficient_t *c0 = &c->coeffs[j];
            fprintf(out, "    %-8s %-8s %lf\n", c->name, c0->row, c0->value);
        }
    }

    /* ---------------- RHS ---------------- */
    fprintf(out, "RHS\n");
    for (int i = 0; i < m->rhs_count; i++) {
        const rhs_t *r1 = &m->rhs[i];
        fprintf(out, "    %-8s %-8s %lf\n", r1->name, r1->row, r1->value);
    }

    /* ---------------- BOUNDS ---------------- */
    fprintf(out, "BOUNDS\n");
    for (int i = 0; i < m->bound_count; i++) {
        const bound_t *b = &m->bounds[i];

        const char *t = NULL;
        switch (b->type) {
            case BOUND_LO: t = "LO"; break;
            case BOUND_UP: t = "UP"; break;
            case BOUND_FX: t = "FX"; break;
            case BOUND_FR: t = "FR"; break;
            case BOUND_PL: t = "PL"; break;
        }

        fprintf(out, " %-2s %-8s %-8s %lf\n",
                t, b->name, b->var, b->value);
    }

    fprintf(out, "ENDATA\n");
}
