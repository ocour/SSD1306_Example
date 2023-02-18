/*
 * SSD1306.h
 *
 *  Created on: 13 Feb 2023
 *      Author: OC
 */

#ifndef INC_DRIVERS_SSD1306_H_
#define INC_DRIVERS_SSD1306_H_

#include "stm32f1xx_hal.h"
#include <stm32f1xx_hal_i2c.h>

#include "fonts.h"

#include <stdint.h>
#include <assert.h>
#include <string.h>

#define SSD1306_128_32_HEIGHT 	32
#define SSD1306_128_32_WIDTH	128

#define SSD1306_128_32_BUFFERSIZE ((SSD1306_128_32_HEIGHT * SSD1306_128_32_WIDTH) / 8)

#define SSD1306_128_32_ADDR (0x3C << 1)

/* PAGE 28 OF DATASHEET */
/* FUNDAMENTAL COMMAND TABLE */
#define CMD_SET_CONTRAST 					0x81
#define CMD_ENTIRE_DISPLAY_ON_RESUME 		0xA4
#define CMD_ENTIRE_DISPLAY_ON_IGNORE 		0xA5
#define CMD_DISPLAY_NORMAL 					0xA6
#define CMD_DISPLAY_INVERSE 				0xA7
#define CMD_DISPLAY_OFF 					0xAE
#define CMD_DISPLAY_ON 						0xAF

/* PAGE 29 */
/* SCROLLING COMMAND TABLE */
#define CMD_DEACTIVATE_SCROLL				0x2E
#define CMD_ACTIVATE_SCROLL					0x2F

/* PAGE 30 */
/* ADDRESSING SETTING COMMAND TABLE */
#define CMD_SET_COLUMN_LWR					0x00	/* 0x00~0F */
#define CMD_SET_COLUMN_HGR					0x10	/* 0x10~1F */
#define CMD_SET_MEMORY_ADR_MODE 			0x20
#define CMD_SET_COLUMN_ADR					0x21
#define CMD_SET_PAGE_ADR					0x22
#define CMD_SET_PAGE_START_ADR				0xB0	/* 0xB0~B7 */

/* PAGE 31 */
/* HARDWARE CONFIGURATION COMMAND TABLE */
#define CMD_SET_DISPLAY_START_LINE			0x40	/* 0x40~7F */
#define CMD_SET_SEGMENT_RMAP_0				0xA0
#define CMD_SET_SEGMENT_RMAP_127			0xA1
#define CMD_SET_MULTIPLEX_RATIO				0xA8
#define CMD_SET_COM_SCAN_DIR				0xC0	/* 0xC0/C8 */
#define CMD_SET_DISPLAY_OFFSET				0xD3
#define CMD_SET_COM_PINS					0xDA

/* PAGE 32 */
/* TIMING & DRIVING SCHEME SETTING COMMAND TABLE */
#define CMD_SET_CLOCK_DIVIDE_RATIO_AND_F	0xD5
#define CMD_SET_PREC_PERIOD					0xD9
#define CMD_SET_COMH_DESELECT_LVL			0xDB
#define CMD_NOP								0xE3

/* PAGE 62 */
/* CHARGE PUMP COMMAND TABLE */
#define CMD_CHARGE_PUMP_SETTING				0x8D

/* PAGE 30 */
#define MM_ADR_MODE_HORIZONTAL				0x0
#define MM_ADR_MODE_VERTICAL				0x1
#define MM_ADR_MODE_PAGE					0x2

/* CONTINUATION BIT, PAGE 20 */
#define WRITE_COMMAND						0x00
#define WRITE_READ_DATA						0x40

/* Colors */
#define SSD1306_COLOR_BLACK					0
#define SSD1306_COLOR_WHITE					1

typedef struct {
	uint16_t devAddress;
	uint8_t buffer[SSD1306_128_32_BUFFERSIZE];

	uint8_t height;
	uint8_t width;
	uint8_t inverted;

	uint16_t x_cursor;
	uint16_t y_cursor;

	I2C_HandleTypeDef *hi2c;
} SSD1306_HandleTypeDef;

void SSD1306_WriteCommand(SSD1306_HandleTypeDef* hssd, uint8_t* data, uint8_t size);

void SSD1306_ClearDisplay(SSD1306_HandleTypeDef* hssd);
void SSD1306_FillDisplay(SSD1306_HandleTypeDef* hssd);
void SSD1306_Display(SSD1306_HandleTypeDef* hssd);
void SSD1306_InvertDisplay(SSD1306_HandleTypeDef* hssd);
void SSD1306_PutPixel(SSD1306_HandleTypeDef* hssd, uint16_t x, uint16_t y, uint8_t color);
void SSD1306_DisplayOFF(SSD1306_HandleTypeDef* hssd);
void SSD1306_DisplayON(SSD1306_HandleTypeDef* hssd);
void SSD1306_DrawBitmap(SSD1306_HandleTypeDef* hssd, uint16_t x, uint16_t y, uint8_t* bitmap, uint16_t width, uint16_t height, uint8_t color);
void SSD1306_DrawChar(SSD1306_HandleTypeDef* hssd, SSD1306_FontTypeDef* font, char ch, uint16_t x, uint16_t y, uint8_t color);
void SSD1306_DrawString(SSD1306_HandleTypeDef* hssd, SSD1306_FontTypeDef* font, char* s, uint16_t x, uint16_t y, uint8_t color);

void SSD1306_Init(I2C_HandleTypeDef *hi2c, SSD1306_HandleTypeDef* hssd, uint16_t devAddress);
void SSD1306_InitCommands(SSD1306_HandleTypeDef* hssd);

#endif /* INC_DRIVERS_SSD1306_H_ */
