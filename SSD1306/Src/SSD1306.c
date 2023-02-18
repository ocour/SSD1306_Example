/*
 * SSD1306.c
 *
 *  Created on: Feb 13, 2023
 *      Author: Admin
 */

#include "SSD1306.h"

/*
 * Writes command to SSD1306
 * @param hssd: SSD1306 handle
 * @param data: command to be transmitted
 */
void SSD1306_WriteCommand(SSD1306_HandleTypeDef* hssd, uint8_t* data, uint8_t size)
{
	assert(hssd);

	/* Byte which signals that a command is being transmitted */
	uint8_t tBuf[2] = { WRITE_COMMAND };

	for(uint8_t i = 0; i < size; i++) {
		memcpy(tBuf+1, data+i, 1);
		HAL_I2C_Master_Transmit(hssd->hi2c, hssd->devAddress, tBuf, 2, HAL_MAX_DELAY);
	}
}

/*
 *	Clears SSD1306 buffer
 *	@param hssd: SSD1306 handle
 *	Call to SSD1306_Display needed to write content to display
 */
void SSD1306_ClearDisplay(SSD1306_HandleTypeDef* hssd)
{
	assert(hssd);

	/* Clears buffer */
	memset(hssd->buffer, 0, SSD1306_128_32_BUFFERSIZE);
}

/*
 * 	Fills buffer
 */
void SSD1306_FillDisplay(SSD1306_HandleTypeDef* hssd)
{
	assert(hssd);

	/* Fills buffer */
	memset(hssd->buffer, 0xff, SSD1306_128_32_BUFFERSIZE);
}

/*
 * 	Writes Buffers content to Display,
 * 	Needs to be called so that Display's content is updated
 */
void SSD1306_Display(SSD1306_HandleTypeDef* hssd)
{
	assert(hssd);
	assert(hssd->height);
	assert(hssd->width);
	assert(hssd->hi2c);

	/* Byte which signals that data will be Written or Read */
	uint8_t tBuf[] = { WRITE_READ_DATA };

	/* Transmit first byte that will indicate that data is being transmitted */
	HAL_I2C_Master_Seq_Transmit_IT(hssd->hi2c, hssd->devAddress, tBuf, 1, I2C_FIRST_FRAME);
	/* POLL */
	while(HAL_I2C_GetState(hssd->hi2c) != HAL_I2C_STATE_READY);

	uint16_t i;
	for(i = 0; i < SSD1306_128_32_BUFFERSIZE - 1; i++) {
		/* Transmit data */
		HAL_I2C_Master_Seq_Transmit_IT(hssd->hi2c, hssd->devAddress, hssd->buffer+i, 1, I2C_NEXT_FRAME);
		/* POLL */
		while(HAL_I2C_GetState(hssd->hi2c) != HAL_I2C_STATE_READY);
	}

	/* Transmit last data */
	HAL_I2C_Master_Seq_Transmit_IT(hssd->hi2c, hssd->devAddress, hssd->buffer+i, 1, I2C_LAST_FRAME);
	/* POLL */
	while(HAL_I2C_GetState(hssd->hi2c) != HAL_I2C_STATE_READY);
}

/*
 *	Inverts Display, not buffer
 */
void SSD1306_InvertDisplay(SSD1306_HandleTypeDef* hssd)
{
	assert(hssd);

	uint8_t command[1];

	if(hssd->inverted == 0) {
		command[0] = CMD_DISPLAY_INVERSE;
		SSD1306_WriteCommand(hssd, command, 1);
		hssd->inverted = 1;
	}
	else {
		command[0] = CMD_DISPLAY_NORMAL;
		SSD1306_WriteCommand(hssd, command, 1);
		hssd->inverted = 0;
	}
}

/*
 *	Places pixel into x and y coordinates,
 *	 SSD1306_Display function needs to be called to write buffer to OLED display
 */
void SSD1306_PutPixel(SSD1306_HandleTypeDef* hssd, uint16_t x, uint16_t y, uint8_t color)
{
	assert(hssd);

	/* Outside buffer dimensions */
	if((x < 0) || (x > SSD1306_128_32_WIDTH) || (y < 0) || (y > SSD1306_128_32_HEIGHT))
		return;

	if(color == SSD1306_COLOR_BLACK) {
		hssd->buffer[x + (y / 8) * SSD1306_128_32_WIDTH] |= (1 << (y & 7));
	}
	else if(color == SSD1306_COLOR_WHITE) {
		hssd->buffer[x + (y / 8) * SSD1306_128_32_WIDTH] &= ~(1 << (y & 7));
	}
}

/*
 * Turns display OFF
 */
void SSD1306_DisplayOFF(SSD1306_HandleTypeDef* hssd)
{
	assert(hssd);

	uint8_t command[] = {
			CMD_DISPLAY_OFF
	};

	SSD1306_WriteCommand(hssd, command, 1);
}

/*
 * Turns display ON
 */
void SSD1306_DisplayON(SSD1306_HandleTypeDef* hssd)
{
	assert(hssd);

	uint8_t command[] = {
			CMD_DISPLAY_ON
	};

	SSD1306_WriteCommand(hssd, command, 1);
}

/*
 * 	Draws a bitmap at x and y coordinates
 * 	width and height are bitmaps width and height
 * 	Credit to Adafruit's GFX library: https://github.com/adafruit/Adafruit-GFX-Library
 */
void SSD1306_DrawBitmap(
		SSD1306_HandleTypeDef* hssd, uint16_t x, uint16_t y,
		uint8_t* bitmap, uint16_t width, uint16_t height, uint8_t color
)
{
	int16_t byteWidth = (width + 7) / 8;
	uint8_t b = 0;

	for(uint16_t j = 0; j < height; j++, y++) {
		for(uint16_t i = 0; i < width; i++) {
			if(i & 7)
				b <<= 1;
			else
				b = bitmap[j * byteWidth + i / 8];
			if(b & 0x80)
				SSD1306_PutPixel(hssd, x + i, y, color);
		}
	}
}

/*
 * Puts a character on the screen at x and y coordinates
 * Updates x and y cursor,
 * To automatically calculate next character placement, give hssd->x_cursor and y_cursor as x and y coordinates
 */
void SSD1306_DrawChar(SSD1306_HandleTypeDef* hssd, SSD1306_FontTypeDef* font, char ch, uint16_t x, uint16_t y, uint8_t color)
{
	hssd->x_cursor = x;
	hssd->y_cursor = y;

	uint16_t i, b, j;

	for (i = 0; i < font->width; i++) {
		b = font->font[(ch - 32) * font->width + i];
		for (j = 0; j < font->height; j++) {
			if ((b << j) & 0x80) {
				SSD1306_PutPixel(hssd, (hssd->x_cursor + i), ((hssd->y_cursor  + font->height) - j), color);
			}
			else {
				SSD1306_PutPixel(hssd, (hssd->x_cursor + i), ((hssd->y_cursor  + font->height) - j), !color);
			}
		}
	}


	hssd->x_cursor += font->width;
}

/*
 * 	Draws string,
 * 	Stops at terminating null character '\0'
 */
void SSD1306_DrawString(SSD1306_HandleTypeDef* hssd, SSD1306_FontTypeDef* font, char* s, uint16_t x, uint16_t y, uint8_t color)
{
	hssd->x_cursor = x;
	hssd->y_cursor = y;

	uint16_t i = 0;
	while(s[i] != '\0') {
		SSD1306_DrawChar(hssd, font, s[i++], hssd->x_cursor, hssd->y_cursor, color);
	}
}

/*
 * 	Initialises SSD1306 handle,
 * 	@Prama hi2c: HAL's i2c handle
 * 	@Param hssd: SSD1306 handle
 * 	@Param devAddress: slave address of SSD1306 device
 */
void SSD1306_Init(I2C_HandleTypeDef *hi2c, SSD1306_HandleTypeDef* hssd, uint16_t devAddress)
{
	/* Check parameters */
	assert(hssd);
	assert(hi2c);
	assert(devAddress);

	/* Save (OLED)device address */
	hssd->devAddress = devAddress;

	/* SET BUFFER TO 0x0 */
	memset(hssd->buffer, 0, SSD1306_128_32_BUFFERSIZE);

	/* SET DISPLAY DIMENSIONS */
	hssd->height = SSD1306_128_32_HEIGHT;
	hssd->width = SSD1306_128_32_WIDTH;

	hssd->inverted = 0;

	/* Set I2C handle */
	hssd->hi2c = hi2c;

	hssd->x_cursor = 0;
	hssd->y_cursor = 0;

	/* Display Initialisation commands */
	SSD1306_InitCommands(hssd);
}

/*
 *	SSD1306_InitCommands
 *	@params hssd: SSD1306 handle
 */
void SSD1306_InitCommands(SSD1306_HandleTypeDef* hssd)
{
	/* Turn Display OFF */
	{
		uint8_t init[] = {
				CMD_DISPLAY_OFF 		// 0xAE
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
	}

	/* Set Horizontal memory Addressing mode  */
	{
		uint8_t init[] = {
				CMD_SET_MEMORY_ADR_MODE, // 0x20
				MM_ADR_MODE_HORIZONTAL
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
	}

	/* Set COM Output Scan Direction */
	{
		uint8_t init[] = {
				CMD_SET_COM_SCAN_DIR | 0x08	// 0xC8
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
	}

	/* Set Column Address  */
	{
		uint8_t init[] = {
				CMD_SET_COLUMN_ADR, 	// 0x21
				0x00,
				(hssd->width - 1)
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
 	}

	/* Set Page Address  */
	{
		uint8_t init[] = {
				CMD_SET_PAGE_ADR, 	// 0x22
				0x00,
				0x03				//<- FOR SOME REASON 0x07(which is the max) does not work.
									// Maybe because i use a 128x32 display, not sure
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
	}

	/* Set Display Contrast */
	{
		uint8_t init[] = {
				CMD_SET_CONTRAST, // 0x81
				0x8F
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
	}

	/* Set Segment re-map */
	{
		uint8_t init[] = {
				CMD_SET_SEGMENT_RMAP_127	// 0xA1
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
	}

	/* Set Multiplex ratio */
	{
		uint8_t init[] = {
				CMD_SET_MULTIPLEX_RATIO, // 0xA8
				(hssd->height - 1)
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
	}

	/* Set Display Start Line */
	{
		uint8_t init[] = {
				CMD_SET_DISPLAY_START_LINE // 0x40
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
	}

	/* Set Display Offset */
	{
		uint8_t init[] = {
				CMD_SET_DISPLAY_OFFSET, // 0xD3
				0x0
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
	}

	/* Set Display's clock ratio */
	{
		uint8_t init[] = {
				CMD_SET_CLOCK_DIVIDE_RATIO_AND_F, // 0xD5
				0x80
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
	}

	/* Set Pre-charge period */
	{
		uint8_t init[] = {
				CMD_SET_PREC_PERIOD, // 0xD9
				0x22
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
	}

	/* Set Vcomh Deselect level */
	{
		uint8_t init[] = {
				CMD_SET_COMH_DESELECT_LVL, // 0xDB
				0x40
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
	}

	/* Set COM Pins */
	{
		uint8_t init[] = {
				CMD_SET_COM_PINS, // 0xDA
				0x02
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
	}

	/* Set Chargepump */
	{
		uint8_t init[] = {
				CMD_CHARGE_PUMP_SETTING,	// 0x8D
				0x14
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
	}

	/* Set Entire Display ON, Resume RAM content  */
	{
		uint8_t init[] = {
				CMD_ENTIRE_DISPLAY_ON_RESUME // 0xA4
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
	}

	/* Set Display mode to Normal */
	{
		uint8_t init[] = {
				CMD_DISPLAY_NORMAL 	// 0xA6
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
	}

	/* Disable Scroll  */
	{
		uint8_t init[] = {
				CMD_DEACTIVATE_SCROLL	// 0x2E
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
	}

	/* Set Display ON */
	{
		uint8_t init[] = {
				CMD_DISPLAY_ON		// 0xAF
		};
		SSD1306_WriteCommand(hssd, init, sizeof(init));
	}
}

