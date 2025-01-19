//#ifndef AUDIO_H
//#define AUDIO_H
//
//#include <stdint.h>
//#include <stddef.h>
//#include "arm_math.h"
//
//// Constants for MFCC calculation
//#define SAMPLE_RATE 8000
//#define N_FFT 2048
//#define N_HOP 512
//#define N_MELS 128
//#define N_MFCC 13
//#define F_MIN 0
//#define F_MAX 4000
//#define SIGNAL_LENGTH 2048
//#define AUDIO_BUFFER_SIZE 1024
//
//// Constants for buffering and storage
//#define PADDING_SIZE 1024
//#define BUFFER_SIZE 4096
//#define DESIRED_LENGTH 24000
//#define FLASH_SECTOR_START 0x50408000
//#define FLASH_BUFFER_SIZE (DESIRED_LENGTH + 2 * PADDING_SIZE)
//#define CHUNK_SIZE 4096
//#define FLOAT_CHUNK_SIZE (CHUNK_SIZE / 2)
//#define BATCH_SIZE 4
//
//// Function prototypes
//void get_indices(int row, int *start, int *end);
//void* init_wav(int file);
//void cleanup_wav(void* wav_reader);
//void get_signal(float32_t* signal,void* wav_reader, int row, int file);
//float32_t normalize_value(float32_t value);
//void stack_features(float** mfcc, float** delta, float** delta_delta,
//                    float* stacked_features, int num_frames, int num_coeffs);
//int8_t clamp(float value);
//int initialize_sd_card();
//void cleanup_sd_card();
//void convert_to_hex_and_save(const float* features, size_t feature_size, uint32_t hex_array[459]);
//void convert_to_hex(const float* features, size_t feature_size, uint32_t hex_array[459]);
//void delta_features(float32_t** mfcc_features,int file);
//float32_t** transpose(float32_t** data);
//int audio_main(void);
//
//#endif // AUDIO_H
