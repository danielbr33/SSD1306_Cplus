/*
 * SSD1306.cpp
 *
 *  Created on: 22.03.2020
 *      Author: danie
 */

#include "SSD1306.h"

#if defined(SSD1306_USE_I2C)
void SSD1306::Reset(void) {
	/* for I2C - do nothing */
}
// Send a byte to the command register
void SSD1306::WriteCommand(uint8_t byte) {
	HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x00, 1, &byte, 1, HAL_MAX_DELAY);
}
// Send data
void SSD1306::WriteData(uint8_t* buffer, size_t buff_size) {
	HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x40, 1, buffer, buff_size, HAL_MAX_DELAY);
}

#elif defined(SSD1306_USE_SPI)
void SSD1306::Reset(void) {
	// CS = High (not selected)
	HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_SET);

	// Reset the OLED
	HAL_GPIO_WritePin(SSD1306_Reset_Port, SSD1306_Reset_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(SSD1306_Reset_Port, SSD1306_Reset_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
}
// Send a byte to the command register
void SSD1306::WriteCommand() {
    HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_RESET); // select OLED
    HAL_GPIO_WritePin(SSD1306_DC_Port, SSD1306_DC_Pin, GPIO_PIN_RESET); // command
	HAL_SPI_Transmit_DMA(&SSD1306_SPI_PORT, lineCommands, 3);
}
// Send data
void SSD1306::WriteData() {
    HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_RESET); // select OLED
    HAL_GPIO_WritePin(SSD1306_DC_Port, SSD1306_DC_Pin, GPIO_PIN_SET); // data
	HAL_SPI_Transmit_DMA(&SSD1306_SPI_PORT, &SSD1306_Buffer[SSD1306_WIDTH*counter], SSD1306_WIDTH);
}

#else
#error "You should define SSD1306_USE_SPI or SSD1306_USE_I2C macro"
#endif

void SSD1306::SPI_Interrupt(){
	if (status==2);
	else if (status==0){
        lineCommands[0]=0xB0 + counter;
        lineCommands[1]=0x00;
        lineCommands[2]=0x10;
        status=1;
		WriteCommand();
	}
	else{
		status=0;
		counter+=1;
		if (counter==8)
			counter=0;
		WriteData();
	}
}

void SSD1306::Init(void) {
	// Reset OLED
	Reset();
    // Wait for the screen to boot
    HAL_Delay(100);

    // Init OLED
    initCommands[0]=TURN_OFF;

    initCommands[1]=SET_MEMORY_ADDR_MODE;
    initCommands[2]=HORIZONTAL_ADDR_MODE;

    initCommands[3]=SET_PAGE_START_ADDR;

	#ifdef SSD1306_MIRROR_VERT
		initCommands[4]=MIRROR_VERTICAL;
	#else
		initCommands[4]=COM_SCAN_DIRECTION;
	#endif

    initCommands[5]=LOW_COLUMN_ADDR;
    initCommands[6]=HIGH_COLUMN_ADDR;

    initCommands[7]=SET_START_LINE_ADDR;

    initCommands[8]=SET_CONTRAST;
    initCommands[9]=CONTRAST;

	#ifdef SSD1306_MIRROR_HORIZ
		initCommands[10]=MIRROR_HORIZONTAL;
	#else
		initCommands[10]=SET_SEGMENT_REMAP;
	#endif

	#ifdef SSD1306_INVERSE_COLOR
		initCommands[11]=INVERSE_COLOR;
	#else
		initCommands[11]=NORMAL_COLOR;
	#endif

    initCommands[12]=SET_MULTIPLEX_RATIO;
	#if (SSD1306_HEIGHT == 32)
		initCommands[13]=RATIO_32;
	#elif (SSD1306_HEIGHT == 64)
		initCommands[13]=RATIO_64;
	#else
	#error "Only 32 or 64 lines of height are supported!"
	#endif

    initCommands[14]=OUT_FOLLOW_RAM_CONTENT;

    initCommands[15]=DISPLAY_OFFSET;
    initCommands[16]=DISPLAY_NOT_OFFSET;

    initCommands[17]=SET_CLOCK_DIVIDE_RATIO;
    initCommands[18]=DIVIDE_RATIO;

    initCommands[19]=SET_PRE_CHARGE_PERIOD;
    initCommands[20]=PRE_CHARGE_PERIOD;

    initCommands[21]=SET_COM_PIN;
	#if (SSD1306_HEIGHT == 32)
		initCommands[22]=COM_PIN_32;
	#elif (SSD1306_HEIGHT == 64)
		initCommands[22]=COM_PIN_64;
	#else
	#error "Only 32 or 64 lines of height are supported!"
	#endif

    initCommands[23]=SET_VCOMH;
    initCommands[24]=VOLTAGE_77;

    initCommands[25]=SET_DC_ENABLE;
    initCommands[26]=DC_ENABLE;
    initCommands[27]=TURN_ON;

    status=2;
    currentX = 0;
    currentY = 0;
    initialized = 1;

    Fill(White);
    HAL_SPI_Transmit_DMA(&SSD1306_SPI_PORT, initCommands, 28);
    status=0;
    SPI_Interrupt();
}

void SSD1306::process(){
	//components to display
	HAL_Delay(5);
}

// Fill the whole screen with the given color
void SSD1306::Fill(SSD1306_COLOR color) {
    /* Set memory */
    uint32_t i;

    for(i = 0; i < sizeof(SSD1306_Buffer); i++) {
        SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
    }
}

//    Draw one pixel in the screenbuffer
//    X => X Coordinate
//    Y => Y Coordinate
//    color => Pixel color
void SSD1306::DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color) {
    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        // Don't write outside the buffer
        return;
    }

    // Check if pixel should be inverted
    if(inverted) {
        color = (SSD1306_COLOR)!color;
    }

    // Draw in the right color
    if(color == White) {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    } else {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}

// Draw 1 char to the screen buffer
// ch       => char om weg te schrijven
// Font     => Font waarmee we gaan schrijven
// color    => Black or White
char SSD1306::WriteChar(char ch, FontDef Font, SSD1306_COLOR color) {
    uint32_t i, b, j;

    // Check if character is valid
    if (ch < 32 || ch > 126)
        return 0;

    // Check remaining space on current line
    if (SSD1306_WIDTH < (currentX + Font.FontWidth) ||
        SSD1306_HEIGHT < (currentY + Font.FontHeight))
    {
        // Not enough space on current line
        return 0;
    }

    // Use the font to write
    for(i = 0; i < Font.FontHeight; i++) {
        b = Font.data[(ch - 32) * Font.FontHeight + i];
        for(j = 0; j < Font.FontWidth; j++) {
            if((b << j) & 0x8000)  {
                DrawPixel(currentX + j, (currentY + i), (SSD1306_COLOR) color);
            } else {
                DrawPixel(currentX + j, (currentY + i), (SSD1306_COLOR)!color);
            }
        }
    }

    // The current space is now taken
    currentX += Font.FontWidth;

    // Return written char for validation
    return ch;
}

// Write full string to screenbuffer
char SSD1306::WriteString(char* str, FontDef Font, SSD1306_COLOR color) {
    // Write until null-byte
    while (*str) {
        if (WriteChar(*str, Font, color) != *str) {
            // Char could not be written
            return *str;
        }

        // Next char
        str++;
    }

    // Everything ok
    return *str;
}

// Position the cursor
void SSD1306::SetCursor(uint8_t x, uint8_t y) {
    currentX = x;
    currentY = y;
}

SSD1306::SSD1306() {
	// TODO Auto-generated constructor stub
	counter=7;
}

SSD1306::~SSD1306() {
	// TODO Auto-generated destructor stub
}
