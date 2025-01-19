#ifndef LIBROSA_H
#define LIBROSA_H

#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>
#include "arm_math.h"

#ifdef __cplusplus
extern "C" {
#endif

// Type definitions
typedef float float32_t;

// Padding modes for signal processing
typedef enum {
    PAD_CONSTANT = 0,
    PAD_REFLECT,
    PAD_SYMMETRIC,
    PAD_WRAP
} pad_mode_t;

// Constants
#define FRAME_SIZE 2048
#define N_HOP 512
#define N_FRAMES 47
#define N_MEL 128
#define N_FFT 1025
#define BATCH_SIZE 128
#define MAXLEN 20000
#define CHUNK_SIZE 512

// Debug configuration
#ifndef DEBUG_ENABLED
#define DEBUG_ENABLED 1
#endif
// Function declarations
int mount_sd_card(void);
void unmount_sd_card(void);
float32_t* safe_malloc(size_t size);
float32_t* stft(const float32_t* x, int x_len, int n_fft, int n_hop, int* out_cols, pad_mode_t pad_mode);
void power_to_db(float32_t* spec, int cols);
void dct(float32_t* x, int rows, int cols, int norm, int type, float32_t* dct_result);
float32_t* melspectrogram(const float32_t* x, int x_len, int sr, int n_fft, int n_hop,
                         int n_mels, int fmin, int fmax, int start_idx, int* out_cols);
//float32_t* mfcc(const float32_t* x, int x_len, int sr, int n_fft, int n_hop,
//                int n_mels, int fmin, int fmax, int n_mfcc, bool norm,
//                int start_idx, int* out_cols);


bool mfcc_compute(const float32_t* x, float32_t* output_buffer,
                 int x_len, int sr, int n_fft, int n_hop,
                 int n_mels, int fmin, int fmax, int n_mfcc,
                 bool norm, int start_idx);
#ifdef __cplusplus
}
#endif

#endif // LIBROSA_H
