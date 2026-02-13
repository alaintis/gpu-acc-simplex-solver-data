#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mps_normalize.h"

static int row_count = 0;
// New name has 32 chars, name is "short"
void new_row(mps_model_t *m, char *new_name, char *name) {
    do {
        sprintf(new_name, "%s%d", name, row_count);
        row_count += 1;
    } while (get_row(m, new_name));
}

static int col_count = 0;
// New name has 32 chars, name is "short"
void new_col(mps_model_t *m, char *new_name, char *name) {
    do {
        sprintf(new_name, "%s%d", name, col_count);
        col_count += 1;
    } while (get_col(m, new_name));
}

void bound_row(mps_model_t *m, char *var, double value, int upper) {
    char row_name[32];
    new_row(m, row_name, upper ? "UPBROW" : "LOBROW");

    row_t row;
    strcpy(row.name, row_name);
    row.type = upper ? ROW_L : ROW_G;
    push_row(m, row);

    rhs_t rhs;
    rhs.value = value;
    strcpy(rhs.name, "BOUNDRHS");
    strcpy(rhs.row, row_name);
    push_rhs(m, rhs);
    
    col_t *col = get_col(m, var);
    coefficient_t coeff;
    strcpy(coeff.row, row_name);
    coeff.value = 1.0;
    push_coeff(col, coeff);
}

void split_var(mps_model_t *m, char *var, char *neg_var) {
    char nv2[32];
    if(neg_var == NULL) neg_var = nv2;

    char name[32];
    sprintf(name, "NEG%s", var);
    new_col(m, neg_var, name);

    col_t *pos_col = get_col(m, var);
    col_t col = {
        .coeff_capacity = 0,
        .coeff_count = 0,
        .coeffs = NULL
    };
    strcpy(col.name, neg_var);

    for(int i = 0; i < pos_col->coeff_count; i++) {
        coefficient_t n_coeff;
        strcpy(n_coeff.row, pos_col->coeffs[i].row);
        n_coeff.value = -pos_col->coeffs[i].value;

        push_coeff(&col, n_coeff);
    }

    push_col(m, col);
}

void replace_var(mps_model_t *m, char *var, double value) {
    col_t *col = get_col(m, var);

    for(int i = 0; i < col->coeff_count; i++) {
        coefficient_t *c = &col->coeffs[i];

        int used = 0;
        for(int j = 0; j < m->rhs_count; j++) {
            if (strcmp(m->rhs[j].row, c->row) == 0) {
                used = 1;
                m->rhs[j].value -= value * c->value;
            }
        }

        if(!used) {
            // Implicit restriction.
            rhs_t rhs;
            rhs.value = - value * c->value;
            strcpy(rhs.name, "FXIMPLICITRHS");
            strcpy(rhs.row, c->row);
            push_rhs(m, rhs);
        }
    }

    // Remove the column by replacint it with the last column.
    free(col->coeffs);
    col->coeffs = NULL;

    col_t *last_col = &m->columns[m->column_count-1];
    if(last_col != col) {
        strcpy(col->name, last_col->name);
        col->coeff_capacity = last_col->coeff_capacity;
        col->coeff_count = last_col->coeff_count;
        col->coeffs = last_col->coeffs;
    }

    m->column_count -= 1;
}


// If we have a negative upperbound we turn the variable around.
void invert_negative(mps_model_t *m) {
    for(int i = 0; i < m->bound_count; i++) {
        bound_t *b = &m->bounds[i];
        if(b->type == BOUND_UP && b->value < 0) {
            for(int j = 0; j < m->bound_count; j++) {
                if (strcmp(b->var, m->bounds[j].var) == 0) {
                    if(m->bounds[j].type == BOUND_LO || m->bounds[j].type == BOUND_UP) {
                        m->bounds[j].value *= -1.0;
                        m->bounds[j].type = (m->bounds[j].type == BOUND_LO) ? BOUND_UP : BOUND_LO;
                    }
                    else {
                        fprintf(stderr, "BAD ROTATION!! %s\n", b->var);
                    }
                }
            }

            col_t *col = get_col(m, b->var);
            for(int j = 0; j < col->coeff_count; j++) {
                col->coeffs[j].value *= -1.0;
            }
        }
    }
}

// Turn bounds into all the options we can consider.
void normalize_bounds(mps_model_t *m) {
    for(int i = 0; i < m->bound_count; i++) {
        bound_t *b = &m->bounds[i];
        switch (b->type)
        {
        case BOUND_UP:
            bound_row(m, b->var, b->value, 1);
            break;
        case BOUND_LO:
            if(b->value < 0) {
                char neg_var[32];
                split_var(m, b->var, neg_var);
                bound_row(m, neg_var, -b->value, 1);
            } else {
                bound_row(m, b->var, b->value, 0);
            }
            break;
        case BOUND_FR:
            split_var(m, b->var, NULL);
            break;
        case BOUND_FX:
            replace_var(m, b->var, b->value);
            break;
        case BOUND_PL:
            break;
        default:
            fprintf(stderr, "UNKNOWN BOUND: %d\n", b->type);
            exit(1);
        }
    }
    m->bound_count = 0;
}

// Turn each L / G row into a new constraint.
void normalize_rows(mps_model_t *m) {
    for(int i = 0; i < m->row_count; i++) {
        if(m->rows[i].type == ROW_G || m->rows[i].type == ROW_L) {
            char slack_name[32];
            new_col(m, slack_name, "SLACK");

            col_t c;
            strcpy(c.name, slack_name);
            c.coeff_capacity = 0;
            c.coeff_count = 0;
            c.coeffs = NULL;
            push_col(m, c);

            coefficient_t coeff;
            strcpy(coeff.row, m->rows[i].name);
            coeff.value = (m->rows[i].type == ROW_L) ? 1.0 : -1.0;

            push_coeff(&m->columns[m->column_count-1], coeff);
            m->rows[i].type = ROW_E;
        }
    }
}

void normalize(mps_model_t *m) {
    invert_negative(m);
    normalize_bounds(m);
    normalize_rows(m);
}
