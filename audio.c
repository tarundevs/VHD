//#include "librosa.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
//#include "file_arrays.h"
//#include "mxc_device.h"
//#include "uart.h"
//#include "savgol_filter.h"
//#include "arm_fft_bin_data.h"
//#include <stdint.h>
//#include <stddef.h>
//#include "max9867.h"
//#include "i2s.h"
//#include "dma.h"
//#include "mxc_delay.h"
//#include "sdhc.h"
//#include "ff.h"
//#include "mxc_errors.h"
//#include "flc.h"
//#include "wavreader.h"
//#include "icc.h"
//#include "audio.h"
//
//// Parameters for MFCC calculation
//#define SAMPLE_RATE 8000
//#define N_FFT 2048
//#define N_HOP 512
//#define N_MELS 128
//#define N_MFCC 13
//#define F_MIN 0
//#define F_MAX 4000
//#define SIGNAL_LENGTH 2048
//#define AUDIO_BUFFER_SIZE 1024
//// Transform and save features
//
//#define PADDING_SIZE 1024
//#define BUFFER_SIZE 4096
//
//// Define constants
//#define DESIRED_LENGTH 24000
//#define PADDING_SIZE 1024
//#define FLASH_SECTOR_START 0x50408000//(MXC_FLASH_MEM_BASE + MXC_FLASH_MEM_SIZE) - (1 * MXC_FLASH_PAGE_SIZE)  // Adjust based on your flash memory map
//#define FLASH_BUFFER_SIZE (DESIRED_LENGTH + 2 * PADDING_SIZE)
//
//// Global variables for SD card
//static FATFS fs;
//static FIL file;
//
//
//
//#define CHUNK_SIZE 4096  // Process 512 bytes at a time
//#define FLOAT_CHUNK_SIZE (CHUNK_SIZE / 2)  // Since each 16-bit sample becomes one float
//
//void get_indices(int row, int *start, int *end) {
//    int base_start = 44;
//    int base_end = 2048;
//
//    if (row == 0) {
//        *start = base_start;
//        *end = 1024 + base_start;
//    } else if (row == 1) {
//        *start = base_start;
//        *end = 1024 + 512 + base_start;
//    } else if (row == 2) {
//        *start = base_start;
//        *end = base_end + base_start;
//    } else {
//        *start = base_start + 512 * (row - 2);
//        *end = base_end + base_start + 512 * (row - 2);
//    }
//}
//// Function to get the start and end indices for a given row
////int get_indices(int row, int *start, int *end) {
////	// Row 0: 44 and 1024+44
////	// Row 1: 44 and 1024+512+44
////	// Row 2: 44 and 1024+1024+44
////	// Row 3: 44+512 and 2048+44+512
////	// Row 4: 44+1024 and 2048+1024+44
////
////	switch (row){
////		case 0:
////			start=44;
////			end=1024+44;
////			return start,end
////			break;
////		case 1:
////			start=44;
////			end=1024+512+44;
////			return start,end
////			break;
////		case 2:
////			start=44;
////			end=2048+44;
////			return start,end
////			break;
////		default:
////			start=44+512*(row-2);
////			end=44+2048+512*(row-2);
////			return start,end
////			break;
////	}
////
////}
//
//void* init_wav(int file) {
//    FRESULT fr;
//    void* wav_reader = NULL;
//
//    fr = f_mount(&fs, "", 1);
//    if (fr != FR_OK) {
//        printf("Error mounting SD card: %d\n", fr);
//        return NULL;
//    }
//
//    wav_reader = wav_read_open(wav_files[file]);
//    if (wav_reader == NULL) {
//        printf("Error opening WAV file\n");
//        f_mount(NULL, "", 0);
//        return NULL;
//    }
//    else{
//    	printf("Opened file %s\n",wav_files[file]);
//    }
//
//    return wav_reader;
//}
//
//void cleanup_wav(void* wav_reader) {
//    if (wav_reader) {
//        wav_read_close(wav_reader);
//    }
//    f_mount(NULL, "", 0);
//}
//
//void get_signal(float32_t* signal,void* wav_reader, int row, int file) {
//    unsigned char chunk_buffer[CHUNK_SIZE];
//    int format = 0, channels = 0, sample_rate = 0, bits_per_sample = 0;
//    unsigned int data_length = 0;
//
//    if (wav_reader == NULL) {
//        printf("Invalid WAV reader\n");
//        return;
//    }
//
//    if (wav_get_header(wav_reader, &format, &channels, &sample_rate,
//                      &bits_per_sample, &data_length) == 0) {
//        printf("Error reading WAV header\n");
//        return;
//    }
//
//    int padding = (row == 0) ? 1024 : (row == 1) ? 512 : 0;
////    float32_t* signal = malloc(2048 * sizeof(float32_t));
//    if (!signal) {
//        printf("Error: Failed to allocate signal buffer\n");
//        return;
//    }
//
//    memset(signal, 0, 2048 * sizeof(float32_t));
//
//    int start, end;
//    get_indices(row, &start, &end);
//    int length = end - start;
//
//    unsigned int bytes_read = wav_read_data(wav_reader, chunk_buffer, length, start);
//    if (bytes_read == -1 || bytes_read == 0) {
//        printf("Error: No data read from WAV file\n");
//        return;
//    }
//
//    int16_t* int16_data = (int16_t*)chunk_buffer;
//    size_t num_samples = bytes_read / sizeof(int16_t);
//    uint32_t target_index = padding;
//
//    for (size_t i = 0; i < num_samples && target_index < 2048; ++i, ++target_index) {
//        int16_t sample = int16_data[i];
//        signal[target_index] = (float32_t)sample / 32768.0f;
//    }
//
//
////    return signal;
//}
//
//
////float32_t* get_signal(int row,int file) {
////    FRESULT fr;
////    void* wav_reader = NULL;
////    unsigned char chunk_buffer[CHUNK_SIZE];
////    int format = 0, channels = 0, sample_rate = 0, bits_per_sample = 0;
////    unsigned int data_length = 0;
////
//////    printf("=== Starting WAV file processing ===\n");
////    fr = f_mount(&fs, "", 1);
////    if (fr != FR_OK) {
////        printf("Error mounting SD card: %d\n", fr);
////        return NULL;
////    }
////
////    wav_reader = wav_read_open(wav_files[file]);
////    if (wav_reader == NULL) {
////        printf("Error opening WAV file\n");
////        goto cleanup;
////    }
////    else{
////    	printf("%s opened\n",wav_files[file]);
////    }
////
////    if (wav_get_header(wav_reader, &format, &channels, &sample_rate,
////                      &bits_per_sample, &data_length) == 0) {
////        printf("Error reading WAV header\n");
////        goto cleanup;
////    }
////
////    // Print WAV file information
//////    printf("WAV Header Info:\n");
//////    printf("Format: %d\n", format);
//////    printf("Channels: %d\n", channels);
//////    printf("Sample Rate: %d\n", sample_rate);
//////    printf("Bits per Sample: %d\n", bits_per_sample);
//////    printf("Data Length: %u\n", data_length);
////
////    int padding = (row == 0) ? 1024 : (row == 1) ? 512 : 0;
////
////    float32_t* signal = malloc(2048 * sizeof(float32_t));
////    if (!signal) {
////        printf("Error: Failed to allocate signal buffer\n");
////        goto cleanup;
////    }
////
////    // Initialize entire buffer to 0
////    memset(signal, 0, 2048 * sizeof(float32_t));
////
////    int start, end;
////    get_indices(row, &start, &end);
////    int length = end - start;
////
//////    printf("Row: %d, Padding: %d, Start: %d, End: %d, Length: %d\n",
//////           row, padding, start, end, length);
////
////    unsigned int bytes_read = wav_read_data(wav_reader, chunk_buffer, length, start);
//////    printf("Bytes read from WAV: %u\n", bytes_read);
////
////    if (bytes_read == -1 || bytes_read == 0) {
////        printf("Error: No data read from WAV file\n");
////        goto cleanup_with_signal;
////    }
////
////    // Print first few raw bytes from chunk_buffer
//////    printf("First 10 raw bytes from chunk_buffer:\n");
//////    for (int i = 0; i < 10 && i < bytes_read; i++) {
//////        printf("%02X ", chunk_buffer[i]);
//////    }
//////    printf("\n");
////
////    int16_t* int16_data = (int16_t*)chunk_buffer;
////    size_t num_samples = bytes_read / sizeof(int16_t);
////    uint32_t target_index = padding;
////
//////    printf("Number of samples to process: %zu\n", num_samples);
////
////    // Process samples with bounds checking
////    for (size_t i = 0; i < num_samples && target_index < 2048; ++i, ++target_index) {
////        int16_t sample = int16_data[i];
////        signal[target_index] = (float32_t)sample / 32768.0f;
////
////        // Debug print first few conversions
//////        if (i < 5) {
//////            printf("Sample conversion %zu: int16=%d, float32=%f\n",
//////                   i, sample, signal[target_index]);
//////        }
////    }
////
////    // Print first 10 non-zero values
//////    printf("\nFirst 10 non-zero values in signal:\n");
//////    int count = 0;
//////    for (uint32_t i = 0; i < 2048 && count < 10; i++) {
//////        if (signal[i] != 0.0f) {
//////            printf("signal[%u] = %.6f\n", i, signal[i]);
//////            count++;
//////        }
//////    }
////
////    wav_read_close(wav_reader);
////    f_mount(NULL, "", 0);
////    return signal;
////
////cleanup_with_signal:
////    free(signal);
////    signal = NULL;
////cleanup:
////    if (wav_reader) wav_read_close(wav_reader);
////    f_mount(NULL, "", 0);
////    return NULL;
////}
//
////float32_t* get_signal(int row) {
////    FRESULT fr;
////    void* wav_reader = NULL;
////    unsigned char chunk_buffer[CHUNK_SIZE];
////    int format = 0, channels = 0, sample_rate = 0, bits_per_sample = 0;
////    unsigned int data_length = 0;
////    // Add detailed debug logging
////    printf("=== Starting WAV file processing ===\n");
////    fr = f_mount(&fs, "", 1);
////    if (fr != FR_OK) {
////        printf("Error mounting SD card: %d\n", fr);
////        return NULL;
////    }
////    wav_reader = wav_read_open("New_MR_001.wav");
////    if (wav_reader == NULL) {
////        printf("Error opening WAV file\n");
////        goto cleanup;
////    }
////    if (wav_get_header(wav_reader, &format, &channels, &sample_rate,
////                      &bits_per_sample, &data_length) == 0) {
////        printf("Error reading WAV header\n");
////        goto cleanup;
////    }
////    int padding = 0;
////    if (row==0)
////    {
////        padding=1024;
////    }
////    if (row==1)
////    {
////        padding=512;
////    }
////
////    float32_t* signal = malloc(2048 * sizeof(float32_t));
////    if (padding > 0) {
////        memset(signal, 0, padding * sizeof(float32_t));
////    }
////    int start, end;
////    printf("Getting indices for row %d...\n", row);
////    get_indices(row, &start, &end);  // Assuming get_indices returns 0 on success
////    printf("Reading WAV data from %d to %d...\n", start, end);
////    int length=end-start;
////    printf("%d\n\n\n\n\n\n\n",length);
////    unsigned int bytes_read = wav_read_data(wav_reader, chunk_buffer, length, start);
////    printf("Read %u bytes of WAV data\n", bytes_read);
////    if (bytes_read == -1) {
////        printf("Error: No data read from WAV file\n");
////        goto cleanup_with_signal;
////    }
////
////    int16_t* int16_data = ((int16_t*)chunk_buffer);
////    size_t num_samples = bytes_read / sizeof(int16_t);
////    uint32_t target_index = padding;
////	for (size_t i = 0; i < num_samples && target_index < 2048; ++i, target_index++)
////	{
////		   signal[target_index] = (float32_t)int16_data[i] / 32768.0f;  // Normalize to [-1, 1]
////	 }
////
////    // 10. Cleanup and return
////    wav_read_close(wav_reader);
////    f_mount(NULL, "", 0);
////
////    printf("First 10 non-zero values in signal:\n");
////    int count = 0;
////    for (uint32_t i = 0; i < 2048 && count < 10; i++) {
////        if (signal[i] != 0.0f) {
////            printf("signal[%u] = %.10lf\n", i, signal[i]);
////            count++;
////        }
////    }
////
////    return signal;
////cleanup_with_signal:
////    free(signal);
////    signal = NULL;
////cleanup:
////    if (wav_reader) wav_read_close(wav_reader);
////    f_mount(NULL, "", 0);
////    return NULL;
////}
//
//
//float32_t normalize_value(float32_t value) {
//    value = (value - 0.5f) * 256.0f;
//    value = roundf(value);
//    if (value < -128.0f) return -128;
//    if (value > 127.0f) return 127;
//    return value;
//}
//
//void stack_features(float** mfcc, float** delta, float** delta_delta,
//                   float* stacked_features, int num_frames, int num_coeffs)
//{
//    for (int frame = 0; frame < num_frames; frame++)
//    {
//        for (int coeff = 0; coeff < num_coeffs; coeff++)
//        {
//            int base_index = frame * (3 * num_coeffs);
//            stacked_features[base_index + coeff] =  normalize_value(mfcc[coeff][frame]);
//            stacked_features[base_index + num_coeffs + coeff] = normalize_value(delta[coeff][frame]);
//            stacked_features[base_index + 2 * num_coeffs + coeff] = normalize_value(delta_delta[coeff][frame]);
//        }
//    }
//}
//
//int8_t clamp(float value) {
//    int rounded_value = (int)roundf(value); // Round the float to the nearest integer
//    if (rounded_value < -128) return -128;
//    if (rounded_value > 127) return 127;
//    return (int8_t)rounded_value;
//}
//
//int initialize_sd_card() {
////    printf("Initializing SD card...\n");
//
//    // Wait for the SD card to be inserted
//    waitCardInserted();
//
//    // Mount the SD card
//    if (mount() != 0) {
//        printf("SD card mount failed.\n");
//        return -1;
//    }
//
////    printf("SD card mounted successfully.\n");
//    return 0;
//}
//void cleanup_sd_card() {
//    if (umount() == 0) {
//        printf("SD card unmounted successfully.\n");
//    } else {
//        printf("Failed to unmount the SD card.\n");
//    }
//}
//
//// Convert float32_t array to 2's complement hex strings
////void convert_to_hex_and_save(const float* features, size_t feature_size, uint32_t hex_array[459]) {
////    size_t hex_array_idx = 0;
////
////    // Process features in batches of 4
////    for (size_t i = 0; i < feature_size; i += BATCH_SIZE) {
////        uint32_t batch_hex = 0;
////
////        printf("\nProcessing batch starting at index %zu:\n", i);
////
////        // Process each batch
////        for (size_t j = 0; j < BATCH_SIZE; j++) {
////            int8_t value = 0;
////
////            // Get value if within bounds, else use 0 (padding)
////            if (i + j < feature_size) {
////                value = clamp(features[i + j]);
////            }
////
////            // Debugging: Print the clamped value
////            printf("  Original value: %f, Clamped value: %d\n", (i + j < feature_size) ? features[i + j] : 0.0f, value);
////
////            // Pack the value into the uint32_t at the correct position
////            batch_hex |= ((uint32_t)(uint8_t)value) << (8 * (3 - j));
////
////            // Debugging: Show the intermediate batch_hex value
////            printf("    Intermediate packed value: 0x%08X\n", batch_hex);
////        }
////
////        // Store the packed uint32_t into the hex array
////        hex_array[hex_array_idx++] = batch_hex;
////
////        // Debugging: Show the final packed value for the batch
////        printf("  Final packed value for this batch: 0x%08X\n", batch_hex);
////    }
////
////
////    printf("\nConversion and saving completed successfully in hex.txt.\n");
////}
////
//void convert_to_hex(const float* features, size_t feature_size, uint32_t hex_array[459]) {
//    size_t hex_array_idx = 0;
//
////    printf("Starting conversion to hex...\n");
//
//    // Process features in batches of 4
//    for (size_t i = 0; i < feature_size; i += BATCH_SIZE)
//    {
//        uint32_t batch_hex = 0;
//
////        printf("\nProcessing batch starting at index %zu:\n", i);
//
//        // Process each batch
//        for (size_t j = 0; j < BATCH_SIZE; j++)
//        {
//            int8_t value = 0;
//
//            // Get value if within bounds, else use 0 (padding)
//            if (i + j < feature_size)
//            {
//                value = clamp(features[i + j]);
//            }
//
//            // Debugging: Print the clamped value
////            printf(" Original value: %f, Clamped value: %d\n",
////                   (i + j < feature_size) ? features[i + j] : 0.0f, value);
//
//            // Pack the value into the uint32_t at the correct position
//            // Changed (3 - j) to j to reverse the byte order
//            batch_hex |= ((uint32_t)(uint8_t)value) << (8 * j);
//        }
//
////        printf("  Final packed value for this batch: 0x%08X\n", batch_hex);
//    }
//
////    printf("\nConversion completed.\n");
//
//
//}
//
//void delta_features(float32_t** mfcc_features,int f)
//{
//    const int num_coefficients = 13;  // MFCC rows
//    const int num_frames = 47;        // MFCC columns
//    int window_length = 9;
//    int poly_order_delta = 1;
//    int deriv_delta = 1;
//    int poly_order_delta_delta = 2;
//    int deriv_delta_delta = 2;
//
//	float32_t** delta_features = savgol_filter(mfcc_features,num_coefficients,num_frames, window_length,poly_order_delta, deriv_delta, 1.0, 0.0);
//	float32_t** delta_delta_features = savgol_filter(mfcc_features,num_coefficients,num_frames, window_length,poly_order_delta_delta, deriv_delta_delta,1.0, 0.0);
//	stack_features(mfcc_features, delta_features, delta_delta_features,data, num_frames, num_coefficients);
//
//	int rows = 13;
//	int cols = 47;
//	for (int i = 0; i < rows; i++) {
//	for (int j = 0; j < cols; j++)
//	{
//		printf("%.5lf, ", mfcc_features[i][j]); // Adjust spacing as needed
//	}
//	printf("\n");
//	}
//
//	// Add these cleanup calls
//	for (int i = 0; i < num_coefficients; i++) {
//		free(delta_features[i]);
//		free(delta_delta_features[i]);
//	}
//	free(delta_features);
//	free(delta_delta_features);
//
//	uint32_t input_0[459];
//	convert_to_hex(data, num_frames * 3 * num_coefficients, input_0);
//
//
//    if (initialize_sd_card() != 0) {
//        printf("SD card initialization failed. Exiting.\n");
//    }
//
//    FIL file;
//    FRESULT res = f_open(&file, header_files[f], FA_WRITE | FA_CREATE_ALWAYS);
//    if (res != FR_OK) {
//        printf("Error opening file: %d\n", res);
//        return;
//    }
//    else {
//    	printf("%s opened\n",header_files[f]);
//    }
//
//
//	for (int i=0;i<459;i++){
//	        char buffer[12]; // Enough for "0xXXXXXXXX," + null terminator
//	        snprintf(buffer, sizeof(buffer), "0x%08X,", input_0[i]);
//	        unsigned int written;
//	        res = f_write(&file, buffer, strlen(buffer), &written);
//	        if (res != FR_OK || written != strlen(buffer)) {
//	            printf("Error writing to file: %d\n", res);
//	            f_close(&file);
//	            return;
//	        }
//		}
//	    f_close(&file);
//	    cleanup_sd_card();
//}
//
////float32_t** transpose(float32_t** data)
////{
////	float32_t **transposed = (float32_t **)malloc(13 * sizeof(float32_t *));
////	    for (int i = 0; i < 47; i++)
////	    {
////	    	transposed[i] = (float32_t *)malloc(47 * sizeof(float32_t));
////	    }
////
//////	printf("hiii");
////    for (int i = 0; i < 47; i++) {
////        for (int j = 0; j < 13; j++) {
////            transposed[j][i] = data[i][j];
////        }
////    }
////    return transposed;
////}
//
//
//float32_t** transpose(float32_t** data)
//{
//    // Allocate for N_MFCC rows (13)
//    float32_t** transposed = (float32_t**)malloc(N_MFCC * sizeof(float32_t*));
//    if (!transposed) return NULL;
//
//    // Allocate each row for N_FRAMES columns (47)
//    for (int i = 0; i < N_MFCC; i++) {
//        transposed[i] = (float32_t*)malloc(N_FRAMES * sizeof(float32_t));
//        if (!transposed[i]) {
//            // Cleanup previously allocated memory
//            for (int j = 0; j < i; j++) {
//                free(transposed[j]);
//            }
//            free(transposed);
//            return NULL;
//        }
//    }
//
//    // Perform transpose operation
//    for (int i = 0; i < N_FRAMES; i++) {
//        for (int j = 0; j < N_MFCC; j++) {
//            transposed[j][i] = data[i][j];
//        }
//    }
//
//    return transposed;
//}
//
//
////
////int audio_main(void)
////{
////    printf("\nMFCC Test Program\n");
////    printf("=================\n");
////
////
////    // Allocate memory for MFCC results
////    float32_t** mfcc_ = (float32_t**)malloc(47 * sizeof(float32_t*));
////    if (mfcc_ == NULL)
////    {
////        printf("Memory allocation failed for MFCC rows\n");
////        return -1;
////    }
////
////    for (int i = 0; i < 47; i++)
////    {
////        mfcc_[i] = (float32_t*)malloc(N_MFCC * sizeof(float32_t));
////        if (mfcc_[i] == NULL)
////        {
////            printf("Memory allocation failed for row %d\n", i);
////            return -1;
////        }
////    }
////
////    float32_t* signal = malloc(2048 * sizeof(float32_t));
////    void* wr=NULL;
////    for (int file=0;file<120;file++)
////    {
////    	wr=init_wav(file);
////		for (int i = 0; i < 47; i++)
////		{
////			get_signal(signal,wr,i,file);
////			if (!signal) {
////						printf("Failed to generate test signal\n");
////						return -1;
////					}
////			if (!signal)
////			{
////				printf("Failed to generate test signal\n");
////				return -1;
////			}
////
////			int out_cols;
////			float32_t* mfcc_features = mfcc(signal,
////											 SIGNAL_LENGTH,
////											 SAMPLE_RATE,
////											 N_FFT,
////											 N_HOP,
////											 N_MELS,
////											 F_MIN,
////											 F_MAX,
////											 N_MFCC,
////											 true,  // normalize
////											 0,     // start_idx
////											 &out_cols);
////
////			if (!mfcc_features)
////			{
////				printf("MFCC calculation failed for row %d\n", i);
////				free(signal);
////				return -1;
////			}
////
////			for (int j = 0; j < N_MFCC; j++)
////			{
////				mfcc_[i][j] = mfcc_features[j];
////			}
////		}
////		cleanup_wav(wr);
////		float32_t** transposed_mfcc=transpose(mfcc_);
////		delta_features(transposed_mfcc,file);
////
////    }
////    return 0;
////}
////
//
//int audio_main(void) {
//    printf("\nMFCC Test Program\n");
//    printf("=================\n");
//
//    // Allocate mfcc_ matrix
//    float32_t** mfcc_ = (float32_t**)malloc(47 * sizeof(float32_t*));
//    if (mfcc_ == NULL) {
//        printf("Memory allocation failed for MFCC rows\n");
//        return -1;
//    }
//
//    for (int i = 0; i < 47; i++) {
//        mfcc_[i] = (float32_t*)malloc(N_MFCC * sizeof(float32_t));
//        if (mfcc_[i] == NULL) {
//            printf("Memory allocation failed for row %d\n", i);
//            // Clean up previously allocated memory
//            for (int j = 0; j < i; j++) {
//                free(mfcc_[j]);
//            }
//            free(mfcc_);
//            return -1;
//        }
//    }
//
//    // Allocate signal buffer
//    float32_t* signal = malloc(2048 * sizeof(float32_t));
//    if (signal == NULL) {
//        printf("Memory allocation failed for signal buffer\n");
//        return -1;
//    }
//
//    void* wr = NULL;
//    printf("Starting main processing loop...\n");
//
//    for (int file = 0; file < 1; file++) {
//        printf("\nProcessing file %d/1000...\n", file + 1);
//
//        wr = init_wav(file);
//        if (wr == NULL) {
//            printf("Failed to initialize WAV for file %d\n", file);
//            continue;
//        }
//
////        printf("Processing frames for file %d...\n", file);
//        for (int i = 0; i < 47; i++) {
////            printf("  Frame %d/47: ", i + 1);
//
////            printf("Getting signal... ");
//            get_signal(signal, wr, i, file);
////            printf("OK, ");
//
////            printf("Computing MFCC... ");
//            // Compute MFCC directly into the allocated matrix
//            if (!mfcc_compute(signal, mfcc_[i], SIGNAL_LENGTH, SAMPLE_RATE,
//                            N_FFT, N_HOP, N_MELS, F_MIN, F_MAX, N_MFCC,
//                            true, 0)) {
//                printf("\nMFCC calculation failed for frame %d\n", i);
//                cleanup_wav(wr);
//                continue;
//            }
////            printf("OK\n");
//        }
//
////        printf("Cleaning up WAV resources... ");
//        cleanup_wav(wr);
////        printf("OK\n");
//
////        printf("Transposing MFCC matrix... ");
//        float32_t** transposed_mfcc = transpose(mfcc_);
//        if (!transposed_mfcc) {
//            printf("FAILED\n");
//            continue;
//        }
////        printf("OK\n");
//
////        printf("Computing delta features... ");
//        delta_features(transposed_mfcc, file);
////        printf("OK\n");
//
//        // Free transposed matrix
//        for (int i = 0; i < N_MFCC; i++) {
//            free(transposed_mfcc[i]);
//        }
//        free(transposed_mfcc);
//    }
//
//    // Cleanup
//    for (int i = 0; i < 47; i++) {
//        free(mfcc_[i]);
//    }
//    free(mfcc_);
//    free(signal);
//
//    printf("Program completed successfully\n");
//    return 0;
//}
