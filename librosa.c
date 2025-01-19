//#include "librosa.h"
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
//#include "mxc_device.h"
//#include "uart.h"
//#include "sdhc.h"
//#include "mxc.h"
//#include "arm_fft_bin_data.h"
//
//#define FRAME_SIZE 2048
//#define N_HOP 512
//#define N_FRAMES 47
//#define N_MEL 128
//#define N_FFT 1025
//#define BATCH_SIZE 128
//#define MAXLEN 20000
//#define CHUNK_SIZE 512
//
//// FatFS objects
//static FATFS fs;
//static FIL fil;
//static FRESULT fr;
//
//// Debug macros
//#define DEBUG_ENABLED 1  // Set to 0 to disable all debug prints
//
//// Optimized debug macros with compile-time conditional
//#if DEBUG_ENABLED
//    #define DEBUG_PRINT(fmt, ...) printf("[DEBUG] %s:%d: " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
//    #define DEBUG_ARRAY(name, arr, n) do { \
//        printf("[DEBUG] %s:%d: %s first %d elements: ", __func__, __LINE__, name, n); \
//        for (int i = 0; i < n && i < 5 && arr; i++) { \
//            printf("%.15f ", arr[i]); \
//        } \
//        printf("\n"); \
//    } while(0)
//#else
//    #define DEBUG_PRINT(fmt, ...)
//    #define DEBUG_ARRAY(name, arr, n)
//#endif
//
//// Pre-computed constants
//static const float MEL_CONSTANT_1 = (1000.0f-0.0f)/(200.0f/3.0f);
//static const float MEL_CONSTANT_2 = logf(6.4f)/27.0f;
//static const float MEL_CONSTANT_3 = 200.0f/3.0f;
//static const float TWO_PI = 2.0f * M_PI;
//static const float LOG10_FACTOR = 10.0f;
//static const float DB_MIN = 1e-10f;
//static const float DB_RANGE = 80.0f;
//
//// Helper functions for frequently used operations
//float32_t* safe_malloc(size_t size) {
//    float32_t* ptr = (float32_t*)calloc(size, sizeof(float32_t));
//    if (ptr) {
//        DEBUG_PRINT("Allocated %zu bytes at %p", size * sizeof(float32_t), (void*)ptr);
//    } else {
//        DEBUG_PRINT("Memory allocation failed!");
//    }
//    return ptr;
//}
//
//// Optimized STFT computation
//float32_t* stft(const float32_t* x, int x_len, int n_fft, int n_hop, int* out_cols, pad_mode_t pad_mode) {
//    if (!x || !out_cols || n_fft <= 0 || n_hop <= 0) return NULL;
//
//    const int n_freqs = n_fft / 2 + 1;
//    *out_cols = n_freqs;
//
//    float32_t* output = safe_malloc(n_freqs);
//    float32_t* fft_buffer = safe_malloc(n_fft * 2);
//    if (!output || !fft_buffer) {
//        free(output);
//        free(fft_buffer);
//        return NULL;
//    }
//
//    // Initialize FFT once
//    arm_cfft_instance_f32 cfft_instance;
//    if (arm_cfft_init_f32(&cfft_instance, n_fft) != ARM_MATH_SUCCESS) {
//        free(output);
//        free(fft_buffer);
//        return NULL;
//    }
//
//    // Optimize window application and FFT preparation
//    #pragma omp parallel for
//    for (int j = 0; j < n_fft; j++) {
//        fft_buffer[2*j] = x[j] * window[j];
//        fft_buffer[2*j + 1] = 0.0f;
//    }
//    DEBUG_ARRAY("Hann window", window, 5);
//    DEBUG_ARRAY("Windowed signal", fft_buffer, 10);
//
//    arm_cfft_f32(&cfft_instance, fft_buffer, 0, 1);
//    DEBUG_ARRAY("FFT output", fft_buffer, 10);
//    // Optimize power spectrum calculation
//    #pragma omp parallel for
//    for (int j = 0; j < n_freqs; j++) {
//        const float32_t real = fft_buffer[2*j];
//        const float32_t imag = fft_buffer[2*j + 1];
//        output[j] = real * real + imag * imag;
//    }
//
//    DEBUG_ARRAY("Power spectrum", output, 5);
//    free(fft_buffer);
//    return output;
//}
//
//// Optimized power to dB conversion
//void power_to_db(float32_t* spec, int cols) {
//    float max_coeff = -FLT_MAX;
//
//    #pragma omp parallel for reduction(max:max_coeff)
//    for (int i = 0; i < cols; i++) {
//        spec[i] = LOG10_FACTOR * log10f(fmaxf(spec[i], DB_MIN));
//        max_coeff = fmaxf(max_coeff, spec[i]);
//    }
//
//    const float threshold = max_coeff - DB_RANGE;
//    #pragma omp parallel for
//    for (int i = 0; i < cols; i++) {
//        spec[i] = fmaxf(spec[i], threshold);
//    }
//}
//
//void dct(float32_t* x, int rows, int cols, int norm, int type, float32_t* dct_result)
//{
//    DEBUG_PRINT("Starting DCT: rows=%d, cols=%d, norm=%d, type=%d", rows, cols, norm, type);
//
//    // Add input range validation
//    float32_t max_input = -FLT_MAX;
//    float32_t min_input = FLT_MAX;
//
//    for (int i = 0; i < 128; i++)
//    {
//        DEBUG_PRINT("Processing row %d", i);
//        // Compute DCT for current row
//        dct_result[i] = 0.0f;
//        for (int k = 0; k < cols; k++)
//        {
//            dct_result[i] += x[k] * coeffs[i][k];
//        }
//    }
//
//    DEBUG_PRINT("DCT computation completed, applying normalization if needed");
//
//    // Apply normalization if required
//    if (norm) {
//        DEBUG_PRINT("Applying normalization");
//        float32_t* ortho = safe_malloc(cols * sizeof(float32_t));
//
//        // Initialize normalization factors
//        for (int i = 0; i < cols; i++) {
//            ortho[i] = sqrtf(0.5f / cols);
//        }
//        ortho[0] = sqrtf(0.25f / cols);
//
//        // Apply normalization to all 128 values
//        for (int j = 0; j < 128; j++) {
//            dct_result[j] *= ortho[j % cols];
//        }
//
//        free(ortho);
//        DEBUG_PRINT("Normalization completed");
//    }
//
//    // Check final output range
//    max_input = -FLT_MAX;
//    min_input = FLT_MAX;
//    for (int i = 0; i < 128; i++)
//    {
//        max_input = fmaxf(max_input, dct_result[i]);
//        min_input = fminf(min_input, dct_result[i]);
//    }
//    DEBUG_PRINT("Output range: [%f, %f]", min_input, max_input);
//    DEBUG_PRINT("DCT computation completed successfully");
//}
//
//// Modified melspectrogram function
//float32_t* melspectrogram(const float32_t* x, int x_len, int sr, int n_fft, int n_hop,
//                         int n_mels, int fmin, int fmax, int start_idx, int* out_cols) {
//    DEBUG_PRINT("Starting melspectrogram computation");
//
//    if (!x || !out_cols) {
//        DEBUG_PRINT("Invalid input parameters!");
//        return NULL;
//    }
//
//    int stft_cols;
//    float32_t* power_spec = stft(x, x_len, n_fft, n_hop, &stft_cols, PAD_CONSTANT);
//
//    if (!power_spec) {
//        return NULL;
//    }
//
//    *out_cols = n_mels;
//    float32_t* mel_spec = safe_malloc(N_MEL);
//
//    if (!mel_spec) {
//        free(power_spec);
//        free(mel_spec);
//        return NULL;
//    }
//
//    // Apply mel filterbank
//    for (int j = 0; j < n_mels; j++) {
//        float32_t sum = 0.0f;
//		int start_index=0;
//		int end_index=row_sizes[j];
//		if (j>0)
//		{
//			start_index=row_sizes[j-1];
//		}
//
//		for (int k=start_index;k<end_index;k++)
//		{
//        	int col=non_zero_columns_flat[k];
//
//            sum += non_zero_values_flat[k] * power_spec[col];
//        }
//        mel_spec[j] = sum;
//    }
//
//    DEBUG_ARRAY("Mel spectrogram output", mel_spec, 5);
//    free(power_spec);
//    return mel_spec;
//}
//
//float32_t* mfcc(const float32_t* x, int x_len, int sr, int n_fft, int n_hop,
//                int n_mels, int fmin, int fmax, int n_mfcc, bool norm,
//                int start_idx, int* out_cols)
//{
//    DEBUG_PRINT("Starting ORIGINAL MFCC computation: sr=%d, n_mfcc=%d, norm=%d", sr, n_mfcc, norm);
//
//    if (!x || !out_cols)
//    {
//        DEBUG_PRINT("Invalid input parameters!");
//        return NULL;
//    }
//
//    int mel_cols;
//    DEBUG_PRINT("Computing melspectrogram");
//    float32_t* mel_spec = melspectrogram(x, x_len, sr, n_fft, n_hop, n_mels,
//                                        fmin, fmax, start_idx, &mel_cols);
//    if (!mel_spec)
//    {
//        DEBUG_PRINT("Melspectrogram computation failed!");
//        return NULL;
//    }
//    DEBUG_ARRAY("Mel spectrogram", mel_spec, 5);
//    DEBUG_PRINT("Converting power to dB");
//    power_to_db(mel_spec, mel_cols);
//    DEBUG_ARRAY("Log mel spectrogram", mel_spec, 5);
//
//    *out_cols = n_mfcc;
//    float32_t* mfcc_features = safe_malloc(n_mfcc);
//    if (!mfcc_features)
//    {
//        DEBUG_PRINT("Failed to allocate MFCC features!");
//        free(mel_spec);
//        return NULL;
//    }
//
//    DEBUG_PRINT("Computing DCT");
//    dct(mel_spec, 1, mel_cols, norm, 2, mfcc_features);
//    DEBUG_ARRAY("Final MFCC features", mfcc_features, 5);
//
////    free(mel_spec);
//    DEBUG_PRINT("MFCC computation completed");
//    return mfcc_features;
//}
//

#include "librosa.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mxc_device.h"
#include "uart.h"
#include "sdhc.h"
#include "mxc.h"
#include "arm_fft_bin_data.h"

// Buffer sizes (these can be modified based on your needs)
#define FFT_BUFFER_SIZE 2048
#define MEL_BUFFER_SIZE 128
#define MFCC_BUFFER_SIZE 13  // Adjust as per the number of MFCC coefficients
#define POWER_SPEC_SIZE 1025
#define MAXLEN 20000
#define CHUNK_SIZE 512

// FatFS objects
static FATFS fs;
static FIL fil;
static FRESULT fr;

// Debug macros
#define DEBUG_ENABLED 0  // Set to 0 to disable all debug prints

#if DEBUG_ENABLED
    #define DEBUG_PRINT(fmt, ...) printf("[DEBUG] %s:%d: " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
    #define DEBUG_ARRAY(name, arr, n) do { \
        printf("[DEBUG] %s:%d: %s first %d elements: ", __func__, __LINE__, name, n); \
        for (int i = 0; i < n && i < 5 && arr; i++) { \
            printf("%.15f ", arr[i]); \
        } \
        printf("\n"); \
    } while(0)
#else
    #define DEBUG_PRINT(fmt, ...)
    #define DEBUG_ARRAY(name, arr, n)
#endif

// Pre-allocated global buffers for reuse
static float32_t fft_buffer[FFT_BUFFER_SIZE * 2];  // For FFT (real + imaginary)
static float32_t power_spec[POWER_SPEC_SIZE];  // For power spectrum
static float32_t mel_spec[MEL_BUFFER_SIZE];  // For mel spectrogram
//static float32_t mfcc_features[MFCC_BUFFER_SIZE];  // For MFCC features
// Global buffer for DCT computation
#define MAX_DCT_ROWS 128
#define MAX_DCT_COLS 1024

static float32_t dct_buffer[MAX_DCT_COLS];

// Pre-computed constants
static const float MEL_CONSTANT_1 = (1000.0f-0.0f)/(200.0f/3.0f);
static const float MEL_CONSTANT_2 = logf(6.4f)/27.0f;
static const float MEL_CONSTANT_3 = 200.0f/3.0f;
static const float TWO_PI = 2.0f * M_PI;
static const float LOG10_FACTOR = 10.0f;
static const float DB_MIN = 1e-10f;
static const float DB_RANGE = 80.0f;

// Helper functions for frequently used operations
float32_t* safe_malloc(size_t size) {
    float32_t* ptr = (float32_t*)calloc(size, sizeof(float32_t));
    if (ptr) {
        DEBUG_PRINT("Allocated %zu bytes at %p", size * sizeof(float32_t), (void*)ptr);
    } else {
        DEBUG_PRINT("Memory allocation failed!");
    }
    return ptr;
}

// Optimized STFT computation using global buffer
float32_t* stft(const float32_t* x, int x_len, int n_fft, int n_hop, int* out_cols, pad_mode_t pad_mode) {
    if (!x || !out_cols || n_fft <= 0 || n_hop <= 0) return NULL;

    const int n_freqs = n_fft / 2 + 1;
    *out_cols = n_freqs;

    // Reuse the global fft_buffer
    memset(fft_buffer, 0, sizeof(fft_buffer));

    arm_cfft_instance_f32 cfft_instance;
    if (arm_cfft_init_f32(&cfft_instance, n_fft) != ARM_MATH_SUCCESS) {
        return NULL;
    }

    // Apply window function
    #pragma omp parallel for
    for (int j = 0; j < n_fft; j++) {
        fft_buffer[2*j] = x[j] * window[j];
        fft_buffer[2*j + 1] = 0.0f;
    }

    arm_cfft_f32(&cfft_instance, fft_buffer, 0, 1);

    // Compute power spectrum
    #pragma omp parallel for
    for (int j = 0; j < n_freqs; j++) {
        const float32_t real = fft_buffer[2*j];
        const float32_t imag = fft_buffer[2*j + 1];
        power_spec[j] = real * real + imag * imag;
    }

    DEBUG_ARRAY("Power spectrum", power_spec, 5);
    return power_spec;
}

// Optimized power to dB conversion using global buffer
void power_to_db(float32_t* spec, int cols) {
    float max_coeff = -FLT_MAX;

    #pragma omp parallel for reduction(max:max_coeff)
    for (int i = 0; i < cols; i++) {
        spec[i] = LOG10_FACTOR * log10f(fmaxf(spec[i], DB_MIN));
        max_coeff = fmaxf(max_coeff, spec[i]);
    }

    const float threshold = max_coeff - DB_RANGE;
    #pragma omp parallel for
    for (int i = 0; i < cols; i++) {
        spec[i] = fmaxf(spec[i], threshold);
    }
}

// Optimized mel spectrogram function using global buffers
float32_t* melspectrogram(const float32_t* x, int x_len, int sr, int n_fft, int n_hop,
                         int n_mels, int fmin, int fmax, int start_idx, int* out_cols) {
    DEBUG_PRINT("Starting melspectrogram computation");

    if (!x || !out_cols) {
        DEBUG_PRINT("Invalid input parameters!");
        return NULL;
    }

    int stft_cols;
    float32_t* power_spec = stft(x, x_len, n_fft, n_hop, &stft_cols, PAD_CONSTANT);

    if (!power_spec) {
        return NULL;
    }

    *out_cols = n_mels;

    // Apply mel filterbank
    for (int j = 0; j < n_mels; j++) {
        float32_t sum = 0.0f;
        int start_index = 0;
        int end_index = row_sizes[j];

        if (j > 0) {
            start_index = row_sizes[j - 1];
        }

        for (int k = start_index; k < end_index; k++) {
            int col = non_zero_columns_flat[k];
            sum += non_zero_values_flat[k] * power_spec[col];
        }
        mel_spec[j] = sum;
    }

    DEBUG_ARRAY("Mel spectrogram output", mel_spec, 5);
    return mel_spec;
}

// DCT computation using global buffer
//void dct(float32_t* x, int rows, int cols, int norm, int type, float32_t* dct_result)
//{
//    DEBUG_PRINT("Starting DCT: rows=%d, cols=%d, norm=%d, type=%d", rows, cols, norm, type);
//
//    // Add input range validation
//    float32_t max_input = -FLT_MAX;
//    float32_t min_input = FLT_MAX;
//
//    // Check that we don't exceed buffer limits
//    if (rows > MAX_DCT_ROWS || cols > MAX_DCT_COLS) {
//        DEBUG_PRINT("Error: Input dimensions exceed buffer limits");
//        return;
//    }
//
//    // Process each row of the input for DCT
//    for (int i = 0; i < rows; i++)
//    {
//        DEBUG_PRINT("Processing row %d", i);
//        // Copy the current row into the global buffer
////        for (int k = 0; k < cols; k++)
////        {
////            dct_buffer[k] = x[i * cols + k];  // Adjust the indexing for 2D input
////        }
//
//        // Compute DCT for the current row (you can optimize this further using precomputed coefficients)
//        dct_result[i] = 0.0f;
//        for (int k = 0; k < cols; k++)
//        {
//            dct_result[i] += x[k] * coeffs[i][k]; // `coeffs` should be defined for your system
//        }
//    }
//
//    DEBUG_PRINT("DCT computation completed, applying normalization if needed");
//
//    // Apply normalization if required
//    if (norm) {
//        DEBUG_PRINT("Applying normalization");
//        float32_t* ortho = dct_buffer; // Reuse the global buffer for normalization factors
//
//        // Initialize normalization factors
//        for (int i = 0; i < cols; i++) {
//            ortho[i] = sqrtf(0.5f / cols);
//        }
//        ortho[0] = sqrtf(0.25f / cols);
//
//        // Apply normalization to all 128 values
//        for (int j = 0; j < rows; j++) {
//            dct_result[j] *= ortho[j % cols];
//        }
//        DEBUG_PRINT("Normalization completed");
//    }
//
//    // Check final output range
//    max_input = -FLT_MAX;
//    min_input = FLT_MAX;
//    for (int i = 0; i < rows; i++)
//    {
//        max_input = fmaxf(max_input, dct_result[i]);
//        min_input = fminf(min_input, dct_result[i]);
//    }
//    DEBUG_PRINT("Output range: [%f, %f]", min_input, max_input);
//    DEBUG_PRINT("DCT computation completed successfully");
//}


void dct(float32_t* x, int rows, int cols, int norm, int type, float32_t* dct_result)
{
    DEBUG_PRINT("Starting DCT: rows=%d, cols=%d, norm=%d, type=%d", rows, cols, norm, type);

    // Add input range validation
    float32_t max_input = -FLT_MAX;
    float32_t min_input = FLT_MAX;

    for (int i = 0; i < 128; i++)
    {
        DEBUG_PRINT("Processing row %d", i);
        // Compute DCT for current row
        dct_result[i] = 0.0f;
        for (int k = 0; k < cols; k++)
        {
            dct_result[i] += x[k] * coeffs[i][k];
        }
    }

    DEBUG_PRINT("DCT computation completed, applying normalization if needed");

    // Apply normalization if required
    if (norm) {
        DEBUG_PRINT("Applying normalization");
        float32_t* ortho = safe_malloc(cols * sizeof(float32_t));

        // Initialize normalization factors
        for (int i = 0; i < cols; i++) {
            ortho[i] = sqrtf(0.5f / cols);
        }
        ortho[0] = sqrtf(0.25f / cols);

        // Apply normalization to all 128 values
        for (int j = 0; j < 128; j++) {
            dct_result[j] *= ortho[j % cols];
        }

        free(ortho);
        DEBUG_PRINT("Normalization completed");
    }

    // Check final output range
    max_input = -FLT_MAX;
    min_input = FLT_MAX;
    for (int i = 0; i < 128; i++)
    {
        max_input = fmaxf(max_input, dct_result[i]);
        min_input = fminf(min_input, dct_result[i]);
    }
    DEBUG_PRINT("Output range: [%f, %f]", min_input, max_input);
    DEBUG_PRINT("DCT computation completed successfully");
}

// Optimized MFCC function using global buffers
//float32_t* mfcc(const float32_t* x, int x_len, int sr, int n_fft, int n_hop,
//                int n_mels, int fmin, int fmax, int n_mfcc, bool norm,
//                int start_idx, int* out_cols) {
//    DEBUG_PRINT("Starting MFCC computation");
//
//    if (!x || !out_cols) {
//        DEBUG_PRINT("Invalid input parameters!");
//        return NULL;
//    }
//
//    int mel_cols;
//    float32_t* mel_spec = melspectrogram(x, x_len, sr, n_fft, n_hop, n_mels, fmin, fmax, start_idx, &mel_cols);
//    if (!mel_spec) {
//        DEBUG_PRINT("Melspectrogram computation failed!");
//        return NULL;
//    }
//
//    power_to_db(mel_spec, mel_cols);
//
//    *out_cols = n_mfcc;
//
//    // Reuse the global buffer for MFCC features
//    memset(mfcc_features, 0, sizeof(mfcc_features));
//
//    dct(mel_spec, 1, mel_cols, norm, 2, mfcc_features);
//    DEBUG_ARRAY("Final MFCC features", mfcc_features, 5);
//
//    DEBUG_PRINT("MFCC computation completed");
//    return mfcc_features;
//}

bool mfcc_compute(const float32_t* x, float32_t* output_buffer,
                 int x_len, int sr, int n_fft, int n_hop,
                 int n_mels, int fmin, int fmax, int n_mfcc,
                 bool norm, int start_idx) {
    if (!x || !output_buffer) {
        return false;
    }

    int mel_cols;
    float32_t* mel_spec = melspectrogram(x, x_len, sr, n_fft, n_hop,
                                        n_mels, fmin, fmax, start_idx, &mel_cols);
    if (!mel_spec) {
        return false;
    }

    power_to_db(mel_spec, mel_cols);

    // Write directly to provided output buffer
    dct(mel_spec, 1, mel_cols, norm, 2, output_buffer);

    // No need to free mel_spec if using static buffers from modified librosa.c
    return true;
}
