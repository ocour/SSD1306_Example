/*
 * SSD1306_Fonts.h
 *
 *  Created on: 17 Feb 2023
 *      Author: Admin
 */

#ifndef INC_DRIVERS_FONTS_H_
#define INC_DRIVERS_FONTS_H_

#include <stdint.h>

typedef struct {
	uint8_t width;
	uint8_t height;
	uint8_t* font;
} SSD1306_FontTypeDef;

// WIDTH: 6; HEIGHT: 8;
extern uint8_t SSD1306_Font6x8[];

#endif /* INC_DRIVERS_FONTS_H_ */
