#include <stdlib.h>
#include <string.h>
#include "mps.h"

void init_model(mps_model_t *m) {
    strcpy(m->name, "");
    m->rows = NULL; m->row_count = 0; m->row_capacity = 0;
    m->rhs = NULL; m->rhs_count = 0; m->rhs_capacity = 0;
    m->bounds = NULL; m->bound_count = 0; m->bound_capacity = 0;
    m->columns = NULL; m->column_count = 0; m->column_capacity = 0;
}

void push_row(mps_model_t *m, row_t r) {
    if (m->row_count >= m->row_capacity) {
        m->row_capacity = m->row_capacity ? m->row_capacity * 2 : 8;
        m->rows = realloc(m->rows, m->row_capacity  *sizeof(row_t));
    }
    m->rows[m->row_count++] = r;
}

void push_col(mps_model_t *m, col_t col) {
    if (m->column_count >= m->column_capacity) {
        m->column_capacity = m->column_capacity ? m->column_capacity * 2 : 8;
        m->columns = realloc(m->columns, m->column_capacity  *sizeof(col_t));
    }
    m->columns[m->column_count++] = col;
}

void push_coeff(col_t *col, coefficient_t c) {
    if (col->coeff_count >= col->coeff_capacity) {
        col->coeff_capacity = col->coeff_capacity ? col->coeff_capacity * 2 : 8;
        col->coeffs = realloc(col->coeffs, col->coeff_capacity  *sizeof(coefficient_t));
    }
    col->coeffs[col->coeff_count++] = c;
}

void push_rhs(mps_model_t *m, rhs_t r) {
    if (m->rhs_count >= m->rhs_capacity) {
        m->rhs_capacity = m->rhs_capacity ? m->rhs_capacity * 2 : 8;
        m->rhs = realloc(m->rhs, m->rhs_capacity  *sizeof(rhs_t));
    }
    m->rhs[m->rhs_count++] = r;
}

void push_bound(mps_model_t *m, bound_t b) {
    if (m->bound_count >= m->bound_capacity) {
        m->bound_capacity = m->bound_capacity ? m->bound_capacity * 2 : 8;
        m->bounds = realloc(m->bounds, m->bound_capacity  *sizeof(bound_t));
    }
    m->bounds[m->bound_count++] = b;
}

col_t *get_col(const mps_model_t *m, char *name) {
    for(int i = 0; i < m->column_count; i++) {
        if (strcmp(m->columns[i].name, name) == 0) {
            return &m->columns[i];
        }
    }
    return NULL;
}

row_t *get_row(const mps_model_t *m, char *name) {
    for(int i = 0; i < m->row_count; i++) {
        if (strcmp(m->rows[i].name, name) == 0) {
            return &m->rows[i];
        }
    }

    return NULL;
}

int get_col_index(const mps_model_t *m, char *name) {
    for(int i = 0; i < m->column_count; i++) {
        if (strcmp(m->columns[i].name, name) == 0) {
            return i;
        }
    }

    return -1;
}

int get_row_index(const mps_model_t *m, char *name) {
    for(int i = 0; i < m->row_count; i++) {
        if (strcmp(m->rows[i].name, name) == 0) {
            return i;
        }
    }

    return -1;
}
