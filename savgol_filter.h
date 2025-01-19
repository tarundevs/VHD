#ifndef SAVGOL_FILTER_H
#define SAVGOL_FILTER_H

#include <stdint.h>
#include "arm_math.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Padding modes for signal processing
 */


/**
 * @brief Apply Savitzky-Golay filter to input signal
 *
 * @param x Input signal array (2D)
 * @param n_rows Number of rows in input
 * @param n_cols Number of columns in input
 * @param window_length Length of filter window (odd number)
 * @param polyorder Order of polynomial (must be less than window_length)
 * @param deriv Order of derivative to compute (0 means smooth only)
 * @param delta Sample spacing for derivatives (default = 1.0)
 * @param mode Padding mode for edge handling
 * @param cval Constant value for PAD_CONSTANT mode
 * @return float32_t** Filtered signal array (must be freed by caller)
 */
float32_t** savgol_filter(const float32_t** x, int n_rows, int n_cols,
                     int window_length, int polyorder,
                     int deriv, float32_t delta, float32_t cval);

/**
 * @brief Calculate Savitzky-Golay filter coefficients
 *
 * @param window_length Length of filter window
 * @param polyorder Order of polynomial
 * @param deriv Order of derivative
 * @param delta Sample spacing
 * @param pos Center position (default = window_length/2)
 * @param coeffs Pre-allocated array for coefficients (size = window_length)
 */
void savgol_coeffs(int window_length, int polyorder, int deriv, float32_t delta,
                  float32_t pos, float32_t* coeffs);

/**
 * @brief Perform 1D correlation with padding
 *
 * @param input Input signal array (2D)
 * @param n_rows Number of rows
 * @param n_cols Number of columns
 * @param weights Filter weights
 * @param weights_size Length of weights array
 * @param mode Padding mode
 * @param cval Constant value for PAD_CONSTANT mode
 * @param output Pre-allocated output array (2D)
 */
void correlate1d(const float32_t** input, int n_rows, int n_cols,
                const float32_t* weights, int weights_size, float32_t cval,
                float32_t** output);

/**
 * @brief Fit polynomial to signal edges
 *
 * @param x Input signal array (2D)
 * @param y Output signal array (2D)
 * @param n_rows Number of rows
 * @param n_cols Number of columns
 * @param window_length Filter window length
 * @param polyorder Polynomial order
 * @param deriv Derivative order
 * @param delta Sample spacing
 */
void fit_edges_polyfit(const float32_t** x, float32_t** y,
                      int n_rows, int n_cols,
                      int window_length, int polyorder,
                      int deriv, float32_t delta);

/**
 * @brief Fit polynomial to one edge of signal
 *
 * @param x Input signal array (2D)
 * @param y Output signal array (2D)
 * @param n_rows Number of rows
 * @param n_cols Number of columns
 * @param window_start Start of window
 * @param window_stop End of window
 * @param interp_start Start of interpolation range
 * @param interp_stop End of interpolation range
 * @param polyorder Polynomial order
 * @param deriv Derivative order
 * @param delta Sample spacing
 */
void fit_edge(const float32_t** x, float32_t** y,
             int n_rows, int n_cols,
             int window_start, int window_stop,
             int interp_start, int interp_stop,
             int polyorder, int deriv, float32_t delta);

#ifdef __cplusplus
}
#endif

#endif /* SAVGOL_FILTER_H */
