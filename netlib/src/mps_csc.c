#include "mps_csc.h"
#include <stdlib.h>
#include <string.h>


int verify_normalized(const mps_model_t *m) {
   if(m->bound_count > 0) return 0;

   int n_count = 0;
   for(int i = 0; i < m->row_count; i++) {
      if(m->rows[i].type != ROW_E) {
         if (m->rows[i].type == ROW_N) {
            n_count += 1;
         }
         else {
            return 0;
         }
      }
   }
   if(n_count != 1) return 0;

   return 1;
}

void write_csc(FILE *out, const mps_model_t *m) {
   if(!verify_normalized(m)) {
      fprintf(stderr, "Non normalized file!");
      exit(1);
   }
   
   int n_index = -1;
   for(int i = 0; i < m->row_count && n_index < 0; i++) {
      if (m->rows[i].type == ROW_N) {
         n_index = i;
      }
   }
   row_t n_row = m->rows[n_index];

   // Count everything except for the non zero values.
   int total_entries = 0;
   for(int i = 0; i < m->column_count; i++) {
      total_entries += m->columns[i].coeff_count;
   }

   int *column_ptr = malloc((m->column_count + 1) * sizeof(int));
   int    *row_index = malloc(total_entries * sizeof(int));
   double *value_arr = malloc(total_entries * sizeof(double));
   double *cost_arr  = calloc(m->column_count, sizeof(double));

   int nnz = 0;
   for(int i = 0; i < m->column_count; i++) {
      column_ptr[i] = nnz;
      col_t *col = &m->columns[i];
      for(int j = 0; j < col->coeff_count; j++) {
         if(strcmp(col->coeffs[j].row, n_row.name) != 0) {
            int index = get_row_index(m, col->coeffs[j].row);
            if (index > n_index) index -= 1;
            row_index[nnz] = index;
            value_arr[nnz] = col->coeffs[j].value;
            nnz += 1;
         } else {
            cost_arr[i] = col->coeffs[j].value;
         }
      }
   }
   column_ptr[m->column_count] = nnz;

   double *b = calloc(m->row_count, sizeof(double));
   for(int i = 0; i < m->rhs_count; i++) {
      int index = get_row_index(m, m->rhs[i].row);
      if (index > n_index) index -= 1;
      b[index] = m->rhs[i].value;
   }

   // name format m:rows n:cols nnz
   fprintf(out, "A csc %d %d %d\n", m->row_count-1, m->column_count, nnz);

   fprintf(out, "%d", column_ptr[0]);
   for(int i = 1; i < m->column_count + 1; i++) fprintf(out, " %d", column_ptr[i]);
   fprintf(out, "\n");

   fprintf(out, "%d", row_index[0]);
   for(int i = 1; i < nnz; i++) fprintf(out, " %d", row_index[i]);
   fprintf(out, "\n");

   fprintf(out, "%lf", value_arr[0]);
   for(int i = 1; i < nnz; i++) fprintf(out, " %lf", value_arr[i]);
   fprintf(out, "\n");

   fprintf(out, "b dense %d\n", m->row_count-1);
   fprintf(out, "%lf", b[0]);
   for(int i = 1; i < m->row_count-1; i++) fprintf(out, " %lf", b[i]);
   fprintf(out, "\n");

   fprintf(out, "c dense %d\n", m->column_count);
   fprintf(out, "%lf", cost_arr[0]);
   for(int i = 1; i < m->column_count; i++) fprintf(out, " %lf", cost_arr[i]);
   fprintf(out, "\n");

   free(column_ptr);
   free(row_index);
   free(value_arr);
   free(cost_arr);
   free(b);
}
