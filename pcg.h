/*******************************************************************************
 * pcg.h
 * Header file for pcg.c
 *
 * Copyright (C) 2019-2024 Maxim Integrated Products, Inc., All rights Reserved.
 *******************************************************************************/

#ifndef PCG_H
#define PCG_H

#include <stdint.h>

/***** Constants *****/
#define MAXLEN 256

/***** External Variables *****/
extern volatile uint32_t cnn_time;
extern int max_class;

/***** Function Prototypes *****/
void LCD_Command(uint8_t cmd);
void LCD_Init(void);
void LCD_String(char *str);
void LCD_Data(uint8_t data);
int lcd(void);
void fail(void);
void load_input(void);
int readFromSD(int num);
void printdata(void);
int check_output(void);
char* pcg_main(int num);

#endif // PCG_H
