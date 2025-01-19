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
//#include "wavreader.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <stdint.h>
//
//#define TAG(a, b, c, d) (((a) << 24) | ((b) << 16) | ((c) << 8) | (d))
//
//struct wav_reader {
//	FILE *wav;
//	long data_pos;
//	uint32_t data_length;
//	uint32_t data_left;
//
//	int format;
//	int sample_rate;
//	int bits_per_sample;
//	int channels;
//	int byte_rate;
//	int block_align;
//
//	int streamed;
//};
//
//static uint32_t read_tag(struct wav_reader* wr) {
//	uint32_t tag = 0;
//	tag = (tag << 8) | fgetc(wr->wav);
//	tag = (tag << 8) | fgetc(wr->wav);
//	tag = (tag << 8) | fgetc(wr->wav);
//	tag = (tag << 8) | fgetc(wr->wav);
//	return tag;
//}
//
//static uint32_t read_int32(struct wav_reader* wr) {
//	uint32_t value = 0;
//	value |= fgetc(wr->wav) << 0;
//	value |= fgetc(wr->wav) << 8;
//	value |= fgetc(wr->wav) << 16;
//	value |= fgetc(wr->wav) << 24;
//	return value;
//}
//
//static uint16_t read_int16(struct wav_reader* wr) {
//	uint16_t value = 0;
//	value |= fgetc(wr->wav) << 0;
//	value |= fgetc(wr->wav) << 8;
//	return value;
//}
//
//static void skip(FILE *f, int n) {
//	int i;
//	for (i = 0; i < n; i++)
//		fgetc(f);
//}
//
//void* wav_read_open(const char *filename) {
//	struct wav_reader* wr = (struct wav_reader*) malloc(sizeof(*wr));
//	memset(wr, 0, sizeof(*wr));
//
//	if (!strcmp(filename, "-"))
//		wr->wav = stdin;
//	else
//		wr->wav = fopen(filename, "rb");
//	if (wr->wav == NULL) {
//		free(wr);
//		return NULL;
//	}
//
//	while (1) {
//		uint32_t tag, tag2, length;
//		tag = read_tag(wr);
//		if (feof(wr->wav))
//			break;
//		length = read_int32(wr);
//		if (!length || length >= 0x7fff0000) {
//			wr->streamed = 1;
//			length = ~0;
//		}
//		if (tag != TAG('R', 'I', 'F', 'F') || length < 4) {
//			fseek(wr->wav, length, SEEK_CUR);
//			continue;
//		}
//		tag2 = read_tag(wr);
//		length -= 4;
//		if (tag2 != TAG('W', 'A', 'V', 'E')) {
//			fseek(wr->wav, length, SEEK_CUR);
//			continue;
//		}
//		// RIFF chunk found, iterate through it
//		while (length >= 8) {
//			uint32_t subtag, sublength;
//			subtag = read_tag(wr);
//			if (feof(wr->wav))
//				break;
//			sublength = read_int32(wr);
//			length -= 8;
//			if (length < sublength)
//				break;
//			if (subtag == TAG('f', 'm', 't', ' ')) {
//				if (sublength < 16) {
//					// Insufficient data for 'fmt '
//					break;
//				}
//				wr->format = read_int16(wr);
//				wr->channels = read_int16(wr);
//				wr->sample_rate = read_int32(wr);
//				wr->byte_rate = read_int32(wr);
//				wr->block_align = read_int16(wr);
//				wr->bits_per_sample = read_int16(wr);
//				if (wr->format == 0xfffe) {
//					if (sublength < 28) {
//						// Insufficient data for waveformatex
//						break;
//					}
//					skip(wr->wav, 8);
//					wr->format = read_int32(wr);
//					skip(wr->wav, sublength - 28);
//				}
//				else {
//					skip(wr->wav, sublength - 16);
//				}
//			}
//			else if (subtag == TAG('d', 'a', 't', 'a')) {
//				wr->data_pos = ftell(wr->wav);
//				wr->data_length = sublength;
//				wr->data_left = wr->data_length;
//				if (!wr->data_length || wr->streamed) {
//					wr->streamed = 1;
//					return wr;
//				}
//				fseek(wr->wav, sublength, SEEK_CUR);
//			}
//			else {
//				skip(wr->wav, sublength);
//			}
//			length -= sublength;
//		}
//		if (length > 0) {
//			// Bad chunk?
//			fseek(wr->wav, length, SEEK_CUR);
//		}
//	}
//	fseek(wr->wav, wr->data_pos, SEEK_SET);
//	return wr;
//}
//
//void wav_read_close(void* obj) {
//	struct wav_reader* wr = (struct wav_reader*) obj;
//	if (wr->wav != stdin)
//		fclose(wr->wav);
//	free(wr);
//}
//
//int wav_get_header(void* obj, int* format, int* channels, int* sample_rate, int* bits_per_sample, unsigned int* data_length) {
//	struct wav_reader* wr = (struct wav_reader*) obj;
//	if (format)
//		*format = wr->format;
//	if (channels)
//		*channels = wr->channels;
//	if (sample_rate)
//		*sample_rate = wr->sample_rate;
//	if (bits_per_sample)
//		*bits_per_sample = wr->bits_per_sample;
//	if (data_length)
//		*data_length = wr->data_length;
//	return wr->format && wr->sample_rate;
//}
//
//int wav_read_data(void* obj, unsigned char* data, unsigned int length) {
//	struct wav_reader* wr = (struct wav_reader*) obj;
//	int n;
//	if (wr->wav == NULL)
//		return -1;
//	if (length > wr->data_left && !wr->streamed) {
//		int loop = 1;
//		if (loop) {
//			fseek(wr->wav, wr->data_pos, SEEK_SET);
//			wr->data_left = wr->data_length;
//		}
//		length = wr->data_left;
//	}
//	n = fread(data, 1, length, wr->wav);
//	wr->data_left -= length;
//	return n;
//}
//

#include "wavreader.h"
#include "mxc.h"
#include "ff.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "stdio.h"

#define TAG(a, b, c, d) (((a) << 24) | ((b) << 16) | ((c) << 8) | (d))
#define DEBUG_PRINT(...)  printf(__VA_ARGS__)

struct wav_reader {
    FIL fil;
    long data_pos;
    uint32_t data_length;
    uint32_t data_left;

    int format;
    int sample_rate;
    int bits_per_sample;
    int channels;
    int byte_rate;
    int block_align;

    int streamed;
};

static uint32_t read_tag(struct wav_reader* wr) {
    uint32_t tag = 0;
    UINT br;
    unsigned char byte;

    for(int i = 0; i < 4; i++) {
        if(f_read(&wr->fil, &byte, 1, &br) != FR_OK || br != 1) {
            DEBUG_PRINT("Failed to read tag byte %d\n", i);
            return 0;
        }
        tag = (tag << 8) | byte;
    }

//    DEBUG_PRINT("Read tag: %c%c%c%c\n",
//        (tag >> 24) & 0xFF,
//        (tag >> 16) & 0xFF,
//        (tag >> 8) & 0xFF,
//        tag & 0xFF);

    return tag;
}

static uint32_t read_int32(struct wav_reader* wr) {
    uint32_t value = 0;
    UINT br;
    unsigned char byte;

    for(int i = 0; i < 4; i++) {
        if(f_read(&wr->fil, &byte, 1, &br) != FR_OK || br != 1) {
            DEBUG_PRINT("Failed to read int32 byte %d\n", i);
            return 0;
        }
        value |= (byte << (i * 8));
    }

//    DEBUG_PRINT("Read int32: %u\n", value);
    return value;
}

static uint16_t read_int16(struct wav_reader* wr) {
    uint16_t value = 0;
    UINT br;
    unsigned char byte;

    for(int i = 0; i < 2; i++) {
        if(f_read(&wr->fil, &byte, 1, &br) != FR_OK || br != 1) {
            DEBUG_PRINT("Failed to read int16 byte %d\n", i);
            return 0;
        }
        value |= (byte << (i * 8));
    }

//    DEBUG_PRINT("Read int16: %u\n", value);
    return value;
}

static FRESULT skip(FIL* fil, UINT n) {
    FRESULT fr;
    fr = f_lseek(fil, f_tell(fil) + n);
    if(fr != FR_OK) {
        DEBUG_PRINT("Skip failed: %d\n", fr);
    }
    return fr;
}

void* wav_read_open(const char *filename) {
    FRESULT fr;
    struct wav_reader* wr;

//    DEBUG_PRINT("Opening WAV file: %s\n", filename);

    wr = (struct wav_reader*) malloc(sizeof(*wr));
    if (!wr) {
        DEBUG_PRINT("Failed to allocate wav_reader\n");
        return NULL;
    }

    memset(wr, 0, sizeof(*wr));

    fr = f_open(&wr->fil, filename, FA_READ);
    if (fr != FR_OK) {
        DEBUG_PRINT("Failed to open file: %d\n", fr);
        free(wr);
        return NULL;
    }

//    DEBUG_PRINT("File opened successfully\n");

    // Read RIFF header
    uint32_t tag = read_tag(wr);
    if (tag != TAG('R', 'I', 'F', 'F')) {
        DEBUG_PRINT("Not a RIFF file\n");
        f_close(&wr->fil);
        free(wr);
        return NULL;
    }

    uint32_t length = read_int32(wr);
//    DEBUG_PRINT("File length: %u\n", length);

    tag = read_tag(wr);
    if (tag != TAG('W', 'A', 'V', 'E')) {
        DEBUG_PRINT("Not a WAVE file\n");
        f_close(&wr->fil);
        free(wr);
        return NULL;
    }

    // Read chunks
    while (1) {
        tag = read_tag(wr);
        if (f_eof(&wr->fil)) break;

        length = read_int32(wr);
//        DEBUG_PRINT("Chunk length: %u\n", length);

        if (tag == TAG('f', 'm', 't', ' ')) {
//            DEBUG_PRINT("Processing fmt chunk\n");
            if (length < 16) {
                DEBUG_PRINT("Invalid fmt chunk\n");
                f_close(&wr->fil);
                free(wr);
                return NULL;
            }

            wr->format = read_int16(wr);
            wr->channels = read_int16(wr);
            wr->sample_rate = read_int32(wr);
            wr->byte_rate = read_int32(wr);
            wr->block_align = read_int16(wr);
            wr->bits_per_sample = read_int16(wr);

//            DEBUG_PRINT("Format: %d\n", wr->format);
//            DEBUG_PRINT("Channels: %d\n", wr->channels);
//            DEBUG_PRINT("Sample Rate: %d\n", wr->sample_rate);
//            DEBUG_PRINT("Bits per Sample: %d\n", wr->bits_per_sample);

            if (length > 16) {
                skip(&wr->fil, length - 16);
            }
        }
        else if (tag == TAG('d', 'a', 't', 'a')) {
//            DEBUG_PRINT("Found data chunk\n");
            wr->data_pos = f_tell(&wr->fil);
            wr->data_length = length;
            wr->data_left = length;
//            DEBUG_PRINT("Data position: %ld\n", wr->data_pos);
//            DEBUG_PRINT("Data length: %u\n", wr->data_length);
            break;  // Stop after finding data chunk
        }
        else {
            DEBUG_PRINT("Skipping unknown chunk\n");
            skip(&wr->fil, length);
        }
    }

    if (wr->data_pos == 0) {
        DEBUG_PRINT("No data chunk found\n");
        f_close(&wr->fil);
        free(wr);
        return NULL;
    }

    // Position file pointer at start of data
    fr = f_lseek(&wr->fil, wr->data_pos);
    if (fr != FR_OK) {
        DEBUG_PRINT("Failed to seek to data position: %d\n", fr);
        f_close(&wr->fil);
        free(wr);
        return NULL;
    }

    return wr;
}

int wav_get_header(void* obj, int* format, int* channels, int* sample_rate, int* bits_per_sample, unsigned int* data_length) {
    struct wav_reader* wr = (struct wav_reader*) obj;
    if (!wr) return 0;

    if (format)
        *format = wr->format;
    if (channels)
        *channels = wr->channels;
    if (sample_rate)
        *sample_rate = wr->sample_rate;
    if (bits_per_sample)
        *bits_per_sample = wr->bits_per_sample;
    if (data_length)
        *data_length = wr->data_length;

    return (wr->format && wr->sample_rate);
}

//int wav_read_data(void* obj, unsigned char* data, unsigned int length) {
//    struct wav_reader* wr = (struct wav_reader*) obj;
//    UINT br;
//    FRESULT fr;
//
//    if (!wr) return -1;
//
//    if (length > wr->data_left) {
//        length = wr->data_left;
//    }
//
//    fr = f_read(&wr->fil, data, length, &br);
//    if (fr != FR_OK) {
//        DEBUG_PRINT("Read failed: %d\n", fr);
//        return -1;
//    }
//
//    wr->data_left -= br;
//    return br;
//}

//int wav_read_data(void* obj, unsigned char* data, unsigned int num_samples, unsigned int cursor)
//{
//    DEBUG_PRINT("Initializing wav reader\n");
//    struct wav_reader* wr = (struct wav_reader*) obj;
//    UINT br;
//    FRESULT fr;
//
//    if (!wr) return -1;
//
//    // Calculate bytes to read (16-bit samples)
//    unsigned int bytes_to_read = num_samples * 2;
//
//    DEBUG_PRINT("Seeking wav reader to position: %u\n", cursor);
//    fr = f_lseek(&wr->fil, cursor);
//    if (fr != FR_OK) {
//        DEBUG_PRINT("Seek failed: %d\n", fr);
//        return -1;
//    }
//
//    // Try reading just the first chunk to verify basic functionality
//    DEBUG_PRINT("Reading first chunk...\n");
//    fr = f_read(&wr->fil, data, 512, &br);
//    if (fr != FR_OK) {
//        DEBUG_PRINT("First chunk read failed: %d\n", fr);
//        return -1;
//    }
//    DEBUG_PRINT("First chunk read successful: %u bytes\n", br);
//
//    // Add a small delay before second chunk
//    for(volatile int i = 0; i < 1000; i++);
//
//    // Try reading second chunk with detailed error reporting
//    DEBUG_PRINT("Reading second chunk...\n");
//    DEBUG_PRINT("Buffer address for second chunk: 0x%x\n", (unsigned int)(data + 512));
//    fr = f_read(&wr->fil, data + 512, 512, &br);
//    if (fr != FR_OK) {
//        DEBUG_PRINT("Second chunk read failed: %d, error code: %d\n", fr, fr);
//        // Return partial success
//        return 256; // 512 bytes = 256 samples
//    }
//
//    wr->data_left -= (br + 512); // Account for both chunks
//    return (br + 512) / 2; // Convert total bytes to samples
//}

int wav_read_data(void* obj, unsigned char* data, unsigned int num_samples, unsigned int cursor)
{
//    DEBUG_PRINT("Initializing wav reader\n");
    struct wav_reader* wr = (struct wav_reader*) obj;
    UINT br;
    FRESULT fr;


    unsigned int bytes_to_read = num_samples*2;  // Just read one safe chunk

//    DEBUG_PRINT("Seeking wav reader to position: %u\n", cursor);
    fr = f_lseek(&wr->fil, cursor);
    if (fr != FR_OK) {
        DEBUG_PRINT("Seek failed: %d\n", fr);
        return -1;
    }

//    DEBUG_PRINT("Reading single chunk of %d bytes\n", num_samples);
//    DEBUG_PRINT("Buffer address: 0x%x\n", (unsigned int)data);

    fr = f_read(&wr->fil, data, bytes_to_read, &br);
    if (fr != FR_OK) {
        DEBUG_PRINT("Read failed: %d\n", fr);
        return -1;
    }

    wr->data_left -= br;
    return br / 2;  // Convert bytes to samples
}

//int wav_read_data(void* obj, unsigned char* data, unsigned int length, unsigned int cursor)
//{
//	DEBUG_PRINT("Initializing wav reader\n");
//	struct wav_reader* wr = (struct wav_reader*) obj;
//    UINT br;
//    FRESULT fr;
//
//    if (!wr) return -1;
//    DEBUG_PRINT("seeking wav reader\n");
//    // Move the file pointer to the specified cursor position
//    fr = f_lseek(&wr->fil, cursor);
//    if (fr != FR_OK)
//    {
//        DEBUG_PRINT("Seek failed: %d\n", fr);
//        return -1;
//    }
//
//    DEBUG_PRINT("reading wav reader\n");
//
//    if (length > wr->data_left) {
//        length = wr->data_left;
//    }
//
//    fr = f_read(&wr->fil, data, length, &br);
//    if (fr != FR_OK) {
//        DEBUG_PRINT("Read failed: %d\n", fr);
//        return -1;
//    }
//    DEBUG_PRINT("exit wav reader\n");
//    wr->data_left -= br;
//    return br;
//}


void wav_read_close(void* obj) {
    struct wav_reader* wr = (struct wav_reader*) obj;
    if (wr) {
        f_close(&wr->fil);
        free(wr);
    }
}
