#ifndef NON_ZERO_COLUMNS_AND_VALUES_H
#define NON_ZERO_COLUMNS_AND_VALUES_H

#include <stddef.h> // For size_t

// Declare the rows and row_sizes
extern const size_t rows;
extern const size_t row_sizes[];

// Declare the flat arrays
extern const float32_t non_zero_columns_flat[];
extern const float32_t non_zero_values_flat[];
extern const float32_t window[2048];
extern const float32_t coeffs[128][128];
extern float32_t data[611*3];

// Declare the dynamic arrays
//extern float32_t** non_zero_columns;
//extern float32_t** non_zero_values;

// Declare the functions
//void initialize_arrays();
//void free_arrays();

#endif // NON_ZERO_COLUMNS_AND_VALUES_H
