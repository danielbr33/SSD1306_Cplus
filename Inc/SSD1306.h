/*
 * SSD1306.h
 *
 *  Created on: 22.03.2020
 *      Author: danie
 */

#ifndef APPLICATION_USER_SSD1306_H_
#define APPLICATION_USER_SSD1306_H_

#ifndef __SSD1306_H__
#define __SSD1306_H__

#include <cstddef>
#include "_ansi.h"
#include "SSD1306_font.h"


#define STM32F3
#if defined(STM32F1)
#include "stm32f1xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#elif defined(STM32L0)
#include "stm32l0xx_hal.h"
#elif defined(STM32L4)
#include "stm32l4xx_hal.h"
#elif defined(STM32F3)
#include "stm32f3xx_hal.h"
#elif defined(STM32H7)
#include "stm32h7xx_hal.h"
#elif defined(STM32F7)
#include "stm32f7xx_hal.h"
#else
 #error "SSD1306 library was tested only on STM32F1, STM32F3, STM32F4, STM32F7, STM32L0, STM32L4, STM32H7 MCU families. Please modify ssd1306.h if you know what you are doing. Also please send a pull request if it turns out the library works on other MCU's as well!"
#endif


#define SSD1306_USE_SPI

/* vvv I2C config vvv */
#ifndef SSD1306_I2C_PORT
#define SSD1306_I2C_PORT		hi2c1
#endif


#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR        (0x3C << 1)
#endif
/* ^^^ I2C config ^^^ */

/* vvv SPI config vvv */
#ifndef SSD1306_SPI_PORT
#define SSD1306_SPI_PORT        hspi2
#endif

#ifndef SSD1306_CS_Port
#define SSD1306_CS_Port         GPIOB
#endif
#ifndef SSD1306_CS_Pin
#define SSD1306_CS_Pin          GPIO_PIN_12
#endif

#ifndef SSD1306_DC_Port
#define SSD1306_DC_Port         GPIOB
#endif
#ifndef SSD1306_DC_Pin
#define SSD1306_DC_Pin          GPIO_PIN_14
#endif

#ifndef SSD1306_Reset_Port
#define SSD1306_Reset_Port      GPIOA
#endif
#ifndef SSD1306_Reset_Pin
#define SSD1306_Reset_Pin       GPIO_PIN_8
#endif
/* ^^^ SPI config ^^^ */

#if defined(SSD1306_USE_I2C)
extern I2C_HandleTypeDef SSD1306_I2C_PORT;
#elif defined(SSD1306_USE_SPI)
extern SPI_HandleTypeDef SSD1306_SPI_PORT;
#else
#error "You should define SSD1306_USE_SPI or SSD1306_USE_I2C macro!"
#endif

// SSD1306 OLED height in pixels
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT          64
#endif

// SSD1306 width in pixels
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH           128
#endif

// Enumeration for screen colors
typedef enum {
    Black = 0x00, // Black color, no pixel
    White = 0x01  // Pixel is set. Color depends on OLED
} SSD1306_COLOR;

// Struct to store transformations

class SSD1306 {
public:
	SSD1306();
	virtual ~SSD1306();
	// Procedure definitions
	void ssd1306_Init(void);
	void ssd1306_Fill(SSD1306_COLOR color);
	void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
	char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color);
	char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color);
	void ssd1306_SetCursor(uint8_t x, uint8_t y);
	void process(void);
	void SPI_Interrupt();
	void loop();
	// Low-level procedures
	void ssd1306_Reset(void);
	void ssd1306_WriteCommand();
	void ssd1306_WriteData();
private:
    uint16_t currentX;
    uint16_t currentY;
    uint8_t inverted;
    uint8_t initialized;
    uint8_t status;
    uint8_t initCommands[28];
    uint8_t lineCommands[3];
    uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT /8];
    uint8_t counter;
};

#endif /* APPLICATION_USER_SSD1306_H_ */
#endif // __SSD1306_H__
