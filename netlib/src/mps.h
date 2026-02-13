#pragma once

typedef enum {
    ROW_N, ROW_L, ROW_G, ROW_E
} row_type_e;

typedef enum {
    BOUND_UP, BOUND_LO, BOUND_FR, BOUND_FX, BOUND_PL
} bound_type_e;

typedef struct {
    char name[32];
    row_type_e type;
} row_t;

typedef struct {
    char row[32];
    double value;
} coefficient_t;

typedef struct {
    char name[32];
    coefficient_t *coeffs;
    int coeff_count;
    int coeff_capacity;
} col_t;

typedef struct {
    char row[32];
    char name[32];
    double value;
} rhs_t;

typedef struct {
    bound_type_e type;
    char name[32];
    char var[32];
    double value;
} bound_t;

typedef struct {
    char name[32];
    row_t *rows;
    int row_count;
    int row_capacity;

    rhs_t *rhs;
    int rhs_count;
    int rhs_capacity;

    bound_t *bounds;
    int bound_count;
    int bound_capacity;

    col_t *columns;
    int column_count;
    int column_capacity;
} mps_model_t;


void init_model(mps_model_t *m);

void push_row(mps_model_t *m, row_t r);
void push_col(mps_model_t *m, col_t col);
void push_coeff(col_t *col, coefficient_t c);
void push_rhs(mps_model_t *m, rhs_t r);
void push_bound(mps_model_t *m, bound_t b);

col_t *get_col(const mps_model_t *m, char *name);
row_t *get_row(const mps_model_t *m, char *name);

int get_col_index(const mps_model_t *m, char *name);
int get_row_index(const mps_model_t *m, char *name);
