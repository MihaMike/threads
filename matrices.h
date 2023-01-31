void init_matrix (double * matrix, int n);
void init_vector (double * vector, int n);
void print_matrix (double * matrix, int n);
void print_vector (double * vector, int n);
void matrix_mult_vector (double *a, double *b, double *c,
                         int n, int thread_num, 
                         int total_threads);
