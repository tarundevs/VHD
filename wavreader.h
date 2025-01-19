///* ------------------------------------------------------------------
//* Copyright (C) 2009 Martin Storsjo
//*
//* Licensed under the Apache License, Version 2.0 (the "License");
//* you may not use this file except in compliance with the License.
//* You may obtain a copy of the License at
//*
//*      http://www.apache.org/licenses/LICENSE-2.0
//*
//* Unless required by applicable law or agreed to in writing, software
//* distributed under the License is distributed on an "AS IS" BASIS,
//* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
//* express or implied.
//* See the License for the specific language governing permissions
//* and limitations under the License.
//* -------------------------------------------------------------------
//*/
//
//#ifndef WAVREADER_H_
//#define WAVREADER_H_
//
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//void* wav_read_open(const char *filename);
//void wav_read_close(void* obj);
//
//int wav_get_header(void* obj, int* format, int* channels, int* sample_rate, int* bits_per_sample, unsigned int* data_length);
//int wav_read_data(void* obj, unsigned char* data, unsigned int length);
//
//#ifdef __cplusplus
//}
//#endif
//
//#endif

#ifndef WAVREADER_H
#define WAVREADER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opens a WAV file for reading
 *
 * @param filename Path to the WAV file
 * @return void* Handle to the opened WAV file, NULL if error
 */
void* wav_read_open(const char *filename);

/**
 * @brief Closes an opened WAV file
 *
 * @param obj WAV reader handle returned by wav_read_open
 */
void wav_read_close(void* obj);

/**
 * @brief Retrieves the WAV file header information
 *
 * @param obj WAV reader handle
 * @param format Pointer to store audio format (PCM = 1)
 * @param channels Pointer to store number of channels
 * @param sample_rate Pointer to store sampling rate
 * @param bits_per_sample Pointer to store bits per sample
 * @param data_length Pointer to store length of audio data
 * @return int 1 if successful, 0 if error
 */
int wav_get_header(void* obj, int* format, int* channels, int* sample_rate,
                  int* bits_per_sample, unsigned int* data_length);

/**
 * @brief Reads audio data from the WAV file
 *
 * @param obj WAV reader handle
 * @param data Buffer to store read data
 * @param length Number of bytes to read
 * @return int Number of bytes actually read, -1 if error
 */
int wav_read_data(void* obj, unsigned char* data, unsigned int length, unsigned int cursor);

#ifdef __cplusplus
}
#endif

#endif /* WAVREADER_H */
