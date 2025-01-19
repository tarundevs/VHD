#include "savgol_filter.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_POLYORDER 10
#define MAX_WINDOW_LENGTH 51

// Helper function declarations
static float32_t factorial(int n);
static float32_t** allocate_2d_array(int rows, int cols);
static void free_2d_array(float32_t** array, int rows);
static float32_t* polyder(const float32_t* p, int p_size, int m, int* result_size);
static void solve_system(float32_t* A, float32_t* b, int m, int n, float32_t* x);
static void create_vandermonde(float32_t* A, int rows, int cols, float32_t offset);

/**
 * Main Savitzky-Golay filter function
 */
float32_t** savgol_filter(const float32_t** x, int n_rows, int n_cols,
                     int window_length, int polyorder,
                     int deriv, float32_t delta, float32_t cval) {
    if (polyorder >= window_length) {
        return NULL;
    }

    // Get coefficients
    float32_t* coeffs = malloc(window_length * sizeof(float32_t));
    if (!coeffs) return NULL;

    int pos = (window_length % 2 == 0) ? window_length/2 - 0.5f : window_length/2;
    savgol_coeffs(window_length, polyorder, deriv, delta, pos, coeffs);

    // Allocate output array
    float32_t** y = allocate_2d_array(n_rows, n_cols);
    if (!y) {
        free(coeffs);
        return NULL;
    }

    // Apply correlation
    correlate1d(x, n_rows, n_cols, coeffs, window_length, cval, y);

    // Fit edges
    fit_edges_polyfit(x, y, n_rows, n_cols, window_length, polyorder, deriv, delta);

    free(coeffs);
    return y;
}

/**
 * Calculate Savitzky-Golay coefficients
 */
void savgol_coeffs(int window_length, int polyorder, int deriv, float32_t delta,
                  float32_t pos, float32_t* coeffs) {
    if (deriv > polyorder) {
        memset(coeffs, 0, window_length * sizeof(float32_t));
        return;
    }

    // Create Vandermonde matrix
    float32_t* A = malloc(window_length * (polyorder + 1) * sizeof(float32_t));
    if (!A) return;

    create_vandermonde(A, window_length, polyorder + 1, pos);

    // Create target vector
    float32_t* y = calloc(polyorder + 1, sizeof(float32_t));
    if (!y) {
        free(A);
        return;
    }

    y[deriv] = factorial(deriv) / powf(delta, deriv);

    // Solve system using CMSIS DSP matrix operations
    float32_t* temp_coeffs = malloc((polyorder + 1) * sizeof(float32_t));
    if (!temp_coeffs) {
        free(A);
        free(y);
        return;
    }

    solve_system(A, y, window_length, polyorder + 1, temp_coeffs);

    // Reverse coefficients for convolution
    for (int i = 0; i < window_length; i++) {
        coeffs[window_length - 1 - i] = temp_coeffs[i];
    }

    free(temp_coeffs);
    free(A);
    free(y);
}

/**
 * Correlate 1D implementation using CMSIS DSP
 */
void correlate1d(const float32_t** input, int n_rows, int n_cols,
                const float32_t* weights, int weights_size, float32_t cval,
                float32_t** output) {
    int pad_size = weights_size / 2;
    float32_t* padded_row = malloc((n_cols + 2 * pad_size) * sizeof(float32_t));
    if (!padded_row) return;

    for (int i = 0; i < n_rows; i++)
    {
        // Handle left padding
        for (int j = 0; j < pad_size; j++)
        {
        		padded_row[j] = cval;


        }

        // Copy main signal
        memcpy(padded_row + pad_size, input[i], n_cols * sizeof(float32_t));

        // Handle right padding
        for (int j = 0; j < pad_size; j++)
        {

                    padded_row[pad_size + n_cols + j] = cval;


        }

        // Perform correlation using CMSIS DSP FIR filter
        arm_fir_instance_f32 S;
        float32_t state[weights_size + n_cols - 1];
        arm_fir_init_f32(&S, weights_size, (float32_t*)weights, state, n_cols);
        arm_fir_f32(&S, padded_row + pad_size, output[i], n_cols);
    }

    free(padded_row);
}

/**
 * Fit edges using polynomial fitting
 */
void fit_edges_polyfit(const float32_t** x, float32_t** y,
                      int n_rows, int n_cols,
                      int window_length, int polyorder,
                      int deriv, float32_t delta) {
    int halflen = window_length / 2;

    // Fit left edge
    fit_edge(x, y, n_rows, n_cols, 0, window_length, 0, halflen,
             polyorder, deriv, delta);

    // Fit right edge
    fit_edge(x, y, n_rows, n_cols, n_cols - window_length, n_cols,
             n_cols - halflen, n_cols, polyorder, deriv, delta);
}

/**
 * Fit edge helper function
 */
void fit_edge(const float32_t** x, float32_t** y,
             int n_rows, int n_cols,
             int window_start, int window_stop,
             int interp_start, int interp_stop,
             int polyorder, int deriv, float32_t delta) {
    int window_size = window_stop - window_start;
    float32_t* x_window = malloc(window_size * sizeof(float32_t));
    float32_t* A = malloc(window_size * (polyorder + 1) * sizeof(float32_t));

    if (!x_window || !A) {
        free(x_window);
        free(A);
        return;
    }

    for (int row = 0; row < n_rows; row++) {
        // Extract window
        for (int i = 0; i < window_size; i++) {
            x_window[i] = x[row][window_start + i];
        }

        // Create Vandermonde matrix
        create_vandermonde(A, window_size, polyorder + 1, 0);

        // Solve system
        float32_t* coeffs = malloc((polyorder + 1) * sizeof(float32_t));
        if (!coeffs) continue;

        solve_system(A, x_window, window_size, polyorder + 1, coeffs);

        // Apply derivative if needed
        int result_size;
        float32_t* final_coeffs = coeffs;
        if (deriv > 0) {
            final_coeffs = polyder(coeffs, polyorder + 1, deriv, &result_size);
            free(coeffs);
            if (!final_coeffs) continue;
        }

        // Compute interpolated values
        for (int i = interp_start; i < interp_stop; i++) {
            float32_t x_val = (float32_t)(i - window_start);
            float32_t y_val = 0.0f;
            float32_t x_power = 1.0f;

            for (int j = 0; j < (deriv > 0 ? result_size : polyorder + 1); j++) {
                y_val += final_coeffs[j] * x_power;
                x_power *= x_val;
            }

            y[row][i] = y_val / powf(delta, deriv);
        }

        if (deriv > 0) {
            free(final_coeffs);
        } else {
            free(coeffs);
        }
    }

    free(x_window);
    free(A);
}

/**
 * Helper function to compute polynomial derivative
 */
static float32_t* polyder(const float32_t* p, int p_size, int m, int* result_size) {
    if (m == 0) {
        float32_t* result = malloc(p_size * sizeof(float32_t));
        if (result) memcpy(result, p, p_size * sizeof(float32_t));
        *result_size = p_size;
        return result;
    }

    if (p_size <= m) {
        float32_t* result = malloc(sizeof(float32_t));
        if (result) *result = 0.0f;
        *result_size = 1;
        return result;
    }

    float32_t* result = malloc(p_size * sizeof(float32_t));
    if (!result) {
        *result_size = 0;
        return NULL;
    }
    memcpy(result, p, p_size * sizeof(float32_t));
    int curr_size = p_size;

    for (int k = 0; k < m; k++) {
        float32_t* temp = malloc((curr_size - 1) * sizeof(float32_t));
        if (!temp) {
            free(result);
            *result_size = 0;
            return NULL;
        }

        for (int i = 0; i < curr_size - 1; i++) {
            temp[i] = result[i] * (float32_t)(curr_size - i - 1);
        }

        free(result);
        result = temp;
        curr_size--;
    }

    *result_size = curr_size;
    return result;
}

/**
 * Helper function to create Vandermonde matrix
 */
static void create_vandermonde(float32_t* A, int rows, int cols, float32_t offset) {
    for (int i = 0; i < rows; i++) {
        float32_t x = (float32_t)i - offset;
        float32_t power = 1.0f;
        for (int j = 0; j < cols; j++) {
            A[i * cols + j] = power;
            power *= x;
        }
    }
}

/**
 * Helper function to solve linear system using CMSIS DSP
 */
static void solve_system(float32_t* A, float32_t* b, int m, int n, float32_t* x) {
    // Use arm_mat_solve_f32 or implement using SVD decomposition
    // For simplicity, using basic matrix operations here
    arm_matrix_instance_f32 A_mat;
    arm_matrix_instance_f32 b_mat;
    arm_matrix_instance_f32 x_mat;

    arm_mat_init_f32(&A_mat, m, n, A);
    arm_mat_init_f32(&b_mat, m, 1, b);
    arm_mat_init_f32(&x_mat, n, 1, x);

    // Pseudo-inverse solution: x = (A^T * A)^-1 * A^T * b
    float32_t* AT = malloc(n * m * sizeof(float32_t));
    float32_t* ATA = malloc(n * n * sizeof(float32_t));
    float32_t* ATb = malloc(n * sizeof(float32_t));

    if (!AT || !ATA || !ATb) {
        free(AT);
        free(ATA);
        free(ATb);
        return;
    }

    arm_matrix_instance_f32 AT_mat;
    arm_matrix_instance_f32 ATA_mat;
    arm_matrix_instance_f32 ATb_mat;

    arm_mat_init_f32(&AT_mat, n, m, AT);
    arm_mat_init_f32(&ATA_mat, n, n, ATA);
    arm_mat_init_f32(&ATb_mat, n, 1, ATb);

    arm_mat_trans_f32(&A_mat, &AT_mat);
    arm_mat_mult_f32(&AT_mat, &A_mat, &ATA_mat);
    arm_mat_mult_f32(&AT_mat, &b_mat, &ATb_mat);

    // Solve ATA * x = ATb using CMSIS DSP
    arm_mat_inverse_f32(&ATA_mat, &ATA_mat);
    arm_mat_mult_f32(&ATA_mat, &ATb_mat, &x_mat);

    free(AT);
    free(ATA);
    free(ATb);
}

/**
 * Helper function to calculate factorial
 */
static float32_t factorial(int n) {
    float32_t result = 1.0f;
    for (int i = 2; i <= n; i++) {
        result *= (float32_t)i;
    }
    return result;
}

/**
 * Helper function to allocate 2D array
 */
static float32_t** allocate_2d_array(int rows, int cols) {
    float32_t** array = (float32_t**)malloc(rows * sizeof(float32_t*));
    if (!array) return NULL;

    for (int i = 0; i < rows; i++) {
        array[i] = (float32_t*)calloc(cols, sizeof(float32_t));
        if (!array[i]) {
            for (int j = 0; j < i; j++) {
                free(array[j]);
            }
            free(array);
            return NULL;
        }
    }
    return array;
}

/**
 * Helper function to free 2D array
 */
static void free_2d_array(float32_t** array, int rows) {
    if (!array) return;
    for (int i = 0; i < rows; i++) {
        free(array[i]);
    }
    free(array);
}
